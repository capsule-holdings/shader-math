// Half (binary16) vectors: construction, swizzles, component-wise ops,
// comparisons, conversions, and the no-implicit-mixing restriction.
//
// Test strictness follows the element type:
//   exact  - Hadamard ops on values exactly representable in binary16
//            (copies, add/sub/mul of small integers, swizzles, ctors)
//   approx - anything with an inner product or a divide (CHECK_NEAR_H;
//            rounding differs between the ARM native path and the promoted
//            f32 fallback, and neither matches a GPU half ALU exactly)
#include <shader_math/shader_math.hpp>

#include "framework.hpp"

#if defined(SHADER_MATH_HAS_HALF)

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;
namespace d = shader_math::detail;

// ---- packing: language compatibility, not a GPU ABI -------------------------
static_assert(sizeof(d::half_t) == 2);
static_assert(sizeof(h::half2) == 8);
static_assert(sizeof(h::half3) == 8);  // 4 x binary16, like sizeof(float3) == 16
static_assert(sizeof(h::half4) == 8);
static_assert(std::is_same_v<h::half3, g::f16vec3>);  // one vector type per width

// ---- no implicit half/float mixing ------------------------------------------
template <class A, class B>
concept can_mul_op = requires(const A& a, const B& b) { a* b; };
template <class A, class B>
concept can_add_op = requires(const A& a, const B& b) { a + b; };

static_assert(can_mul_op<h::half3, h::half3>);
static_assert(!can_mul_op<h::half3, h::float3>);  // explicit conversion required
static_assert(!can_add_op<h::half3, h::float3>);
static_assert(can_mul_op<h::half3, h::half>);
static_assert(!can_mul_op<h::half3, float>);   // half3 * 2.0f does not compile
static_assert(!can_mul_op<h::half3, double>);
static_assert(!can_mul_op<h::half3, int>);
static_assert(can_mul_op<h::float3, float>);   // float vectors keep loose scalars
static_assert(can_mul_op<h::float3, double>);
static_assert(can_mul_op<h::float3, int>);
static_assert(!std::is_convertible_v<h::half3, h::float3>);  // explicit only
static_assert(!std::is_convertible_v<h::float3, h::half3>);
static_assert(std::is_constructible_v<h::float3, h::half3>);
static_assert(std::is_constructible_v<h::half3, h::float3>);

static void test_half_hlsl() {
    using namespace shader_math::hlsl;

    // ctors and lanes: exact (small integers are exact in binary16)
    half3 a(1.0f, 2.0f, 3.0f);
    CHECK_EQF(static_cast<float>(a[0]), 1.0f);
    CHECK_EQF(static_cast<float>(a[1]), 2.0f);
    CHECK_EQF(static_cast<float>(a[2]), 3.0f);

    const half3 splat(half(4.0f));
    CHECK_EQF(static_cast<float>(splat[0]), 4.0f);
    CHECK_EQF(static_cast<float>(splat[2]), 4.0f);

    // Hadamard: exact
    const half3 b(4.0f, 8.0f, 16.0f);
    const half3 sum = a + b;
    CHECK_EQF(static_cast<float>(sum[0]), 5.0f);
    CHECK_EQF(static_cast<float>(sum[1]), 10.0f);
    CHECK_EQF(static_cast<float>(sum[2]), 19.0f);
    const half3 prod = a * b;
    CHECK_EQF(static_cast<float>(prod[0]), 4.0f);
    CHECK_EQF(static_cast<float>(prod[1]), 16.0f);
    CHECK_EQF(static_cast<float>(prod[2]), 48.0f);
    const half3 neg = -a;
    CHECK_EQF(static_cast<float>(neg[1]), -2.0f);

    // scalar ops require an exact half scalar
    const half3 scaled = a * half(2.0f);
    CHECK_EQF(static_cast<float>(scaled[2]), 6.0f);
    half3 comp = a;
    comp += half(1.0f);
    CHECK_EQF(static_cast<float>(comp[0]), 2.0f);

    // swizzles: exact
    const half3 zyx = a.zyx;
    CHECK_EQF(static_cast<float>(zyx[0]), 3.0f);
    CHECK_EQF(static_cast<float>(zyx[2]), 1.0f);
    half4 w(1.0f, 2.0f, 3.0f, 4.0f);
    w.xy = half2(9.0f, 8.0f);
    CHECK_EQF(static_cast<float>(w[0]), 9.0f);
    CHECK_EQF(static_cast<float>(w[1]), 8.0f);
    CHECK_EQF(static_cast<float>(w[3]), 4.0f);
    w.x = half(7.0f);
    CHECK_EQF(static_cast<float>(w[0]), 7.0f);
    // single-lane swizzle as the scalar operand of a vector op
    const half3 by_lane = a * w.x;  // a * 7
    CHECK_EQF(static_cast<float>(by_lane[1]), 14.0f);

    // comparisons: HLSL per-component bvec, reduce with all()/any()
    CHECK(all(a == a));
    CHECK(!all(a == b));
    CHECK(any(a < b));
    CHECK(all(a <= b));
    const bool3 lt = a < half3(2.0f, 2.0f, 4.0f);
    CHECK(lt[0]);
    CHECK(!lt[1]);
    CHECK(lt[2]);

    // dot / mul(v,v): approx (inner product)
    CHECK_NEAR_H(dot(a, b), 68.0f);
    CHECK_NEAR_H(mul(a, b), 68.0f);
    CHECK_NEAR_H(mul(half(3.0f), half(5.0f)), 15.0f);

    // length / normalize: approx (sqrt + divide)
    const half3 unit = normalize(half3(3.0f, 0.0f, 4.0f));
    CHECK_NEAR_H(length(half3(3.0f, 0.0f, 4.0f)), 5.0f);
    CHECK_NEAR_H(unit[0], 0.6f);
    CHECK_NEAR_H(unit[2], 0.8f);

    // cross: exact for these small integers
    const half3 c = cross(half3(1.0f, 0.0f, 0.0f), half3(0.0f, 1.0f, 0.0f));
    CHECK_EQF(static_cast<float>(c[2]), 1.0f);

    // explicit conversions both ways
    const float3 widened(a);
    CHECK_EQF(widened[1], 2.0f);
    const half3 narrowed(float3(1.0f, 0.5f, 0.25f));  // exact in binary16
    CHECK_EQF(static_cast<float>(narrowed[1]), 0.5f);
    CHECK_EQF(static_cast<float>(narrowed[2]), 0.25f);

    // binary16 rounding: 1 + 2^-11 is NOT representable, rounds to nearest even
    const half rounded = half(1.0f + 0x1.0p-11f);
    CHECK_EQF(static_cast<float>(rounded), 1.0f);
    const half up = half(1.0f + 0x1.8p-11f);  // above halfway: rounds up to 1+2^-10
    CHECK_EQF(static_cast<float>(up), 1.0f + 0x1.0p-10f);
}

static void test_half_glsl() {
    using namespace shader_math::glsl;

    // NOTE: unqualified float16_t would be ambiguous on ARM (arm_neon.h has a
    // global float16_t); constructor arguments convert, so floats suffice.
    const f16vec3 a(1.0f, 2.0f, 3.0f);
    const f16vec3 b(1.0f, 2.0f, 3.0f);
    const f16vec3 c(1.0f, 9.0f, 3.0f);

    // GLSL == / != on vectors: single bool
    CHECK(a == b);
    CHECK(a != c);

    // per-component comparison functions return bvec
    CHECK(all(equal(a, b)));
    CHECK(any(notEqual(a, c)));
    CHECK(any(lessThan(a, c)));

    // Hadamard + swizzle: exact
    const f16vec3 s = a + a;
    CHECK_EQF(static_cast<float>(s[2]), 6.0f);
    CHECK_EQF(static_cast<float>(f16vec3(a.zyx)[0]), 3.0f);

    // dot: approx
    CHECK_NEAR_H(dot(a, b), 14.0f);
}

void test_half() {
    test_half_hlsl();
    test_half_glsl();
}

#else  // no half scalar type on this compiler: the half surface is absent

void test_half() {}

#endif
