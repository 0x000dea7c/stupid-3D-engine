#pragma once

#include <cstddef>

#include "glm/ext/vector_float2.hpp"

namespace lain {

namespace input_manager {

enum class key : std::size_t {
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
  count,
  unknown,
};

enum class mouse_button : std::size_t {
  left,

  count,
  unknown
};

void Initialise();

void BeginFrame();

void UpdateKey(key const k, bool const pressed);

void UpdateMouseButton(mouse_button const button, bool const clicked);

bool IsKeyPressed(key const k);

bool IsKeyHeld(key const k);

bool IsKeyReleased(key const k);

bool IsMouseButtonPressed(mouse_button const button);

bool IsCursorMoving();

void UpdateCursorPosition(glm::vec2 const& pos);

void UpdateCursorPosition2(glm::vec2 const& pos); // KLUDGE

void SetCursorIsMoving();

glm::vec2 GetCursorPosition();

}; // namespace input_manager

}; // namespace lain
