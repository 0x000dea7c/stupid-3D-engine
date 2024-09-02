#pragma once

#include "l_types.h"

namespace lain
{
  namespace level_editor
  {
    void Initialise();

    void ProcessInput();

    void Update(f32 const deltaTime);

    void Render();
  };
};
