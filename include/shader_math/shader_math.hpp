// shader-math: HLSL/GLSL compatible float vector & matrix library (C++20).
//
//   using namespace hlsl;  ->  float3, float4x4, * is component-wise, mul()
//   using namespace glsl;  ->  vec3, mat4, * is the matrix product
//
// Never import both namespaces into the same scope.
#pragma once

#include "glsl.hpp"
#include "hlsl.hpp"

// Convenience top-level namespace aliases. Define SHADER_MATH_NO_ALIAS before
// including this header if the short names collide with something else.
#ifndef SHADER_MATH_NO_ALIAS
namespace hlsl = ::shader_math::hlsl;
namespace glsl = ::shader_math::glsl;
#endif
