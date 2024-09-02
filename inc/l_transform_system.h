#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "l_entity_system.h"

namespace lain
{
  struct transform_component final
  {
    glm::mat4 _model;
    glm::quat _rotation;
    glm::vec3 _position;
    glm::vec3 _scale;
  };

  namespace transform_system
  {
    void Update();

    void AddEntity(entity_id id, transform_component&& t);

    void SetEntity(entity_id id, transform_component&& t);

    // OPTIMISE: maybe returning a copy is expensive.
    transform_component GetTransform(entity_id id);

    void RemoveAllEntities();

    void RemoveEntity(entity_id id);
  };
};
