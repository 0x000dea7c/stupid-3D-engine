#include "l_math.hpp"
#include "glm/common.hpp"

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

glm::vec4 ScreenSpaceToNormalisedDeviceCoordinates(glm::vec4 const& pos, float const width,
                                                   float const height) {
  return glm::vec4{(pos.x * 2.f) / width - 1.f, 1.f - (pos.y * 2.f) / height, 0.f, 0.f};
}

glm::vec4 NormalisedDeviceCoordinatesToClipSpace(glm::vec4 const& pos) {
  glm::vec4 newPos{pos};

  newPos.z = -1.f; /* point forward, in OpenGL means z = -1 */
  newPos.w = 1.f;

  return newPos;
}

glm::vec4 ClipSpaceToViewSpace(glm::vec4 const& pos, glm::mat4 const& projection) {
  glm::mat4 const inverse{glm::inverse(projection)};
  glm::vec4 viewSpacePos{inverse * pos};

  viewSpacePos /= viewSpacePos.w;

  return viewSpacePos;
}

glm::vec4 ViewSpaceToWorldSpace(glm::vec4 const& pos, glm::mat4 const& view) {
  glm::mat4 const inverse{glm::inverse(view)};
  glm::vec4 const worldSpace{inverse * pos};

  return worldSpace;
}

}; // namespace lain
