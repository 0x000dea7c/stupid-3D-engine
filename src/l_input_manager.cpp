#include "l_input_manager.hpp"

namespace lain {

namespace input_manager {

static bool _currentKeys[static_cast<std::size_t>(key::count)];
static bool _previousKeys[static_cast<std::size_t>(key::count)];
static bool _previousMouseButtons[static_cast<std::size_t>(mouse_button::count)];
static bool _currentMouseButtons[static_cast<std::size_t>(mouse_button::count)];
static glm::vec2 _cursorCoords;
static glm::vec2 _cursorCoordsScreenSpace;
static bool _cursorIsMoving;

void Initialise() {
  for (std::size_t i{0}; i < static_cast<std::size_t>(key::count); ++i) {
    _currentKeys[i] = false;
    _previousKeys[i] = false;
  }

  for (std::size_t i{0}; i < static_cast<std::size_t>(mouse_button::count); ++i) {
    _previousMouseButtons[i] = false;
    _currentMouseButtons[i] = false;
  }
}

void BeginFrame() {
  for (std::size_t i{0}; i < static_cast<std::size_t>(key::count); ++i) {
    _previousKeys[i] = _currentKeys[i];
  }

  for (std::size_t i{0}; i < static_cast<std::size_t>(mouse_button::count); ++i) {
    _previousMouseButtons[i] = _currentMouseButtons[i];
  }

  _cursorIsMoving = false;
}

void UpdateKey(key const k, bool const pressed) {
  _currentKeys[static_cast<std::size_t>(k)] = pressed;
}

void UpdateMouseButton(mouse_button const button, bool const clicked) {
  _currentMouseButtons[static_cast<std::size_t>(button)] = clicked;
}

bool KeyPressed(key const k) {
  return !_previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
}

bool KeyHeld(key const k) {
  return _previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
}

bool KeyReleased(key const k) {
  return _previousKeys[static_cast<unsigned int>(k)] && !_currentKeys[static_cast<std::size_t>(k)];
}

bool MouseButtonIsPressed(mouse_button const button) {
  return !_previousMouseButtons[static_cast<std::size_t>(button)] &&
         _currentMouseButtons[static_cast<std::size_t>(button)];
}

void UpdateCursorPosition(glm::vec2 const& pos) { _cursorCoords = pos; }

void UpdateCursorPosition2(glm::vec2 const& pos) { _cursorCoordsScreenSpace = pos; }

void SetCursorIsMoving() { _cursorIsMoving = true; }

bool IsKeyPressed(key const k) {
  return !_previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
}

bool IsKeyHeld(key const k) {
  return _previousKeys[static_cast<std::size_t>(k)] && _currentKeys[static_cast<std::size_t>(k)];
}

bool IsKeyReleased(key const k) {
  return _previousKeys[static_cast<std::size_t>(k)] && !_currentKeys[static_cast<std::size_t>(k)];
}

bool IsMouseButtonPressed(mouse_button const button) {
  return _previousMouseButtons[static_cast<std::size_t>(button)] &&
         !_currentMouseButtons[static_cast<std::size_t>(button)];
}

bool IsCursorMoving() { return _cursorIsMoving; }

glm::vec2 GetCursorPosition() { return _cursorCoords; }

}; // namespace input_manager

}; // namespace lain
