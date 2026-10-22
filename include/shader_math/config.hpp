// shader-math: HLSL/GLSL compatible vector & matrix library.
// ISA detection and portability macros. Boost is used for detection only.
#pragma once

#include <boost/config.hpp>
#include <boost/predef.h>

#define SHADER_MATH_INLINE BOOST_FORCEINLINE

// The ISA selection below compares BOOST_HW_SIMD_* values; with an older
// Predef they are undefined, evaluate to 0 >= 0, and pick NEON on any host.
#if !defined(BOOST_HW_SIMD_ARM) || !defined(BOOST_HW_SIMD_X86)
#  error "shader-math: Boost.Predef 1.60+ is required (BOOST_HW_SIMD_* not defined)"
#endif

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

//   SHADER_MATH_HAS_AVX - AVX1 (256-bit float ops). Used only to pack two
//                         rows/columns of a 4x4 float matrix product into one
//                         YMM register, and only together with FMA (AVX1-only
//                         CPUs without FMA3 keep the 128-bit path). Public
//                         types never change width.
#if defined(__AVX__)
#  define SHADER_MATH_HAS_AVX 1
#endif

// Half precision (binary16):
//   SHADER_MATH_HAS_HALF - a usable half scalar type exists, so the half
//                          vector types are defined at all. GCC/Clang expose
//                          _Float16 (x86: gcc 12+ / clang 15+, AArch64:
//                          gcc 13+ / any clang) and advertise it via
//                          __FLT16_MANT_DIG__. gcc < 13 on AArch64 defines
//                          that macro but accepts _Float16 only in C, not C++.
//                          MSVC has no _Float16; a bit-stored software type
//                          (detail::binary16) is used instead. On unsupported
//                          compilers the half surface silently disappears;
//                          the float surface is unaffected.
//   SHADER_MATH_HAS_FP16 - the ARM FEAT_FP16 vector ALU is available
//                          (__ARM_FEATURE_FP16_VECTOR_ARITHMETIC, e.g.
//                          -march=armv8.2-a+fp16), so half math runs on real
//                          16-bit lanes. Everywhere else half math widens the
//                          lanes to f32, uses the float ops, and narrows back.
// SHADER_MATH_TEST_FORCE_BINARY16 selects the MSVC software half
// (detail::binary16) on any compiler; test hook for exercising that path
// without MSVC. Not a user-facing option.
#if defined(SHADER_MATH_TEST_FORCE_BINARY16)
#  define SHADER_MATH_HAS_HALF 1
#elif defined(_MSC_VER) && !defined(__clang__)
#  define SHADER_MATH_HAS_HALF 1
#elif defined(__FLT16_MANT_DIG__) && \
    (defined(__clang__) || __GNUC__ >= 13 || defined(__x86_64__) || defined(__i386__))
#  define SHADER_MATH_HAS_HALF 1
#  define SHADER_MATH_HALF_IS_FLOAT16 1
#endif

#if defined(SHADER_MATH_HAS_HALF) && defined(SHADER_MATH_ISA_NEON) && \
    defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC)
#  define SHADER_MATH_HAS_FP16 1
#endif

#if defined(__cpp_concepts)
#  if __cpp_concepts < 201907L
#    error "shader-math requires C++20 concepts (gcc 10+ / clang 14+ / MSVC 19.30+)"
#  endif
#else
#  error "shader-math requires C++20 (compile with -std=c++20 or /std:c++20)"
#endif
