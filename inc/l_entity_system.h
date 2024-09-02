#pragma once

#include "l_types.h"

#include <limits>

namespace lain
{
  using entity_id = u32;

  entity_id constexpr no_entity = std::numeric_limits<u32>::max();

  namespace entity_system
  {
    entity_id AddEntity();

    void RemoveEntity(entity_id id);

    void RemoveAllEntities();

    entity_id GetEntityCount();

    bool IsDestroyed(entity_id id);
  };
};
