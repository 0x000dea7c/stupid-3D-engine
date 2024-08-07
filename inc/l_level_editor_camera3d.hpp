#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "l_input_manager.hpp"

namespace lain {

class level_editor_camera3d final {
public:
  level_editor_camera3d(glm::vec3 const& center, glm::vec3 const& up, float const yaw,
                        float const pitch, input_manager& inputManager);

  glm::mat4 GetViewMatrix() const { return glm::lookAt(_center, _center + _front, _up); }

  void ProcessKeyboard(float const deltaTime);

  void ProcessCursor(glm::vec2 const& cursorCoords);

  void ProcessMouseScroll(float const yOffset);

  void UpdateCenter();

  glm::vec3 const& GetCenter() const { return _center; }

private:
  void UpdateCameraVectors();

  glm::mat4 _view;
  glm::vec3 _center;
  glm::vec3 _targetCenter; // for smooth movement
  glm::vec3 _front;
  glm::vec3 _up;
  glm::vec3 _right;
  glm::vec3 _worldUp;
  glm::vec3 _direction;

  input_manager& _inputManager;

  // in radians already!
  float _yaw;
  float _pitch;

  // options
  float _speed;
  float _sensitivity;
  float _lerpFactor;
};

}; // namespace lain
