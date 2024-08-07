#pragma once

#include "glad/glad.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <functional>
#include <string>

namespace lain {

struct texture;
struct shader;
class mesh;

struct pair_hash final {
  unsigned int operator()(const std::pair<unsigned int, std::string>& arg) const {
    const auto hash1 = std::hash<unsigned int>{}(arg.first);
    const auto hash2 = std::hash<std::string>{}(arg.second);
    return hash1 ^ (hash2 << 1);
  }
};

class renderer {
public:
  renderer(float const width, float const height);

  void UseShader(unsigned int const id) const { glUseProgram(id); }

  void SetUniformMat4(unsigned int const id, std::string const& uniname, glm::mat4 const& m) const {
    glUniformMatrix4fv(GetUniformLocation(id, uniname), 1, false, glm::value_ptr(m));
  }

  void SetUniformVec2(unsigned int const id, std::string const& uniname, glm::vec2 const& value) const {
    glUniform2f(GetUniformLocation(id, uniname), value.x, value.y);
  }

  void SetUniformVec3(unsigned int const id, std::string const& uniname, glm::vec3 const& value) const {
    glUniform3f(GetUniformLocation(id, uniname), value.x, value.y, value.z);
  }

  void SetUniformVec4(unsigned int const id, std::string const& uniname, glm::vec4 const& value) const {
    glUniform4f(GetUniformLocation(id, uniname), value.x, value.y, value.z, value.w);
  }

  void SetUniformInt(unsigned int const id, std::string const& uniname, int const value) const {
    glUniform1i(GetUniformLocation(id, uniname), value);
  }

  void DrawMesh(shader const* shader, mesh const& mesh) const;

private:
  unsigned int GetUniformLocation(unsigned int const id, std::string const& uniname) const;

  // cached uniforms
  mutable std::unordered_map<std::pair<unsigned int, std::string>, unsigned int, pair_hash> _uniforms;

protected:
  glm::mat4 _perspective;
  glm::mat4 _orthographic;

private:
  // glm::vec2 _cursorSize;
  // shader const* _cursorShader;
  // texture const* _cursorTexture;
};

}; // namespace lain
