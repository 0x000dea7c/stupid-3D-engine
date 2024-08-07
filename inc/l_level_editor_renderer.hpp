#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "l_entity.hpp"
#include "l_renderer.hpp"
#include <unordered_set>

namespace lain {

class resource_manager;
class event_manager;
class model;
class entity_component_system;

class level_editor_renderer : public renderer {
public:
  level_editor_renderer(float const width, float const height, resource_manager& resourceManager,
                        event_manager& eventManager, entity_component_system& ecs);

  void RenderEditMode(glm::mat4 const& cameraViewMatrix,
                      std::unordered_set<entity_id> const& entities,
                      entity_id const selectedEntity);

  void RenderMoveMode(glm::mat4 const& cameraViewMatrix, glm::vec3 const& cameraPosition,
                      std::unordered_set<entity_id> const& entities);

private:
  void RenderGridAndAxis(glm::mat4 const& cameraViewMatrix) const;

  void RenderEntities(glm::mat4 const& cameraViewMatrix,
                      std::unordered_set<entity_id> const& entities) const;

  shader const* _gridAxisLineShader;
  shader const* _gridAxisCheckerboardShader;
  shader const* _modelWithTexturesShader;
  shader const* _modelWithoutTexturesShader;
  event_manager& _eventManager;
  resource_manager& _resourceManager;
  entity_component_system& _ecs;
};

}; // namespace lain
