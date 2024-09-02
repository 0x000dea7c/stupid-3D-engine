#pragma once

#include "l_math.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "l_entity_system.h"
#include <vector>

namespace lain
{
  struct physics_component final
  {
    std::vector<aabb> _collisionShape; // Where it is now.
    std::vector<aabb> _collisionShapeStart; // Where it was when it was added.
  };

  namespace physics_system
  {
    void Update();

    void AddEntity(entity_id const id, physics_component&& p);

    void SetEntity(entity_id const id, physics_component&& p);

    void RemoveAllEntities();

    void RemoveEntity(entity_id const id);

    void AddCollisionShapeForEntity(entity_id const id, aabb shape);

    std::vector<aabb> const& GetCollisionShapes(entity_id const id);
  };
};
