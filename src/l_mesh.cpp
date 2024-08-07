#include "l_mesh.hpp"
#include "glad/glad.h"

namespace lain {

mesh::mesh(std::vector<vertex_data>&& vertices, std::vector<unsigned int>&& indices,
           std::vector<mesh_texture>&& textures, glm::vec3 const& diffuseColour)
    : _vertices{vertices},
      _indices{indices},
      _textures{textures},
      _diffuseColour{diffuseColour} {
  SetupMesh();
}

void mesh::SetupMesh() {
  unsigned int vao, vbo, ebo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(vertex_data), _vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data),
                        reinterpret_cast<void*>(offsetof(vertex_data, _position)));

  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data),
                        reinterpret_cast<void*>(offsetof(vertex_data, _normal)));

  // texcoords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data),
                        reinterpret_cast<void*>(offsetof(vertex_data, _texCoords)));

  _vao = vao;
  _vbo = vbo;
  _ebo = ebo;
}

} // namespace lain
