#pragma once

#include "glad/glad.h"
#include "l_entity.hpp"
#include "l_model.hpp"
#include "l_shader.hpp"
#include "l_texture.hpp"
#include <cassert>
#include <filesystem>
#include <memory>
#include <unordered_map>

namespace lain {

class level_editor;
class mesh;

class resource_manager final {
public:
  resource_manager() = default;

  ~resource_manager() = default;

  bool Initialise(level_editor& levelEditor);

  bool CreateGridAxisLineShader(std::filesystem::path const& vertex,
                                std::filesystem::path const& fragment, int const id);

  bool CreateGridCheckerboardShader(std::filesystem::path const& vertex,
                                    std::filesystem::path const& fragment, int const id,
                                    level_editor& levelEditor);

  bool CreateCursorShader(std::filesystem::path const& vertex,
                          std::filesystem::path const& fragment, int const id);

  bool LoadTextureFromFile(std::filesystem::path const& file, bool const flip, int const wrapS,
                           int const wrapT, int const id);

  shader const* GetShader(int const id) const { return _shaders.at(id).get(); }

  texture const* GetTexture(int const id) const { return _textures.at(id).get(); }

  entity_id SpawnEntityFromModelType(model_type const type);

  model const* GetModelFromEntity(entity_id const id) const;

  void RemoveEntityModelRelationship(entity_id const id) {
    auto it = _entityModelRelationship.find(id);
    _entityModelRelationship.erase(it);
  }

  // -------------------------------------------------------------------------------------------------------
  // stateless
  // -------------------------------------------------------------------------------------------------------
  static unsigned int LoadTextureFromFile(std::filesystem::path const& file);

  static unsigned int CompileAndLinkShaders(std::filesystem::path const& vertex,
                                            std::filesystem::path const& fragment);

private:
  std::unordered_map<int, std::unique_ptr<shader>> _shaders;
  std::unordered_map<int, std::unique_ptr<texture>> _textures;
  std::unordered_map<model_type, std::unique_ptr<model>> _models;
  std::unordered_map<entity_id, model_type> _entityModelRelationship;
};

}; // namespace lain
