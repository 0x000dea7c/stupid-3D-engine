#include "l_entity_system.h"
#include <vector>
#include <algorithm>

namespace lain
{
    namespace entity_system
    {
        struct entity final
        {
            entity_id _id;
            bool _destroyed; // This might come in handy but it's not used yet, it was an idea
        };

        static std::vector<entity> _entities;

        entity_id AddEntity()
        {
            entity_id id = _entities.size();
            _entities.emplace_back(id);
            return id;
        }

        void RemoveEntity(entity_id const id)
        {
            _entities.erase(_entities.begin() + id);
        }

        void RemoveAllEntities()
        {
            _entities.clear();
        }

        entity_id GetEntityCount()
        {
            return _entities.size();
        }
    };
};
