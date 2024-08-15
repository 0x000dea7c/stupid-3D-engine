#pragma once

#include <string>

namespace lain {

// -----------------------------------------------------------
// Initialise every library you use: SDL2, OpenGL, ImGui, etc.
// -----------------------------------------------------------
namespace application {

bool Initialise(bool const fullScreen);

void ToggleFullScreen();

void Shutdown();

void Run();

float GetWindowWidth();

float GetWindowHeight();

void SetWindowTitle(std::string&& newTitle);

}; // namespace application

}; // namespace lain
