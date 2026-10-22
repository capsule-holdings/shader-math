// Prove the compiled backend is the real SIMD ISA for this host.
// Numerical tests pass on the scalar fallback too; this file fails if
// ISA detection silently selected the 4-float struct on an SSE2/NEON target.
#include <cmath>
#include <type_traits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

using shader_math::detail::freg;
using shader_math::detail::ops;

// Compiler target flags, independent of Boost Predef / library macros.
#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
#  define SM_TEST_HOST_SSE2 1
#elif defined(__ARM_NEON) || defined(__ARM_NEON__) || defined(__aarch64__)
#  define SM_TEST_HOST_NEON 1
#endif

#if !defined(SHADER_MATH_FORCE_SCALAR)
#  if defined(SM_TEST_HOST_SSE2)
#    if !defined(SHADER_MATH_ISA_SSE2)
#      error "shader-math: x86 SSE2 host did not select the SSE2 backend"
#    endif
#  elif defined(SM_TEST_HOST_NEON)
#    if !defined(SHADER_MATH_ISA_NEON)
#      error "shader-math: ARM NEON host did not select the NEON backend"
#    endif
#  endif
#endif

// FMA3 sub-feature detection must not silently miss a capable target.
#if defined(__FMA__) && !defined(SHADER_MATH_HAS_FMA)
#  error "shader-math: __FMA__ target did not set SHADER_MATH_HAS_FMA"
#endif
#if defined(_MSC_VER) && defined(__AVX2__) && !defined(SHADER_MATH_HAS_FMA)
#  error "shader-math: MSVC /arch:AVX2 target did not set SHADER_MATH_HAS_FMA"
#endif

// The interface contract itself must not be vacuous: a type without the ops
// surface, or the right ops with the wrong register type, must fail it.
// (Note: freg vs mreg cannot serve as the "wrong type" pair -- they are the
// same __m128 on SSE2.)
#if defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wignored-attributes"
#endif
static_assert(!shader_math::detail::SimdFloatOps<int, shader_math::detail::freg,
                                                 shader_math::detail::mreg, float>);
static_assert(!shader_math::detail::SimdFloatOps<shader_math::detail::ops, int,
                                                 shader_math::detail::mreg, float>);
#if defined(__GNUC__)
#  pragma GCC diagnostic pop
#endif

// FEAT_FP16 detection must not silently miss a capable target.
#if defined(__ARM_FEATURE_FP16_VECTOR_ARITHMETIC) && defined(SHADER_MATH_ISA_NEON) && \
    !defined(SHADER_MATH_HAS_FP16)
#  error "shader-math: ARM FEAT_FP16 target did not set SHADER_MATH_HAS_FP16"
#endif

#if defined(SHADER_MATH_ISA_SSE2)
// GCC/Clang attach __vector_size__ to __m128; is_same_v is still the right
// check but trips -Wignored-attributes.
#  if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wignored-attributes"
#  endif
static_assert(std::is_same_v<freg, __m128>,
              "SSE2 backend must use __m128, not the scalar freg");
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

void test_fmadd_rounding();
void test_avx_2row();
void test_neg_signed_zero();

void test_simd() {
#if defined(SHADER_MATH_ISA_SSE2)
    // ops::add / ops::mul must be the SSE2-baseline intrinsics (same bits, same type).
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

    test_fmadd_rounding();
    test_avx_2row();
    test_neg_signed_zero();
}

// Prove ops::fmadd has the rounding behavior the backend promises.
// Probe: p = 1+2^-20, q = 1-2^-20, r = -1. p*q = 1 - 2^-40 exactly.
//   fused (single rounding):   p*q + r = -2^-40
//   two-step (SSE2 mul+add):   round(p*q) = 1.0f, then 1.0f + r = 0.0f
// A bit-compare of ops::fmadd against _mm_fmadd_ps would be a tautology;
// comparing against the exactly-representable true result is not.
void test_fmadd_rounding() {
    const float fused = -0x1.0p-40f;
    [[maybe_unused]] const float twostep = 0.0f;

    const shader_math::detail::freg p = ops::splat(1.0f + 0x1.0p-20f);
    const shader_math::detail::freg q = ops::splat(1.0f - 0x1.0p-20f);
    const shader_math::detail::freg r = ops::splat(-1.0f);
    const float got = ops::lane(ops::fmadd(p, q, r), 0);

#if defined(SHADER_MATH_ISA_NEON)
    // vfmaq_f32 is always fused.
    CHECK_EQF(got, fused);
#elif defined(SHADER_MATH_ISA_SSE2)
#  if defined(SHADER_MATH_HAS_FMA)
    CHECK_EQF(got, fused);
#  else
    CHECK_EQF(got, twostep);
#  endif
#else
    // Scalar backend: a*b + c in one expression; the compiler may or may not
    // contract it into an fma depending on -ffp-contract and the host ISA.
    CHECK(got == fused || got == twostep);
#endif
}

// The AVX1 two-row 4x4 kernel must be bit-identical to the 128-bit template
// path (same operation sequence, same rounding). The template is still
// callable with explicit template arguments, which bypasses the non-template
// overload.
void test_avx_2row() {
#if defined(SHADER_MATH_HAS_AVX_2ROW)
    {
        using namespace shader_math::hlsl;
        const float4x4 a(0.5f, -1.25f, 3.75f, 2.0f, 1.5f, 0.25f, -2.5f, 4.0f, -0.75f, 2.25f,
                         1.0f, -3.0f, 0.125f, -4.5f, 2.75f, 1.75f);
        const float4x4 b(2.0f, 0.5f, -1.0f, 3.25f, -0.25f, 1.75f, 2.5f, -3.5f, 4.25f, -2.0f,
                         0.75f, 1.25f, -1.5f, 3.0f, -0.5f, 2.25f);
        const float4x4 ymm = mul(a, b);                 // non-template AVX overload
        const float4x4 xmm = mul<4, 4, 4, float>(a, b); // forced 128-bit template
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) CHECK_EQF(ymm[r][c], xmm[r][c]);
    }
    {
        using namespace shader_math::glsl;
        const mat4 a(0.5f, -1.25f, 3.75f, 2.0f, 1.5f, 0.25f, -2.5f, 4.0f, -0.75f, 2.25f, 1.0f,
                     -3.0f, 0.125f, -4.5f, 2.75f, 1.75f);
        const mat4 b(2.0f, 0.5f, -1.0f, 3.25f, -0.25f, 1.75f, 2.5f, -3.5f, 4.25f, -2.0f, 0.75f,
                     1.25f, -1.5f, 3.0f, -0.5f, 2.25f);
        const mat4 ymm = a * b;                                // non-template AVX overload
        const mat4 xmm = operator*<4, 4, 4, float>(a, b);      // forced 128-bit template
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r) CHECK_EQF(ymm[c][r], xmm[c][r]);
    }
#endif
}

// Negation flips the sign bit on every backend: -(+0) is -0 and -(-0) is +0,
// so 1/-v gives -inf (IEEE negate, not 0 - x).
void test_neg_signed_zero() {
    using shader_math::hlsl::float3;
    const float3 n = -float3(0.0f);
    CHECK(std::signbit(float(n.x)));
    CHECK(std::signbit(float(n.y)));
    CHECK(std::signbit(float(n.z)));
    CHECK(!std::signbit(float((-n).x)));
    CHECK(std::isinf(float((1.0f / n).x)) && float((1.0f / n).x) < 0.0f);

    const freg z = ops::neg(ops::zero());
    for (int i = 0; i < 4; ++i) CHECK(std::signbit(ops::lane(z, i)));

#if defined(SHADER_MATH_HAS_HALF)
    using shader_math::hlsl::half3;
    const half3 hn = -half3(0.0f);
    CHECK(std::signbit(static_cast<float>(hn.x)));
    CHECK(!std::signbit(static_cast<float>((-hn).x)));
#endif
}
