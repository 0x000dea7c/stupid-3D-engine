#pragma once

#include <climits>

namespace lain
{
    using entity_id = unsigned int;

    entity_id constexpr no_entity = UINT_MAX;

    namespace entity_system
    {
        entity_id AddEntity();

        void RemoveEntity(entity_id const id);

        void RemoveAllEntities();

        entity_id GetEntityCount();
    };
};
