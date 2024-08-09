#pragma once

#include "glad/glad.h"
#include "l_entity.hpp"
#include <cassert>
#include <filesystem>
#include <vector>

namespace lain {

class model;
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

model const* GetModelDataFromEntity(entity_id const id);

void RemoveEntityModelRelationship(entity_id const id);

unsigned int CreatePrimitiveVAO(std::vector<float> const& vertices);

}; // namespace resource_manager

}; // namespace lain
