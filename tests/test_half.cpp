// Half (binary16) vectors: construction, swizzles, component-wise ops,
// comparisons, conversions, and the no-implicit-mixing restriction.
//
// Test strictness follows the element type:
//   exact  - Hadamard ops on values exactly representable in binary16
//            (copies, add/sub/mul of small integers, swizzles, ctors)
//   approx - anything with an inner product or a divide (CHECK_NEAR_H;
//            rounding differs between the ARM native path and the promoted
//            f32 fallback, and neither matches a GPU half ALU exactly)
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <vector>

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
static_assert(!std::is_same_v<h::half3, g::f16vec3>);  // one vector type per language

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

// matrices follow the same rules
static_assert(sizeof(h::half4x4) == 32);  // 4 rows x 4 x binary16
static_assert(can_mul_op<h::half3x3, h::half3x3>);        // Hadamard (HLSL)
static_assert(!can_mul_op<h::half3x3, h::float3x3>);      // no implicit mixing
static_assert(!can_add_op<h::half3x3, h::float3x3>);
static_assert(!can_mul_op<h::half3x3, float>);            // half3x3 * 2.0f fails
static_assert(can_mul_op<h::half3x3, h::half>);
static_assert(!std::is_convertible_v<h::half3x3, h::float3x3>);
static_assert(std::is_constructible_v<h::float3x3, h::half3x3>);  // explicit
static_assert(std::is_constructible_v<h::half3x3, h::float3x3>);
static_assert(!std::is_convertible_v<g::f16mat3, h::half3x3>);    // cross-language
static_assert(std::is_constructible_v<h::half3x3, g::f16mat3>);
template <class M, class V>
concept can_hlsl_mul = requires(const M& m, const V& v) { shader_math::hlsl::mul(m, v); };
static_assert(can_hlsl_mul<h::half3x3, h::half3>);
static_assert(!can_hlsl_mul<h::half3x3, h::float3>);  // element types must match
static_assert(!can_hlsl_mul<h::float3x3, h::half3>);

static void test_half_hlsl() {
    {
        const h::float4 f(1.0f, 2.0f, 3.0f, 4.0f);
        const h::half4 r(f.wzyx);  // element-type conversion from a swizzle
        CHECK_EQF(static_cast<float>(r[0]), 4.0f);
        CHECK_EQF(static_cast<float>(r[3]), 1.0f);
        const h::float3 back(h::half4(f).zyx);
        CHECK_EQF(back[0], 3.0f);
    }
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

static void test_half_matrix_hlsl() {
    using namespace shader_math::hlsl;

    // ctor order (row-major fill) and element accessors: exact
    // NOTE: element accessors convert to `half` first; a direct
    // static_cast<float> would need two user conversions with binary16 (MSVC).
    half2x2 m(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK_EQF(static_cast<float>(static_cast<half>(m._m01)), 2.0f);
    CHECK_EQF(static_cast<float>(static_cast<half>(m._m10)), 3.0f);
    CHECK_EQF(static_cast<float>(static_cast<half>(m._22)), 4.0f);
    m._m00 = half(9.0f);
    CHECK_EQF(static_cast<float>(m[0][0]), 9.0f);

    // scalar splat ctor (HLSL rule) and Hadamard * : exact
    const half2x2 twos(half(2.0f));
    const half2x2 had = m * twos;
    CHECK_EQF(static_cast<float>(had[0][0]), 18.0f);
    CHECK_EQF(static_cast<float>(had[1][1]), 8.0f);

    // identity mul: exact
    const half3x3 id = half3x3::identity();
    const half3 v(1.0f, 2.0f, 3.0f);
    const half3 same = mul(id, v);
    CHECK_EQF(static_cast<float>(same[0]), 1.0f);
    CHECK_EQF(static_cast<float>(same[2]), 3.0f);

    // simple scale matrix: mul in both directions and the matrix product
    const half3x3 scale(2.0f, 0.0f, 0.0f,
                        0.0f, 4.0f, 0.0f,
                        0.0f, 0.0f, 8.0f);
    const half3 sv = mul(scale, v);
    CHECK_NEAR_H(sv[0], 2.0f);
    CHECK_NEAR_H(sv[1], 8.0f);
    CHECK_NEAR_H(sv[2], 24.0f);
    const half3 vs = mul(v, scale);
    CHECK_NEAR_H(vs[1], 8.0f);
    const half3x3 sq = mul(scale, scale);
    CHECK_NEAR_H(static_cast<float>(sq[2][2]), 64.0f);

    // transpose: exact
    const half2x3 wide(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    const half3x2 tall = transpose(wide);
    CHECK_EQF(static_cast<float>(tall[2][1]), 6.0f);

    // det / inverse: identity and a simple diagonal only (true fp16
    // underflows quickly), relaxed comparison
    CHECK_EQF(static_cast<float>(determinant(id)), 1.0f);
    CHECK_NEAR_H(determinant(scale), 64.0f);
    const half3x3 inv = shader_math::ext::inverse(scale);
    CHECK_NEAR_H(static_cast<float>(inv[0][0]), 0.5f);
    CHECK_NEAR_H(static_cast<float>(inv[1][1]), 0.25f);
    CHECK_NEAR_H(static_cast<float>(inv[2][2]), 0.125f);

    // comparisons: per-component bmat, reduce with all()/any()
    CHECK(all(id == id));
    CHECK(any(id != scale));

    // element-type conversion: explicit, both ways
    const float3x3 widened(scale);
    CHECK_EQF(widened[1][1], 4.0f);
    const half3x3 narrowed(float3x3(1.0f, 0.0f, 0.0f,
                                    0.0f, 0.5f, 0.0f,
                                    0.0f, 0.0f, 0.25f));
    CHECK_EQF(static_cast<float>(narrowed[2][2]), 0.25f);
}

static void test_half_matrix_glsl() {
    using namespace shader_math::glsl;

    // GLSL scalar ctor: diagonal (f16mat3(1.0) is the identity)
    const f16mat3 id(1.0f);
    CHECK_EQF(static_cast<float>(id[0][0]), 1.0f);
    CHECK_EQF(static_cast<float>(id[1][0]), 0.0f);

    // column-major ctor order
    const f16mat2 m(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK_EQF(static_cast<float>(m[0][1]), 2.0f);  // column 0, row 1
    CHECK_EQF(static_cast<float>(m[1][0]), 3.0f);

    // linear-algebra * : identity and simple scale
    const f16vec3 v(1.0f, 2.0f, 3.0f);
    const f16vec3 same = id * v;
    CHECK_EQF(static_cast<float>(same[1]), 2.0f);
    const f16mat3 scale(2.0f, 0.0f, 0.0f,
                        0.0f, 4.0f, 0.0f,
                        0.0f, 0.0f, 8.0f);
    const f16vec3 sv = scale * v;
    CHECK_NEAR_H(sv[2], 24.0f);
    const f16vec3 vs = v * scale;
    CHECK_NEAR_H(vs[0], 2.0f);

    // matrixCompMult / transpose: exact
    const f16mat2 had = matrixCompMult(m, m);
    CHECK_EQF(static_cast<float>(had[1][1]), 16.0f);
    const f16mat2 t = transpose(m);
    CHECK_EQF(static_cast<float>(t[0][1]), 3.0f);

    // det / inverse on the simple scale: relaxed
    CHECK_NEAR_H(determinant(scale), 64.0f);
    const f16mat3 inv = inverse(scale);
    CHECK_NEAR_H(static_cast<float>(inv[1][1]), 0.25f);

    // == is a single bool in GLSL
    CHECK(id == id);
    CHECK(id != scale);
}

#if !defined(SHADER_MATH_HAS_FP16)
// The widened path's hops::widen / narrow (F16C on x86 when available) must
// give the same bits as the scalar half_t <-> float conversions: every half
// bit pattern one way, and rounding-sensitive floats the other. NaNs only
// have to stay NaN (payload handling differs between hardware and the MSVC
// software binary16).
static std::uint16_t half_bits(d::half_t h) {
    std::uint16_t b;
    std::memcpy(&b, &h, sizeof(b));
    return b;
}
static d::half_t half_from_bits(std::uint16_t b) {
    d::half_t h;
    std::memcpy(&h, &b, sizeof(h));
    return h;
}
static bool same_float(float a, float b) {
    if (std::isnan(a) || std::isnan(b)) return std::isnan(a) && std::isnan(b);
    std::uint32_t ua;
    std::uint32_t ub;
    std::memcpy(&ua, &a, sizeof(ua));
    std::memcpy(&ub, &b, sizeof(ub));
    return ua == ub;
}

static void test_half_conversions() {
    bool widen_ok = true;
    for (std::uint32_t b = 0; b < 0x10000u && widen_ok; b += 4) {
        d::half_t in[4];
        for (std::uint32_t k = 0; k < 4; ++k) in[k] = half_from_bits(static_cast<std::uint16_t>(b + k));
        const d::freg f = d::hops::widen(d::hops::load4(in));
        for (int k = 0; k < 4 && widen_ok; ++k) {
            widen_ok = same_float(d::ops::lane(f, k), static_cast<float>(in[k]));
            if (!widen_ok) std::printf("FAIL widen half bits 0x%04x\n", static_cast<unsigned>(b) + k);
        }
    }
    CHECK(widen_ok);

    // halfway points between neighbouring halves (ties to even), the
    // overflow edge, the subnormal range, and a sweep of ordinary values
    std::vector<float> in;
    for (std::uint32_t b = 0; b < 0x7C00u; b += 7) {
        const float lo = static_cast<float>(half_from_bits(static_cast<std::uint16_t>(b)));
        const float hi = static_cast<float>(half_from_bits(static_cast<std::uint16_t>(b + 1)));
        in.push_back(lo);
        in.push_back(0.5f * (lo + hi));
        in.push_back(-0.5f * (lo + hi));
    }
    for (float x : {65504.0f, 65519.0f, 65520.0f, 70000.0f, 1e-8f, 3e-8f, 5.96e-8f, 6.1e-5f,
                    -0.0f, std::numeric_limits<float>::infinity()})
        in.push_back(x);
    while (in.size() % 4) in.push_back(0.0f);
    bool narrow_ok = true;
    for (std::size_t i = 0; i < in.size() && narrow_ok; i += 4) {
        const d::hreg h = d::hops::narrow(d::ops::load4(&in[i]));
        for (int k = 0; k < 4 && narrow_ok; ++k) {
            const d::half_t want = static_cast<d::half_t>(in[i + k]);
            const d::half_t got = d::hops::lane(h, k);
            narrow_ok = half_bits(got) == half_bits(want);
            if (!narrow_ok)
                std::printf("FAIL narrow %a: 0x%04x vs 0x%04x\n", static_cast<double>(in[i + k]),
                            half_bits(got), half_bits(want));
        }
    }
    CHECK(narrow_ok);
}
#else
static void test_half_conversions() {}
#endif

void test_half() {
    test_half_hlsl();
    test_half_glsl();
    test_half_matrix_hlsl();
    test_half_matrix_glsl();
    test_half_conversions();
}

#else  // no half scalar type on this compiler: the half surface is absent

void test_half() {}

#endif
