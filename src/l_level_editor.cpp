#include "l_level_editor.hpp"
#include "l_entity.hpp"
#include "l_event_manager.hpp"
#include "l_input_manager.hpp"
#include "l_level_editor_camera3d.hpp"
#include "l_level_editor_renderer.hpp"
#include "l_resource_manager.hpp"
#include <iostream>

namespace lain {

level_editor::level_editor(level_editor_camera3d& camera, input_manager& inputManager,
                           event_manager& eventManager, entity_component_system& ecs,
                           resource_manager& resourceManager)
    : _renderer{nullptr},
      _camera{camera},
      _inputManager{inputManager},
      _eventManager{eventManager},
      _ecs{ecs},
      _resourceManager{resourceManager},
      _gridSquareSize{0.5f},
      _halfGridExtent{20.f},
      _selectedEntity{0} {
  _mode = level_editor_mode::move;

  _eventManager.Subscribe(event_type::level_editor_spawn_maze,
                          [this](event const& /* event*/) { SpawnMazeModel(); });
  _eventManager.Subscribe(event_type::level_editor_spawn_ball,
                          [this](event const& /* event*/) { SpawnBallModel(); });
  _eventManager.Subscribe(event_type::level_editor_clear,
                          [this](event const& /* event*/) { ClearModels(); });
}

void level_editor::ProcessInput(float const deltaTime) {
  switch (_mode) {
  case level_editor_mode::edit:
    ProcessKeyboardInEditMode();
    break;
  case level_editor_mode::move:
    ProcessCursorInMoveMode();
    ProcessKeyboardInMoveMode(deltaTime);
    break;
  }
}

void level_editor::Update(float const /* deltaTime */) {
  switch (_mode) {
  case level_editor_mode::edit:
    break;
  case level_editor_mode::move:
    _camera.UpdateCenter();
    break;
  }
}

void level_editor::Render() {
  switch (_mode) {
  case level_editor_mode::edit:
    _renderer->RenderEditMode(_camera.GetViewMatrix(), _entities, _selectedEntity);
    break;
  case level_editor_mode::move:
    _renderer->RenderMoveMode(_camera.GetViewMatrix(), _camera.GetCenter(), _entities);
    break;
  }
}

void level_editor::SetRenderer(level_editor_renderer* renderer) { _renderer = renderer; }

void level_editor::ProcessKeyboardInMoveMode(float const deltaTime) {
  if (_inputManager.KeyPressed(key::f1)) {
    _mode = level_editor_mode::edit;
  } else if (_inputManager.KeyPressed(key::f2)) {
    _mode = level_editor_mode::move;
  }

  _camera.ProcessKeyboard(deltaTime);
}

void level_editor::ProcessKeyboardInEditMode() {
  if (_inputManager.KeyPressed(key::f1)) {
    _mode = level_editor_mode::edit;
  } else if (_inputManager.KeyPressed(key::f2)) {
    _mode = level_editor_mode::move;
  }
}

void level_editor::ProcessCursorInMoveMode() {
  if (_inputManager._cursorIsMoving) {
    _camera.ProcessCursor(_inputManager._cursorCoords);
  }
}

void level_editor::SpawnMazeModel() {
  entity_id const id{_resourceManager.SpawnEntityFromModelType(model_type::maze)};

  if (id == 0) {
    std::clog << __FUNCTION__ << ": entity not created, skipping" << std::endl;
    return;
  }

  _entities.insert(id);

  _ecs.AddTransformComponent(
      id, transform_component(glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1.f)));

  _selectedEntity = id;
}

void level_editor::SpawnBallModel() {
  entity_id const id{_resourceManager.SpawnEntityFromModelType(model_type::ball)};

  if (id == 0) {
    std::clog << __FUNCTION__ << ": entity not created, skipping" << std::endl;
    return;
  }

  _entities.insert(id);

  _ecs.AddTransformComponent(id, transform_component(glm::quat(0.f, 0.f, 0.f, 0.f),
                                                     glm::vec3(2.f, 0.f, 0.f), glm::vec3(0.5f)));

  _selectedEntity = id;
}

void level_editor::ClearModels() {
  // TODO: at some point you'd want to remove models as well (from the resource manager), however,
  // that will only be a problem if you spawn a lot of entities with different model files
  for (auto const& id : _entities) {
    _resourceManager.RemoveEntityModelRelationship(id);
  }

  _entities.clear();

  _selectedEntity = 0;
}

}; // namespace lain
