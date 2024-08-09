#include "l_resource_manager.hpp"
#include "l_common.hpp"
#include "l_entity.hpp"
#include "l_model.hpp"
#include "l_shader.hpp"
#include "l_texture.hpp"
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace lain {

namespace resource_manager {

std::unordered_map<int, std::unique_ptr<shader>> _shaders;
std::unordered_map<int, std::unique_ptr<texture>> _textures;
std::unordered_map<model_type, std::unique_ptr<model>> _models;
std::unordered_map<entity_id, model_type> _entityModelRelationship;

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

model const* GetModelDataFromEntity(entity_id const id) {
  return _models.at(_entityModelRelationship.at(id)).get();
}

shader const* GetShader(int const id) { return _shaders.at(id).get(); }

texture const* GetTexture(int const id) { return _textures.at(id).get(); }

void RemoveEntityModelRelationship(entity_id const id) {
  auto it = _entityModelRelationship.find(id);
  _entityModelRelationship.erase(it);
}

unsigned int CreatePrimitiveVAO(std::vector<float> const& vertices) {
  unsigned int vao, vbo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, reinterpret_cast<void*>(0));

  return vao;
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
