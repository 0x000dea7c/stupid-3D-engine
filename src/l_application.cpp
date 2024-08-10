#include "l_application.hpp"
#include "SDL2/SDL.h"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_entity.hpp"
#include "l_game.hpp"
#include "l_input_manager.hpp"
#include "l_platform.hpp"
#include "l_renderer.hpp"
#include "l_resource_manager.hpp"
#include <chrono>
#include <iostream>

namespace lain {

static ecs::entity_component_system _ecs;

void ConstrainCursorInWindow() { SDL_SetRelativeMouseMode(SDL_TRUE); }

void ReleaseCursorFromWindow() { SDL_SetRelativeMouseMode(SDL_FALSE); }

namespace application {

// --------
// Constants
// --------
static char const* kWindowTitle{"STUPID ENGINE"};
static int constexpr kOpenGLMajorVersion{4};
static int constexpr kOpenGLMinorVersion{6};
static int constexpr kDepthBufferSize{24};
static int constexpr kDoubleBuffer{
    1}; // 1 means true, which means the output will be double buffered

// ---------------------------------------------------------------------.
//
// No one needs to know about this implementation, just use the functions that it offers in its
// interface
// -----------------------------------------------------------------------------
static int _width{0};
static int _height{0};
static bool _isInFullScreen{false};
static SDL_Window* _window;
static int _SDLWindowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED};
static SDL_GLContext _context;

// -------------------
// Internal functions
// -------------------

static float Time() {
  using namespace std::chrono;
  static const auto start = high_resolution_clock::now();
  return duration<float>(high_resolution_clock::now() - start).count();
}

static void InitialiseImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;

  ImGui_ImplSDL2_InitForOpenGL(_window, _context);
  ImGui_ImplOpenGL3_Init();
}

static input_manager::key SDLKeyToEngine(int const key) {
  switch (key) {
  case SDLK_w:
    return input_manager::key::w;
  case SDLK_s:
    return input_manager::key::s;
  case SDLK_a:
    return input_manager::key::a;
  case SDLK_d:
    return input_manager::key::d;
  case SDLK_q:
    return input_manager::key::q;
  case SDLK_e:
    return input_manager::key::e;
  case SDLK_k:
    return input_manager::key::k;
  case SDLK_F1:
    return input_manager::key::f1;
  case SDLK_F2:
    return input_manager::key::f2;
  case SDLK_ESCAPE:
    return input_manager::key::esc;
  default:
    return input_manager::key::unknown;
  }
}

static input_manager::mouse_button SDLMouseButtonToEngine(int const mouseButton) {
  switch (mouseButton) {
  case SDL_BUTTON_LEFT:
    return input_manager::mouse_button::left;
  default:
    return input_manager::mouse_button::unknown;
  }
}

bool Initialise(int const width, int const height, bool const fullScreen) {
  _width = static_cast<int>(width * 1.f);
  _height = static_cast<int>(height * 1.f);
  _isInFullScreen = fullScreen;

  // No need to sync C and C++ I/O streams, this might improve performance
  std::ios::sync_with_stdio(false);

  // -----
  // SDL
  // -----
  if (_isInFullScreen) {
    _SDLWindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << __FUNCTION__ << ": couldn't initialise SDL: " << SDL_GetError() << '\n';
    return false;
  }

  _window = SDL_CreateWindow(kWindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width,
                             _height, _SDLWindowFlags);

  if (_window == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't create SDL window: " << SDL_GetError() << '\n';
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, kOpenGLMajorVersion);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, kOpenGLMinorVersion);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, kDoubleBuffer);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, kDepthBufferSize);

  _context = SDL_GL_CreateContext(_window);

  SDL_GL_MakeCurrent(_window, _context);

  SDL_GL_SetSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << __FUNCTION__ << ": failed to initialise GLAD\n";
    return false;
  }

  // -----
  // ImGui
  // -----
  InitialiseImGui();

  // -------------
  // OpenGL stuff
  // -------------
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  // --------------------------
  // Game stuff initialisation
  // --------------------------
  game::Initialise(&_ecs);
  resource_manager::Initialise();
  renderer::Initialise(_width, _height, &_ecs);

  return true;
}

void ToggleFullScreen() {
  _isInFullScreen = !_isInFullScreen;

  int const flags{_isInFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0};

  SDL_SetWindowFullscreen(_window, flags);
}

void Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(_context);

  if (_window != nullptr) {
    SDL_DestroyWindow(_window);
  }

  SDL_Quit();
}

void Run() {
  float lastFrame{0.f};

  while (!game::IsShuttingDown()) {
    float const currFrame{Time()};
    float const deltaTime{currFrame - lastFrame};
    lastFrame = currFrame;

    input_manager::BeginFrame();

    glViewport(0, 0, _width, _height);

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_QUIT) {
        game::ForceShutdown();
      }

      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(_window)) {
        game::ForceShutdown();
      }

      if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto const key = SDLKeyToEngine(event.key.keysym.sym);
        bool const isPressed{event.type == SDL_KEYDOWN};
        input_manager::UpdateKey(key, isPressed);
      }

      if (event.type == SDL_MOUSEMOTION) {
        input_manager::UpdateCursorPosition(glm::vec2(event.motion.xrel, -event.motion.yrel));
        input_manager::SetCursorIsMoving();
      }

      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        input_manager::mouse_button const button{SDLMouseButtonToEngine(event.button.button)};
        input_manager::UpdateMouseButton(button, event.type == SDL_MOUSEBUTTONDOWN);
      }
    }

    game::ProcessInput();

    game::Update(deltaTime);

    game::Render();

    SDL_GL_SwapWindow(_window);
  }
}

float GetWindowWidth() { return static_cast<float>(_width); }

float GetWindowHeight() { return static_cast<float>(_height); }

}; // namespace application
}; // namespace lain
