#include "l_level_editor_camera3d.hpp"
#include <iostream>

namespace lain {

level_editor_camera3d::level_editor_camera3d(glm::vec3 const& center, glm::vec3 const& up,
                                             float const yaw, float const pitch,
                                             input_manager& inputManager)
    : _center{center},
      _targetCenter{center},
      _worldUp{up},
      _direction{glm::vec3(0.f)},
      _inputManager{inputManager},
      _yaw{yaw},
      _pitch{pitch},
      _speed{5.f},
      _sensitivity{0.1f},
      _lerpFactor{0.5f} {
  UpdateCameraVectors();
}

void level_editor_camera3d::UpdateCameraVectors() {
  glm::vec3 front;
  front.x = std::cos(_yaw) * std::cos(_pitch);
  front.y = std::sin(_pitch);
  front.z = std::sin(_yaw) * std::cos(_pitch);

  _front = glm::normalize(front);
  _right = glm::normalize(glm::cross(front, _worldUp));
  _up = glm::normalize(glm::cross(_right, _front));
}

void level_editor_camera3d::ProcessKeyboard(float const deltaTime) {
  glm::vec3 direction{0.f}; // support diagonal movement

  if (_inputManager.KeyHeld(key::w)) {
    direction += _front;
  }

  if (_inputManager.KeyHeld(key::s)) {
    direction -= _front;
  }

  if (_inputManager.KeyHeld(key::a)) {
    direction -= _right;
  }

  if (_inputManager.KeyHeld(key::d)) {
    direction += _right;
  }

  if (_inputManager.KeyHeld(key::q)) {
    direction += _up;
  }

  if (_inputManager.KeyHeld(key::e)) {
    direction -= _up;
  }

  if (direction != glm::vec3(0.f)) {
    glm::normalize(direction);
  }

  _targetCenter = _center + (direction * _speed * deltaTime);
}

void level_editor_camera3d::ProcessCursor(glm::vec2 const& cursorCoords) {
  _yaw += glm::radians(cursorCoords.x * _sensitivity);
  _pitch += glm::radians(cursorCoords.y * _sensitivity);

  // Lazy way to avoid weird camera bugs
  if (_pitch < -1.f) {
    _pitch = -1.f;
  } else if (_pitch > 1.f) {
    _pitch = 1.f;
  }

  UpdateCameraVectors();
}

void level_editor_camera3d::ProcessMouseScroll(float const /* yOffset */) {}

void level_editor_camera3d::UpdateCenter() { _center += (_targetCenter - _center) * _lerpFactor; }

}; // namespace lain
