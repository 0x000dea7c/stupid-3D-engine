#pragma once

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include "l_math.h"
#include <string>
#include <vector>

namespace lain
{
  struct vertex_data final
  {
    glm::vec3 _position;
    glm::vec3 _normal;
    glm::vec2 _texCoords;
  };

  struct mesh_texture final
  {
    u32 _id;
    std::string _type;
    std::string _path; // cache
  };

  struct mesh final
  {
    std::vector<vertex_data> _vertices;
    std::vector<u32> _indices;
    std::vector<mesh_texture> _textures;
    aabb _boundingBox;
    glm::vec3 _diffuseColour;
    u32 _vao;
    u32 _vbo;
    u32 _ebo;

    mesh(std::vector<vertex_data>&& vertices,
	 std::vector<u32>&& indices,
	 std::vector<mesh_texture>&& textures,
	 glm::vec3 const& diffuseColour,
	 aabb&& boundingBox);
  };
};
