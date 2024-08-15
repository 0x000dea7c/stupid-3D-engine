#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "l_math.hpp"
#include <cfloat>
#include <unordered_map>
#include <vector>

namespace lain {

using entity_id = unsigned int;

struct transform_component final {
  glm::quat _rotation; // avoid gimbal lock
  glm::vec3 _position;
  glm::vec3 _scale;

  glm::mat4 GetModel() const {
    glm::mat4 model{1.f};
    model = glm::translate(model, _position);
    model *= glm::mat4_cast(_rotation);
    model = glm::scale(model, _scale);
    return model;
  }
};

// NOTE: don't know if every entity in the game will need an AABB, probably
// yes but since you don't know it's preferable to have it as a separate one.

struct aabb_component final {
  aabb_component() = default;

  aabb_component(aabb aabb)
      : _aabb{aabb},
        _initial{_aabb} {}

  aabb _aabb;
  aabb _initial;
};

// FIXME: shame shame shame
inline void ECS_Update(transform_component const& transform, aabb_component& aabb) {
  glm::mat4 matrix{transform.GetModel()};

  glm::vec3 aabbCorners[] = {{aabb._initial._min.x, aabb._initial._min.y, aabb._initial._min.z},
                             {aabb._initial._min.x, aabb._initial._min.y, aabb._initial._max.z},
                             {aabb._initial._min.x, aabb._initial._max.y, aabb._initial._min.z},
                             {aabb._initial._min.x, aabb._initial._max.y, aabb._initial._max.z},
                             {aabb._initial._max.x, aabb._initial._min.y, aabb._initial._min.z},
                             {aabb._initial._max.x, aabb._initial._min.y, aabb._initial._max.z},
                             {aabb._initial._max.x, aabb._initial._max.y, aabb._initial._min.z},
                             {aabb._initial._max.x, aabb._initial._max.y, aabb._initial._max.z}};

  glm::vec3 newMin(FLT_MAX);
  glm::vec3 newMax(-FLT_MAX);

  for (auto const& corner : aabbCorners) {
    auto transformedCorner = matrix * glm::vec4(corner, 1.f);
    newMin = glm::min(glm::vec3(transformedCorner), newMin);
    newMax = glm::max(glm::vec3(transformedCorner), newMax);
  }

  aabb._aabb._min = newMin;
  aabb._aabb._max = newMax;
}

namespace ecs {

inline entity_id GetNewEntityId() {
  static entity_id currentId{1};
  return currentId++;
}

struct entity_component_system final {
  // NOTE: need to use a class if you want to optimise AddTransformComponent using templates
  template <typename T> void AddTransformComponent(entity_id const id, T&& transform) {
    auto it = _transforms.find(id);

    if (it != _transforms.end()) {
      it->second = std::move(transform);
    } else {
      _transforms.emplace(id, std::forward<T>(transform));
    }
  }

  transform_component const& GetTransformComponent(entity_id const id) {
    return _transforms.at(id);
  }

  std::vector<aabb_component>& GetAABBSComponents(entity_id const id) { return _aabbs.at(id); }

  // @TODO: fixme asapppppppppppppppppppppppppp
  void AddAABBComponent(entity_id const id, aabb aabb) {
    if (_aabbs.find(id) != _aabbs.end()) {
      _aabbs[id].push_back(aabb);
    } else {
      _aabbs[id].emplace_back(aabb_component(aabb));
    }
  }

  // @TODO: fixme asapppppppppppppppppppppppppp
  void AddAABBComponent(entity_id const id, aabb_component aabb) {
    if (_aabbs.find(id) != _aabbs.end()) {
      _aabbs[id].push_back(aabb);
    } else {
      _aabbs[id].emplace_back(aabb);
    }
  }

  void ResetAABBs(entity_id const id) { _aabbs[id].clear(); }

  void Clear() {
    _transforms.clear();
    _aabbs.clear();
  }

  std::unordered_map<entity_id, transform_component> _transforms;
  std::unordered_map<entity_id, std::vector<aabb_component>> _aabbs;
};

}; // namespace ecs

}; // namespace lain
