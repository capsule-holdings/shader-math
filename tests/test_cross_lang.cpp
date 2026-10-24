// Cross-language equivalence: hlsl::floatRxC and glsl::matCxR are the same
// mathematical object; products computed on either side must agree.
#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;

template <class A, class B>
concept can_add = requires(const A& a, const B& b) { a + b; };
template <class A, class B>
concept can_dot = requires(const A& a, const B& b) { shader_math::detail::dot(a, b); };
template <class A, class B>
concept can_times = requires(const A& a, const B& b) { a * b; };
template <class A, class B>
concept can_mul = requires(const A& a, const B& b) { h::mul(a, b); };

void test_cross_lang() {
    // ---- element correspondence: float3x4 (3 rows x 4 cols) == mat4x3 --------
    {
        h::float3x4 A = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
        g::mat4x3 G(A);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c) CHECK_EQF(G[c][r], A[r][c]);

        // round trip
        h::float3x4 B(G);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 4; ++c) CHECK_EQF(B[r][c], A[r][c]);
    }
    // non-square 2x3: float2x3 == mat3x2
    {
        h::float2x3 A = {1, 2, 3, 4, 5, 6};
        g::mat3x2 G(A);
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 3; ++c) CHECK_EQF(G[c][r], A[r][c]);
    }

    // ---- product equivalence: hlsl mul(A,B) == glsl gA * gB -------------------
    {
        h::float2x3 A = {1, -2, 3, 4, 5, -6};
        h::float3x4 B = {1, 2, 3, 4, -5, 6, 7, 8, 9, 10, -11, 12};
        h::float2x4 C = h::mul(A, B);

        g::mat3x2 gA(A);
        g::mat4x3 gB(B);
        g::mat4x2 gC = gA * gB;
        for (int r = 0; r < 2; ++r)
            for (int c = 0; c < 4; ++c) CHECK_NEAR(gC[c][r], C[r][c]);
    }

    // ---- vector transform equivalence ------------------------------------------
    {
        h::float3x3 M = {2, -1, 0, 1, 3, 5, -2, 4, 1};
        g::mat3 gM(M);
        const h::float3 v(1.0f, -2.0f, 3.0f);
        const g::vec3 u(v);

        // HLSL mul(v, M) (row vector) == GLSL v * m
        auto hv = h::mul(v, M);
        auto gv = u * gM;
        for (int i = 0; i < 3; ++i) CHECK_NEAR(hv[i], gv[i]);

        // HLSL mul(M, v) (column vector) == GLSL m * v
        auto hc = h::mul(M, v);
        auto gc = gM * u;
        for (int i = 0; i < 3; ++i) CHECK_NEAR(hc[i], gc[i]);
    }

    // ---- transpose / determinant / inverse equivalence --------------------------
    {
        h::float4x4 M = {4, 0, 0, 1, 0, 3, 0, 2, 1, 0, 2, 0, 0, 2, 0, 5};
        g::mat4 gM(M);
        CHECK_NEAR(h::determinant(M), g::determinant(gM));

        h::float4x4 hi = shader_math::ext::inverse(M);
        g::mat4 gi = g::inverse(gM);
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) CHECK_NEAR(gi[c][r], hi[r][c]);

        h::float4x4 ht = h::transpose(M);
        g::mat4 gt = g::transpose(gM);
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) CHECK_EQF(gt[c][r], ht[r][c]);
    }

    // ---- vectors: distinct types, explicit conversion ---------------------------
    {
        static_assert(!std::is_same_v<h::float3, g::vec3>);
        static_assert(!std::is_same_v<h::bool3, g::bvec3>);
        static_assert(!std::is_convertible_v<h::float3, g::vec3>);
        static_assert(!std::is_convertible_v<g::vec3, h::float3>);
        static_assert(std::is_constructible_v<g::vec3, h::float3>);
        static_assert(std::is_constructible_v<h::bool3, g::bvec3>);
        static_assert(!can_add<h::float3, g::vec3>);
        static_assert(!can_dot<h::float3, g::vec3>);
        static_assert(!can_times<g::mat3, h::float3> && can_times<g::mat3, g::vec3>);
        static_assert(!can_mul<h::float3x3, g::vec3> && can_mul<h::float3x3, h::float3>);

        const h::float3 a(1.0f, 2.0f, 3.0f);
        const g::vec3 b(a);
        CHECK_EQF(b[2], 3.0f);
        CHECK_NEAR(g::dot(b, g::vec3(a.zyx)), 10.0f);
        CHECK(h::all(h::float3(b) == a));
    }
}
