#pragma once

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "l_math.hpp"
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

inline void PrintAABB(std::string&& label, lain::aabb const& aabb) {
  std::clog << label << std::endl;
  std::clog << "MIN" << std::endl;
  std::clog << "(" << aabb._min.x << ", " << aabb._min.y << ", " << aabb._min.z << ")" << std::endl;
  std::clog << "MAX" << std::endl;
  std::clog << "(" << aabb._max.x << ", " << aabb._max.y << ", " << aabb._max.z << ")" << std::endl;
}

inline void PrintRay(std::string&& label, lain::ray const& ray) {
  std::clog << label << std::endl;
  PrintVec3("DIRECTION", ray._direction);
  PrintVec3("POSITION", ray._position);
}
