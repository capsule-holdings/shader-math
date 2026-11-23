#include <shader_math/shader_math.hpp>

#include "framework.hpp"

// Reference row-major matrix product: (MxK) * (KxN)
static void ref_matmul(const float* a, const float* b, float* o, int M, int K, int N) {
    for (int i = 0; i < M; ++i)
        for (int j = 0; j < N; ++j) {
            float s = 0.0f;
            for (int k = 0; k < K; ++k) s += a[i * K + k] * b[k * N + j];
            o[i * N + j] = s;
        }
}

void test_matrix_hlsl() {
    using namespace shader_math::hlsl;

    // ---- constructors: row-major fill (per Microsoft docs examples) ---------
    {
        float2x2 m = {1.0f, 1.1f,   // row 1
                      2.0f, 2.1f};  // row 2
        CHECK_EQF(float(m._m00), 1.0f);
        CHECK_EQF(float(m._m11), 2.1f);
        CHECK_EQF(float(m._11), 1.0f);
        const float2 diag(m._11, m._22);  // scalar ctor args take proxies by value
        CHECK_EQF(diag[0], float(m._11));
        CHECK_EQF(diag[1], float(m._22));
        CHECK_EQF(float(m._22), 2.1f);
        CHECK_EQF(m[0][0], 1.0f);
        CHECK_EQF(m[0][1], 1.1f);
        CHECK_EQF(m[1][1], 2.1f);
        // m[1] is a row
        float2 row1 = m[1];
        CHECK_EQF(row1[0], 2.0f);
        CHECK_EQF(row1[1], 2.1f);
    }
    // scalar ctor: HLSL splats ALL components (unlike GLSL's diagonal)
    {
        float3x3 m(0.5f);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c) CHECK_EQF(m[r][c], 0.5f);
        float4x4 z = float4x4(0.0f);
        CHECK(all(z == float4x4()));
    }
    // row-vector ctor
    {
        float3 r0(1.0f, 2.0f, 3.0f), r1(4.0f, 5.0f, 6.0f), r2(7.0f, 8.0f, 9.0f);
        float3x3 m(r0, r1, r2);
        CHECK_EQF(m[1][2], 6.0f);
        CHECK_EQF(m[2][0], 7.0f);
    }
    // element accessor writes
    {
        float3x3 m;
        m._m01 = 5.0f;
        CHECK_EQF(m[0][1], 5.0f);
        m._23 = 7.0f;  // one-based: row 2, col 3 -> [1][2]
        CHECK_EQF(m[1][2], 7.0f);
        m._m01 += 1.0f;
        CHECK_EQF(m[0][1], 6.0f);
    }
    // shrink cast (upper-left cut)
    {
        float4x4 m = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        float2x2 s(m);
        CHECK_EQF(s[0][0], 1.0f);
        CHECK_EQF(s[0][1], 2.0f);
        CHECK_EQF(s[1][0], 5.0f);
        CHECK_EQF(s[1][1], 6.0f);
        float2x3 t(m);
        CHECK_EQF(t[1][2], 7.0f);
    }

    // ---- '*' is component-wise (per-component math docs example) -------------
    {
        float3x3 mat1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        float3x3 mat2 = {10, 20, 30, 40, 50, 60, 70, 80, 90};
        float3x3 h = mat1 * mat2;  // Hadamard
        CHECK_EQF(h[0][0], 10.0f);
        CHECK_EQF(h[0][1], 40.0f);
        CHECK_EQF(h[0][2], 90.0f);
        CHECK_EQF(h[1][0], 160.0f);
        CHECK_EQF(h[1][1], 250.0f);
        CHECK_EQF(h[1][2], 360.0f);
        CHECK_EQF(h[2][0], 490.0f);
        CHECK_EQF(h[2][1], 640.0f);
        CHECK_EQF(h[2][2], 810.0f);

        // mul() is the true matrix product: top-left = 1*10 + 2*40 + 3*70 = 300
        float3x3 p = mul(mat1, mat2);
        CHECK_NEAR(p[0][0], 300.0f);
        float ea[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        float eb[9] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
        float eo[9];
        ref_matmul(ea, eb, eo, 3, 3, 3);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c) CHECK_NEAR(p[r][c], eo[r * 3 + c]);
    }

    // ---- other component-wise operators --------------------------------------
    {
        float2x2 a = {1, 2, 3, 4};
        float2x2 b = {10, 20, 30, 40};
        CHECK_EQF((a + b)[1][1], 44.0f);
        CHECK_EQF((b - a)[0][0], 9.0f);
        CHECK_EQF((b / a)[1][0], 10.0f);
        CHECK_EQF((a * 2.0f)[1][1], 8.0f);
        CHECK_EQF((2.0f + a)[0][0], 3.0f);
        CHECK_EQF((-a)[0][1], -2.0f);
        float2x2 c = a;
        c += b;
        CHECK_EQF(c[0][0], 11.0f);
        c *= 2.0f;
        CHECK_EQF(c[0][0], 22.0f);
    }

    // ---- mul: vector overloads -----------------------------------------------
    {
        float3x3 m = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        float3 v(1.0f, 2.0f, 3.0f);

        // row vector * matrix: r[j] = sum_i v[i] * m[i][j]
        float3 rv = mul(v, m);
        CHECK_NEAR(rv[0], 1.0f * 1 + 2.0f * 4 + 3.0f * 7);
        CHECK_NEAR(rv[1], 1.0f * 2 + 2.0f * 5 + 3.0f * 8);
        CHECK_NEAR(rv[2], 1.0f * 3 + 2.0f * 6 + 3.0f * 9);

        // matrix * column vector: r[i] = dot(row_i, v)
        float3 cv = mul(m, v);
        CHECK_NEAR(cv[0], 1.0f * 1 + 2.0f * 2 + 3.0f * 3);
        CHECK_NEAR(cv[1], 4.0f * 1 + 5.0f * 2 + 6.0f * 3);
        CHECK_NEAR(cv[2], 7.0f * 1 + 8.0f * 2 + 9.0f * 3);

        // mul(v, M) != mul(M, v) unless M is symmetric
        CHECK(!near_f(rv[0], cv[0]));

        // scalar overloads
        CHECK_NEAR(mul(2.0f, 3.0f), 6.0f);
        CHECK_NEAR(mul(v, v), dot(v, v));  // vector x vector == dot
        float3 sv = mul(2.0f, v);
        CHECK_EQF(sv[2], 6.0f);
        float3x3 sm = mul(m, 2.0f);
        CHECK_EQF(sm[0][0], 2.0f);

        // swizzle argument works
        float4 v4(1.0f, 2.0f, 3.0f, 4.0f);
        float3 sw = mul(v4.xyz, m);
        CHECK_NEAR(sw[0], rv[0]);
    }

    // ---- mul: non-square (2x3 * 3x4 -> 2x4) ----------------------------------
    {
        float2x3 a = {1, 2, 3, 4, 5, 6};
        float3x4 b = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        float2x4 p = mul(a, b);
        float ea[6] = {1, 2, 3, 4, 5, 6};
        float eb[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        float eo[8];
        ref_matmul(ea, eb, eo, 2, 3, 4);
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 4; ++c) CHECK_NEAR(p[r][c], eo[r * 4 + c]);
    }

    // ---- transpose ------------------------------------------------------------
    {
        float3x4 m = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        static_assert(std::is_same_v<decltype(transpose(m)), float4x3>);
        float4x3 t = transpose(m);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c) CHECK_EQF(t[c][r], m[r][c]);
    }

    // ---- determinant / inverse -------------------------------------------------
    {
        float2x2 m2 = {3, 5, 2, 1};
        CHECK_NEAR(determinant(m2), 3.0f * 1 - 5.0f * 2);

        float3x3 m3 = {2, 0, 1, 1, 3, 2, 1, 1, 1};
        // det = 2*(3-2) - 0*(1-2) + 1*(1-3) = 0 -> pick a non-singular one
        float3x3 m3b = {4, 3, 2, 1, 5, 7, 9, 6, 8};
        const float d3 = determinant(m3b);
        CHECK(!near_f(d3, 0.0f));
        float3x3 i3 = mul(m3b, shader_math::ext::inverse(m3b));
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c) CHECK_NEAR(i3[r][c], r == c ? 1.0f : 0.0f);
        (void)m3;

        float4x4 m4 = {4, 0, 0, 1, 0, 3, 0, 2, 1, 0, 2, 0, 0, 2, 0, 5};
        const float d4 = determinant(m4);
        CHECK(!near_f(d4, 0.0f));
        float4x4 i4 = mul(m4, shader_math::ext::inverse(m4));
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) CHECK_NEAR(i4[r][c], r == c ? 1.0f : 0.0f);

        // identity determinant
        CHECK_NEAR(determinant(float4x4::identity()), 1.0f);
    }

    // ---- comparisons: per-component bool results (HLSL semantics) -------------
    {
        float3 a(1.0f, 2.0f, 3.0f);
        float3 b(1.0f, 5.0f, 3.0f);
        bool3 eq = (a == b);
        CHECK(eq[0]);
        CHECK(!eq[1]);
        CHECK(eq[2]);
        CHECK(!all(eq));
        CHECK(any(eq));
        CHECK(any(a < b));
        CHECK(!all(a < b));
        CHECK(all(a <= b));
        bool3 ne = (a != b);
        CHECK(!ne[0]);
        CHECK(ne[1]);
        // bool-vector ops
        CHECK(all(eq == !ne));
        CHECK(!any(eq != !ne));
        // vector vs scalar comparison
        CHECK(all(a >= 1.0f));
        CHECK(any(a == 3.0f));

        float2x2 ma = {1, 2, 3, 4};
        float2x2 mb = {1, 9, 3, 4};
        bool2x2 meq = (ma == mb);
        CHECK(meq[0][0]);
        CHECK(!meq[0][1]);
        CHECK(!all(meq));
        CHECK(any(meq));
        CHECK(all(ma == ma));
    }

    // ---- 1-dimension matrices (1xN, Nx1, 1x1) ------------------------------
    {
        // Construction, indexing, element accessors, conversions
        const float1x3 m13(1.0f, 2.0f, 3.0f);
        const float3x1 m31(4.0f, 5.0f, 6.0f);
        const float1x1 m11(7.0f);
        CHECK_EQF(m13[0][0], 1.0f);
        CHECK_EQF(m13[0][2], 3.0f);
        CHECK_EQF(float(m13._m02), 3.0f);
        CHECK_EQF(float(m13._12), 2.0f);
        CHECK_EQF(m31[0][0], 4.0f);
        CHECK_EQF(m31[2][0], 6.0f);
        CHECK_EQF(float(m31._m20), 6.0f);
        CHECK_EQF(float(m31._21), 5.0f);
        CHECK_EQF(m11[0][0], 7.0f);
        CHECK_EQF(float(m11._m00), 7.0f);

        // Splat, vector <-> matrix, 1x1 <-> scalar / float1, component list, shrink cast
        const float1x3 sp = 2.0f;
        CHECK(all(sp == float1x3(2.0f, 2.0f, 2.0f)));
        const float3 v13 = m13;
        const float3 v31 = m31;
        CHECK(all(v13 == float3(1.0f, 2.0f, 3.0f)));
        CHECK(all(v31 == float3(4.0f, 5.0f, 6.0f)));
        const float1x3 from_v3 = v13;
        const float3x1 col_from_v3 = v31;
        CHECK(all(from_v3 == m13));
        CHECK(all(col_from_v3 == m31));
        const float s11 = m11;
        const float1 f1_from_m11 = m11;
        const float1x1 m11_from_f1 = f1_from_m11;
        CHECK_EQF(s11, 7.0f);
        CHECK_EQF(float(f1_from_m11), 7.0f);
        CHECK_EQF(m11_from_f1[0][0], 7.0f);

        const float3x1 from_f1s(float1(1.0f), float1(2.0f), float1(3.0f));
        const float3x1 from_mixed(float2(1.0f, 2.0f), 3.0f);
        CHECK(all(from_f1s == from_mixed));

        const float3x3 big(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const float1x3 cut13 = static_cast<float1x3>(big);
        const float3x1 cut31 = static_cast<float3x1>(big);
        const float1x1 cut11 = static_cast<float1x1>(big);
        CHECK(all(cut13 == float1x3(1, 2, 3)));
        CHECK(all(cut31 == float3x1(1, 4, 7)));
        CHECK_EQF(float(cut11), 1.0f);

        // Arithmetic, scalar, unary, same-size vector mixing
        CHECK(all((m13 + m13) == float1x3(2, 4, 6)));
        CHECK(all((m31 * 2.0f) == float3x1(8, 10, 12)));
        CHECK(all((2.0f + m31) == float3x1(6, 7, 8)));
        CHECK(all((-m31) == float3x1(-4, -5, -6)));
        CHECK(all((m11 + 3.0f) == float1x1(10.0f)));
        static_assert(std::is_same_v<decltype(m11 + 3.0f), float1x1>);
        static_assert(std::is_same_v<decltype(m13 + v13), float1x3>);
        static_assert(std::is_same_v<decltype(v13 + m13), float3>);
        static_assert(std::is_same_v<decltype(m31 + v31), float3x1>);
        static_assert(std::is_same_v<decltype(v31 + m31), float3>);
        CHECK(all((m13 + v31) == float1x3(5, 7, 9)));
        CHECK(all((v13 + m31) == float3(5, 7, 9)));
        float1x3 acc13 = m13;
        acc13 += v31;
        CHECK(all(acc13 == float1x3(5, 7, 9)));
        float3 acc3 = v13;
        acc3 += m31;
        CHECK(all(acc3 == float3(5, 7, 9)));
        CHECK(all(m13 == v13));
        CHECK(all(v31 == m31));

        // mul: all 1-dim combinations
        const float1 p_mv = mul(m13, v31);  // 1*4 + 2*5 + 3*6 = 32
        const float1 p_vm = mul(v13, m31);  // 32
        const float1x1 p_mm11 = mul(m13, m31);
        static_assert(std::is_same_v<decltype(mul(m13, v31)), float1>);
        static_assert(std::is_same_v<decltype(mul(v13, m31)), float1>);
        static_assert(std::is_same_v<decltype(mul(m13, m31)), float1x1>);
        CHECK_EQF(float(p_mv), 32.0f);
        CHECK_EQF(float(p_vm), 32.0f);
        CHECK_EQF(float(p_mm11), 32.0f);

        const float3x3 outer = mul(m31, m13);
        CHECK(all(outer[0] == float3(4, 8, 12)));
        CHECK(all(outer[1] == float3(5, 10, 15)));
        CHECK(all(outer[2] == float3(6, 12, 18)));

        const float3 p_v1m = mul(float1(2.0f), m13);
        const float3 p_mv1 = mul(m31, float1(2.0f));
        static_assert(std::is_same_v<decltype(mul(float1(2.0f), m13)), float3>);
        static_assert(std::is_same_v<decltype(mul(m31, float1(2.0f))), float3>);
        static_assert(std::is_same_v<decltype(mul(2.0f, m13)), float1x3>);
        static_assert(std::is_same_v<decltype(mul(v13.x, m13)), float1x3>);
        static_assert(std::is_same_v<decltype(mul(m31, 2.0f)), float3x1>);
        CHECK(all(p_v1m == float3(2, 4, 6)));
        CHECK(all(p_mv1 == float3(8, 10, 12)));
        CHECK(all(mul(2.0f, m13) == float1x3(2, 4, 6)));
        CHECK(all(mul(m31, 2.0f) == float3x1(8, 10, 12)));

        const float3x1 mv_col = mul(big, m31);
        const float1x3 vm_row = mul(m13, big);
        CHECK(all(mv_col == float3x1(32, 77, 122)));
        CHECK(all(vm_row == float1x3(30, 36, 42)));
        CHECK_EQF(float(mul(m11, m11)), 49.0f);
        CHECK_EQF(float(mul(m11, float1(3.0f))), 21.0f);
        CHECK_EQF(float(mul(float1(3.0f), m11)), 21.0f);

        // transpose, determinant, ext::inverse, component-wise intrinsics
        CHECK(all(transpose(m13) == float3x1(1, 2, 3)));
        CHECK(all(transpose(m31) == float1x3(4, 5, 6)));
        CHECK(all(transpose(m11) == m11));
        CHECK_EQF(determinant(m11), 7.0f);
        CHECK_NEAR(float(shader_math::ext::inverse(float1x1(4.0f))), 0.25f);

        CHECK(all(sqrt(float3x1(4.0f, 9.0f, 16.0f)) == float3x1(2.0f, 3.0f, 4.0f)));
        CHECK(all(abs(float3x1(-1.0f, 2.0f, -3.0f)) == float3x1(1.0f, 2.0f, 3.0f)));
        CHECK(all(clamp(m31, 4.5f, 5.5f) == float3x1(4.5f, 5.0f, 5.5f)));
        CHECK(all(min(m31, 5.0f) == float3x1(4.0f, 5.0f, 5.0f)));
        CHECK(all(fmod(float3x1(5.5f, -5.5f, 4.0f), 2.0f) == float3x1(1.5f, -1.5f, 0.0f)));
        CHECK(all(mad(m31, float3x1(2.0f), float3x1(1.0f)) == float3x1(9.0f, 11.0f, 13.0f)));
        CHECK(!any(isnan(m31)));
        CHECK(all(isfinite(m31)));

        // Integer 1-dim matrices and scalar promotions
        const int3x1 im31(-2, 3, -5);
        const int1x3 im13(1, 2, 3);
        static_assert(std::is_same_v<decltype(im31 * 2u), uint3x1>);
        static_assert(std::is_same_v<decltype(im31 & 1u), uint3x1>);
        static_assert(std::is_same_v<decltype(im31 % 2u), uint3x1>);
        static_assert(std::is_same_v<decltype(im31 << 1), int3x1>);
        static_assert(std::is_same_v<decltype(min(im31, 2u)), uint3x1>);
        static_assert(std::is_same_v<decltype(sqrt(im31)), float3x1>);
        CHECK(all(abs(im31) == int3x1(2, 3, 5)));
        CHECK(all((im31 << 1) == int3x1(-4, 6, -10)));
        CHECK(all(mul(im13, im31) == int1x1(-11)));
        CHECK(all(mul(im31, im13)[0] == int3(-2, -4, -6)));
        CHECK(all(int3x1(float3x1(1.9f, -2.9f, 3.1f)) == int3x1(1, -2, 3)));

        // Boolean matrices (matrix<bool, R, C>)
        const bool1x3 b13(true, false, true);
        const bool3x1 b31 = transpose(b13);
        const bool3 bv = b13;
        const bool1x3 b13_from_v = bv;
        CHECK(all(b13 == b13_from_v));
        CHECK(b31[0][0] && !b31[1][0] && b31[2][0]);
        CHECK(all((!b31) == bool3x1(false, true, false)));
        CHECK(any(b31) && !all(b31));
        const bool1x1 b11 = (m11 < 10.0f);
        CHECK(bool(b11));
        if (!(m11 < 10.0f)) CHECK(false);

#if defined(SHADER_MATH_HAS_HALF)
        const half3x1 h31(half(1.0f), half(2.0f), half(3.0f));
        const half1x3 h13 = transpose(h31);
        const float3x1 f31 = static_cast<float3x1>(h31);
        const half3x1 h31_back = static_cast<half3x1>(f31);
        CHECK(all(f31 == float3x1(1.0f, 2.0f, 3.0f)));
        CHECK(all(h31_back == h31));
        CHECK_EQF(float(half(mul(h13, h31)[0][0])), 14.0f);
#endif
    }
}
