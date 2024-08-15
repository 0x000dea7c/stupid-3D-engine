#pragma once

#include "glad/glad.h"
#include "l_entity.hpp"
#include "l_model.hpp"
#include <cassert>
#include <filesystem>
#include <vector>

namespace lain {

class shader;
class texture;

namespace resource_manager {

// ---------------------------------------------------------------------------
// Loads and compiles every shader needed by the game, loads audio files, etc.
// ---------------------------------------------------------------------------
void Initialise();

bool LoadTextureFromFile(std::filesystem::path const& file, bool const flip, int const wrapS,
                         int const wrapT, int const id);

unsigned int LoadTextureFromFile(std::filesystem::path const& file);

shader const* GetShader(int const id);

texture const* GetTexture(int const id);

// TODO: don't know about these, maybe they just don't belong here
void AddEntityModelRelationship(entity_id const id, model_type const type);

model const* GetModelDataFromEntity(entity_id const id);

void RemoveEntityModelRelationship(entity_id const id);

std::pair<unsigned int, unsigned int> CreatePrimitiveVAO(std::vector<float> const& vertices,
                                                         GLenum const usage);

shader CreateCubeVAO(std::vector<float> const& vertices, GLenum const usage,
                     std::vector<unsigned int> const& indices);

void LoadModel(model_type const type);

}; // namespace resource_manager

}; // namespace lain
