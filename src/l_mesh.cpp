#include "l_mesh.hpp"
#include "glad/glad.h"

namespace lain {

static void SetupMesh(mesh& mesh) {
  glGenVertexArrays(1, &mesh._vao);
  glGenBuffers(1, &mesh._vbo);
  glGenBuffers(1, &mesh._ebo);

  glBindVertexArray(mesh._vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh._vbo);

  glBufferData(GL_ARRAY_BUFFER, mesh._vertices.size() * sizeof(vertex_data), mesh._vertices.data(),
               GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh._ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh._indices.size() * sizeof(unsigned int),
               mesh._indices.data(), GL_STATIC_DRAW);

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
}

mesh::mesh(std::vector<vertex_data>&& vertices, std::vector<unsigned int>&& indices,
           std::vector<mesh_texture>&& textures, glm::vec3 const& diffuseColour, aabb&& boundingBox)
    : _vertices{vertices},
      _indices{indices},
      _textures{textures},
      _boundingBox{boundingBox},
      _diffuseColour{diffuseColour} {
  SetupMesh(*this);
}

} // namespace lain
