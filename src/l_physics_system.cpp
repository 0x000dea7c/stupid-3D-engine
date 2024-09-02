#include "l_physics_system.h"
#include "l_transform_system.h"
#include "glm/ext/vector_float3.hpp"

namespace lain
{
  namespace physics_system
  {
    static std::vector<physics_component> _entities;

    void Update()
    {
      for (u32 i{0}; i < _entities.size(); ++i) {
	// Get entity's model matrix.
	auto const model = transform_system::GetTransform(i)._model;

	for (u32 j{0}; j < _entities[i]._collisionShapeStart.size(); ++j) {
	  // Update collision shape (it's hardcoded to be an AABB)
	  glm::vec3 newMin{
	    glm::vec3(model * glm::vec4(_entities[i]._collisionShapeStart[j]._min, 1.f))
	  };

	  glm::vec3 newMax{
	    glm::vec3(model * glm::vec4(_entities[i]._collisionShapeStart[j]._max, 1.f))
	  };

	  _entities[i]._collisionShape[j]._min = newMin;
	  _entities[i]._collisionShape[j]._max = newMax;
	}
      }
    }

    void AddEntity(entity_id id, physics_component&& p)
    {
      if (id < _entities.size()) {
	_entities[id] = std::move(p);
      } else {
	_entities.emplace_back(p);
      }
    }

    void SetEntity(entity_id id, physics_component&& p)
    {
      _entities[id] = std::move(p);
    }

    void RemoveAllEntities()
    {
      _entities.clear();
    }

    void RemoveEntity(entity_id id)
    {
      _entities.erase(_entities.begin() + id);
    }

    void AddCollisionShapeForEntity(entity_id id, aabb shape)
    {
      _entities[id]._collisionShape.emplace_back(shape);
      _entities[id]._collisionShapeStart.emplace_back(shape);
    }

    std::vector<aabb> const& GetCollisionShapes(entity_id id)
    {
      return _entities[id]._collisionShape;
    }

    physics_component GetPhysicsComponent(entity_id id)
    {
      return _entities[id];
    }
  };
};
