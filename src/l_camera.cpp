#include "l_camera.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/geometric.hpp"

namespace lain
{
  void camera3D::ProcessKeyboard(glm::vec3 const& input)
  {
    _direction = glm::vec3(0.f);

    if (input.x == 1) {
      _direction += _right;
    } else if (input.x == -1) {
      _direction -= _right;
    }

    if (input.y == 1) {
      _direction += _up;
    } else if (input.y == -1) {
      _direction -= _up;
    }

    if (input.z == 1) {
      _direction += _front;
    } else if (input.z == -1) {
      _direction -= _front;
    }

    if (_direction != glm::vec3 (0.f)) {
      glm::normalize (_direction);
    }
  }

  void camera3D::ProcessCursor(glm::vec2 const& cursorCoords)
  {
    _yaw += glm::radians(cursorCoords.x * _sensitivity);
    _pitch += glm::radians(cursorCoords.y * _sensitivity);

    // Lazy way to avoid weird camera bugs
    if (_pitch < -1.f) {
      _pitch = -1.f;
    } else if (_pitch > 1.f) {
      _pitch = 1.f;
    }

    glm::vec3 front;
    front.x = std::cos(_yaw) * std::cos(_pitch);
    front.y = std::sin(_pitch);
    front.z = std::sin(_yaw) * std::cos(_pitch);

    _front = glm::normalize(front);
    _right = glm::normalize(glm::cross(front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
  }

  void camera3D::SetTargetPosition(f32 deltaTime)
  {
    if (_direction != glm::vec3 (0.f)) {
      _targetPosition = _position + (_direction * float(_speed) * float(deltaTime) );
    } else {
      _targetPosition = _position;
    }
  }

  glm::mat4 camera3D::GetViewMatrix() const
  {
    return glm::lookAt(_position, _position + _front, _up);
  }

  void camera3D::Update([[maybe_unused]]f32 deltaTime)
  {
    _position += (_targetPosition - _position) * float(_lerp);
  }
};
