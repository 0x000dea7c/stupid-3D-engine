#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"

namespace lain
{
    // ------------------------------
    // Geometry and math misc
    // ------------------------------
    struct ray final
    {
        glm::vec3 _position;
        glm::vec3 _direction;
    };

    struct aabb final
    {
        glm::vec3 _min;
        glm::vec3 _max;
    };

    bool RayIntersectsAABB(ray const& ray, aabb const& aabb);

    glm::vec4 ScreenSpaceToNormalisedDeviceCoordinates(glm::vec4 const& pos,
                                                       float const width,
                                                       float const height);

    glm::vec4 NormalisedDeviceCoordinatesToClipSpace(glm::vec4 const& pos);

    glm::vec4 ClipSpaceToViewSpace(glm::vec4 const& pos, glm::mat4 const& projection);

    glm::vec4 ViewSpaceToWorldSpace(glm::vec4 const& pos, glm::mat4 const& view);
};
