#pragma once

#include "l_types.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include <cstddef>
#include <cstring>

namespace lain {

  // -------------------------------------------------------
  // please don't look at this file it's embarrasing
  // -------------------------------------------------------

  constexpr static std::size_t CompileTimeStringLength(char const* str)
  {
    u32 len{0};

    while (*str++) {
      ++len;
    }

    return len;
  }

  constexpr static i32 fnv1a(char const* str, u32 const len)
  {
    i32 constexpr prime{0x01000193}; // FNV-1a 32-bit prime
    u32 hash{0x811c9dc5};   // FNV-1a 32-bit offset basis

    for (std::size_t i{0}; i < len; ++i) {
      hash ^= static_cast<u32>(str[i]);
      hash *= prime;
    }

    return hash;
  };

  i32 constexpr kPrimitiveShaderId{fnv1a("PShader", CompileTimeStringLength("PShader"))};

  i32 constexpr kLevelEditorModelWithTextureShaderId{fnv1a("LEModelWTex", CompileTimeStringLength("LEModelWTex"))};

  i32 constexpr kLevelEditorModelWithoutTextureShaderId{fnv1a("LEModelWOTex",CompileTimeStringLength("LEModelWOTex"))};

  i32 constexpr kBoundingBoxShaderId{fnv1a("BBShader", CompileTimeStringLength("BBShader"))};
};
