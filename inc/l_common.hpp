#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include <cstddef>
#include <cstring>

namespace lain {

// -------------------------------------------------------
// please don't look at this file it's embarrasing
// -------------------------------------------------------

constexpr static std::size_t CompileTimeStringLength(char const* str) {
  std::size_t len{0};

  while (*str++) {
    ++len;
  }

  return len;
}

constexpr static int fnv1a(char const* str, std::size_t const len) {
  int constexpr prime{0x01000193}; // FNV-1a 32-bit prime
  unsigned int hash{0x811c9dc5};   // FNV-1a 32-bit offset basis

  for (std::size_t i{0}; i < len; ++i) {
    hash ^= static_cast<unsigned int>(str[i]);
    hash *= prime;
  }

  return hash;
};

int constexpr kPrimitiveShaderId{
    fnv1a("PrimitiveShader", CompileTimeStringLength("PrimitiveShader"))};

int constexpr kLevelEditorModelWithTextureShaderId{
    fnv1a("LevelEditorModelWithTexture", CompileTimeStringLength("LevelEditorModelWithTexture"))};

int constexpr kLevelEditorModelWithoutTextureShaderId{
    fnv1a("LevelEditorModelWithoutTextureShader",
          CompileTimeStringLength("LevelEditorModelWithoutTextureShader"))};

} // namespace lain
