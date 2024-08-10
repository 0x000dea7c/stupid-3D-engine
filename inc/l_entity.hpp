#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
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

  std::unordered_map<entity_id, transform_component> _transforms;
};

}; // namespace ecs

}; // namespace lain
