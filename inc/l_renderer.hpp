#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include <string>

namespace lain {

class shader;
class mesh;

namespace renderer {

void Initialise(float const width, float const height);

// ------------------------------------------------------------
// id -> program id, the one you created using glCreateProgram!
// ------------------------------------------------------------
void UseShader(unsigned int const id);

void SetUniformMat4(unsigned int const id, std::string const& uniname, glm::mat4 const& m);

void SetUniformVec2(unsigned int const id, std::string const& uniname, glm::vec2 const& value);

void SetUniformVec3(unsigned int const id, std::string const& uniname, glm::vec3 const& value);

void SetUniformVec4(unsigned int const id, std::string const& uniname, glm::vec4 const& value);

void SetUniformInt(unsigned int const id, std::string const& uniname, int const value);

void DrawMeshWithTexture(mesh const& mesh);

void DrawMeshWithoutTexture(mesh const& mesh);

void DrawLines(unsigned int const id, unsigned int const vao, std::size_t const count,
               glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

}; // namespace renderer

}; // namespace lain
