// #include "SDL2/SDL.h"
#include "glad/glad.h"
#include "l_application.hpp"
// #include "l_entity.hpp"
// #include "l_event_manager.hpp"
// #include "l_game.hpp"
// #include "l_input_manager.hpp"
// #include "l_level_editor.hpp"
// #include "l_level_editor_camera3d.hpp"
// #include "l_level_editor_renderer.hpp"
// #include "l_main_menu.hpp"
// #include "l_main_menu_renderer.hpp"
// #include "l_platform.hpp"
// #include "l_resource_manager.hpp"
// #include <chrono>
#include <cstdlib>
// #include <iostream>

using namespace lain;

int main() {
  //
  // NOTE: the approach of not using classes for the input manager, event manager, resource manager,
  // etc, is done because you only need a single reference to them.
  //
  // When you need more than one, you use classes or structs. The interface is much cleaner this
  // way as well.
  //
  // You can also use singletons, but their interface look awful.
  //
  if (!application::Initialise(1920.f, 1080.f, false)) {
    application::Shutdown();
    return EXIT_FAILURE;
  }

  application::Run();

  application::Shutdown();

  // TODO: you can probably get rid of 50% of this shit you wrote
  // entity_component_system ecs;
  // event_manager eventManager;
  // input_manager inputManager;
  // main_menu_renderer mainMenuRenderer(1920.f, 1080.f, eventManager);
  // main_menu mainMenu(mainMenuRenderer);
  // DONE UNTIL HERE
  // level_editor_camera3d levelEditorCamera{glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f),
  // 0.f,
  //                                         0.f, inputManager};
  // // TODO: figure out this shit
  // resource_manager resourceManager;
  // level_editor levelEditor(levelEditorCamera, inputManager, eventManager, ecs, resourceManager);
  // if (!resourceManager.Initialise(levelEditor)) {
  //   std::cerr << __FUNCTION__ << ": couldn't initialise resource manager.\n";
  //   return EXIT_FAILURE;
  // }
  // level_editor_renderer levelEditorRenderer(1920.f, 1080.f, resourceManager, eventManager, ecs);
  // // TODO: this needs fixing. The reason for this is that it's not well designed.
  // levelEditor.SetRenderer(&levelEditorRenderer);
  // game game(levelEditor, eventManager, mainMenu);

  // float lastFrame{0.f};

  // while (!game.IsDone()) {
  //   inputManager.BeginFrame();

  //   glViewport(0, 0, 1920, 1080);

  //   float const currFrame{Time()};
  //   float const deltaTime{currFrame - lastFrame};

  //   ImGuiIO& io = ImGui::GetIO();
  //   io.MouseDrawCursor = true;

  //   lastFrame = currFrame;

  //   SDL_Event event;
  //   while (SDL_PollEvent(&event)) {
  //     ImGui_ImplSDL2_ProcessEvent(&event);

  //     if (event.type == SDL_QUIT) {
  //       game.Done();
  //     }

  //     if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
  //         event.window.windowID == SDL_GetWindowID(window)) {
  //       game.Done();
  //     }

  //     if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
  //       lain::key const key{SDLKeyToEngine(event.key.keysym.sym)};
  //       bool const isPressed{event.type == SDL_KEYDOWN};
  //       inputManager.UpdateKey(key, isPressed);
  //     }

  //     if (event.type == SDL_MOUSEMOTION) {
  //       inputManager._cursorCoords.x = event.motion.xrel;
  //       inputManager._cursorCoords.y = -event.motion.yrel;
  //       inputManager._cursorIsMoving = true;
  //     }

  //     if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
  //       mouse_button const button{SDLMouseButtonToEngine(event.button.button)};
  //       inputManager.UpdateMouseButton(button, event.type == SDL_MOUSEBUTTONDOWN);
  //     }
  //   }

  //   if (game.IsDone() || inputManager.KeyPressed(key::esc)) {
  //     break;
  //   }

  //   int x, y;
  //   SDL_GetMouseState(&x, &y);

  //   inputManager._cursorCoordsScreenSpace.x = x;
  //   inputManager._cursorCoordsScreenSpace.y = y;

  //   game.ProcessInput(deltaTime);

  //   game.Update(deltaTime);

  //   game.Render();
  // }

  return EXIT_SUCCESS;
}
