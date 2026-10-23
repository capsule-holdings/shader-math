// Opt-in multi-element matrix accessors: built twice, with
// SHADER_MATH_MATRIX_SWIZZLE (2-element lists) and SHADER_MATH_MATRIX_SWIZZLE3
// (also 3-element). Its own executables: the macros change the matrix class
// definition, so they must not be mixed with translation units built without.
#if !defined(SHADER_MATH_MATRIX_SWIZZLE) && !defined(SHADER_MATH_MATRIX_SWIZZLE3)
#  error "build this test with SHADER_MATH_MATRIX_SWIZZLE or SHADER_MATH_MATRIX_SWIZZLE3"
#endif

#include <cstdio>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;

template <class M>
concept has_m00_m11 = requires(M m) { m._m00_m11; };
template <class M>
concept has_m22_m00 = requires(M m) { m._m22_m00; };  // row 2 / column 2 exist only in 3+
template <class M>
concept has_triple = requires(M m) { m._m30_m31_m32; };
template <class M>
concept has_four = requires(M m) { m._m00_m11_m22_m33; };
template <class M>
concept can_write_dup = requires(M m) { m._m00_m00 = h::float2(); };
template <class M>
concept can_write_pair = requires(M m) { m._m00_m11 = h::float2(); };

static_assert(has_m00_m11<h::float2x2>);
static_assert(!has_m22_m00<h::float2x2>);  // only the matrix's own shape
static_assert(has_m22_m00<h::float3x3>);
static_assert(!has_four<h::float4x4>);     // never 4 elements
static_assert(!can_write_dup<h::float4x4>);
static_assert(can_write_pair<h::float4x4>);
#if defined(SHADER_MATH_MATRIX_SWIZZLE3)
static_assert(has_triple<h::float4x4>);
static_assert(!has_triple<h::float3x4>);  // row 3 is outside a 3x4
#else
static_assert(!has_triple<h::float4x4>);
#endif

int main() {
    using namespace shader_math::hlsl;

    float4x4 m(1.0f, 2.0f, 3.0f, 4.0f,
               5.0f, 6.0f, 7.0f, 8.0f,
               9.0f, 10.0f, 11.0f, 12.0f,
               13.0f, 14.0f, 15.0f, 16.0f);

    // ---- reads: both spellings, any order, repeats allowed ----------------
    const float2 diag = m._11_22;
    CHECK_EQF(diag[1], 6.0f);
    const float2 rep = m._m12_m12;
    CHECK_EQF(rep[1], 7.0f);
    const float2 cross = m._m30_m03;
    CHECK(all(cross == float2(13.0f, 4.0f)));
    CHECK_NEAR(dot(m._m00_m11, float2(1.0f)), 1.0f + 6.0f);
    const float2 sum = m._m00_m01 + m._m10_m11;
    CHECK_EQF(sum[1], 8.0f);

    // ---- writes and compound assignment ------------------------------------
    m._m00_m11 = float2(-1.0f, -2.0f);
    CHECK_EQF(float(m._m00), -1.0f);
    CHECK_EQF(float(m._22), -2.0f);
    m._m01_m10 = m._m10_m01;  // swap through a gather
    CHECK_EQF(float(m._m01), 5.0f);
    CHECK_EQF(float(m._m10), 2.0f);
    m._41_42 += float2(100.0f);
    CHECK_EQF(float(m._m31), 114.0f);
    m._m20_m21 = float4(7.0f, 8.0f, 9.0f, 0.0f).zy;  // from a vector swizzle
    CHECK_EQF(float(m._m20), 9.0f);
    CHECK_EQF(float(m._m21), 8.0f);
    CHECK_EQF(float(m._m22), 11.0f);  // untouched neighbour

    // ---- non-square shapes -------------------------------------------------
    const float3x2 u(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    const float2 v = u._31_12;
    CHECK_EQF(v[0], 5.0f);
    CHECK_EQF(v[1], 2.0f);

#if defined(SHADER_MATH_MATRIX_SWIZZLE3)
    // ---- 3-element lists ---------------------------------------------------
    const float4x4 n(1.0f, 2.0f, 3.0f, 4.0f,
                     5.0f, 6.0f, 7.0f, 8.0f,
                     9.0f, 10.0f, 11.0f, 12.0f,
                     13.0f, 14.0f, 15.0f, 16.0f);
    const float3 t = n._m30_m31_m32;  // translation row
    CHECK_EQF(t[0], 13.0f);
    CHECK_EQF(t[2], 15.0f);
    CHECK(all(t == n._41_42_43));
    const float3 col = n._m03_m13_m23;
    CHECK_EQF(col[1], 8.0f);
    const float3 rep3 = n._m12_m12_m00;
    CHECK_EQF(rep3[2], 1.0f);
    CHECK_NEAR(dot(n._m00_m11_m22, float3(1.0f)), 1.0f + 6.0f + 11.0f);
    m._41_42_43 += float3(1.0f);
    CHECK_EQF(float(m._m31), 115.0f);
    CHECK_EQF(float(m._m32), 16.0f);
    const float2x3 s(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    const float3 c = s._m02_m12_m00;
    CHECK(all(c == float3(3.0f, 6.0f, 1.0f)));
#endif

#if defined(SHADER_MATH_HAS_HALF)
    half3x3 hm(half(1.0f), half(2.0f), half(3.0f), half(4.0f), half(5.0f), half(6.0f),
               half(7.0f), half(8.0f), half(9.0f));
    const half2 hd = hm._m00_m22;
    CHECK_EQF(float(hd[1]), 9.0f);
    hm._m02_m20 = hm._m20_m02;
    CHECK_EQF(float(hm._m02), 7.0f);
#endif

    std::printf("[shader-math matrix swizzle] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
