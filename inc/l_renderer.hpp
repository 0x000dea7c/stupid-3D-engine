#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "l_entity.hpp"
#include <string>
#include <vector>

namespace lain {

class shader;
class mesh;
class camera3D;

namespace renderer {

void Initialise(float const width, float const height, ecs::entity_component_system* ecs);

// ------------------------------------------------------------
// id -> program id, the one you created using glCreateProgram!
// ------------------------------------------------------------
void UseShader(unsigned int const id);

void SetUniformMat4(unsigned int const id, std::string const& uniname, glm::mat4 const& m);

void SetUniformVec2(unsigned int const id, std::string const& uniname, glm::vec2 const& value);

void SetUniformVec3(unsigned int const id, std::string const& uniname, glm::vec3 const& value);

void SetUniformVec4(unsigned int const id, std::string const& uniname, glm::vec4 const& value);

void SetUniformInt(unsigned int const id, std::string const& uniname, int const value);

void DrawEntities(std::vector<entity_id> const& entities, camera3D const& camera);

void DrawLines(unsigned int const id, unsigned int const vao, std::size_t const count,
               glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

glm::mat4 GetCurrentProjectionMatrix();

}; // namespace renderer

}; // namespace lain
