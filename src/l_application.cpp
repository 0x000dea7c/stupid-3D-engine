#include "l_application.hpp"
#include "glad/glad.h"
#include <sstream>

namespace lain {

// Pretty much copied this from Internet hehe
static void DebugMessageCallback(GLenum const source, GLenum const type, GLenum const id,
                                 GLenum const severity, [[maybe_unused]] GLsizei const length,
                                 GLchar const* message, [[maybe_unused]] void const* params) {
  // ignoring not-so-useful messages
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204 || id == 131222 ||
      id == 131154 || id == 0) {
    return;
  }

  std::stringstream debugMessageStream;
  debugMessageStream << message << '\n';

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    debugMessageStream << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    debugMessageStream << "Source: Window Manager";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    debugMessageStream << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    debugMessageStream << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    debugMessageStream << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    debugMessageStream << "Source: Other";
    break;
  }

  debugMessageStream << '\n';

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    debugMessageStream << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    debugMessageStream << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    debugMessageStream << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    debugMessageStream << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    debugMessageStream << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    debugMessageStream << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    debugMessageStream << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    debugMessageStream << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    debugMessageStream << "Type: Other";
    break;
  }

  debugMessageStream << '\n';

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    debugMessageStream << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    debugMessageStream << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    debugMessageStream << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    debugMessageStream << "Severity: notification";
    break;
  }
}

// You're multiplying by 0.8f to give some space for the window decoration?
application::application(int const width, int const height, bool const fullScreen)
    : _width{static_cast<int>(width * 0.8f)},
      _height{static_cast<int>(height * 0.8f)},
      _isFullScreen(fullScreen) {}

void application::Initialise() {
  glDebugMessageCallback(DebugMessageCallback, nullptr);

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glEnable(GL_CULL_FACE);
}

void application::ToggleFullScreen() { _isFullScreen = !_isFullScreen; }

}; // namespace lain
