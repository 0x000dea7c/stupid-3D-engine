#pragma once

namespace lain {

// ------------------------------------------------------------------------------------------------
// Initialises OpenGL settings and keeps track of current resolution. It handles full screen as
// well. The reason why you're not initialising SDL, glad, etc in here is because if you want to
// change from SDL2 from glfw or have your own OpenGL function pointers defined, it will be annoying
// to change, plus you never know if glfw will work well in the platform you're using, so...
// ------------------------------------------------------------------------------------------------
class application final {
public:
  application(int const width, int const height, bool const fullScreen);

  void Initialise();

  void ToggleFullScreen();

private:
  int _width;
  int _height;
  bool _isFullScreen;
};

}; // namespace lain
