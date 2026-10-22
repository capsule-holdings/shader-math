// Component-wise intrinsic functions (v2): exact / approximate tiers, half,
// language-specific name isolation, and the ScalarFor operand rules.
//
// NOTE at namespace scope: no using-directives here (like test_restrict.cpp);
// the unqualified concepts below resolve through ADL only, which is exactly
// what must NOT find the language-specific spellings.
#include <cmath>
#include <concepts>
#include <limits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;

// ---- language isolation: HLSL-only / GLSL-only spellings must not leak ------
// The cores live in detail as *_impl, so ADL on the shared vector type finds
// nothing under the user-facing names.
template <class... As>
concept adl_lerp = requires(const As&... as) { lerp(as...); };
template <class A>
concept adl_saturate = requires(const A& a) { saturate(a); };
template <class A>
concept adl_frac = requires(const A& a) { frac(a); };
template <class A>
concept adl_rcp = requires(const A& a) { rcp(a); };
template <class A>
concept adl_rsqrt = requires(const A& a) { rsqrt(a); };
template <class... As>
concept adl_mix = requires(const As&... as) { mix(as...); };
template <class A>
concept adl_fract = requires(const A& a) { fract(a); };
template <class A>
concept adl_inversesqrt = requires(const A& a) { inversesqrt(a); };
template <class A>
concept adl_roundEven = requires(const A& a) { roundEven(a); };

static_assert(!adl_lerp<h::float3, h::float3, h::float3>);
static_assert(!adl_saturate<h::float3>);
static_assert(!adl_frac<h::float3>);
static_assert(!adl_rcp<h::float3>);
static_assert(!adl_rsqrt<h::float3>);
static_assert(!adl_mix<g::vec3, g::vec3, g::vec3>);
static_assert(!adl_fract<g::vec3>);
static_assert(!adl_inversesqrt<g::vec3>);
static_assert(!adl_roundEven<g::vec3>);

// positive: the qualified language-specific names exist
static_assert(requires(const h::float3& v) { shader_math::hlsl::lerp(v, v, v); });
static_assert(requires(const h::float3& v) { shader_math::hlsl::saturate(v); });
static_assert(requires(const h::float3& v) { shader_math::hlsl::frac(v); });
static_assert(requires(const h::float3& v) { shader_math::hlsl::rcp(v); });
static_assert(requires(const h::float3& v) { shader_math::hlsl::rsqrt(v); });
static_assert(requires(const g::vec3& v) { shader_math::glsl::mix(v, v, v); });
static_assert(requires(const g::vec3& v) { shader_math::glsl::fract(v); });
static_assert(requires(const g::vec3& v) { shader_math::glsl::inversesqrt(v); });
static_assert(requires(const g::vec3& v) { shader_math::glsl::roundEven(v); });

// geometry / trigonometry additions: HLSL-only and GLSL-only spellings
template <class A>
concept adl_atan2 = requires(const A& a) { atan2(a, a); };
template <class A>
concept adl_fmod = requires(const A& a) { fmod(a, a); };
template <class A>
concept adl_mod = requires(const A& a) { mod(a, a); };
template <class A>
concept adl_mad = requires(const A& a) { mad(a, a, a); };
template <class A>
concept adl_fma = requires(const A& a) { fma(a, a, a); };
// typed: the MSVC UCRT's unconstrained global isfinite(_Ty) -> bool is
// visible here too; what must not be reachable is a bool-vector overload
template <class A>
concept adl_isfinite = requires(const A& a) {
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v>>;
};
template <class A>
concept adl_atan_yx = requires(const A& a) { atan(a, a); };
static_assert(!adl_atan2<h::float3>);
static_assert(!adl_fmod<h::float3>);
static_assert(!adl_mod<g::vec3>);
static_assert(!adl_mad<h::float3>);
static_assert(!adl_fma<g::vec3>);
static_assert(!adl_isfinite<h::float3>);
static_assert(!adl_atan_yx<g::vec3>);  // the shared detail::atan is unary only

// each language sees only its own spelling
namespace hlsl_scope {
using namespace shader_math::hlsl;
template <class A>
concept has_atan2 = requires(const A& a) { atan2(a, a); };
template <class A>
concept has_atan_yx = requires(const A& a) { atan(a, a); };
template <class A>
concept has_fmod = requires(const A& a) { fmod(a, a); };
template <class A>
concept has_mod = requires(const A& a) { mod(a, a); };
template <class A>
concept has_mad = requires(const A& a) { mad(a, a, a); };
template <class A>
concept has_fma = requires(const A& a) { fma(a, a, a); };
// typed: the MSVC UCRT's global isfinite(_Ty) -> bool is visible in every
// scope, so "has isfinite" means an overload returning the bool vector
template <class A>
concept has_isfinite = requires(const A& a) {
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v>>;
};
template <class A>
concept has_sincos = requires(const A& a, A& s, A& c) { sincos(a, s, c); };
static_assert(has_atan2<float3> && !has_atan_yx<float3>);
static_assert(has_fmod<float3> && !has_mod<float3>);
static_assert(has_mad<float3> && !has_fma<float3>);
static_assert(has_isfinite<float3> && has_sincos<float3>);
}  // namespace hlsl_scope

namespace glsl_scope {
using namespace shader_math::glsl;
template <class A>
concept has_atan2 = requires(const A& a) { atan2(a, a); };
template <class A>
concept has_atan_yx = requires(const A& a) { atan(a, a); };
template <class A>
concept has_fmod = requires(const A& a) { fmod(a, a); };
template <class A>
concept has_mod = requires(const A& a) { mod(a, a); };
template <class A>
concept has_mad = requires(const A& a) { mad(a, a, a); };
template <class A>
concept has_fma = requires(const A& a) { fma(a, a, a); };
template <class A>
concept has_isfinite = requires(const A& a) {
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v>>;
};
template <class A>
concept has_sincos = requires(const A& a, A& s, A& c) { sincos(a, s, c); };
static_assert(has_atan_yx<vec3> && !has_atan2<vec3>);
static_assert(has_mod<vec3> && !has_fmod<vec3>);
static_assert(has_fma<vec3> && !has_mad<vec3>);
static_assert(!has_isfinite<vec3> && !has_sincos<vec3>);
}  // namespace glsl_scope

// ---- ScalarFor operand rules (common-name functions are ADL-visible) --------
template <class A, class S>
concept adl_min_s = requires(const A& a, const S& s) { min(a, s); };
static_assert(adl_min_s<h::float3, float>);
static_assert(adl_min_s<h::float3, int>);  // float vectors keep loose scalars
#if defined(SHADER_MATH_HAS_HALF)
static_assert(adl_min_s<h::half3, h::half>);
static_assert(!adl_min_s<h::half3, float>);  // no implicit half/float mixing
static_assert(!adl_lerp<h::half3, h::half3, float>);
#endif

static void test_funcs_exact() {
    using namespace shader_math::hlsl;

    const float3 v(-1.5f, 0.0f, 2.5f);

    const float3 av = abs(v);
    CHECK_EQF(av[0], 1.5f);
    CHECK_EQF(av[1], 0.0f);
    CHECK_EQF(av[2], 2.5f);

    const float3 sg = sign(v);
    CHECK_EQF(sg[0], -1.0f);
    CHECK_EQF(sg[1], 0.0f);
    CHECK_EQF(sg[2], 1.0f);

    const float3 w(2.0f, -3.0f, 1.0f);
    const float3 mn = min(v, w);
    const float3 mx = max(v, w);
    CHECK_EQF(mn[0], -1.5f);
    CHECK_EQF(mn[1], -3.0f);
    CHECK_EQF(mx[0], 2.0f);
    CHECK_EQF(mx[2], 2.5f);
    CHECK_EQF(min(v, 0.5f)[2], 0.5f);   // scalar form
    CHECK_EQF(max(v, 0.0f)[0], 0.0f);

    const float3 cl = clamp(v, -1.0f, 1.0f);
    CHECK_EQF(cl[0], -1.0f);
    CHECK_EQF(cl[1], 0.0f);
    CHECK_EQF(cl[2], 1.0f);
    const float3 st = saturate(v);
    CHECK_EQF(st[0], 0.0f);
    CHECK_EQF(st[2], 1.0f);

    // rounding tier: -1.5 / 0.25 / 2.5 hit every rule differently
    const float3 r(-1.5f, 0.25f, 2.5f);
    CHECK_EQF(floor(r)[0], -2.0f);
    CHECK_EQF(floor(r)[1], 0.0f);
    CHECK_EQF(ceil(r)[0], -1.0f);
    CHECK_EQF(ceil(r)[2], 3.0f);
    CHECK_EQF(trunc(r)[0], -1.0f);
    CHECK_EQF(trunc(r)[2], 2.0f);
    CHECK_EQF(round(r)[0], -2.0f);  // half away from zero
    CHECK_EQF(round(r)[2], 3.0f);

    const float3 fr = frac(float3(1.25f, -0.25f, 3.0f));
    CHECK_EQF(fr[0], 0.25f);
    CHECK_EQF(fr[1], 0.75f);  // frac(-0.25) = -0.25 - (-1) = 0.75
    CHECK_EQF(fr[2], 0.0f);

    const float3 sq = sqrt(float3(4.0f, 9.0f, 0.25f));
    CHECK_EQF(sq[0], 2.0f);
    CHECK_EQF(sq[1], 3.0f);
    CHECK_EQF(sq[2], 0.5f);
    CHECK_EQF(rcp(float3(2.0f, 4.0f, 0.5f))[1], 0.25f);   // powers of two: exact
    CHECK_EQF(rsqrt(float3(4.0f, 16.0f, 1.0f))[1], 0.25f);

    const float3 sp = step(0.0f, v);
    CHECK_EQF(sp[0], 0.0f);
    CHECK_EQF(sp[1], 1.0f);  // x >= edge
    CHECK_EQF(sp[2], 1.0f);
    const float3 spv = step(float3(1.0f, -1.0f, 3.0f), v);
    CHECK_EQF(spv[0], 0.0f);
    CHECK_EQF(spv[1], 1.0f);
    CHECK_EQF(spv[2], 0.0f);
}

static void test_funcs_near() {
    using namespace shader_math::hlsl;

    const float3 a(1.0f, 2.0f, 3.0f);
    const float3 b(5.0f, 6.0f, 7.0f);
    const float3 lp = lerp(a, b, 0.25f);
    CHECK_NEAR(lp[0], 2.0f);
    CHECK_NEAR(lp[2], 4.0f);
    const float3 lpv = lerp(a, b, float3(0.0f, 0.5f, 1.0f));
    CHECK_EQF(lpv[0], 1.0f);
    CHECK_NEAR(lpv[1], 4.0f);
    CHECK_EQF(lpv[2], 7.0f);

    const float3 ss = smoothstep(0.0f, 4.0f, float3(-1.0f, 2.0f, 5.0f));
    CHECK_EQF(ss[0], 0.0f);
    CHECK_NEAR(ss[1], 0.5f);
    CHECK_EQF(ss[2], 1.0f);

    // transcendentals against the same libm the implementation calls
    const float3 x(0.5f, 1.0f, 2.0f);
    CHECK_NEAR(exp(x)[0], std::exp(0.5f));
    CHECK_NEAR(exp2(x)[2], 4.0f);
    CHECK_NEAR(log(x)[1], 0.0f);
    CHECK_NEAR(log2(x)[2], 1.0f);
    CHECK_NEAR(pow(x, 2.0f)[2], 4.0f);
    CHECK_NEAR(pow(x, float3(1.0f, 2.0f, 3.0f))[2], 8.0f);
    CHECK_NEAR(sin(x)[1], std::sin(1.0f));
    CHECK_NEAR(cos(x)[1], std::cos(1.0f));
    CHECK_NEAR(tan(x)[0], std::tan(0.5f));

    const float2 l2 = log(float2(1.0f, 2.0f));
    CHECK_EQF(l2[0], 0.0f);
}

static void test_funcs_glsl() {
    using namespace shader_math::glsl;

    const vec3 v(-1.5f, 0.25f, 2.5f);
    CHECK_EQF(fract(vec3(1.25f, -0.25f, 3.0f))[1], 0.75f);
    CHECK_NEAR(mix(vec3(1.0f, 2.0f, 3.0f), vec3(5.0f, 6.0f, 7.0f), 0.25f)[0], 2.0f);
    CHECK_EQF(inversesqrt(vec3(4.0f, 16.0f, 1.0f))[0], 0.5f);
    // roundEven vs round on exact halves
    CHECK_EQF(roundEven(v)[0], -2.0f);  // -1.5 -> -2 (even)
    CHECK_EQF(roundEven(v)[2], 2.0f);   // 2.5 -> 2 (even)
    CHECK_EQF(round(v)[2], 3.0f);       // half away from zero
    CHECK_EQF(clamp(v, 0.0f, 1.0f)[0], 0.0f);
    CHECK_EQF(step(0.0f, v)[2], 1.0f);
}

static void test_funcs_geometry_hlsl() {
    using namespace shader_math::hlsl;

    CHECK_EQF(distance(float3(1.0f, 2.0f, 3.0f), float3(4.0f, 6.0f, 3.0f)), 5.0f);

    // reflect about +y
    const float3 r = reflect(float3(1.0f, -1.0f, 0.0f), float3(0.0f, 1.0f, 0.0f));
    CHECK_EQF(r[0], 1.0f);
    CHECK_EQF(r[1], 1.0f);
    CHECK_EQF(r[2], 0.0f);

    // refract: eta = 1 passes straight through; grazing glass->air is TIR
    const float3 in = normalize(float3(1.0f, -1.0f, 0.0f));
    const float3 up(0.0f, 1.0f, 0.0f);
    const float3 same = refract(in, up, 1.0f);
    CHECK_NEAR(same[0], in[0]);
    CHECK_NEAR(same[1], in[1]);
    const float3 tir = refract(in, up, 1.5f);
    CHECK_EQF(tir[0], 0.0f);
    CHECK_EQF(tir[1], 0.0f);
    // Snell: sin(t) = eta * sin(i) for air->glass
    const float3 t = refract(in, up, 1.0f / 1.5f);
    CHECK_NEAR(t[0], in[0] / 1.5f);
    CHECK_NEAR(length(t), 1.0f);

    const float3 nn(0.0f, 0.0f, 1.0f);
    CHECK_EQF(faceforward(nn, float3(0.0f, 0.0f, -1.0f), nn)[2], 1.0f);
    CHECK_EQF(faceforward(nn, float3(0.0f, 0.0f, 1.0f), nn)[2], -1.0f);

    const float3 x(0.5f, -0.25f, 1.0f);
    CHECK_NEAR(asin(x)[0], std::asin(0.5f));
    CHECK_NEAR(acos(x)[1], std::acos(-0.25f));
    CHECK_NEAR(atan(x)[2], std::atan(1.0f));
    CHECK_NEAR(sinh(x)[0], std::sinh(0.5f));
    CHECK_NEAR(cosh(x)[1], std::cosh(-0.25f));
    CHECK_NEAR(tanh(x)[2], std::tanh(1.0f));
    // atan2(y, x): all four quadrants
    const float2 a2 = atan2(float2(1.0f, -1.0f), float2(-1.0f, -1.0f));
    CHECK_NEAR(a2[0], std::atan2(1.0f, -1.0f));
    CHECK_NEAR(a2[1], std::atan2(-1.0f, -1.0f));
    CHECK_NEAR(radians(float2(180.0f, 90.0f))[0], 3.14159265f);
    CHECK_NEAR(degrees(float2(3.14159265f, 0.0f))[0], 180.0f);

    // fmod keeps the sign of x (trunc-based)
    const float3 fm = fmod(float3(-7.0f, 7.0f, 5.5f), float3(3.0f, -3.0f, 2.0f));
    CHECK_EQF(fm[0], -1.0f);
    CHECK_EQF(fm[1], 1.0f);
    CHECK_EQF(fm[2], 1.5f);
    CHECK_EQF(fmod(float2(-7.0f, 7.0f), 3.0f)[0], -1.0f);  // scalar divisor

    CHECK_NEAR(mad(float2(2.0f, 3.0f), float2(4.0f, 5.0f), float2(1.0f, -1.0f))[1], 14.0f);

    float3 ip;
    const float3 fr = modf(float3(2.75f, -2.75f, 0.5f), ip);
    CHECK_EQF(fr[0], 0.75f);
    CHECK_EQF(ip[0], 2.0f);
    CHECK_EQF(fr[1], -0.75f);  // sign of x
    CHECK_EQF(ip[1], -2.0f);
    const float inf = std::numeric_limits<float>::infinity();
    float2 ipi;
    CHECK_EQF(modf(float2(inf, -inf), ipi)[0], 0.0f);
    CHECK_EQF(ipi[1], -inf);

    float3 s;
    float3 c;
    sincos(x, s, c);
    CHECK_NEAR(s[0], std::sin(0.5f));
    CHECK_NEAR(c[2], std::cos(1.0f));

    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float4 special(nan, inf, -inf, 1.0f);
    const bool4 isn = isnan(special);
    const bool4 isi = isinf(special);
    const bool4 isf = isfinite(special);
    CHECK(isn[0] && !isn[1] && !isn[2] && !isn[3]);
    CHECK(!isi[0] && isi[1] && isi[2] && !isi[3]);
    CHECK(!isf[0] && !isf[1] && !isf[2] && isf[3]);
    const bool2 sw = isnan(special.xw);  // swizzle operand
    CHECK(sw[0] && !sw[1]);
    CHECK(isinf(special.yz)[1]);
    CHECK(isfinite(special.zw)[1]);
}

static void test_funcs_geometry_glsl() {
    using namespace shader_math::glsl;

    // mod takes the sign of y (floor-based): differs from hlsl::fmod
    const vec3 md = mod(vec3(-7.0f, 7.0f, 5.5f), vec3(3.0f, -3.0f, 2.0f));
    CHECK_NEAR(md[0], 2.0f);
    CHECK_NEAR(md[1], -2.0f);
    CHECK_NEAR(md[2], 1.5f);
    CHECK_NEAR(mod(vec2(-7.0f, 7.0f), 3.0f)[0], 2.0f);
    CHECK_NEAR(atan(vec2(1.0f, -1.0f), vec2(-1.0f, -1.0f))[1], std::atan2(-1.0f, -1.0f));
    CHECK_NEAR(atan(vec2(1.0f, 0.0f))[0], std::atan(1.0f));
    CHECK_NEAR(fma(vec2(2.0f, 3.0f), vec2(4.0f, 5.0f), vec2(1.0f, -1.0f))[0], 9.0f);
    CHECK_EQF(distance(vec2(0.0f, 0.0f), vec2(3.0f, 4.0f)), 5.0f);
    CHECK_EQF(reflect(vec2(1.0f, -1.0f), vec2(0.0f, 1.0f))[1], 1.0f);
    const bvec2 nan2 = isnan(vec2(std::numeric_limits<float>::quiet_NaN(), 0.0f));
    CHECK(nan2[0] && !nan2[1]);
}

#if defined(SHADER_MATH_HAS_HALF)
static void test_funcs_half() {
    using namespace shader_math::hlsl;

    const half3 v(-1.5f, 0.0f, 2.5f);
    CHECK_EQF(static_cast<float>(abs(v)[0]), 1.5f);
    CHECK_EQF(static_cast<float>(sign(v)[0]), -1.0f);
    CHECK_EQF(static_cast<float>(min(v, half(0.5f))[2]), 0.5f);
    CHECK_EQF(static_cast<float>(clamp(v, half(-1.0f), half(1.0f))[2]), 1.0f);
    CHECK_EQF(static_cast<float>(saturate(v)[0]), 0.0f);
    CHECK_EQF(static_cast<float>(floor(v)[0]), -2.0f);
    CHECK_EQF(static_cast<float>(round(v)[2]), 3.0f);
    CHECK_EQF(static_cast<float>(frac(half3(1.25f, -0.25f, 3.0f))[0]), 0.25f);
    CHECK_EQF(static_cast<float>(sqrt(half3(4.0f, 9.0f, 0.25f))[1]), 3.0f);
    CHECK_EQF(static_cast<float>(step(half(0.0f), v)[2]), 1.0f);

    CHECK_NEAR_H(lerp(half3(1.0f, 2.0f, 3.0f), half3(5.0f, 6.0f, 7.0f), half(0.25f))[0], 2.0f);
    CHECK_NEAR_H(smoothstep(half(0.0f), half(4.0f), half3(-1.0f, 2.0f, 5.0f))[1], 0.5f);
    CHECK_NEAR_H(exp(half3(0.0f, 1.0f, 2.0f))[1], 2.71828f);
    CHECK_NEAR_H(pow(half3(2.0f, 3.0f, 4.0f), half(2.0f))[0], 4.0f);
    CHECK_NEAR_H(rsqrt(half3(4.0f, 16.0f, 1.0f))[1], 0.25f);

    CHECK_NEAR_H(distance(half3(1.0f, 2.0f, 3.0f), half3(4.0f, 6.0f, 3.0f)), 5.0f);
    CHECK_NEAR_H(reflect(half2(1.0f, -1.0f), half2(0.0f, 1.0f))[1], 1.0f);
    CHECK_NEAR_H(atan2(half2(1.0f, 1.0f), half2(1.0f, -1.0f))[0], 0.785398f);
    CHECK_NEAR_H(fmod(half2(-7.0f, 7.0f), half(3.0f))[0], -1.0f);
    CHECK(isinf(half2(half(65504.0f) * half(2.0f), half(1.0f)))[0]);
}
#else
static void test_funcs_half() {}
#endif

void test_funcs() {
    test_funcs_exact();
    test_funcs_near();
    test_funcs_glsl();
    test_funcs_geometry_hlsl();
    test_funcs_geometry_glsl();
    test_funcs_half();
}
