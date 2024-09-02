#pragma once

#include "l_types.h"

namespace lain
{
  namespace game
  {
    void Initialise();

    bool IsShuttingDown();

    void ForceShutdown();

    void ProcessInput();

    void Update(f32 deltaTime);

    void Render();
  };
};
