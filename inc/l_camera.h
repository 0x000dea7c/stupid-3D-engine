#pragma once

#include "l_types.h"

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

namespace lain
{

  struct camera3D final
  {
    glm::vec3 _position;
    glm::vec3 _targetPosition;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;
    glm::vec3 _worldUp;
    glm::vec3 _direction;
    f32 _yaw;   // in radians
    f32 _pitch; // in radians
    f32 _speed;
    f32 _sensitivity;
    f32 _lerp;

    void ProcessKeyboard(glm::vec3 const& input);

    void ProcessCursor(glm::vec2 const& cursorCoords);

    void SetTargetPosition(f32 deltaTime);

    glm::mat4 GetViewMatrix() const;

    void Update(f32 deltaTime);
  };

};
