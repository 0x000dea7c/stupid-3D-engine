#include "l_transform_system.h"
#include "glm/ext/quaternion_float.hpp"
#include <vector>
#include <iostream>

namespace lain
{
  namespace transform_system
  {
    static std::vector<transform_component> _entities;

    void Update()
    {
      for (u32 i{0}; i < _entities.size(); ++i) {
	// Compute model matrix for each entity
	_entities[i]._model  = glm::mat4{1.f};
	_entities[i]._model  = glm::translate(_entities[i]._model, _entities[i]._position);
	_entities[i]._model *= glm::mat4_cast(_entities[i]._rotation);
	_entities[i]._model  = glm::scale(_entities[i]._model, _entities[i]._scale);
      }
    }

    void AddEntity(entity_id id, transform_component&& t)
    {
      if (id < _entities.size()) {
	_entities[id] = std::move(t);
      } else {
	_entities.emplace_back(t);
      }
    }

    void SetEntity(entity_id id, transform_component&& t)
    {
      _entities[id] = std::move(t);
    }

    transform_component GetTransform(entity_id id)
    {
      return _entities[id];
    }

    void RemoveAllEntities()
    {
      _entities.clear();
    }

    void RemoveEntity(entity_id id)
    {
      _entities.erase(_entities.begin() + id);
    }
  }
};
