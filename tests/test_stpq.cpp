// Opt-in GLSL .stpq swizzles (SHADER_MATH_GLSL_STPQ). Its own executable:
// the macro changes the vector class definition, so it must not be mixed
// with translation units built without it.
#if !defined(SHADER_MATH_GLSL_STPQ)
#  error "build this test with SHADER_MATH_GLSL_STPQ"
#endif

#include <cstdio>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

template <class V>
concept can_write_ss = requires(V v) { v.ss = v.st; };
template <class V>
concept has_s = requires(V v) { v.s; };
template <class V>
concept has_stp = requires(V v) { v.stp; };

// glsl vectors only: hlsl has no .stpq
static_assert(has_stp<shader_math::glsl::vec4> && has_s<shader_math::glsl::bvec3>);
static_assert(!has_s<shader_math::hlsl::float4> && !has_stp<shader_math::hlsl::float4>);
static_assert(!has_s<shader_math::hlsl::bool3>);

int main() {
    using namespace shader_math::glsl;

    vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK_EQF(float(v.s), 1.0f);
    CHECK_EQF(float(v.q), 4.0f);
    const vec2 st = v.st;
    CHECK_EQF(st[0], 1.0f);
    CHECK_EQF(st[1], 2.0f);
    const vec4 r = v.qpts;
    CHECK_EQF(r[0], 4.0f);
    CHECK_EQF(r[3], 1.0f);
    CHECK(v.stp == v.xyz);  // same lanes as xyz / rgb

    v.ts = vec2(9.0f, 8.0f);  // write through a permutation
    CHECK_EQF(v[0], 8.0f);
    CHECK_EQF(v[1], 9.0f);
    v.p = 7.0f;
    CHECK_EQF(v.z, 7.0f);
    static_assert(!can_write_ss<vec4>);  // repeated components stay read-only

    const vec2 tc = vec2(0.25f, 0.75f);
    CHECK_EQF(float((tc.st * 2.0f).t), 1.5f);

    const bvec3 b = lessThan(vec3(1.0f, 5.0f, 3.0f), vec3(2.0f));
    CHECK(b.s && !b.t && !b.p);

    std::printf("[shader-math stpq] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
