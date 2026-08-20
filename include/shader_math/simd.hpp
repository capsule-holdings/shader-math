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

// AVX1+FMA3 two-row packing for the float 4x4 matrix product. Kernel-internal
// only: freg stays __m128 and no public type changes width.
#if defined(SHADER_MATH_ISA_SSE2) && defined(SHADER_MATH_HAS_FMA) && defined(SHADER_MATH_HAS_AVX)
#  include "detail/simd_avx.hpp"
#  define SHADER_MATH_HAS_AVX_2ROW 1
#endif

// Half (binary16) layer: scalar type + 4-wide hreg/hops. Native 16-bit lanes
// only on ARM FEAT_FP16; everywhere else lanes are widened to f32.
#if defined(SHADER_MATH_HAS_HALF)
#  include "detail/half_type.hpp"
#  if defined(SHADER_MATH_HAS_FP16)
#    include "detail/simd_fp16_neon.hpp"
#  else
#    include "detail/simd_fp16_promote.hpp"
#  endif
#endif

#undef SHADER_MATH_SIMD_DISPATCH
