#include "l_model.hpp"
#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/types.h"
#include "l_mesh.hpp"
#include "l_resource_manager.hpp"
#include <iostream>

namespace lain {

// ---------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------
std::filesystem::path model::GetModelPath(model_type const type) const {
  switch (type) {
  case model_type::ball:
    return "./res/models/ball.obj";
  case model_type::maze:
    return "./res/models/maze.obj";
  default:
    return "";
  }
}

bool model::LoadModel(model_type const type) {
  std::filesystem::path const& file{GetModelPath(type)};

  Assimp::Importer importer;

  aiScene const* scene{importer.ReadFile(file.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs)};

  if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load model: " << importer.GetErrorString() << '\n';
    return false;
  }

  _directory = file.parent_path();

  ProcessNode(scene->mRootNode, scene);

  return true;
}

void model::ProcessNode(aiNode* node, aiScene const* scene) {
  for (unsigned int i{0}; i < node->mNumMeshes; ++i) {
    _meshes.emplace_back(ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene));
  }

  for (unsigned int i{0}; i < node->mNumChildren; ++i) {
    ProcessNode(node->mChildren[i], scene);
  }
}

mesh model::ProcessMesh(aiMesh* aiMesh, aiScene const* scene) {
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

  std::vector<mesh_texture> diffuseMaps{LoadMaterialTextures(material, aiTextureType_DIFFUSE, "textureDiffuse")};
  textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

  std::vector<mesh_texture> specularMaps{LoadMaterialTextures(material, aiTextureType_SPECULAR, "textureSpecular")};
  textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

  return mesh{std::move(vertices), std::move(indices), std::move(textures), diffuseColour};
}

std::vector<mesh_texture> model::LoadMaterialTextures(aiMaterial* material, aiTextureType type,
                                                      std::string const& typeName) {
  std::vector<mesh_texture> textures;

  for (unsigned int i{0}; i < material->GetTextureCount(type); ++i) {
    aiString str;
    material->GetTexture(type, i, &str);

    auto it = _texturesCache.find(str.C_Str());

    if (it != _texturesCache.end()) {
      textures.push_back(it->second);
      continue;
    }

    std::filesystem::path filepath{_directory};
    filepath /= str.C_Str();

    unsigned int const textureId{resource_manager::LoadTextureFromFile(filepath)};

    if (textureId == 0) {
      std::cerr << __FUNCTION__ << ": couldn't load texture from file " << filepath << '\n';
      continue;
    }

    mesh_texture const newTexture{textureId, typeName, str.C_Str()};

    _texturesCache[str.C_Str()] = newTexture;

    textures.push_back(newTexture);
  }

  return textures;
}

}; // namespace lain
