#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/gtc/quaternion.hpp"
#include <unordered_map>

namespace lain {

using entity_id = unsigned int;

// dumb, but useful for now
inline entity_id GetNewEntityId() {
  static entity_id currentId{1};
  return currentId++;
}

struct transform_component final {
  glm::quat _rotation; // avoid gimbal lock
  glm::vec3 _position;
  glm::vec3 _scale;

  transform_component(glm::quat const& rotation, glm::vec3 const& position, glm::vec3 const& scale)
      : _rotation{rotation},
        _position{position},
        _scale{scale} {}

  transform_component(transform_component&& other)
      : _rotation{std::move(other._rotation)},
        _position{std::move(other._position)},
        _scale{std::move(other._scale)} {}

  transform_component& operator=(transform_component const& other) {
    if (this != &other) {
      _rotation = other._rotation;
      _position = other._position;
      _scale = other._scale;
    }
    return *this;
  }

  glm::mat4 GetModel() const {
    glm::mat4 model{1.f};
    model = glm::translate(model, _position);
    model *= glm::mat4_cast(_rotation);
    model = glm::scale(model, _scale);
    return model;
  }
};

// My stupid approach to ECS
class entity_component_system final {
public:
  // -------------------------------------------------------------------------------
  // This template + universal reference is to try use move semantics!!!!!!
  // -------------------------------------------------------------------------------
  template <typename T> void AddTransformComponent(entity_id const id, T&& transform) {
    auto it = _transforms.find(id);

    if (it != _transforms.end()) {
      it->second = std::move(transform);
    } else {
      _transforms.emplace(id, std::forward<T>(transform));
    }
  }

  transform_component const& GetTransformComponent(entity_id const id) const {
    return _transforms.at(id);
  }

private:
  std::unordered_map<entity_id, transform_component> _transforms;
};

}; // namespace lain
