// shader-math: HLSL/GLSL compatible vector & matrix library.
// ISA detection and portability macros. Boost is used for detection only.
#pragma once

#include <boost/config.hpp>
#include <boost/predef.h>

#define SHADER_MATH_INLINE BOOST_FORCEINLINE

// ISA selection (compile-time, one per translation unit set):
//   SHADER_MATH_FORCE_SCALAR  - user override, scalar 4-wide reference path
//   NEON                      - AArch64 / ARM with NEON
//   SSE2                      - x86/x64 with SSE2 (baseline on x64)
//   scalar                    - anything else (RISC-V etc.)
#if defined(SHADER_MATH_FORCE_SCALAR)
#  define SHADER_MATH_ISA_SCALAR 1
#elif BOOST_HW_SIMD_ARM >= BOOST_HW_SIMD_ARM_NEON_VERSION || defined(__ARM_NEON) || defined(__ARM_NEON__)
#  define SHADER_MATH_ISA_NEON 1
#elif BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE2_VERSION || defined(__SSE2__) || defined(_M_X64) || \
    (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#  define SHADER_MATH_ISA_SSE2 1
#else
#  define SHADER_MATH_ISA_SCALAR 1
#endif

// Sub-feature flags (compile-time only; no runtime dispatch):
//   SHADER_MATH_HAS_FMA - FMA3 available. GCC/Clang define __FMA__ (e.g. with
//                         -mfma or -march=x86-64-v3). MSVC never defines
//                         __FMA__, but /arch:AVX2 (__AVX2__) implies FMA3.
#if defined(__FMA__) || (defined(_MSC_VER) && defined(__AVX2__))
#  define SHADER_MATH_HAS_FMA 1
#endif

#if defined(__cpp_concepts)
#  if __cpp_concepts < 201907L
#    error "shader-math requires C++20 concepts (gcc 10+ / clang 10+ / MSVC 19.30+)"
#  endif
#else
#  error "shader-math requires C++20 (compile with -std=c++20 or /std:c++20)"
#endif
