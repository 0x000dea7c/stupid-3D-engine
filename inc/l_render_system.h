#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "l_entity_system.h"
#include "l_types.h"
#include <string>
#include <vector>

namespace lain
{
  class shader;
  class mesh;
  class camera3D;
  class model;

  struct render_component final
  {
    model const* _data;
  };

  namespace render_system
  {
    void Initialise(f32 const width, f32 const height);

    void UseShader(u32 const id);

    void SetUniformMat4(u32 const id, std::string const& uniname, glm::mat4 const& m);

    void SetUniformVec2(u32 const id, std::string const& uniname, glm::vec2 const& value);

    void SetUniformVec3(u32 const id, std::string const& uniname, glm::vec3 const& value);

    void SetUniformVec4(u32 const id, std::string const& uniname, glm::vec4 const& value);

    void SetUniformInt(u32 const id, std::string const& uniname, int const value);

    void DrawEntities(camera3D const& camera);

    void DrawLines(u32 const id, u32 const vao, u32 const count,
		   glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

    void DrawBoundingBox(u32 const id, u32 const vao, glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

    void DrawBoundingBoxWithoutModel(u32 const id, u32 const vao, glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

    glm::mat4 GetCurrentProjectionMatrix();

    void AddEntity(render_component&& r);

    void SetEntity(entity_id const id, render_component&& r);

    void RemoveEntity(entity_id const id);

    void RemoveAllEntities();
  };
};
