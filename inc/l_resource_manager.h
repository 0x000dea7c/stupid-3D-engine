#pragma once

#include "glad/glad.h"
#include "l_entity_system.h"
#include "l_model.h"
#include "l_types.h"

#include <filesystem>
#include <vector>

namespace lain
{
  class shader;
  class texture;

  namespace resource_manager
  {
    // ---------------------------------------------------------------------------
    // Loads and compiles every shader needed by the game, loads audio files, etc.
    // ---------------------------------------------------------------------------
    void Initialise();

    bool LoadTextureFromFile(std::filesystem::path const& file,
			     bool const flip,
			     i32 const wrapS,
			     i32 const wrapT,
			     i32 const id);

    u32 LoadTextureFromFile(std::filesystem::path const& file);

    shader const* GetShader(i32 const id);

    texture const* GetTexture(i32 const id);

    // TODO: don't know about these, maybe they just don't belong here
    void AddEntityModelRelationship(entity_id const id, model_type const type);

    model const* GetModelDataFromEntity(entity_id const id);

    void RemoveEntityModelRelationship(entity_id const id);

    shader CreatePrimitiveVAO(std::vector<f32> const& vertices, GLenum const usage);

    shader CreateCubeVAO(std::vector<f32> const& vertices,
			 GLenum const usage,
			 std::vector<u32> const& indices);

    void LoadModel(model_type const type);

    model_type GetModelType(entity_id const id);
  };
};
