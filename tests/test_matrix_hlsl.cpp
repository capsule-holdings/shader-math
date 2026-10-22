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
        float3x3 i3 = mul(m3b, inverse(m3b));
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c) CHECK_NEAR(i3[r][c], r == c ? 1.0f : 0.0f);
        (void)m3;

        float4x4 m4 = {4, 0, 0, 1, 0, 3, 0, 2, 1, 0, 2, 0, 0, 2, 0, 5};
        const float d4 = determinant(m4);
        CHECK(!near_f(d4, 0.0f));
        float4x4 i4 = mul(m4, inverse(m4));
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
}
