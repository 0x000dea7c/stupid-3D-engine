#include "l_application.h"
#include "SDL2/SDL.h"
#include "SDL_video.h"
#include "glad/glad.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include "l_game.h"
#include "l_input_manager.h"
#include "l_platform.h"
#include "l_render_system.h"
#include "l_resource_manager.h"
#include "l_physics_system.h"
#include "l_transform_system.h"
#include <chrono>
#include <iostream>

namespace lain
{
  namespace application
  {
    static char const* kWindowTitle{"STUPID ENGINE"};
    static i32 constexpr kOpenGLMajorVersion{4};
    static i32 constexpr kOpenGLMinorVersion{6};
    static i32 constexpr kDepthBufferSize{24};
    static i32 constexpr kDoubleBuffer{1}; // 1 means true, which means the output will be double buffered

    static i32 _width{0};
    static i32 _height{0};
    static bool _isInFullScreen{false};
    static SDL_Window* _window;
    static i32 _SDLWindowFlags{SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED};
    static SDL_GLContext _context;

    static f32 Time();
    static void InitialiseImGui();
    static input_manager::key SDLKeyToEngine(i32 key);
    static input_manager::mouse_button SDLMouseButtonToEngine(i32 mouseButton);

    bool Initialise(bool fullScreen)
    {
      _isInFullScreen = fullScreen;

      // No need to sync C and C++ I/O streams, this might improve performance.
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

      SDL_DisplayMode displayMode;

      if (SDL_GetDesktopDisplayMode(0, &displayMode) != 0) {
	std::cerr << __FUNCTION__ << ": couldn't get desktop display mode: " << SDL_GetError() << '\n';
	return false;
      }

      _width = displayMode.w;
      _height = displayMode.h;

      _window = SDL_CreateWindow(kWindowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _width, _height, _SDLWindowFlags);

      if (_window == nullptr) {
	std::cerr << __FUNCTION__ << ": couldn't create SDL window: " << SDL_GetError() << '\n';
	return false;
      }

      // XXX: I don't think you need to do this, there's probably a way to create the window with the right resolution
      auto displayIndex = SDL_GetWindowDisplayIndex(_window);

      if (displayIndex < 0) {
	std::cerr << __FUNCTION__ << ": couldn't get display index info: " << SDL_GetError() << '\n';
	return false;
      }

      if (SDL_GetDesktopDisplayMode(displayIndex, &displayMode) != 0) {
	std::cerr << __FUNCTION__ << ": couldn't get desktop display mode: " << SDL_GetError() << '\n';
	return false;
      }

      _width = displayMode.w;
      _height = displayMode.h;

      SDL_SetWindowSize(_window, _width, _height);

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
      game::Initialise();
      resource_manager::Initialise();
      render_system::Initialise(_width, _height);

      return true;
    }

    void ToggleFullScreen()
    {
      _isInFullScreen = !_isInFullScreen;
      i32 const flags{_isInFullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0};
      SDL_SetWindowFullscreen(_window, flags);
    }

    void Shutdown()
    {
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();

      SDL_GL_DeleteContext(_context);

      if (_window != nullptr) {
	SDL_DestroyWindow(_window);
      }

      SDL_Quit();
    }

    void Run()
    {
      f32 lastFrame{0.f};

      while (!game::IsShuttingDown()) {
	f32 const currFrame{Time()};
	f32 const deltaTime{currFrame - lastFrame};
	lastFrame = currFrame;

	input_manager::BeginFrame();

	glViewport(0, 0, _width, _height);

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
	  ImGui_ImplSDL2_ProcessEvent(&event);

	  if (event.type == SDL_QUIT) {
	    game::ForceShutdown();
	  }

	  bool const userForcesShutdown{event.type == SDL_WINDOWEVENT &&
					event.window.event == SDL_WINDOWEVENT_CLOSE &&
					event.window.windowID == SDL_GetWindowID(_window)};

	  if (userForcesShutdown) {
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

    f32 GetWindowWidth()
    {
      return static_cast<f32>(_width);
    }

    f32 GetWindowHeight()
    {
      return static_cast<f32>(_height);
    }

    void SetWindowTitle(std::string&& newTitle)
    {
      SDL_SetWindowTitle(_window, newTitle.c_str());
    }

    static f32 Time()
    {
      using namespace std::chrono;
      static const auto start = high_resolution_clock::now();
      return duration<f32>(high_resolution_clock::now() - start).count();
    }

    static void InitialiseImGui()
    {
      IMGUI_CHECKVERSION();
      ImGui::CreateContext();
      ImGuiIO& io = ImGui::GetIO();
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
      io.ConfigFlags |= ImGuiConfigFlags_NavEnableSetMousePos;
      ImGui_ImplSDL2_InitForOpenGL(_window, _context);
      ImGui_ImplOpenGL3_Init();
    }

    static input_manager::key SDLKeyToEngine(i32 key) {
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
      case SDLK_b:
	return input_manager::key::b;
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

    static input_manager::mouse_button SDLMouseButtonToEngine(i32 mouseButton)
    {
      switch (mouseButton) {
      case SDL_BUTTON_LEFT:
	return input_manager::mouse_button::left;
      default:
	return input_manager::mouse_button::unknown;
      }
    }
  };

  void ConstrainCursorInWindow()
  {
    SDL_SetRelativeMouseMode(SDL_TRUE);
  }

  void ReleaseCursorFromWindow()
  {
    SDL_SetRelativeMouseMode(SDL_FALSE);
  }
};
