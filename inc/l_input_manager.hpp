#pragma once

#include <cstddef>

#include "glm/ext/vector_float2.hpp"

namespace lain {

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

struct input_manager final {
  input_manager() {
    for (std::size_t i{0}; i < static_cast<std::size_t>(key::count); ++i) {
      _currentKeys[i] = false;
      _previousKeys[i] = false;
    }
  }

  bool KeyPressed(key const k) const {
    return !_previousKeys[static_cast<unsigned int>(k)] &&
           _currentKeys[static_cast<std::size_t>(k)];
  }

  bool KeyHeld(key const k) const {
    return _previousKeys[static_cast<unsigned int>(k)] && _currentKeys[static_cast<std::size_t>(k)];
  }

  bool KeyReleased(key const k) const {
    return _previousKeys[static_cast<unsigned int>(k)] &&
           !_currentKeys[static_cast<std::size_t>(k)];
  }

  void BeginFrame() {
    for (unsigned int i{0}; i < static_cast<unsigned int>(key::count); ++i) {
      _previousKeys[i] = _currentKeys[i];
    }

    _cursorIsMoving = false;
  }

  // update key state
  void UpdateKey(key const k, bool const pressed) noexcept {
    _currentKeys[static_cast<std::size_t>(k)] = pressed;
  }

  glm::vec2 GetCursorCoords() const { return _cursorCoords; }

  bool _currentKeys[static_cast<std::size_t>(key::count)];
  bool _previousKeys[static_cast<std::size_t>(key::count)];
  glm::vec2 _cursorCoords;
  bool _cursorIsMoving;
};

}; // namespace lain
