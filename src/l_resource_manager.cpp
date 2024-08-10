#include "l_resource_manager.hpp"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "l_common.hpp"
#include "l_entity.hpp"
#include "l_model.hpp"
#include "l_shader.hpp"
#include "l_texture.hpp"
#include <array>
#include <cassert>
#include <cfloat>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace lain {

namespace resource_manager {

std::unordered_map<int, std::unique_ptr<shader>> _shaders;
std::unordered_map<int, std::unique_ptr<texture>> _textures;
std::unordered_map<model_type, std::unique_ptr<model>> _models;
std::unordered_map<entity_id, model_type> _entityModelRelationship;
std::unordered_map<std::string, mesh_texture> _meshTexturesCache;

// -----------------------------------------------------------------------------------------------
// INTERNAL STATELESS
// -----------------------------------------------------------------------------------------------
static bool ShaderHasCompilationErrors(unsigned int const program, shader_type const type) {
  int success{0};
  std::array<char, 512> log{'\0'};

  switch (type) {
  case shader_type::vertex:
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
      glGetShaderInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't compile vertex shader:" << log.data() << '\n';
      return true;
    }
    break;
  case shader_type::fragment:
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
      glGetShaderInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't compile fragment shader: " << log.data() << '\n';
      return true;
    }
    break;
  case shader_type::program:
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
      glGetProgramInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't link program: " << log.data() << '\n';
      return true;
    }
    break;
  }

  return false;
}

static unsigned int CompileAndLinkShaders(std::filesystem::path const& vertex,
                                          std::filesystem::path const& fragment) {
  std::ifstream vertfs(vertex);
  std::ifstream fragfs(fragment);

  if (!vertfs) {
    std::cerr << __FUNCTION__ << ": couldn't open vertex file: " << vertex << '\n';
    return false;
  }

  if (!fragfs) {
    std::cerr << __FUNCTION__ << ": couldn't open fragment file: " << fragment << '\n';
    return false;
  }

  std::stringstream vertss;
  std::stringstream fragss;
  vertss << vertfs.rdbuf();
  fragss << fragfs.rdbuf();

  std::string const vertcode{vertss.str()};
  std::string const fragcode{fragss.str()};

  char const* vertcodec{vertcode.c_str()};
  char const* fragcodec{fragcode.c_str()};

  unsigned int vertexShaderId{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertexShaderId, 1, &vertcodec, nullptr);
  glCompileShader(vertexShaderId);
  if (ShaderHasCompilationErrors(vertexShaderId, shader_type::vertex)) {
    glDeleteShader(vertexShaderId);
    return 0;
  }

  unsigned int fragmentShaderId{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragmentShaderId, 1, &fragcodec, nullptr);
  glCompileShader(fragmentShaderId);
  if (ShaderHasCompilationErrors(fragmentShaderId, shader_type::fragment)) {
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return 0;
  }

  unsigned int shaderProgram{glCreateProgram()};
  glAttachShader(shaderProgram, vertexShaderId);
  glAttachShader(shaderProgram, fragmentShaderId);
  glLinkProgram(shaderProgram);
  if (ShaderHasCompilationErrors(shaderProgram, shader_type::program)) {
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return 0;
  }

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  return shaderProgram;
}

// -----------------------------------------------------------------------------------------------
// PUBLIC
// -----------------------------------------------------------------------------------------------
void Initialise() {
  // ---------------------------------------------------------------------------------------------
  // shaders
  // ---------------------------------------------------------------------------------------------
  unsigned int id{CompileAndLinkShaders("./res/shaders/LevelEditor_ModelWithTextures.vert",
                                        "./res/shaders/LevelEditor_ModelWithTextures.frag")};

  assert(id != 0 && "couldn't create shader for models with textures in object editor");

  _shaders[kLevelEditorModelWithTextureShaderId] = std::make_unique<shader>(id);

  id = CompileAndLinkShaders("./res/shaders/LevelEditor_ModelWithoutTextures.vert",
                             "./res/shaders/LevelEditor_ModelWithoutTextures.frag");

  assert(id != 0 && "couldn't create shader for models without textures in object editor");

  _shaders[kLevelEditorModelWithoutTextureShaderId] = std::make_unique<shader>(id);

  id = CompileAndLinkShaders("./res/shaders/Primitive.vert", "./res/shaders/Primitive.frag");

  assert(id != 0 && "couldn't create primitive shader");

  _shaders[kPrimitiveShaderId] = std::make_unique<shader>(id);
}

bool LoadTextureFromFile(std::filesystem::path const& file, bool const flip, int const wrapS,
                         int const wrapT, int const id) {
  int width, height, channels;

  if (flip) {
    stbi_set_flip_vertically_on_load(true);
  }

  unsigned char* data{stbi_load(file.c_str(), &width, &height, &channels, 0)};

  if (data == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load image " << file << '\n';
    return false;
  }

  GLenum format;

  switch (channels) {
  case 1:
    format = GL_RED;
    break;
  case 2:
    format = GL_RG;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  // don't assume dimensions are multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  unsigned int glTexId;
  glGenTextures(1, &glTexId);
  glBindTexture(GL_TEXTURE_2D, glTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  if (flip) {
    stbi_set_flip_vertically_on_load(false);
  }

  _textures[id] = std::make_unique<texture>(glTexId, width, height, channels);

  return true;
}

unsigned int LoadTextureFromFile(std::filesystem::path const& file) {
  int width, height, channels;

  unsigned char* data{stbi_load(file.c_str(), &width, &height, &channels, 0)};

  if (data == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load image " << file << '\n';
    return 0;
  }

  GLenum format;

  switch (channels) {
  case 1:
    format = GL_RED;
    break;
  case 2:
    format = GL_RG;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  // don't assume dimensions are multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  unsigned int glTexId;
  glGenTextures(1, &glTexId);
  glBindTexture(GL_TEXTURE_2D, glTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return glTexId;
}

void AddEntityModelRelationship(entity_id const id, model_type const type) {
  _entityModelRelationship[id] = type;
}

void RemoveEntityModelRelationship(entity_id const id) {
  auto it = _entityModelRelationship.find(id);
  _entityModelRelationship.erase(it);
}

model const* GetModelDataFromEntity(entity_id const id) {
  return _models.at(_entityModelRelationship.at(id)).get();
}

shader const* GetShader(int const id) { return _shaders.at(id).get(); }

texture const* GetTexture(int const id) { return _textures.at(id).get(); }

std::pair<unsigned int, unsigned int> CreatePrimitiveVAO(std::vector<float> const& vertices,
                                                         GLenum const usage) {
  unsigned int vao, vbo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), usage);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));

  return std::make_pair<>(vao, vbo);
}

static std::vector<mesh_texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type,
                                                      std::string const& typeName, model* model) {
  std::vector<mesh_texture> textures;

  for (unsigned int i{0}; i < material->GetTextureCount(type); ++i) {
    aiString str;
    material->GetTexture(type, i, &str);

    auto it = _meshTexturesCache.find(str.C_Str());

    if (it != _meshTexturesCache.end()) {
      textures.push_back(it->second);
      continue;
    }

    std::filesystem::path filepath{model->_directory};
    filepath /= str.C_Str();

    unsigned int const textureId{resource_manager::LoadTextureFromFile(filepath)};

    if (textureId == 0) {
      std::cerr << __FUNCTION__ << ": couldn't load texture from file " << filepath << '\n';
      continue;
    }

    mesh_texture const newTexture{textureId, typeName, str.C_Str()};

    _meshTexturesCache[str.C_Str()] = newTexture;

    textures.push_back(newTexture);
  }

  return textures;
}

static mesh ProcessMesh(aiMesh* aiMesh, aiScene const* scene, model* model) {
  std::vector<vertex_data> vertices;
  std::vector<unsigned int> indices;
  std::vector<mesh_texture> textures;
  bool const meshHasTexCoords{aiMesh->mTextureCoords[0] != 0};

  for (unsigned int i{0}; i < aiMesh->mNumVertices; ++i) {
    glm::vec3 position, normal;
    glm::vec2 texCoords{0.f};

    position.x = aiMesh->mVertices[i].x;
    position.y = aiMesh->mVertices[i].y;
    position.z = aiMesh->mVertices[i].z;

    if (model->_boundingBox._min.x == FLT_MAX) {
      model->_boundingBox._min = position;
      model->_boundingBox._max = position;
    } else {
      model->_boundingBox._min = glm::min(position, model->_boundingBox._min);
      model->_boundingBox._max = glm::max(position, model->_boundingBox._max);
    }

    normal.x = aiMesh->mNormals[i].x;
    normal.y = aiMesh->mNormals[i].y;
    normal.z = aiMesh->mNormals[i].z;

    if (meshHasTexCoords) {
      texCoords.x = aiMesh->mTextureCoords[0][i].x;
      texCoords.y = aiMesh->mTextureCoords[0][i].y;
    }

    vertices.emplace_back(vertex_data{position, normal, texCoords});
  }

  for (unsigned int i{0}; i < aiMesh->mNumFaces; ++i) {
    aiFace face{aiMesh->mFaces[i]};

    for (unsigned int j{0}; j < face.mNumIndices; ++j) {
      indices.push_back(face.mIndices[j]);
    }
  }

  aiMaterial* material{scene->mMaterials[aiMesh->mMaterialIndex]};

  aiColor3D aiColour{1.f};
  material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColour);

  // TODO: add specular as well
  glm::vec3 diffuseColour{aiColour.r, aiColour.g, aiColour.b};

  std::vector<mesh_texture> diffuseMaps{
      LoadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse", model)};

  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<mesh_texture> specularMaps{
      LoadMaterialTextures(material, aiTextureType_SPECULAR, "textureSpecular", model)};

  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  return mesh{std::move(vertices), std::move(indices), std::move(textures), diffuseColour};
}

static void ProcessNode(aiNode* node, aiScene const* scene, model* model) {
  model->_boundingBox._min.x = FLT_MAX;

  for (unsigned int i{0}; i < node->mNumMeshes; ++i) {
    model->_meshes.emplace_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene, model));
  }

  for (unsigned int i{0}; i < node->mNumChildren; ++i) {
    ProcessNode(node->mChildren[i], scene, model);
  }
}

static std::filesystem::path GetPathFromModelType(model_type const type) {
  switch (type) {
  case model_type::ball:
    return std::filesystem::path("./res/models/ball.obj");
    break;
  case model_type::maze:
    return std::filesystem::path("./res/models/maze.obj");
    break;
  default:
    return "";
    break;
  }
}

void LoadModel(model_type const type) {
  if (_models.find(type) != _models.end()) {
    return;
  }

  auto newModel = std::make_unique<model>();

  std::filesystem::path path{GetPathFromModelType(type)};

  Assimp::Importer importer;

  aiScene const* scene{importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs)};

  if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      scene->mRootNode == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load model: " << importer.GetErrorString() << '\n';
    assert(false && "couldn't load model");
  }

  newModel->_directory = path.parent_path();

  ProcessNode(scene->mRootNode, scene, newModel.get());

  _models[type] = std::move(newModel);
}

}; // namespace resource_manager

//
// TODO: this goes into the entity manager
//
// entity_id resource_manager::SpawnEntityFromModelType(model_type const type) {
//   entity_id const entityId{GetNewEntityId()};

//   // cache
//   if (_models.find(type) == _models.end()) {
//     auto newModel = std::make_unique<model>();

//     if (!newModel->LoadModel(type)) {
//       std::clog << __FUNCTION__ << ": couldn't load model" << std::endl;
//       return 0;
//     }

//     _models[type] = std::move(newModel);
//   }

//   _entityModelRelationship[entityId] = type;

//   return entityId;
// }

}; // namespace lain
