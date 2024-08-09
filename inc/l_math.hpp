#pragma once

#include "glm/ext/vector_float3.hpp"

namespace lain {

// ------------------------------
// Geometry and math misc
// ------------------------------

struct ray final {
  glm::vec3 _position;
  glm::vec3 _direction;
};

struct aabb final {
  glm::vec3 _min;
  glm::vec3 _max;
};

bool RayIntersectsAABB(ray const& ray, aabb const& aabb);

}; // namespace lain
