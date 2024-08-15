#include "l_level_editor.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_application.hpp"
#include "l_camera.hpp"
#include "l_common.hpp"
#include "l_debug.hpp"
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

// BUG: entity_added is a temporary state whose purpose is to avoid deselecting
// an entity when it was just added because an intersection test is done just
// right after that
enum class level_editor_mode { move, edit };

static float constexpr kGridSquareSize{0.5f};
static float constexpr kHalfGridExtent{20.f};
static glm::vec4 constexpr kGreyColour{0.5f, 0.5f, 0.5f, 1.f};
static glm::vec4 constexpr kRedColour{1.f, 0.0f, 0.0f, 1.f};
static glm::vec4 constexpr kGreenColour{0.f, 1.0f, 0.0f, 1.f};
static glm::vec4 constexpr kDebugColour{0.33f, 0.1f, 0.1f, 1.f};
static glm::vec4 constexpr kYellowColour{1.f, 1.f, 0.1f, 1.f};

static level_editor_mode _mode;
static camera3D _camera;
// TODO: change to shader
static VertexObjectHandle _grid;
static VertexObjectHandle _axisX;
static VertexObjectHandle _axisZ;
static VertexObjectHandle _ray;
static shader _boundingBox;
static glm::vec3 _currentCameraDirection;
static entity_id _selectedEntity;
static std::vector<entity_id> _entities;
static glm::vec3 _imGuiEntityPosition;
static ecs::entity_component_system* _ecs;
static glm::vec3 _rayToCursorPos;
static bool _debugDrawCursorRayInEditMode;
static bool _debugDrawEntityAABB;
static ray _cameraToCursorRay;

static int GetSquaresToDraw() { return static_cast<int>((2 * kHalfGridExtent) / kGridSquareSize); }

void Initialise(ecs::entity_component_system* ecs) {
  assert(ecs != nullptr);

  _ecs = ecs;

  _debugDrawCursorRayInEditMode = false;
  _debugDrawEntityAABB = false;

  _camera._position = glm::vec3(0.f);
  _camera._targetPosition = glm::vec3(0.f);
  _camera._worldUp = glm::vec3(0.f, 1.f, 0.f);
  _camera._speed = 20.f;
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

  vertices.assign(6, 0.f);

  _ray = resource_manager::CreatePrimitiveVAO(vertices, GL_DYNAMIC_DRAW);

  std::vector<unsigned int> const indices = {
      0, 1, 1, 2, 2, 3, 3, 0, // Bottom face
      4, 5, 5, 6, 6, 7, 7, 4, // Top face
      0, 4, 1, 5, 2, 6, 3, 7  // Vertical lines
  };

  vertices.assign(24, 0.f);

  _boundingBox = resource_manager::CreateCubeVAO(vertices, GL_DYNAMIC_DRAW, indices);
}

static void ProcessInputInEditMode() {
  if (!ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) &&
      input_manager::IsMouseButtonPressed(input_manager::mouse_button::left)) {
    bool pickedEntity{false};
    std::size_t i{0};

    // TODO: it's probably not a good idea to grab objects using their aabb because
    // you want to be precise.
    while (!pickedEntity && i < _entities.size()) {
      if (_selectedEntity != _entities[i]) {
        for (auto const& aabb : _ecs->GetAABBSComponents(_entities[i])) {
          if (RayIntersectsAABB(_cameraToCursorRay, aabb._aabb)) {
            _selectedEntity = _entities[i];
            pickedEntity = true;
          }
        }
      }

      ++i;
    }

    if (!pickedEntity) {
      _selectedEntity = 0;
    }
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

static void AddEntityToLevel(int const selection) {
  model_type type;

  if (selection == 0) {
    type = model_type::maze;
  } else {
    type = model_type::ball;
  }

  resource_manager::LoadModel(type);

  _selectedEntity = ecs::GetNewEntityId();

  auto transform =
      transform_component(glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f), glm::vec3(1.f));

  _ecs->AddTransformComponent(_selectedEntity, transform);

  resource_manager::AddEntityModelRelationship(_selectedEntity, type);

  for (auto const& mesh : resource_manager::GetModelDataFromEntity(_selectedEntity)->_meshes) {
    _ecs->AddAABBComponent(_selectedEntity, mesh._boundingBox);
  }

  _entities.push_back(_selectedEntity);
}

static void UpdateCursorInEditMode() {
  ImGuiIO& io = ImGui::GetIO();

  glm::vec4 cursorPos(io.MousePos.x, io.MousePos.y, 0.f, 0.f);

  cursorPos = ScreenSpaceToNormalisedDeviceCoordinates(cursorPos, application::GetWindowWidth(),
                                                       application::GetWindowHeight());

  cursorPos = NormalisedDeviceCoordinatesToClipSpace(cursorPos);

  cursorPos = ClipSpaceToViewSpace(cursorPos, renderer::GetCurrentProjectionMatrix());

  cursorPos = ViewSpaceToWorldSpace(cursorPos, _camera.GetViewMatrix());

  // DEBUG
  _rayToCursorPos = glm::vec3(cursorPos) - _camera._position;

  _cameraToCursorRay._direction = glm::normalize(glm::vec3(cursorPos) - _camera._position);

  _cameraToCursorRay._position = _camera._position;
}

static void DeleteEntities() {
  _ecs->Clear();
  _entities.clear();
  _selectedEntity = 0;
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
    AddEntityToLevel(selection);
  }

  ImGui::End();

  if (_selectedEntity != 0) {
    // if you're here it means that you created the entity, which means the transfom component and
    // aabb component are already created
    auto transform = _ecs->GetTransformComponent(_selectedEntity);
    bool modified{false};

    _imGuiEntityPosition.x = transform._position.x;
    _imGuiEntityPosition.y = transform._position.y;
    _imGuiEntityPosition.z = transform._position.z;

    ImGui::Begin("Entity Information", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Text("Id: %d", _selectedEntity);
    ImGui::NewLine();

    if (ImGui::DragFloat3("Position", &_imGuiEntityPosition.x, 0.1, -10.f, 10.f)) {
      modified = true;
    }

    ImGui::NewLine();

    if (ImGui::Button("Delete Entities")) {
      DeleteEntities();
    }

    ImGui::End();

    transform._position.x = _imGuiEntityPosition.x;
    transform._position.y = _imGuiEntityPosition.y;
    transform._position.z = _imGuiEntityPosition.z;

    // TODO: this shouldnt be here for too long
    if (modified) {
      _ecs->ResetAABBs(_selectedEntity);

      for (auto const& mesh : resource_manager::GetModelDataFromEntity(_selectedEntity)->_meshes) {
        auto bb = aabb_component(mesh._boundingBox);

        ECS_Update(transform, bb);

        _ecs->AddAABBComponent(_selectedEntity, bb);

        _ecs->AddTransformComponent(_selectedEntity, transform);
      }
    }
  }

  UpdateCursorInEditMode();
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

static void DrawEntities() {
  static unsigned int id{resource_manager::GetShader(kPrimitiveShaderId)->_id};

  if (_debugDrawEntityAABB) {
    for (auto const& entityId : _entities) {

      for (auto const& aabb : _ecs->GetAABBSComponents(entityId)) {

        float vertices[] = {
            aabb._initial._min.x, aabb._initial._min.y, aabb._initial._min.z, aabb._initial._max.x,
            aabb._initial._min.y, aabb._initial._min.z, aabb._initial._max.x, aabb._initial._max.y,
            aabb._initial._min.z, aabb._initial._min.x, aabb._initial._max.y, aabb._initial._min.z,
            aabb._initial._min.x, aabb._initial._min.y, aabb._initial._max.z, aabb._initial._max.x,
            aabb._initial._min.y, aabb._initial._max.z, aabb._initial._max.x, aabb._initial._max.y,
            aabb._initial._max.z, aabb._initial._min.x, aabb._initial._max.y, aabb._initial._max.z,
        };

        glBindVertexArray(_boundingBox._vao);
        glBindBuffer(GL_ARRAY_BUFFER, _boundingBox._vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        renderer::DrawBoundingBox(id, _boundingBox._vao, entityId, _camera.GetViewMatrix(),
                                  kYellowColour);
      }
    }
  }

  renderer::DrawEntities(_entities, _camera);
}

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

  if (input_manager::IsKeyPressed(input_manager::key::b)) {
    _debugDrawEntityAABB = !_debugDrawEntityAABB;
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
