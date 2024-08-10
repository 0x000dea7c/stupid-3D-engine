#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include "l_math.hpp"
#include <unordered_map>

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
  aabb _aabb;

  void Update(transform_component const& transform) {
    glm::vec4 newmin(_aabb._min, 0.f);
    glm::vec4 newmax(_aabb._max, 0.f);
    glm::mat4 matrix{transform.GetModel()};

    newmin = matrix * newmin;
    newmax = matrix * newmax;

    _aabb._min.x = newmin.x;
    _aabb._min.y = newmin.y;
    _aabb._min.z = newmin.z;

    _aabb._max.x = newmax.x;
    _aabb._max.y = newmax.y;
    _aabb._max.z = newmax.z;
  }
};

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

  aabb_component& GetAABBComponent(entity_id const id) { return _aabbs.at(id); }

  template <typename T> void AddAABBComponent(entity_id const id, T&& arg) {
    auto it = _aabbs.find(id);

    if (it != _aabbs.end()) {
      it->second = aabb_component(std::move(arg));
    } else {
      _aabbs.emplace(id, aabb_component(std::forward<T>(arg)));
    }
  }

  std::unordered_map<entity_id, transform_component> _transforms;
  std::unordered_map<entity_id, aabb_component> _aabbs;
};

}; // namespace ecs

}; // namespace lain
