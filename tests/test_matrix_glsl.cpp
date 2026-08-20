#include <shader_math/shader_math.hpp>

#include "framework.hpp"

void test_matrix_glsl() {
    using namespace shader_math::glsl;

    // ---- constructors: column-major fill (GLSL spec order) -------------------
    {
        // values fill the FIRST COLUMN first (wikibooks example)
        mat3 m = {1.1f, 2.1f, 3.1f,   // column 0
                  1.2f, 2.2f, 3.2f,   // column 1
                  1.3f, 2.3f, 3.3f};  // column 2
        vec3 col0 = m[0];
        CHECK_EQF(col0[0], 1.1f);
        CHECK_EQF(col0[1], 2.1f);
        CHECK_EQF(col0[2], 3.1f);
        CHECK_EQF(m[0][1], 2.1f);  // m[col][row]
        CHECK_EQF(m[2][0], 1.3f);
    }
    // scalar ctor: diagonal (mat3(1.0) is the identity — NOT a splat)
    {
        mat3 id(1.0f);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r) CHECK_EQF(id[c][r], c == r ? 1.0f : 0.0f);
        // non-square: diagonal of min(C,R)
        mat3x2 d(2.0f);
        CHECK_EQF(d[0][0], 2.0f);
        CHECK_EQF(d[1][1], 2.0f);
        CHECK_EQF(d[2][0], 0.0f);
    }
    // column-vector ctor
    {
        vec3 c0(0.0f, 1.0f, 0.0f), c1(1.0f, 0.0f, 0.0f), c2(0.0f, 0.0f, 1.0f);
        mat3 n(c0, c1, c2);
        CHECK_EQF(n[0][1], 1.0f);
        CHECK_EQF(n[1][0], 1.0f);
        CHECK_EQF(n[2][2], 1.0f);
    }
    // matrix-from-matrix resize (GLSL rules)
    {
        mat2 m2 = {1.1f, 2.1f, 1.2f, 2.2f};
        mat3 grown(m2);  // extra row/col from the identity
        CHECK_EQF(grown[0][0], 1.1f);
        CHECK_EQF(grown[1][1], 2.2f);
        CHECK_EQF(grown[2][2], 1.0f);
        CHECK_EQF(grown[2][0], 0.0f);
        CHECK_EQF(grown[0][2], 0.0f);

        mat2 shrunk(grown);  // upper-left cut
        CHECK_EQF(shrunk[0][0], 1.1f);
        CHECK_EQF(shrunk[1][1], 2.2f);
        CHECK_EQF(shrunk[0][1], 2.1f);
    }

    // ---- operator*: true matrix product ---------------------------------------
    {
        // A: 2 rows x 2 cols with rows (1 2; 3 4) stored column-major
        mat2 a = {1.0f, 3.0f, 2.0f, 4.0f};
        mat2 b = {5.0f, 7.0f, 6.0f, 8.0f};  // rows (5 6; 7 8)
        mat2 p = a * b;
        // expected rows: (19 22; 43 50)
        CHECK_NEAR(p[0][0], 19.0f);
        CHECK_NEAR(p[0][1], 43.0f);
        CHECK_NEAR(p[1][0], 22.0f);
        CHECK_NEAR(p[1][1], 50.0f);

        // component-wise multiply is matrixCompMult, never '*'
        mat2 h = matrixCompMult(a, b);
        CHECK_EQF(h[0][0], 5.0f);
        CHECK_EQF(h[0][1], 21.0f);
        CHECK_EQF(h[1][0], 12.0f);
        CHECK_EQF(h[1][1], 32.0f);
    }
    // matrix * vector / vector * matrix
    {
        mat3 m = {1, 4, 7, 2, 5, 8, 3, 6, 9};  // rows (1 2 3; 4 5 6; 7 8 9)
        vec3 v(1.0f, 2.0f, 3.0f);

        vec3 mv = m * v;  // column-vector convention
        CHECK_NEAR(mv[0], 1.0f * 1 + 2.0f * 2 + 3.0f * 3);
        CHECK_NEAR(mv[1], 4.0f * 1 + 5.0f * 2 + 6.0f * 3);
        CHECK_NEAR(mv[2], 7.0f * 1 + 8.0f * 2 + 9.0f * 3);

        vec3 vm = v * m;  // row-vector convention
        CHECK_NEAR(vm[0], 1.0f * 1 + 2.0f * 4 + 3.0f * 7);
        CHECK_NEAR(vm[1], 1.0f * 2 + 2.0f * 5 + 3.0f * 8);
        CHECK_NEAR(vm[2], 1.0f * 3 + 2.0f * 6 + 3.0f * 9);
    }
    // non-square product: mat3x2(3 cols, 2 rows) * mat2x3(2 cols, 3 rows) -> mat2x2? no:
    // (2x3 math) * (3x2 math) -> 2x2 math -> glsl mat2x2
    {
        mat3x2 a = {1, 4, 2, 5, 3, 6};        // rows (1 2 3; 4 5 6)
        mat2x3 b = {7, 9, 11, 8, 10, 12};     // rows (7 8; 9 10; 11 12)
        mat2 p = a * b;
        // expected rows: (58 64; 139 154)
        CHECK_NEAR(p[0][0], 58.0f);
        CHECK_NEAR(p[1][0], 64.0f);
        CHECK_NEAR(p[0][1], 139.0f);
        CHECK_NEAR(p[1][1], 154.0f);
    }
    // scalar ops and compound assignment
    {
        mat2 a = {1, 2, 3, 4};
        CHECK_EQF((a * 2.0f)[1][1], 8.0f);
        CHECK_EQF((2.0f * a)[0][0], 2.0f);
        CHECK_EQF((a + a)[1][0], 6.0f);
        CHECK_EQF((a - 1.0f)[0][0], 0.0f);
        CHECK_EQF((a / a)[1][1], 1.0f);
        mat2 c = a;
        c *= mat2(1.0f);  // multiply by identity keeps c
        CHECK(c == a);
        c += a;
        CHECK_EQF(c[0][0], 2.0f);
    }

    // ---- == / != return a single bool (GLSL semantics) ------------------------
    {
        vec3 a(1.0f, 2.0f, 3.0f);
        vec3 b(1.0f, 2.0f, 3.0f);
        vec3 c(1.0f, 9.0f, 3.0f);
        bool eq = (a == b);
        CHECK(eq);
        CHECK(a != c);
        mat2 ma = {1, 2, 3, 4};
        mat2 mb = {1, 2, 3, 4};
        mat2 mc = {1, 2, 3, 5};
        CHECK(ma == mb);
        CHECK(ma != mc);
    }
    // per-component comparisons are named functions returning bvec
    {
        vec3 a(1.0f, 5.0f, 3.0f);
        vec3 b(1.0f, 2.0f, 9.0f);
        bvec3 eq = equal(a, b);
        CHECK(eq[0]);
        CHECK(!eq[1]);
        CHECK(any(eq));
        CHECK(!all(eq));
        bvec3 lt = lessThan(a, b);
        CHECK(!lt[0]);
        CHECK(!lt[1]);
        CHECK(lt[2]);
        bvec3 ge = greaterThanEqual(a, b);
        CHECK(ge[0]);
        CHECK(ge[1]);
        CHECK(!ge[2]);
        bvec3 ne = notEqual(a, b);
        CHECK(not_(eq) == ne);  // bvec == bvec -> bool in glsl
    }

    // ---- outerProduct -----------------------------------------------------------
    {
        vec3 c(1.0f, 2.0f, 3.0f);
        vec2 r(4.0f, 5.0f);
        // c (3x1) * r (1x2) -> 3x2 math -> glsl mat2x3 (2 cols, 3 rows)
        mat2x3 o = outerProduct(c, r);
        for (int j = 0; j < 2; ++j)
            for (int i = 0; i < 3; ++i) CHECK_NEAR(o[j][i], c[i] * r[j]);
    }

    // ---- transpose / determinant / inverse ---------------------------------------
    {
        mat3x2 m = {1, 4, 2, 5, 3, 6};  // 2x3 math
        static_assert(std::is_same_v<decltype(transpose(m)), mat2x3>);
        mat2x3 t = transpose(m);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 2; ++j) CHECK_EQF(t[j][i], m[i][j]);

        mat2 m2 = {3, 2, 5, 1};  // rows (3 5; 2 1)
        CHECK_NEAR(determinant(m2), 3.0f * 1 - 5.0f * 2);

        mat3 m3 = {4, 1, 9, 3, 5, 6, 2, 7, 8};
        CHECK(!near_f(determinant(m3), 0.0f));
        mat3 i3 = m3 * inverse(m3);
        for (int c = 0; c < 3; ++c)
            for (int r = 0; r < 3; ++r) CHECK_NEAR(i3[c][r], c == r ? 1.0f : 0.0f);

        mat4 m4 = {4, 0, 1, 0, 0, 3, 0, 2, 0, 0, 2, 0, 1, 2, 0, 5};
        CHECK(!near_f(determinant(m4), 0.0f));
        mat4 i4 = m4 * inverse(m4);
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r) CHECK_NEAR(i4[c][r], c == r ? 1.0f : 0.0f);
    }
}
