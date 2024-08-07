#include "l_level_editor_renderer.hpp"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_common.hpp"
#include "l_entity.hpp"
#include "l_event_manager.hpp"
#include "l_model.hpp"
#include "l_resource_manager.hpp"

namespace lain {

static glm::vec4 constexpr redColour{1.f, 0.f, 0.f, 1.f};
static glm::vec4 constexpr greenColour{0.f, 1.f, 0.f, 1.f};
static glm::vec4 constexpr greyColour{0.5f, 0.5f, 0.5f, 1.f};

level_editor_renderer::level_editor_renderer(float const width, float const height,
                                             resource_manager& resourceManager,
                                             event_manager& eventManager,
                                             entity_component_system& ecs)
    : renderer{width, height},
      _eventManager{eventManager},
      _resourceManager{resourceManager},
      _ecs{ecs} {

  _gridAxisLineShader = resourceManager.GetShader(gridAxisLineShaderId);
  _gridAxisCheckerboardShader = resourceManager.GetShader(gridAxisCheckerboardShaderId);
  _modelWithTexturesShader = resourceManager.GetShader(levelEditorModelWithTextureShaderId);
  _modelWithoutTexturesShader = resourceManager.GetShader(levelEditorModelWithoutTextureShaderId);

  // axis lines
  UseShader(_gridAxisLineShader->_id);
  SetUniformMat4(_gridAxisLineShader->_id, "projection", _perspective);
  SetUniformMat4(_gridAxisLineShader->_id, "model", glm::mat4(1.f));

  // grid squares
  UseShader(_gridAxisCheckerboardShader->_id);
  SetUniformMat4(_gridAxisCheckerboardShader->_id, "projection", _perspective);
  SetUniformMat4(_gridAxisCheckerboardShader->_id, "model", glm::mat4(1.f));
  SetUniformVec4(_gridAxisCheckerboardShader->_id, "colour", greyColour);

  // models
  UseShader(_modelWithTexturesShader->_id);
  SetUniformMat4(_modelWithTexturesShader->_id, "projection", _perspective);

  UseShader(_modelWithoutTexturesShader->_id);
  SetUniformMat4(_modelWithoutTexturesShader->_id, "projection", _perspective);
}

void level_editor_renderer::RenderMoveMode(glm::mat4 const& cameraViewMatrix,
                                           glm::vec3 const& cameraPosition,
                                           std::unordered_set<entity_id> const& entities) {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGuiIO& io = ImGui::GetIO();
  io.MouseDrawCursor = false;

  RenderGridAndAxis(cameraViewMatrix);

  RenderEntities(cameraViewMatrix, entities);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Move mode");
  ImGui::Text("Position: (%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void level_editor_renderer::RenderEditMode(glm::mat4 const& cameraViewMatrix,
                                           std::unordered_set<entity_id> const& entities,
                                           entity_id const selectedEntity) {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ImGuiIO& io = ImGui::GetIO();
  io.MouseDrawCursor = true;

  RenderGridAndAxis(cameraViewMatrix);

  RenderEntities(cameraViewMatrix, entities);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("Edit mode");
  ImGui::NewLine();
  ImGui::Text("Add entity");
  ImGui::NewLine();

  // TODO: stupid stupid stupid stupid stupid stupid
  static int selection{0};

  if (ImGui::RadioButton("Maze", &selection, 0)) {
  }

  if (ImGui::RadioButton("Ball", &selection, 1)) {
  }

  ImGui::NewLine();

  if (ImGui::Button("Spawn")) {
    if (selection == 1) {
      _eventManager.Post(event_type::level_editor_spawn_ball);
    } else {
      _eventManager.Post(event_type::level_editor_spawn_maze);
    }
  }

  ImGui::NewLine();

  if (ImGui::Button("Clear")) {
    _eventManager.Post(event_type::level_editor_clear);
  }

  ImGui::End();

  if (selectedEntity != 0) {
    ImGui::Begin("Entity Options");
    auto const& transform = _ecs.GetTransformComponent(selectedEntity);
    static float posX{transform._position.x};
    static float posY{transform._position.y};
    static float posZ{transform._position.z};
    static float rotX{transform._rotation.x};
    static float rotY{transform._rotation.y};
    static float rotZ{transform._rotation.z};
    static float scaX{transform._scale.x};
    static float scaY{transform._scale.y};
    static float scaZ{transform._scale.z};

    ImGui::Text("Position");
    ImGui::NewLine();
    ImGui::InputFloat("X", &posX, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Y", &posY, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Z", &posZ, -10.f, 10.f, "%.2f");
    ImGui::Text("Rotation");
    ImGui::InputFloat("X", &rotX, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Y", &rotY, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Z", &rotZ, -10.f, 10.f, "%.2f");
    ImGui::Text("Scale");
    ImGui::InputFloat("X", &scaX, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Y", &scaY, -10.f, 10.f, "%.2f");
    ImGui::InputFloat("Z", &scaZ, -10.f, 10.f, "%.2f");

    ImGui::End();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void level_editor_renderer::RenderGridAndAxis(glm::mat4 const& cameraViewMatrix) const {
  // Draw grid
  UseShader(_gridAxisCheckerboardShader->_id);
  SetUniformMat4(_gridAxisCheckerboardShader->_id, "view", cameraViewMatrix);
  glBindVertexArray(_gridAxisCheckerboardShader->_vao);

  // TODO: only you can do it so terrible
  glDrawArrays(GL_LINES, 0, 1000);

  // Draw both axis
  UseShader(_gridAxisLineShader->_id);
  glBindVertexArray(_gridAxisLineShader->_vao);

  SetUniformMat4(_gridAxisLineShader->_id, "view", cameraViewMatrix);

  // Draw X axis
  SetUniformVec4(_gridAxisLineShader->_id, "colour", redColour);
  glDrawArrays(GL_LINES, 0, 2);

  // Draw Z axis
  SetUniformVec4(_gridAxisLineShader->_id, "colour", greenColour);
  glDrawArrays(GL_LINES, 2, 2);
}

void level_editor_renderer::RenderEntities(glm::mat4 const& cameraViewMatrix,
                                           std::unordered_set<entity_id> const& entities) const {
  for (auto const& entity : entities) {
    model const* model = _resourceManager.GetModelFromEntity(entity);
    glm::mat4 const modelMatrix{_ecs.GetTransformComponent(entity).GetModel()};

    for (auto const& mesh : model->GetMeshes()) {
      if (mesh.HasTextures()) {
        UseShader(_modelWithTexturesShader->_id);
        SetUniformMat4(_modelWithTexturesShader->_id, "model", modelMatrix);
        SetUniformMat4(_modelWithTexturesShader->_id, "view", cameraViewMatrix);
        DrawMesh(_modelWithTexturesShader, mesh);
      } else {
        UseShader(_modelWithoutTexturesShader->_id);
        SetUniformMat4(_modelWithoutTexturesShader->_id, "model", modelMatrix);
        SetUniformMat4(_modelWithoutTexturesShader->_id, "view", cameraViewMatrix);
        DrawMesh(_modelWithoutTexturesShader, mesh);
      }
    }
  }
}

}; // namespace lain
