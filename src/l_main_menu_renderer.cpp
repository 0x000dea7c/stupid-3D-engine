#include "l_main_menu_renderer.hpp"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_event_manager.hpp"

namespace lain {

main_menu_renderer::main_menu_renderer(float const width, float const height, event_manager& eventManager)
    : renderer{width, height},
      _eventManager{eventManager} {}

void main_menu_renderer::RenderMainMenu() {
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // ImGuiIO& io = ImGui::GetIO();
  // io.MouseDrawCursor = true;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();

  ImGui::NewFrame();
  ImGui::Begin("Menu Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

  if (ImGui::Button("Play")) {
    _eventManager.Post(event(event_type::main_menu_click_play));
  }

  if (ImGui::Button("Level Editor")) {
    _eventManager.Post(event(event_type::main_menu_click_level_editor));
  }

  if (ImGui::Button("Options")) {
    _eventManager.Post(event(event_type::main_menu_click_options));
  }

  if (ImGui::Button("Quit")) {
    _eventManager.Post(event(event_type::main_menu_click_quit));
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // io.MouseDrawCursor = false;

  // _resourceManager.UseShader(_cursorShader->_id);
  // _resourceManager.SetUniformVec2(_cursorShader->_id, "cursorPos", mousePos);

  // glBindVertexArray(_cursorShader->_vao);
  // glBindTexture(GL_TEXTURE_2D, _cursorTexture->_id);
  // glDrawArrays(GL_TRIANGLES, 0, 6);
}

}; // namespace lain
