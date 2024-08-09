#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

namespace lain {

struct camera3D final {
  glm::vec3 _position;
  glm::vec3 _targetPosition;
  glm::vec3 _front;
  glm::vec3 _up;
  glm::vec3 _right;
  glm::vec3 _worldUp;
  glm::vec3 _direction;
  float _yaw;   // in radians
  float _pitch; // in radians
  float _speed;
  float _sensitivity;
  float _lerp;

  void ProcessKeyboard(glm::vec3 const& input);

  void ProcessCursor(glm::vec2 const& cursorCoords);

  void SetTargetPosition(float const deltaTime);

  glm::mat4 GetViewMatrix() const;

  void Update(float const deltaTime);
};

}; // namespace lain
