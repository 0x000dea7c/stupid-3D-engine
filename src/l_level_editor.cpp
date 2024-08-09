#include "l_level_editor.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_camera.hpp"
#include "l_common.hpp"
#include "l_input_manager.hpp"
#include "l_platform.hpp"
#include "l_renderer.hpp"
#include "l_resource_manager.hpp"
#include "l_shader.hpp"
#include <iostream>
#include <vector>

namespace lain {

namespace level_editor {

enum class level_editor_mode { move, edit };

static float constexpr kGridSquareSize{0.5f};
static float constexpr kHalfGridExtent{20.f};
static glm::vec4 constexpr kGreyColour{0.5f, 0.5f, 0.5f, 1.f};

static level_editor_mode _mode;
static camera3D _camera;
static unsigned int _gridVAO;
static unsigned int _axisVAO;
static glm::vec3 currentCameraDirection;

static int GetSquaresToDraw() { return static_cast<int>((2 * kHalfGridExtent) / kGridSquareSize); }

void Initialise() {
  _camera._position = glm::vec3(0.f);
  _camera._targetPosition = glm::vec3(0.f);
  _camera._worldUp = glm::vec3(0.f, 1.f, 0.f);
  _camera._speed = 10.f;
  _camera._sensitivity = 0.1f;
  _camera._lerp = 0.1f;
  _camera._yaw = 0.f;
  _camera._pitch = 0.f;

  // ---------------------------------------------
  // Create grid vertices and the corresponding vao
  // ---------------------------------------------
  int const squares{level_editor::GetSquaresToDraw()};

  std::vector<float> vertices;

  for (int i{-squares}; i <= squares; ++i) {
    // lines along the X axis
    vertices.push_back(-level_editor::kHalfGridExtent * 2);
    vertices.push_back(0.f);
    vertices.push_back(i * level_editor::kGridSquareSize);

    vertices.push_back(level_editor::kHalfGridExtent * 2);
    vertices.push_back(0.f);
    vertices.push_back(i * level_editor::kGridSquareSize);

    // lines along the Z axis
    vertices.push_back(i * level_editor::kGridSquareSize);
    vertices.push_back(0.f);
    vertices.push_back(-level_editor::kHalfGridExtent * 2);

    vertices.push_back(i * level_editor::kGridSquareSize);
    vertices.push_back(0.f);
    vertices.push_back(level_editor::kHalfGridExtent * 2);
   }

  _gridVAO = resource_manager::CreatePrimitiveVAO(vertices);

  assert(_gridVAO != 0 && " couldn't create grid vao");

  // ---------------------------------------------
  // Create vertices for the X-Z axis
  // ---------------------------------------------
  vertices = {
      0.f,    0.01f, 0.f,    // origin
      1000.f, 0.01f, 0.f,    // X
      0.f,    0.01f, 0.f,    // origin
      0.f,    0.01f, 1000.f, // Z
  };

  _axisVAO = resource_manager::CreatePrimitiveVAO(vertices);

  assert(_axisVAO != 0 && " couldn't create axis vao");
}

static void ProcessInputInEditMode() {
  if (input_manager::IsCursorMoving()) {
    // TODO: do raycasting starting from camera position towards where the mouse is pointing.
    // Find closest entity (if there's any) and select it.
  }
}

static void ProcessInputInMoveMode() {
  // -----------
  // Keyboard
  // -----------
  currentCameraDirection = glm::vec3(0);

  if (input_manager::IsKeyHeld(input_manager::key::w)) {
    currentCameraDirection.z = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::s)) {
    currentCameraDirection.z = -1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::a)) {
    currentCameraDirection.x = -1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::d)) {
    currentCameraDirection.x = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::q)) {
    currentCameraDirection.y = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::e)) {
    currentCameraDirection.y = -1;
  }

  _camera.ProcessKeyboard(currentCameraDirection);

  if (input_manager::IsCursorMoving()) {
    _camera.ProcessCursor(input_manager::GetCursorPosition());
  }
}

static void UpdateInMoveMode(float const deltaTime) {
  _camera.SetTargetPosition(deltaTime);

  _camera.Update(deltaTime);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Move mode");
  ImGui::Text("Position: (%.2f, %.2f, %.2f)", _camera._position.x, _camera._position.y,
              _camera._position.z);
  ImGui::End();
}

static void UpdateInEditMode() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Edit mode");
  ImGui::NewLine();
  ImGui::Text("Add entity");
  ImGui::NewLine();

  static int selection{0};

  ImGui::RadioButton("Maze", &selection, 0);
  ImGui::RadioButton("Ball", &selection, 1);

  if (ImGui::Button("Add")) {
    std::clog << "Added entity" << std::endl;
  }

  ImGui::NewLine();

  ImGui::End();
}

static void RenderInEditMode() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void RenderInMoveMode() {
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void DrawGrid() {
  static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};
  renderer::DrawLines(id, _gridVAO, 1000, _camera.GetViewMatrix(), kGreyColour);
}

static void DrawEntities() {}

void ProcessInput() {
  // --------------------------------------------------------------------------------------
  // Releasing or constraining the cursor is done to avoid conflicts between ImGui and SDL2
  // --------------------------------------------------------------------------------------
  if (input_manager::IsKeyPressed(input_manager::key::f1)) {
    _mode = level_editor_mode::edit;
  } else if (input_manager::IsKeyPressed(input_manager::key::f2)) {
    _mode = level_editor_mode::move;
  }

  switch (_mode) {
  case level_editor_mode::edit:
    ProcessInputInEditMode();
    break;
  case level_editor_mode::move:
    ProcessInputInMoveMode();
    break;
  }
}

void Update(float const deltaTime) {
  ImGuiIO& io = ImGui::GetIO();

  switch (_mode) {
  case level_editor_mode::edit:
    ReleaseCursorFromWindow();
    io.MouseDrawCursor = true;
    UpdateInEditMode();
    break;
  case level_editor_mode::move:
    ConstrainCursorInWindow();
    io.MouseDrawCursor = false;
    UpdateInMoveMode(deltaTime);
    break;
  }
}

void Render() {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  DrawGrid();

  DrawEntities();

  switch (_mode) {
  case level_editor_mode::edit:
    RenderInEditMode();
    break;
  case level_editor_mode::move:
    RenderInMoveMode();
    break;
  }
}

} // namespace level_editor

// void level_editor::SpawnMazeModel() {
//   entity_id const id{_resourceManager.SpawnEntityFromModelType(model_type::maze)};

//   if (id == 0) {
//     std::clog << __FUNCTION__ << ": entity not created, skipping" << std::endl;
//     return;
//   }

//   _entities.insert(id);

//   _ecs.AddTransformComponent(
//       id, transform_component(glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1.f)));

//   _selectedEntity = id;
// }

// void level_editor::SpawnBallModel() {
//   entity_id const id{_resourceManager.SpawnEntityFromModelType(model_type::ball)};

//   if (id == 0) {
//     std::clog << __FUNCTION__ << ": entity not created, skipping" << std::endl;
//     return;
//   }

//   _entities.insert(id);

//   _ecs.AddTransformComponent(id, transform_component(glm::quat(0.f, 0.f, 0.f, 0.f),
//                                                      glm::vec3(2.f, 0.f, 0.f), glm::vec3(0.5f)));

//   _selectedEntity = id;
// }

// void level_editor::ClearModels() {
//   // TODO: at some point you'd want to remove models as well (from the resource manager),
//   however,
//   // that will only be a problem if you spawn a lot of entities with different model files
//   for (auto const& id : _entities) {
//     _resourceManager.RemoveEntityModelRelationship(id);
//   }

//   _entities.clear();

//   _selectedEntity = 0;
// }

// void level_editor::ProcessCursorInEditMode() {
//   // -----------------------------------------------------------------------
//   // Convert cursor positions from screen space to world space and see if it
//   // intersects with an entity when you're pressing the left click button
//   // -----------------------------------------------------------------------
//   if (_inputManager.MouseButtonIsPressed(mouse_button::left)) {
//     //
//     --------------------------------------------------------------------------------------------
//     // TEMP: Problem is that the bounding box of the model is in local space, so you need to
//     // convert it
//     //
//     // Cast a ray from the camera position that points forward (negative z). If it intersects
//     with
//     // any object, pick the closest one to the camera
//     //
//     --------------------------------------------------------------------------------------------
//     glm::vec4 cursorWorldSpace{_inputManager._cursorCoordsScreenSpace.x,
//                                _inputManager._cursorCoordsScreenSpace.y, 0.f, 0.f};

//     // convert to ndc
//     cursorWorldSpace.x = ((2.f * cursorWorldSpace.x) / 1920.f) - 1.f;
//     cursorWorldSpace.y = 1.f - ((2.f * cursorWorldSpace.y) / 1080.f);

//     // homogeneous clip space
//     cursorWorldSpace.w = 1.f;

//     // camera space
//     cursorWorldSpace = glm::inverse(_camera.GetViewMatrix()) * cursorWorldSpace;
//     cursorWorldSpace.z = -1.f;
//     cursorWorldSpace.w = 0.f;

//     // world space
//     cursorWorldSpace = glm::inverse(_renderer->_perspective) * cursorWorldSpace;

//     ray ray;
//     ray._position = _camera.GetCenter();
//     ray._direction = glm::vec3(cursorWorldSpace) - ray._position;
//     ray._direction = glm::normalize(ray._direction);

//     for (auto const& entity : _entities) {
//       auto entityBoundingBox = _resourceManager.GetModelFromEntity(entity)->GetBoundingBox();

//       auto const& transform = _ecs.GetTransformComponent(entity);
//       auto const& model = transform.GetModel();

//       entityBoundingBox._min = model * glm::vec4(entityBoundingBox._min, 1.f);
//       entityBoundingBox._max = model * glm::vec4(entityBoundingBox._max, 1.f);

//       if (RayIntersectsAABB(ray, entityBoundingBox)) {
//         std::clog << "You clicked the entity -> " << entity << std::endl;
//       }
//     }
//   }
// }

}; // namespace lain
