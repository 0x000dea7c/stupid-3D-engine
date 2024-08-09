#pragma once

namespace lain {

// -----------------------------------------------------------
// Initialise every library you use: SDL2, OpenGL, ImGui, etc.
// -----------------------------------------------------------
namespace application {

bool Initialise(int const width, int const height, bool const fullScreen);

void ToggleFullScreen();

void Shutdown();

void Run();

}; // namespace application

}; // namespace lain
