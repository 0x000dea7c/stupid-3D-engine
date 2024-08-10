#include "l_renderer.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "l_camera.hpp"
#include "l_common.hpp"
#include "l_mesh.hpp"
#include "l_resource_manager.hpp"
#include "l_shader.hpp"
#include <unordered_map>

namespace lain {

namespace renderer {

static ecs::entity_component_system* _ecs;

// utility class
struct pair_hash final {
  unsigned int operator()(const std::pair<unsigned int, std::string>& arg) const {
    const auto hash1 = std::hash<unsigned int>{}(arg.first);
    const auto hash2 = std::hash<std::string>{}(arg.second);
    return hash1 ^ (hash2 << 1);
  }
};

using cache_type =
    std::unordered_map<std::pair<unsigned int, std::string>, unsigned int, pair_hash>;

// ---------
// Constants
// ---------
static float constexpr kFovY{45.f};
static float constexpr kNearPlaneDistance{0.1f};
static float constexpr kFarPlaneDistance{1000.f};

// --------
// State
// --------
glm::mat4 _perspective;
cache_type _uniforms;
shader const* _meshWithTextureShader;
shader const* _meshWithoutTextureShader;

// ------------------
// Internal functions
// ------------------
static unsigned int GetUniformLocation(unsigned int const id, std::string const& uniname) {
  const auto key = std::make_pair(id, uniname);
  const auto it = _uniforms.find(key);

  if (it == _uniforms.end()) {
    auto const location = glGetUniformLocation(id, uniname.c_str());
    _uniforms[key] = location;
    return location;
  }

  return it->second;
}

// --------------------
// Public functions
// --------------------
void UseShader(unsigned int const id) { glUseProgram(id); }

void SetUniformMat4(unsigned int const id, std::string const& uniname, glm::mat4 const& m) {
  glUniformMatrix4fv(GetUniformLocation(id, uniname), 1, false, glm::value_ptr(m));
}

void SetUniformVec2(unsigned int const id, std::string const& uniname, glm::vec2 const& value) {
  glUniform2f(GetUniformLocation(id, uniname), value.x, value.y);
}

void SetUniformVec3(unsigned int const id, std::string const& uniname, glm::vec3 const& value) {
  glUniform3f(GetUniformLocation(id, uniname), value.x, value.y, value.z);
}

void SetUniformVec4(unsigned int const id, std::string const& uniname, glm::vec4 const& value) {
  glUniform4f(GetUniformLocation(id, uniname), value.x, value.y, value.z, value.w);
}

void SetUniformInt(unsigned int const id, std::string const& uniname, int const value) {
  glUniform1i(GetUniformLocation(id, uniname), value);
}

void Initialise(float const width, float const height, ecs::entity_component_system* ecs) {
  assert(ecs != nullptr);

  _ecs = ecs;

  _perspective =
      glm::perspective(glm::radians(kFovY), width / height, kNearPlaneDistance, kFarPlaneDistance);

  _meshWithTextureShader = resource_manager::GetShader(kLevelEditorModelWithTextureShaderId);

  _meshWithoutTextureShader = resource_manager::GetShader(kLevelEditorModelWithoutTextureShaderId);

  UseShader(_meshWithTextureShader->_id);
  SetUniformMat4(_meshWithTextureShader->_id, "projection", _perspective);

  UseShader(_meshWithoutTextureShader->_id);
  SetUniformMat4(_meshWithoutTextureShader->_id, "projection", _perspective);
}

static void DrawMeshWithTexture(mesh const& mesh) {
  std::size_t diffuseIndex{1}, specularIndex{1};
  std::string number;
  std::string name;

  for (std::size_t i{0}; i < mesh._textures.size(); ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    name = mesh._textures[i]._type;

    if (name == "textureDiffuse") {
      number = std::to_string(diffuseIndex++);
    } else if (name == "textureSpecular") {
      number = std::to_string(specularIndex++);
    }

    SetUniformInt(_meshWithTextureShader->_id, name + number, i);
    glBindTexture(GL_TEXTURE_2D, mesh._textures[i]._id);
  }

  glBindVertexArray(mesh._vao);
  glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
}

static void DrawMeshWithNoTexture(mesh const& mesh) {
  SetUniformVec3(_meshWithoutTextureShader->_id, "diffuseColour", mesh._diffuseColour);

  glBindVertexArray(mesh._vao);
  glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
}

void DrawEntities(std::vector<entity_id> const& entities, camera3D const& camera) {
  glm::mat4 const viewMatrix{camera.GetViewMatrix()};

  UseShader(_meshWithTextureShader->_id);
  SetUniformMat4(_meshWithTextureShader->_id, "view", viewMatrix);

  UseShader(_meshWithoutTextureShader->_id);
  SetUniformMat4(_meshWithoutTextureShader->_id, "view", viewMatrix);

  for (auto const& entity : entities) {
    glm::mat4 const modelMatrix{_ecs->GetTransformComponent(entity).GetModel()};

    UseShader(_meshWithTextureShader->_id);
    SetUniformMat4(_meshWithTextureShader->_id, "model", modelMatrix);

    UseShader(_meshWithoutTextureShader->_id);
    SetUniformMat4(_meshWithoutTextureShader->_id, "model", modelMatrix);

    auto const& meshes = resource_manager::GetModelDataFromEntity(entity)->_meshes;

    for (auto const& mesh : meshes) {
      if (!mesh._textures.empty()) {
        UseShader(_meshWithTextureShader->_id);
        DrawMeshWithTexture(mesh);
      } else {
        UseShader(_meshWithoutTextureShader->_id);
        DrawMeshWithNoTexture(mesh);
      }
    }
  }
}

void DrawLines(unsigned int const id, unsigned int const vao, std::size_t const count,
               glm::mat4 const& view, glm::vec4 const& colour) {
  UseShader(id);

  SetUniformMat4(id, "projection", _perspective);
  SetUniformMat4(id, "view", view);
  SetUniformMat4(id, "model", glm::mat4(1.f));
  SetUniformVec4(id, "colour", colour);

  glBindVertexArray(vao);
  glDrawArrays(GL_LINES, 0, count);
}

}; // namespace renderer

}; // namespace lain
