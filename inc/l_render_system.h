#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "l_entity_system.h"
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
        void Initialise(float const width, float const height);

        void UseShader(unsigned int const id);

        void SetUniformMat4(unsigned int const id, std::string const& uniname, glm::mat4 const& m);

        void SetUniformVec2(unsigned int const id, std::string const& uniname, glm::vec2 const& value);

        void SetUniformVec3(unsigned int const id, std::string const& uniname, glm::vec3 const& value);

        void SetUniformVec4(unsigned int const id, std::string const& uniname, glm::vec4 const& value);

        void SetUniformInt(unsigned int const id, std::string const& uniname, int const value);

        void DrawEntities(camera3D const& camera);

        void DrawLines(unsigned int const id, unsigned int const vao, std::size_t const count,
                       glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

        void DrawBoundingBox(unsigned int const id, unsigned int const vao, glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

        void DrawBoundingBoxWithoutModel(unsigned int const id, unsigned int const vao, glm::mat4 const& view, glm::vec4 const& colour = glm::vec4(1.f));

        glm::mat4 GetCurrentProjectionMatrix();

        void AddEntity(render_component&& r);

        void SetEntity(entity_id const id, render_component&& r);

        void RemoveEntity(entity_id const id);

        void RemoveAllEntities();
    };
};
