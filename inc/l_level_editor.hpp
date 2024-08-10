#pragma once

namespace lain {

namespace ecs {
struct entity_component_system;
};

namespace level_editor {

void Initialise(ecs::entity_component_system* ecs);

void ProcessInput();

void Update(float const deltaTime);

void Render();

}; // namespace level_editor

}; // namespace lain
