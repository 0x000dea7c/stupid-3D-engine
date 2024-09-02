#pragma once

#include <string>

#include "l_types.h"

namespace lain
{
  // -----------------------------------------------------------
  // Initialise every library you use: SDL2, OpenGL, ImGui, etc.
  // -----------------------------------------------------------
  namespace application
  {
    bool Initialise(bool const fullScreen);

    void ToggleFullScreen();

    void Shutdown();

    void Run();

    f32 GetWindowWidth();

    f32 GetWindowHeight();

    void SetWindowTitle(std::string&& newTitle);
  };
};
