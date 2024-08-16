#include "l_main_menu.h"
#include "glad/glad.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_event_manager.h"

namespace lain
{
    namespace main_menu
    {
        void ProcessInput()
        {
        }

        void Update()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("Menu Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

            if (ImGui::Button("Play")) {
                event_manager::Post(event(event_type::main_menu_click_play));
            }

            if (ImGui::Button("Level Editor")) {
                event_manager::Post(event(event_type::main_menu_click_level_editor));
            }

            if (ImGui::Button("Options")) {
                event_manager::Post(event(event_type::main_menu_click_options));
            }

            if (ImGui::Button("Quit")) {
                event_manager::Post(event(event_type::main_menu_click_quit));
            }

            ImGui::End();
        }

        void Render()
        {
            glClearColor(0.f, 0.f, 0.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    };
};
