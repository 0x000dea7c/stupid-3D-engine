#include "l_math.h"
#include <cfloat>

namespace lain
{
  bool RayIntersectsAABB(ray const& ray, aabb const& aabb)
  {
    // OPTIMISE: there's a lot of ways to improve this.
    f32 tmin = -FLT_MAX;
    f32 tmax = FLT_MAX;

    if (ray._direction.x != 0.f) {
      f32 tx1 = (aabb._min.x - ray._position.x) / ray._direction.x;
      f32 tx2 = (aabb._max.x - ray._position.x) / ray._direction.x;
      tmin = glm::max(tmin, glm::min(tx1, tx2));
      tmax = glm::min(tmax, glm::max(tx1, tx2));
    } else if (ray._position.x < aabb._min.x || ray._position.x > aabb._max.x) {
      return false;
    }

    if (ray._direction.y != 0.f) {
      f32 ty1 = (aabb._min.y - ray._position.y) / ray._direction.y;
      f32 ty2 = (aabb._max.y - ray._position.y) / ray._direction.y;
      tmin = glm::max(tmin, glm::min(ty1, ty2));
      tmax = glm::min(tmax, glm::max(ty1, ty2));
    } else if (ray._position.y < aabb._min.y || ray._position.y > aabb._max.y) {
      return false;
    }

    if (ray._direction.z != 0.f) {
      f32 tz1 = (aabb._min.z - ray._position.z) / ray._direction.z;
      f32 tz2 = (aabb._max.z - ray._position.z) / ray._direction.z;
      tmin = glm::max(tmin, glm::min(tz1, tz2));
      tmax = glm::min(tmax, glm::max(tz1, tz2));
    } else if (ray._position.z < aabb._min.z || ray._position.z > aabb._max.z) {
      return false;
    }

    return tmax >= glm::max(0.f, float(tmin));
  }

  glm::vec4 ScreenSpaceToNormalisedDeviceCoordinates(glm::vec4 const& pos, f32 width, f32 height)
  {
    return glm::vec4{(pos.x * 2.f) / width - 1.f, 1.f - (pos.y * 2.f) / height, 0.f, 0.f};
  }

  glm::vec4 NormalisedDeviceCoordinatesToClipSpace(glm::vec4 const& pos)
  {
    glm::vec4 newPos{pos};

    newPos.z = -1.f; /* point forward, in OpenGL means z = -1 */
    newPos.w = 1.f;

    return newPos;
  }

  glm::vec4 ClipSpaceToViewSpace(glm::vec4 const& pos, glm::mat4 const& projection)
  {
    glm::mat4 const inverse{glm::inverse(projection)};
    glm::vec4 viewSpacePos{inverse * pos};

    viewSpacePos /= viewSpacePos.w;

    return viewSpacePos;
  }

  glm::vec4 ViewSpaceToWorldSpace(glm::vec4 const& pos, glm::mat4 const& view)
  {
    glm::mat4 const inverse{glm::inverse(view)};
    glm::vec4 const worldSpace{inverse * pos};

    return worldSpace;
  }
};
