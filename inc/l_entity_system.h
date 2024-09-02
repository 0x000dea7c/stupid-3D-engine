#pragma once

#include "l_types.h"

#include <climits>

namespace lain
{
  using entity_id = u32;

  entity_id constexpr no_entity = UINT32_MAX;

  namespace entity_system
  {
    entity_id AddEntity();

    void RemoveEntity(entity_id const id);

    void RemoveAllEntities();

    entity_id GetEntityCount();

    bool IsDestroyed(entity_id const id);
  };
};
