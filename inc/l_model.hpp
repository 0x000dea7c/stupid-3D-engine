#pragma once

#include "l_mesh.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>

#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"

namespace lain {

enum class model_type : unsigned int { ball, maze };

class model final {
public:
  bool LoadModel(model_type const type);

  std::vector<mesh> const& GetMeshes() const { return _meshes; }

private:
  std::filesystem::path GetModelPath(model_type const type) const;

  void ProcessNode(aiNode* node, aiScene const* scene);

  mesh ProcessMesh(aiMesh* aiMesh, aiScene const* scene);

  std::vector<mesh_texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string const& typeName);

  std::vector<mesh> _meshes;
  std::unordered_map<std::string, mesh_texture> _texturesCache;
  std::string _directory;
};

}; // namespace lain
