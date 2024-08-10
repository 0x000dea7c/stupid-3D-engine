#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include <iostream>

inline void PrintVec3(std::string&& label, glm::vec3 const& v) {
  std::clog << label << std::endl;
  std::clog << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

inline void PrintVec4(std::string&& label, glm::vec4 const& v) {
  std::clog << label << std::endl;
  std::clog << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

inline void PrintFloat(std::string&& label, float const v) {
  std::clog << label << std::endl;
  std::clog << v << std::endl;
}
