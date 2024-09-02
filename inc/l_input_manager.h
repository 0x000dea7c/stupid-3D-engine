#pragma once

#include <cstddef>

#include "glm/ext/vector_float2.hpp"
#include "l_types.h"

namespace lain
{
  namespace input_manager
  {
    enum class key : u32
      {
	w = 0,
	a,
	s,
	d,
	esc,
	up,
	down,
	left,
	right,
	f1,
	f2,
	i,
	j,
	k,
	l,
	q,
	e,
	b,
	count,
	unknown,
      };

    enum class mouse_button : u32
      {
	left,
	count,
	unknown
      };

    void Initialise();

    void BeginFrame();

    void UpdateKey(key k, bool pressed);

    void UpdateMouseButton(mouse_button button, bool clicked);

    bool IsKeyPressed(key k);

    bool IsKeyHeld(key k);

    bool IsKeyReleased(key k);

    bool IsMouseButtonPressed(mouse_button button);

    bool IsMouseButtonHeld(mouse_button button);

    bool IsMouseButtonReleased(mouse_button button);

    bool IsCursorMoving();

    void UpdateCursorPosition(glm::vec2 const& pos);

    void SetCursorIsMoving();

    glm::vec2 GetCursorPosition();
  };
};
