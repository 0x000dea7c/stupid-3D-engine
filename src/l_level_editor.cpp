#include "l_level_editor.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_application.hpp"
#include "l_camera.hpp"
#include "l_common.hpp"
#include "l_entity.hpp"
#include "l_input_manager.hpp"
#include "l_math.hpp"
#include "l_platform.hpp"
#include "l_renderer.hpp"
#include "l_resource_manager.hpp"
#include "l_shader.hpp"
#include <vector>

namespace lain {

namespace level_editor {

using VertexObjectHandle = std::pair<unsigned int, unsigned int>;

enum class level_editor_mode { move, edit };

static float constexpr kGridSquareSize{0.5f};
static float constexpr kHalfGridExtent{20.f};
static glm::vec4 constexpr kGreyColour{0.5f, 0.5f, 0.5f, 1.f};
static glm::vec4 constexpr kRedColour{1.f, 0.0f, 0.0f, 1.f};
static glm::vec4 constexpr kGreenColour{0.f, 1.0f, 0.0f, 1.f};
static glm::vec4 constexpr kDebugColour{0.33f, 0.1f, 0.1f, 1.f};

static level_editor_mode _mode;
static camera3D _camera;
static VertexObjectHandle _grid;
static VertexObjectHandle _axisX;
static VertexObjectHandle _axisZ;
static VertexObjectHandle _ray;
static glm::vec3 _currentCameraDirection;
static entity_id _selectedEntity;
static std::vector<entity_id> _entities;
static glm::vec3 _imGuiEntityPosition;
static ecs::entity_component_system* _ecs;
static glm::vec3 _rayToCursorPos;
static bool _debugDrawCursorRayInEditMode;

static int GetSquaresToDraw() { return static_cast<int>((2 * kHalfGridExtent) / kGridSquareSize); }

void Initialise(ecs::entity_component_system* ecs) {
  assert(ecs != nullptr);

  _ecs = ecs;

  _debugDrawCursorRayInEditMode = false;

  _camera._position = glm::vec3(0.f);
  _camera._targetPosition = glm::vec3(0.f);
  _camera._worldUp = glm::vec3(0.f, 1.f, 0.f);
  _camera._speed = 10.f;
  _camera._sensitivity = 0.1f;
  _camera._lerp = 0.1f;
  _camera._yaw = 0.f;
  _camera._pitch = 0.f;

  // ---------------------------------------------
  // Create grid vertices and the corresponding vao, math here is terribly wrong but it does what
  // you want fn
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

  _grid = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

  // ---------------------------------------------
  // Create vertices for the X-Z axis
  // ---------------------------------------------
  vertices = {
      0.f,    0.01f, 0.f, // origin
      1000.f, 0.01f, 0.f, // X
  };

  _axisX = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

  vertices = {
      0.f, 0.01f, 0.f,    // origin
      0.f, 0.01f, 1000.f, // Z
  };

  _axisZ = resource_manager::CreatePrimitiveVAO(vertices, GL_STATIC_DRAW);

  vertices = {0, 0, 0, 0, 0, 0};

  _ray = resource_manager::CreatePrimitiveVAO(vertices, GL_DYNAMIC_DRAW);
}

static void ProcessInputInEditMode() {
  if (input_manager::IsCursorMoving()) {
    // TODO: do raycasting starting from camera position towards where the mouse is pointing.
    // Find closest entity (if there's any) and select it.
  }

  if (input_manager::IsKeyPressed(input_manager::key::k)) {
    _debugDrawCursorRayInEditMode = !_debugDrawCursorRayInEditMode;
  }
}

static void ProcessInputInMoveMode() {
  // -----------
  // Keyboard
  // -----------
  _currentCameraDirection = glm::vec3(0);

  if (input_manager::IsKeyHeld(input_manager::key::w)) {
    _currentCameraDirection.z = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::s)) {
    _currentCameraDirection.z = -1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::a)) {
    _currentCameraDirection.x = -1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::d)) {
    _currentCameraDirection.x = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::q)) {
    _currentCameraDirection.y = 1;
  }

  if (input_manager::IsKeyHeld(input_manager::key::e)) {
    _currentCameraDirection.y = -1;
  }

  _camera.ProcessKeyboard(_currentCameraDirection);

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

  ImGui::NewLine();

  if (ImGui::Button("Add")) {
    model_type type;

    if (selection == 0) {
      type = model_type::maze;
    } else {
      type = model_type::ball;
    }

    // TODO: some module or something should do all of this as a unit
    resource_manager::LoadModel(type);

    _selectedEntity = ecs::GetNewEntityId();

    _ecs->AddTransformComponent(
        _selectedEntity,
        transform_component(glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1.f)));

    resource_manager::AddEntityModelRelationship(_selectedEntity, type);

    _ecs->AddAABBComponent(_selectedEntity,
                           resource_manager::GetModelDataFromEntity(_selectedEntity)->_boundingBox);

    _entities.push_back(_selectedEntity);

    _imGuiEntityPosition.x = _imGuiEntityPosition.y = _imGuiEntityPosition.z = 0.f;
  }

  ImGui::End();

  if (_selectedEntity != 0) {
    ImGui::Begin("Entity Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Id: %d", _selectedEntity);
    ImGui::Text("Position");
    ImGui::NewLine();
    ImGui::SliderFloat("X", &_imGuiEntityPosition.x, -10.f, 10.f, nullptr);
    ImGui::SliderFloat("Y", &_imGuiEntityPosition.y, -10.f, 10.f, nullptr);
    ImGui::SliderFloat("Z", &_imGuiEntityPosition.z, -10.f, 10.f, nullptr);
    ImGui::End();

    auto const transform =
        transform_component(glm::quat(0.f, 0.f, 0.f, 0.f), _imGuiEntityPosition, glm::vec3(1.f));

    // Use the modified information to change entities' transform
    _ecs->AddTransformComponent(_selectedEntity, transform);

    auto& aabb = _ecs->GetAABBComponent(_selectedEntity);

    aabb.Update(transform);
  }

  // ------------------
  // Entity picking
  // ------------------
  ImGuiIO& io = ImGui::GetIO();

  glm::vec4 cursorPos(io.MousePos.x, io.MousePos.y, 0.f, 0.f);

  cursorPos = ScreenSpaceToNormalisedDeviceCoordinates(cursorPos, application::GetWindowWidth(),
                                                       application::GetWindowHeight());

  cursorPos = NormalisedDeviceCoordinatesToClipSpace(cursorPos);

  cursorPos = ClipSpaceToViewSpace(cursorPos, renderer::GetCurrentProjectionMatrix());

  cursorPos = ViewSpaceToWorldSpace(cursorPos, _camera.GetViewMatrix());

  _rayToCursorPos = glm::vec3(cursorPos) - _camera._position;

  ray cameraToCursorRay;
  cameraToCursorRay._direction = glm::normalize(glm::vec3(cursorPos) - _camera._position);
  cameraToCursorRay._position = _camera._position;

  for (auto const& entity : _entities) {
    auto transform = _ecs->GetTransformComponent(entity);

    aabb entityaabb;
    entityaabb._min = transform._position - 1.f;
    entityaabb._min = transform._position + 1.f;

    if (RayIntersectsAABB(cameraToCursorRay, entityaabb)) {
    }
  }
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
  renderer::DrawLines(id, _grid.first, 1000, _camera.GetViewMatrix(), kGreyColour);
}

static void DrawWorldAxis() {
  static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};
  renderer::DrawLines(id, _axisX.first, 2, _camera.GetViewMatrix(), kRedColour);
  renderer::DrawLines(id, _axisZ.first, 2, _camera.GetViewMatrix(), kGreenColour);
}

static void DrawEntities() { renderer::DrawEntities(_entities, _camera); }

static void Debug_DrawRayToCursor() {
  static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};

  float const vertices[] = {0.f,
                            0.f,
                            0.f,
                            _rayToCursorPos.x * 1000.f,
                            _rayToCursorPos.y * 1000.f,
                            _rayToCursorPos.z * 1000.f};

  glBindVertexArray(_ray.first);
  glBindBuffer(GL_ARRAY_BUFFER, _ray.second);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  renderer::DrawLines(id, _ray.first, 2, _camera.GetViewMatrix(), kDebugColour);
}

void ProcessInput() {
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

  DrawWorldAxis();

  DrawGrid();

  DrawEntities();

  switch (_mode) {
  case level_editor_mode::edit:
    RenderInEditMode();

    if (_debugDrawCursorRayInEditMode) {
      Debug_DrawRayToCursor();
    }

    break;
  case level_editor_mode::move:
    RenderInMoveMode();
    break;
  }
}

} // namespace level_editor

}; // namespace lain
