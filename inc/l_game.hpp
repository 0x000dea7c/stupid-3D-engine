#pragma once

namespace lain {

namespace ecs {
struct entity_component_system;
};

namespace game {

void Initialise(ecs::entity_component_system* ecs);

// -----------------------------------
// You use in game menu to shut down
// -----------------------------------
bool IsShuttingDown();

// -------------------------------------------------
// You use Ctrl+C, press X button in the window, etc
// --------------------------------------------------
void ForceShutdown();

void ProcessInput();

void Update(float const deltaTime);

void Render();

}; // namespace game

}; // namespace lain
