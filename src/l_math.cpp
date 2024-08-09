#include "l_math.hpp"
#include "glm/common.hpp"
#include <iostream>

namespace lain {

bool RayIntersectsAABB(ray const& ray, aabb const& aabb) {
  // -------------------------------
  // TODO: understand the math!!!!!!
  // -------------------------------
  glm::vec3 tmin{(aabb._min - ray._position) / ray._direction};
  glm::vec3 tmax{(aabb._max - ray._position) / ray._direction};

  glm::vec3 t1{glm::min(tmin, tmax)};
  glm::vec3 t2{glm::max(tmin, tmax)};

  double tNear = std::fmax(t1.x, std::fmax(t1.y, t1.z));
  double tFar = std::fmin(t2.x, std::fmin(t2.y, t2.z));

  return tNear <= tFar && tFar >= 0.0;
}

}; // namespace lain
