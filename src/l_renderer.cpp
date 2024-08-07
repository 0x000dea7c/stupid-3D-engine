#include "l_renderer.hpp"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "l_mesh.hpp"
#include "l_shader.hpp"

namespace lain {

renderer::renderer(float const width, float const height)
    : _perspective{glm::perspective(glm::radians(45.f), width / height, 0.1f, 100.f)},
      _orthographic{glm::ortho(0.f, width, height, 0.f, -1.f, 1.f)} {
  // cursor stuff
  // _cursorShader = _resourceManager.GetShader(cursorId);
  // _cursorTexture = _resourceManager.GetTexture(cursorId);
  // _cursorSize = glm::vec2(_cursorTexture->_width, _cursorTexture->_height);
  // _resourceManager.UseShader(_cursorShader->_id);
  // _resourceManager.SetUniformMat4(_cursorShader->_id, "projection", _orthographic);
  // _resourceManager.SetUniformVec2(_cursorShader->_id, "cursorSize", _cursorSize);
  // _resourceManager.SetUniformInt(_cursorShader->_id, "image", 0);
}

unsigned int renderer::GetUniformLocation(unsigned int const id, std::string const& uniname) const {
  const auto key = std::make_pair(id, uniname);
  const auto it = _uniforms.find(key);

  if (it == _uniforms.end()) {
    auto const location = glGetUniformLocation(id, uniname.c_str());
    _uniforms[key] = location;
    return location;
  }

  return it->second;
}

void renderer::DrawMesh(shader const* shader, mesh const& mesh) const {
  std::size_t diffuseIndex{1}, specularIndex{1};
  auto const& textures = mesh.GetTextures();
  auto const& indicesSize = mesh.GetIndices().size();
  std::string number;
  std::string name;

  // TODO: this piece of code smells bad
  if (!textures.empty()) {
    for (std::size_t i{0}; i < textures.size(); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      name = textures[i]._type;

      if (name == "textureDiffuse") {
        number = std::to_string(diffuseIndex++);
      } else if (name == "textureSpecular") {
        number = std::to_string(specularIndex++);
      }

      // SetUniformInt(shader->_id, "material." + name + number, i);
      SetUniformInt(shader->_id, name + number, i);
      glBindTexture(GL_TEXTURE_2D, textures[i]._id);
    }
  } else {
    SetUniformVec3(shader->_id, "diffuseColour", mesh.GetDiffuseColour());
  }

  // TODO: you are definitely stupid glBindVertexArray(shader->_vao);
  glBindVertexArray(mesh.GetVao());
  glDrawElements(GL_TRIANGLES, indicesSize, GL_UNSIGNED_INT, 0);
}

}; // namespace lain
