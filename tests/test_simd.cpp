// Prove the compiled backend is the real SIMD ISA for this host.
// Numerical tests pass on the scalar fallback too; this file fails if
// ISA detection silently selected the 4-float struct on an SSE/NEON target.
#include <type_traits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

using shader_math::detail::freg;
using shader_math::detail::ops;

// Compiler target flags, independent of Boost Predef / library macros.
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#  define SM_TEST_HOST_SSE 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__)
#  define SM_TEST_HOST_NEON 1
#endif

#if !defined(SHADER_MATH_FORCE_SCALAR)
#  if defined(SM_TEST_HOST_SSE)
#    if !defined(SHADER_MATH_ISA_SSE)
#      error "shader-math: x86 SSE2 host did not select the SSE backend"
#    endif
#  elif defined(SM_TEST_HOST_NEON)
#    if !defined(SHADER_MATH_ISA_NEON)
#      error "shader-math: ARM NEON host did not select the NEON backend"
#    endif
#  endif
#endif

#if defined(SHADER_MATH_ISA_SSE)
// GCC/Clang attach __vector_size__ to __m128; is_same_v is still the right
// check but trips -Wignored-attributes.
#  if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wignored-attributes"
#  endif
static_assert(std::is_same_v<freg, __m128>,
              "SSE backend must use __m128, not the scalar freg");
#  if defined(__GNUC__)
#    pragma GCC diagnostic pop
#  endif
#elif defined(SHADER_MATH_ISA_NEON)
static_assert(std::is_same_v<freg, float32x4_t>,
              "NEON backend must use float32x4_t, not the scalar freg");
#elif defined(SHADER_MATH_ISA_SCALAR)
template <class T, class = void>
struct has_scalar_lanes : std::false_type {};
template <class T>
struct has_scalar_lanes<T, std::void_t<decltype(std::declval<T>().f[0])>> : std::true_type {};
static_assert(has_scalar_lanes<freg>::value,
              "scalar backend must use the 4-float freg struct");
#else
#  error "shader-math: no ISA backend macro is set"
#endif

void test_simd() {
#if defined(SHADER_MATH_ISA_SSE)
    // ops::add / ops::mul must be the SSE intrinsics (same bits, same type).
    const __m128 a = _mm_set_ps(4.0f, 3.0f, 2.0f, 1.0f);
    const __m128 b = _mm_set_ps(40.0f, 30.0f, 20.0f, 10.0f);
    CHECK(_mm_movemask_ps(_mm_cmpeq_ps(ops::add(a, b), _mm_add_ps(a, b))) == 0xF);
    CHECK(_mm_movemask_ps(_mm_cmpeq_ps(ops::mul(a, b), _mm_mul_ps(a, b))) == 0xF);
    CHECK(_mm_movemask_ps(_mm_cmpeq_ps(ops::sub(a, b), _mm_sub_ps(a, b))) == 0xF);
    CHECK(_mm_movemask_ps(ops::cmp_eq(a, a)) == 0xF);
    CHECK(_mm_movemask_ps(ops::cmp_eq(a, b)) == 0);
#elif defined(SHADER_MATH_ISA_NEON)
    const float32x4_t a = {1.0f, 2.0f, 3.0f, 4.0f};
    const float32x4_t b = {10.0f, 20.0f, 30.0f, 40.0f};
    float native[4];
    float via_ops[4];
    vst1q_f32(native, vaddq_f32(a, b));
    vst1q_f32(via_ops, ops::add(a, b));
    CHECK_EQF(via_ops[0], native[0]);
    CHECK_EQF(via_ops[1], native[1]);
    CHECK_EQF(via_ops[2], native[2]);
    CHECK_EQF(via_ops[3], native[3]);
    vst1q_f32(native, vmulq_f32(a, b));
    vst1q_f32(via_ops, ops::mul(a, b));
    CHECK_EQF(via_ops[0], native[0]);
    CHECK_EQF(via_ops[1], native[1]);
    CHECK_EQF(via_ops[2], native[2]);
    CHECK_EQF(via_ops[3], native[3]);
#else
    freg a{{1.0f, 2.0f, 3.0f, 4.0f}};
    freg b{{10.0f, 20.0f, 30.0f, 40.0f}};
    const freg s = ops::add(a, b);
    CHECK_EQF(s.f[0], 11.0f);
    CHECK_EQF(s.f[1], 22.0f);
    CHECK_EQF(s.f[2], 33.0f);
    CHECK_EQF(s.f[3], 44.0f);
    CHECK(requires(freg r) { r.f[0]; });
#endif
}
