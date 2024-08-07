#include "l_resource_manager.hpp"
#include "l_entity.hpp"
#include "l_level_editor.hpp"
#include "l_shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "l_common.hpp"

namespace lain {

// -----------------------------------------------------------------------------------------------
// INTERNAL STATELESS
// -----------------------------------------------------------------------------------------------
static bool ShaderHasCompilationErrors(unsigned int const program, shader_type const type) {
  int success{0};
  std::array<char, 512> log{'\0'};

  switch (type) {
  case shader_type::vertex:
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
      glGetShaderInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't compile vertex shader:" << log.data() << '\n';
      return true;
    }
    break;
  case shader_type::fragment:
    glGetShaderiv(program, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
      glGetShaderInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't compile fragment shader: " << log.data() << '\n';
      return true;
    }
    break;
  case shader_type::program:
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
      glGetProgramInfoLog(program, log.size(), nullptr, log.data());
      std::cerr << __FUNCTION__ << ": couldn't link program: " << log.data() << '\n';
      return true;
    }
    break;
  }

  return false;
}

// -----------------------------------------------------------------------------------------------
// PUBLIC
// -----------------------------------------------------------------------------------------------
unsigned int resource_manager::CompileAndLinkShaders(std::filesystem::path const& vertex,
                                                     std::filesystem::path const& fragment) {
  std::ifstream vertfs(vertex);
  std::ifstream fragfs(fragment);

  if (!vertfs) {
    std::cerr << __FUNCTION__ << ": couldn't open vertex file: " << vertex << '\n';
    return false;
  }

  if (!fragfs) {
    std::cerr << __FUNCTION__ << ": couldn't open fragment file: " << fragment << '\n';
    return false;
  }

  std::stringstream vertss;
  std::stringstream fragss;
  vertss << vertfs.rdbuf();
  fragss << fragfs.rdbuf();

  std::string const vertcode{vertss.str()};
  std::string const fragcode{fragss.str()};

  char const* vertcodec{vertcode.c_str()};
  char const* fragcodec{fragcode.c_str()};

  unsigned int vertexShaderId{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertexShaderId, 1, &vertcodec, nullptr);
  glCompileShader(vertexShaderId);
  if (ShaderHasCompilationErrors(vertexShaderId, shader_type::vertex)) {
    glDeleteShader(vertexShaderId);
    return 0;
  }

  unsigned int fragmentShaderId{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragmentShaderId, 1, &fragcodec, nullptr);
  glCompileShader(fragmentShaderId);
  if (ShaderHasCompilationErrors(fragmentShaderId, shader_type::fragment)) {
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return 0;
  }

  unsigned int shaderProgram{glCreateProgram()};
  glAttachShader(shaderProgram, vertexShaderId);
  glAttachShader(shaderProgram, fragmentShaderId);
  glLinkProgram(shaderProgram);
  if (ShaderHasCompilationErrors(shaderProgram, shader_type::program)) {
    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
    return 0;
  }

  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);

  return shaderProgram;
}

bool resource_manager::Initialise(level_editor& levelEditor) {
  // ---------------------------------------------------------------------------------------------
  // shaders
  // ---------------------------------------------------------------------------------------------
  if (!CreateGridAxisLineShader("./res/shaders/GridAxisLine.vert",
                                "./res/shaders/GridAxisLine.frag", gridAxisLineShaderId)) {
    std::cerr << __FUNCTION__ << ": couldn't create grid axis line shader\n";
    return false;
  }

  if (!CreateGridCheckerboardShader("./res/shaders/GridAxisLine.vert",
                                    "./res/shaders/GridAxisLine.frag", gridAxisCheckerboardShaderId,
                                    levelEditor)) {
    std::cerr << __FUNCTION__ << ": couldn't create grid axis checkerboard shader\n";
    return false;
  }

  if (!CreateCursorShader("./res/shaders/Cursor.vert", "./res/shaders/Cursor.frag", cursorId)) {
    std::cerr << __FUNCTION__ << ": couldn't create cursor shader\n";
    return false;
  }

  // KLUDGE!
  unsigned int id{CompileAndLinkShaders("./res/shaders/LevelEditor_ModelWithTextures.vert",
                                        "./res/shaders/LevelEditor_ModelWithTextures.frag")};

  if (id == 0) {
    std::cerr << __FUNCTION__
              << ": couldn't create shader for models with textures in object editor\n";
    return false;
  }

  _shaders[levelEditorModelWithTextureShaderId] = std::make_unique<shader>(id);

  id = CompileAndLinkShaders("./res/shaders/LevelEditor_ModelWithoutTextures.vert",
                             "./res/shaders/LevelEditor_ModelWithoutTextures.frag");

  if (id == 0) {
    std::cerr << __FUNCTION__
              << ": couldn't create shader for models without textures in object editor\n";
    return false;
  }

  _shaders[levelEditorModelWithoutTextureShaderId] = std::make_unique<shader>(id);

  // ---------------------------------------------------------------------------------------------
  // textures
  // ---------------------------------------------------------------------------------------------
  if (!LoadTextureFromFile("./res/misc/cursor2.png", false, GL_REPEAT, GL_REPEAT, cursorId)) {
    std::cerr << __FUNCTION__ << ": couldn't load cursor image.\n";
    return false;
  }

  return true;
}

bool resource_manager::CreateGridAxisLineShader(std::filesystem::path const& vertex,
                                                std::filesystem::path const& fragment,
                                                int const id) {
  auto const shaderId = CompileAndLinkShaders(vertex, fragment);

  if (shaderId == 0) {
    std::cerr << __FUNCTION__ << ": couldn't compile and link shaders.\n";
    return false;
  }

  float constexpr axesVertices[] = {
      0.f,    0.01f, 0.f,    // origin
      1000.f, 0.01f, 0.f,    // X
      0.f,    0.01f, 0.f,    // origin
      0.f,    0.01f, 1000.f, // Z
  };

  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(axesVertices), axesVertices, GL_STATIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  _shaders[id] = std::make_unique<shader>(shaderId, vao, vbo);

  return true;
}

bool resource_manager::CreateGridCheckerboardShader(std::filesystem::path const& vertex,
                                                    std::filesystem::path const& fragment,
                                                    int const id, level_editor& levelEditor) {
  unsigned int const shaderId{CompileAndLinkShaders(vertex, fragment)};

  if (shaderId == 0) {
    std::cerr << __FUNCTION__ << ": couldn't compile and link shaders.\n";
    return false;
  }

  int const squares{levelEditor.GetSquaresToDraw()};

  std::vector<float> gridVertices;

  for (int i{-squares}; i <= squares; ++i) {
    // lines along the X axis
    gridVertices.push_back(-levelEditor.GetHalfGridExtent() * 2);
    gridVertices.push_back(0.f);
    gridVertices.push_back(i * levelEditor.GetGridSquareSize());

    gridVertices.push_back(levelEditor.GetHalfGridExtent() * 2);
    gridVertices.push_back(0.f);
    gridVertices.push_back(i * levelEditor.GetGridSquareSize());

    // lines along the Z axis
    gridVertices.push_back(i * levelEditor.GetGridSquareSize());
    gridVertices.push_back(0.f);
    gridVertices.push_back(-levelEditor.GetHalfGridExtent() * 2);

    gridVertices.push_back(i * levelEditor.GetGridSquareSize());
    gridVertices.push_back(0.f);
    gridVertices.push_back(levelEditor.GetHalfGridExtent() * 2);
  }

  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(),
               GL_STATIC_DRAW);

  // positions
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  _shaders[id] = std::make_unique<shader>(shaderId, vao, vbo);

  return true;
}

bool resource_manager::CreateCursorShader(std::filesystem::path const& vertex,
                                          std::filesystem::path const& fragment, int const id) {
  auto const shaderId = CompileAndLinkShaders(vertex, fragment);

  if (shaderId == 0) {
    std::cerr << __FUNCTION__ << ": couldn't compile and link shaders.\n";
    return false;
  }

  // quad and texcoords
  float constexpr data[] = {-0.5f, 0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.0f, 0.0f,
                            0.5f,  -0.5f, 1.0f, 0.0f, -0.5f, 0.5f,  0.0f, 1.0f,
                            0.5f,  -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  1.0f, 1.0f};

  unsigned int vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

  // positions
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // texcoords
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  _shaders[id] = std::make_unique<shader>(shaderId, vao, vbo);

  return true;
}

bool resource_manager::LoadTextureFromFile(std::filesystem::path const& file, bool const flip,
                                           int const wrapS, int const wrapT, int const id) {
  int width, height, channels;

  if (flip) {
    stbi_set_flip_vertically_on_load(true);
  }

  unsigned char* data{stbi_load(file.c_str(), &width, &height, &channels, 0)};

  if (data == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load image " << file << '\n';
    return false;
  }

  GLenum format;

  switch (channels) {
  case 1:
    format = GL_RED;
    break;
  case 2:
    format = GL_RG;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  // don't assume dimensions are multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  unsigned int glTexId;
  glGenTextures(1, &glTexId);
  glBindTexture(GL_TEXTURE_2D, glTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  if (flip) {
    stbi_set_flip_vertically_on_load(false);
  }

  _textures[id] = std::make_unique<texture>(glTexId, width, height, channels);

  return true;
}

entity_id resource_manager::SpawnEntityFromModelType(model_type const type) {
  entity_id const entityId{GetNewEntityId()};

  // cache
  if (_models.find(type) == _models.end()) {
    auto newModel = std::make_unique<model>();

    if (!newModel->LoadModel(type)) {
      std::clog << __FUNCTION__ << ": couldn't load model" << std::endl;
      return 0;
    }

    _models[type] = std::move(newModel);
  }

  _entityModelRelationship[entityId] = type;

  return entityId;
}

model const* resource_manager::GetModelFromEntity(entity_id const id) const {
  return _models.at(_entityModelRelationship.at(id)).get();
}

unsigned int resource_manager::LoadTextureFromFile(std::filesystem::path const& file) {
  int width, height, channels;

  unsigned char* data{stbi_load(file.c_str(), &width, &height, &channels, 0)};

  if (data == nullptr) {
    std::cerr << __FUNCTION__ << ": couldn't load image " << file << '\n';
    return 0;
  }

  GLenum format;

  switch (channels) {
  case 1:
    format = GL_RED;
    break;
  case 2:
    format = GL_RG;
    break;
  case 3:
    format = GL_RGB;
    break;
  case 4:
    format = GL_RGBA;
    break;
  }

  // don't assume dimensions are multiple of 4
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  unsigned int glTexId;
  glGenTextures(1, &glTexId);
  glBindTexture(GL_TEXTURE_2D, glTexId);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return glTexId;
}

}; // namespace lain
