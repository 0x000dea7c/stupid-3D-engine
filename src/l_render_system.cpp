#include "l_render_system.h"
#include "glad/glad.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "l_camera.h"
#include "l_common.h"
#include "l_mesh.h"
#include "l_resource_manager.h"
#include "l_shader.h"
#include "l_transform_system.h"
#include <unordered_map>

namespace lain
{
  namespace render_system
  {
    // utility class
    struct pair_hash final
    {
      u32 operator()(const std::pair<u32, std::string>& arg) const
      {
	const auto hash1 = std::hash<u32>{}(arg.first);
	const auto hash2 = std::hash<std::string>{}(arg.second);
	return hash1 ^ (hash2 << 1);
      }
    };

    using cache_type
    = std::unordered_map<std::pair<u32, std::string>, u32, pair_hash>;

    static f32 constexpr kFovY{45.f};
    static f32 constexpr kNearPlaneDistance{0.1f};
    static f32 constexpr kFarPlaneDistance{1000.f};
    static glm::mat4 _perspective;
    static cache_type _uniforms;
    static shader const* _meshWithTextureShader;
    static shader const* _meshWithoutTextureShader;
    static std::vector<render_component> _entities;

    static u32 GetUniformLocation(u32 const id, std::string const& uniname);
    static void DrawMeshWithTexture(mesh const& mesh);
    static void DrawMeshWithNoTexture(mesh const& mesh);

    void Initialise(f32 const width, f32 const height)
    {
      _perspective = glm::perspective(glm::radians(kFovY), width / height, kNearPlaneDistance, kFarPlaneDistance);

      _meshWithTextureShader = resource_manager::GetShader(kLevelEditorModelWithTextureShaderId);

      _meshWithoutTextureShader = resource_manager::GetShader(kLevelEditorModelWithoutTextureShaderId);

      UseShader(_meshWithTextureShader->_id);
      SetUniformMat4(_meshWithTextureShader->_id, "projection", _perspective);

      UseShader(_meshWithoutTextureShader->_id);
      SetUniformMat4(_meshWithoutTextureShader->_id, "projection", _perspective);
    }

    void UseShader(u32 const id)
    {
      glUseProgram(id);
    }

    void SetUniformMat4(u32 const id, std::string const& uniname, glm::mat4 const& m)
    {
      glUniformMatrix4fv(GetUniformLocation(id, uniname), 1, false, glm::value_ptr(m));
    }

    void SetUniformVec2(u32 const id, std::string const& uniname, glm::vec2 const& value)
    {
      glUniform2f(GetUniformLocation(id, uniname), value.x, value.y);
    }

    void SetUniformVec3(u32 const id, std::string const& uniname, glm::vec3 const& value)
    {
      glUniform3f(GetUniformLocation(id, uniname), value.x, value.y, value.z);
    }

    void SetUniformVec4(u32 const id, std::string const& uniname, glm::vec4 const& value)
    {
      glUniform4f(GetUniformLocation(id, uniname), value.x, value.y, value.z, value.w);
    }

    void SetUniformInt(u32 const id, std::string const& uniname, int const value)
    {
      glUniform1i(GetUniformLocation(id, uniname), value);
    }

    void DrawEntities(camera3D const& camera)
    {
      glm::mat4 const viewMatrix{camera.GetViewMatrix()};

      UseShader(_meshWithTextureShader->_id);
      SetUniformMat4(_meshWithTextureShader->_id, "view", viewMatrix);

      UseShader(_meshWithoutTextureShader->_id);
      SetUniformMat4(_meshWithoutTextureShader->_id, "view", viewMatrix);

      for (u32 i{0}; i < _entities.size(); ++i) {
	auto const model = transform_system::GetTransform(i)._model;

	UseShader(_meshWithTextureShader->_id);
	SetUniformMat4(_meshWithTextureShader->_id, "model", model);

	UseShader(_meshWithoutTextureShader->_id);
	SetUniformMat4(_meshWithoutTextureShader->_id, "model", model);

	for (auto const& mesh : _entities[i]._data->_meshes) {
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

    void DrawLines(u32 const id, u32 const vao, u32 const count,
		   glm::mat4 const& view, glm::vec4 const& colour)
    {
      UseShader(id);

      SetUniformMat4(id, "projection", _perspective);
      SetUniformMat4(id, "view", view);
      SetUniformMat4(id, "model", glm::mat4(1.f));
      SetUniformVec4(id, "colour", colour);

      glBindVertexArray(vao);
      glDrawArrays(GL_LINES, 0, count);
    }

    void DrawBoundingBox(u32 const id, u32 const vao, glm::mat4 const& view, glm::vec4 const& colour)
    {
      UseShader(id);

      SetUniformMat4(id, "projection", _perspective);
      SetUniformMat4(id, "view", view);
      SetUniformMat4(id, "model", glm::mat4{1.f});
      SetUniformVec4(id, "colour", colour);

      glBindVertexArray(vao);
      glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    }

    void DrawBoundingBoxWithoutModel(u32 const id, u32 const vao,
				     glm::mat4 const& view, glm::vec4 const& colour)
    {
      UseShader(id);

      SetUniformMat4(id, "projection", _perspective);
      SetUniformMat4(id, "view", view);
      SetUniformMat4(id, "model", glm::mat4(1.f));
      SetUniformVec4(id, "colour", colour);

      glBindVertexArray(vao);
      glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
    }

    glm::mat4 GetCurrentProjectionMatrix()
    {
      return _perspective;
    }

    void AddEntity(render_component&& r)
    {
      _entities.emplace_back(r);
    }

    void SetEntity(entity_id const id, render_component&& r)
    {
      _entities[id] = std::move(r);
    }

    void RemoveEntity(entity_id const id)
    {
      _entities.erase(_entities.begin() + id);
    }

    void RemoveAllEntities()
    {
      _entities.clear();
    }

    static void DrawMeshWithTexture(mesh const& mesh)
    {
      u32 diffuseIndex{1}, specularIndex{1};
      std::string number;
      std::string name;

      for (u32 i{0}; i < mesh._textures.size(); ++i) {
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

    static void DrawMeshWithNoTexture(mesh const& mesh)
    {
      SetUniformVec3(_meshWithoutTextureShader->_id, "diffuseColour", mesh._diffuseColour);
      glBindVertexArray(mesh._vao);
      glDrawElements(GL_TRIANGLES, mesh._indices.size(), GL_UNSIGNED_INT, 0);
    }

    static u32 GetUniformLocation(u32 const id, std::string const& uniname)
    {
      const auto key = std::make_pair(id, uniname);
      const auto it = _uniforms.find(key);

      if (it == _uniforms.end()) {
	auto const location = glGetUniformLocation(id, uniname.c_str());
	_uniforms[key] = location;
	return location;
      }

      return it->second;
    }
  };
};
