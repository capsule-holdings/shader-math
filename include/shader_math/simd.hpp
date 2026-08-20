// 4-wide SIMD abstraction: NEON / SSE2 / scalar.
// All higher-level algorithms are written once against `ops`; this file only
// dispatches to the per-ISA implementation selected by config.hpp. Intrinsics
// live in the detail/simd_*.hpp implementation files.
#pragma once

#include "config.hpp"

#define SHADER_MATH_SIMD_DISPATCH 1

#if defined(SHADER_MATH_ISA_NEON)
#  include "detail/simd_neon.hpp"
#elif defined(SHADER_MATH_ISA_SSE2)
#  include "detail/simd_sse2.hpp"
#else
#  include "detail/simd_scalar.hpp"
#endif

#undef SHADER_MATH_SIMD_DISPATCH
