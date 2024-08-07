#include "SDL2/SDL.h"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_entity.hpp"
#include "l_event_manager.hpp"
#include "l_game.hpp"
#include "l_input_manager.hpp"
#include "l_level_editor.hpp"
#include "l_level_editor_camera3d.hpp"
#include "l_level_editor_renderer.hpp"
#include "l_main_menu.hpp"
#include "l_main_menu_renderer.hpp"
#include "l_resource_manager.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace lain;

static void InitialiseImGui(SDL_Window* window, SDL_GLContext const context) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init();
}

static lain::key SDLKeyToEngine(int const key) {
  // TODO: handle all keys
  switch (key) {
  case SDLK_w:
    return lain::key::w;
  case SDLK_s:
    return lain::key::s;
  case SDLK_a:
    return lain::key::a;
  case SDLK_d:
    return lain::key::d;
  case SDLK_q:
    return lain::key::q;
  case SDLK_e:
    return lain::key::e;
  case SDLK_F1:
    return lain::key::f1;
  case SDLK_F2:
    return lain::key::f2;
  case SDLK_ESCAPE:
    return lain::key::esc;
  default:
    return lain::key::unknown;
  }
}

static float Time() {
  using namespace std::chrono;
  static const auto start = high_resolution_clock::now();
  return duration<float>(high_resolution_clock::now() - start).count();
}

int main() {
  // no need to sync C and C++ I/O streams, this might improve performance
  std::ios::sync_with_stdio(false);

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << __FUNCTION__ << ": couldn't initialise SDL: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  SDL_Window* window{SDL_CreateWindow(
      "Lain - Rolling Ball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080,
      SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED)};

  if (window == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't create SDL window: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_GLContext const context{SDL_GL_CreateContext(window)};

  SDL_GL_MakeCurrent(window, context);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << __FUNCTION__ << ": failed to initialise GLAD\n";
    return EXIT_FAILURE;
  }

  SDL_GL_SetSwapInterval(1);

  InitialiseImGui(window, context);

  if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0) {
    std::cerr << __FUNCTION__ << ": couldn't set relative mouse mode: " << SDL_GetError() << '\n';
    return EXIT_FAILURE;
  }

  // TODO: you can probably get rid of 50% of this shit you wrote
  entity_component_system ecs;
  event_manager eventManager;
  main_menu_renderer mainMenuRenderer(1920.f, 1080.f, eventManager);
  main_menu mainMenu(mainMenuRenderer);
  input_manager inputManager;
  level_editor_camera3d levelEditorCamera{glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f), 0.f,
                                          0.f, inputManager};
  // TODO: figure out this shit
  resource_manager resourceManager;
  level_editor levelEditor(levelEditorCamera, inputManager, eventManager, ecs, resourceManager);
  if (!resourceManager.Initialise(levelEditor)) {
    std::cerr << __FUNCTION__ << ": couldn't initialise resource manager.\n";
    return EXIT_FAILURE;
  }
  level_editor_renderer levelEditorRenderer(1920.f, 1080.f, resourceManager, eventManager, ecs);
  // TODO: this needs fixing. The reason for this is that it's not well designed.
  levelEditor.SetRenderer(&levelEditorRenderer);
  game game(levelEditor, eventManager, mainMenu);

  float lastFrame{0.f};

  // TODO: doesn't here you Baka
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  while (!game.IsDone()) {
    inputManager.BeginFrame();
    glViewport(0, 0, 1920, 1080);
    float const currFrame{Time()};
    float const deltaTime{currFrame - lastFrame};

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;

    lastFrame = currFrame;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_QUIT) {
        game.Done();
      }

      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(window)) {
        game.Done();
      }

      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        lain::key const key{SDLKeyToEngine(event.key.keysym.sym)};
        bool const isPressed{event.type == SDL_KEYDOWN};
        inputManager.UpdateKey(key, isPressed);
      }

      if (event.type == SDL_MOUSEMOTION) {
        inputManager._cursorCoords.x = event.motion.xrel;
        inputManager._cursorCoords.y = -event.motion.yrel;
        inputManager._cursorIsMoving = true;
      }
    }

    if (game.IsDone() || inputManager.KeyPressed(key::esc)) {
      break;
    }

    game.ProcessInput(deltaTime);

    game.Update(deltaTime);

    game.Render(inputManager._cursorCoords);

    SDL_GL_SwapWindow(window);
  }

  return EXIT_SUCCESS;
}
