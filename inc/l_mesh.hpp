#pragma once

#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"

#include <string>
#include <vector>

namespace lain {

struct vertex_data final {
  glm::vec3 _position;
  glm::vec3 _normal;
  glm::vec2 _texCoords;
};

struct mesh_texture final {
  std::size_t _id;
  std::string _type;
  std::string _path; // cache
};

class mesh final {
public:
  mesh(std::vector<vertex_data>&& vertices, std::vector<unsigned int>&& indices, std::vector<mesh_texture>&& textures,
       glm::vec3 const& diffuseColour);

  std::vector<mesh_texture> const& GetTextures() const { return _textures; }

  std::vector<unsigned int> const& GetIndices() const { return _indices; }

  // TODO: stupid
  unsigned int GetVao() const { return _vao; }

  // TODO: can you be more stupid
  glm::vec3 const& GetDiffuseColour() const { return _diffuseColour; }

  bool HasTextures() const { return !_textures.empty(); }

private:
  void SetupMesh();

  std::vector<vertex_data> _vertices;
  std::vector<unsigned int> _indices;
  std::vector<mesh_texture> _textures;
  glm::vec3 _diffuseColour;
  unsigned int _vao;
  unsigned int _vbo;
  unsigned int _ebo;
};

}; // namespace lain
