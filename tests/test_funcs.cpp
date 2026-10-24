// Component-wise intrinsic functions (v2): exact / approximate tiers, half,
// language-specific name isolation, and the ScalarFor operand rules.
//
// NOTE at namespace scope: no using-directives here (like test_restrict.cpp);
// the unqualified concepts below resolve through ADL only, which must find a
// language's own spellings on its vectors and never the other language's.
#include <cmath>
#include <concepts>
#include <limits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;

// ---- language isolation: HLSL-only / GLSL-only spellings must not leak ------
// The language tag of a vector makes its language namespace an associated
// namespace, so ADL finds that language's names and nothing of the other.
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

static_assert(adl_lerp<h::float3, h::float3, h::float3>);
static_assert(adl_saturate<h::float3> && adl_frac<h::float3>);
static_assert(adl_rcp<h::float3> && adl_rsqrt<h::float3>);
static_assert(!adl_lerp<g::vec3, g::vec3, g::vec3>);
static_assert(!adl_saturate<g::vec3> && !adl_frac<g::vec3>);
static_assert(!adl_rcp<g::vec3> && !adl_rsqrt<g::vec3>);
static_assert(adl_mix<g::vec3, g::vec3, g::vec3>);
static_assert(adl_fract<g::vec3> && adl_inversesqrt<g::vec3> && adl_roundEven<g::vec3>);
static_assert(!adl_mix<h::float3, h::float3, h::float3>);
static_assert(!adl_fract<h::float3> && !adl_inversesqrt<h::float3> && !adl_roundEven<h::float3>);
// a qualified call does not accept the other language's vectors either
template <class A>
concept hlsl_frac = requires(const A& a) { shader_math::hlsl::frac(a); };
template <class A>
concept glsl_fract = requires(const A& a) { shader_math::glsl::fract(a); };
static_assert(hlsl_frac<h::float3> && !hlsl_frac<g::vec3>);
static_assert(glsl_fract<g::vec3> && !glsl_fract<h::float3>);

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
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v, typename A::lang_t>>;
};
template <class A>
concept adl_atan_yx = requires(const A& a) { atan(a, a); };
static_assert(adl_atan2<h::float3> && !adl_atan2<g::vec3>);
static_assert(adl_fmod<h::float3> && !adl_fmod<g::vec3>);
static_assert(adl_mod<g::vec3> && !adl_mod<h::float3>);
static_assert(adl_mad<h::float3> && !adl_mad<g::vec3>);
static_assert(adl_fma<g::vec3> && !adl_fma<h::float3>);
static_assert(adl_isfinite<h::float3> && !adl_isfinite<g::vec3>);
static_assert(adl_atan_yx<g::vec3> && !adl_atan_yx<h::float3>);

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
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v, typename A::lang_t>>;
};
template <class A>
concept has_sincos = requires(const A& a, A& s, A& c) { sincos(a, s, c); };
static_assert(has_atan2<float3> && !has_atan_yx<float3>);
static_assert(has_fmod<float3> && !has_mod<float3>);
static_assert(has_mad<float3> && !has_fma<float3>);
static_assert(has_isfinite<float3> && has_sincos<float3>);
template <class A>
concept has_log10 = requires(const A& a) { log10(a); };
template <class A>
concept has_ldexp = requires(const A& a) { ldexp(a, a); };
template <class A>
concept has_frexp = requires(const A& a, A& e) { frexp(a, e); };
template <class A>
concept has_select = requires(const A& a) { select(a < a, a, a); };
template <class A>
concept has_dst = requires(const A& a) { dst(a, a); };
template <class A>
concept has_lit = requires(typename A::elem_t s) { lit(s, s, s); };
template <class A>
concept has_mod_op = requires(const A& a) { a % a; };
template <class A>
concept has_asinh = requires(const A& a) { asinh(a); acosh(a); atanh(a); };
template <class A>
concept has_mix_bvec = requires(const A& a) { mix(a, a, lessThan(a, a)); };
static_assert(has_log10<float4> && has_ldexp<float4> && has_frexp<float4> && has_select<float4>);
static_assert(has_dst<float4> && has_lit<float4> && has_mod_op<float4> && !has_asinh<float4>);
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
    { isfinite(a) } -> std::same_as<shader_math::detail::bvec<A::size_v, typename A::lang_t>>;
};
template <class A>
concept has_sincos = requires(const A& a, A& s, A& c) { sincos(a, s, c); };
static_assert(has_atan_yx<vec3> && !has_atan2<vec3>);
static_assert(has_mod<vec3> && !has_fmod<vec3>);
// GLSL's generic functions take genType only: no component-wise matrix forms
template <class M>
concept has_mat_abs = requires(const M& m) { abs(m); };
template <class M>
concept has_mat_min = requires(const M& m) { min(m, m); };
static_assert(!has_mat_abs<mat3> && !has_mat_min<mat2x3>);
static_assert(has_fma<vec3> && !has_mad<vec3>);
static_assert(!has_isfinite<vec3> && !has_sincos<vec3>);
template <class A>
concept has_log10 = requires(const A& a) { log10(a); };
template <class A>
concept has_ldexp = requires(const A& a) { ldexp(a, a); };
template <class A>
concept has_frexp = requires(const A& a, A& e) { frexp(a, e); };
template <class A>
concept has_select = requires(const A& a) { select(lessThan(a, a), a, a); };
template <class A>
concept has_dst = requires(const A& a) { dst(a, a); };
template <class A>
concept has_lit = requires(typename A::elem_t s) { lit(s, s, s); };
template <class A>
concept has_mod_op = requires(const A& a) { a % a; };
template <class A>
concept has_asinh = requires(const A& a) { asinh(a); acosh(a); atanh(a); };
template <class A>
concept has_mix_bvec = requires(const A& a) { mix(a, a, lessThan(a, a)); };
static_assert(!has_log10<vec4> && !has_ldexp<vec4> && !has_frexp<vec4> && !has_select<vec4>);
static_assert(!has_dst<vec4> && !has_lit<vec4> && !has_mod_op<vec4> && has_asinh<vec4> &&
              has_mix_bvec<vec4>);
// all / any on numeric vectors are HLSL's; GLSL's take bool vectors only
template <class A>
concept has_numeric_all = requires(const A& a) { all(a); any(a); };
static_assert(!has_numeric_all<vec3> && has_numeric_all<bvec3>);
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

    // lerp is a + t * (b - a), as DXC compiles it for DXIL
    CHECK_EQF(lerp(float2(1.0f, 0.0f), float2(3.0f, 0.0f), 0.5f)[0], 2.0f);

    // rounding tier: -1.5 / 0.25 / 2.5 hit every rule differently
    const float3 r(-1.5f, 0.25f, 2.5f);
    CHECK_EQF(floor(r)[0], -2.0f);
    CHECK_EQF(floor(r)[1], 0.0f);
    CHECK_EQF(ceil(r)[0], -1.0f);
    CHECK_EQF(ceil(r)[2], 3.0f);
    CHECK_EQF(trunc(r)[0], -1.0f);
    CHECK_EQF(trunc(r)[2], 2.0f);
    CHECK_EQF(round(r)[0], -2.0f);  // halves go to even (HLSL round = DXC Round_ne)
    CHECK_EQF(round(r)[2], 2.0f);
    const float4 halves = round(float4(0.5f, -0.5f, 3.5f, -2.5f));
    CHECK_EQF(halves[0], 0.0f);
    CHECK(std::signbit(halves[1]) && halves[1] == 0.0f);
    CHECK_EQF(halves[2], 4.0f);
    CHECK_EQF(halves[3], -2.0f);

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
    // mix is x * (1 - a) + y * a: exact at both ends however far apart x and y are
    CHECK_EQF(mix(vec2(1.0e8f, -3.0f), vec2(1.0f, 7.0f), 1.0f)[0], 1.0f);
    CHECK_EQF(mix(vec2(1.0f, -3.0f), vec2(1.0e8f, 7.0f), 0.0f)[0], 1.0f);
    CHECK_EQF(mix(vec2(1.0e8f, 0.0f), vec2(1.0f, 0.0f), vec2(1.0f, 0.5f))[0], 1.0f);
    CHECK_EQF(inversesqrt(vec3(4.0f, 16.0f, 1.0f))[0], 0.5f);
    // roundEven vs round on exact halves
    CHECK_EQF(roundEven(v)[0], -2.0f);  // -1.5 -> -2 (even)
    CHECK_EQF(roundEven(v)[2], 2.0f);   // 2.5 -> 2 (even)
    CHECK_EQF(round(v)[2], 2.0f);       // round is roundEven (the halfway direction is the implementation's)
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

static void test_funcs_compat_gaps() {
    {
        using namespace shader_math::hlsl;
        const float4 x(100.0f, 0.001f, 2.0f, 1.0f);
        CHECK_NEAR(log10(x)[0], 2.0f);
        CHECK_NEAR(log10(x)[1], -3.0f);
        CHECK_EQF(ldexp(float4(1.5f, -3.0f, 0.75f, 1.0f), float4(2.0f, -1.0f, 3.0f, 0.0f))[0], 6.0f);
        CHECK_EQF(ldexp(float4(1.5f, -3.0f, 0.75f, 1.0f), float4(2.0f, -1.0f, 3.0f, 0.0f))[1], -1.5f);
        float4 e;
        const float4 m = frexp(float4(8.0f, -0.375f, 1.0f, 0.0f), e);
        CHECK_EQF(m[0], 0.5f);   // 8 = 0.5 * 2^4
        CHECK_EQF(e[0], 4.0f);
        CHECK_EQF(m[1], -0.75f);  // -0.375 = -0.75 * 2^-1
        CHECK_EQF(e[1], -1.0f);
        CHECK_EQF(m[3], 0.0f);
        CHECK_EQF(e[3], 0.0f);

        const float4 a(1.0f, 2.0f, 3.0f, 4.0f);
        const float4 b(-1.0f, -2.0f, -3.0f, -4.0f);
        const float4 s = select(a > 2.0f, a, b);  // a where true
        CHECK_EQF(s[0], -1.0f);
        CHECK_EQF(s[3], 4.0f);
        CHECK_EQF(select(a > 2.0f, a, 0.0f)[1], 0.0f);
        CHECK_EQF(select(a > 2.0f, 9.0f, b)[2], 9.0f);

        const float4 d = dst(float4(9.0f, 2.0f, 3.0f, 4.0f), float4(5.0f, 0.5f, 7.0f, 8.0f));
        CHECK_EQF(d[0], 1.0f);
        CHECK_EQF(d[1], 1.0f);  // a.y * b.y
        CHECK_EQF(d[2], 3.0f);  // a.z
        CHECK_EQF(d[3], 8.0f);  // b.w
        const float4 l = lit(0.5f, 0.5f, 2.0f);
        CHECK_EQF(l[0], 1.0f);
        CHECK_EQF(l[1], 0.5f);
        CHECK_NEAR(l[2], 0.25f);
        CHECK_EQF(l[3], 1.0f);
        CHECK_EQF(lit(-0.5f, 0.5f, 2.0f)[1], 0.0f);  // facing away: no diffuse
        CHECK_EQF(lit(-0.5f, 0.5f, 2.0f)[2], 0.0f);  // ... and no specular
        CHECK_EQF(lit(0.5f, -0.5f, 2.0f)[2], 0.0f);

        // % is fmod: trunc-based, sign of x
        const float4 r = float4(-7.0f, 7.0f, 5.5f, -5.5f) % float4(3.0f, -3.0f, 2.0f, 2.0f);
        CHECK_EQF(r[0], -1.0f);
        CHECK_EQF(r[1], 1.0f);
        CHECK_EQF(r[2], 1.5f);
        CHECK_EQF(r[3], -1.5f);
        CHECK_EQF((float4(7.0f) % 4.0f)[0], 3.0f);
        CHECK_EQF((float2x2(5.0f, -5.0f, 7.5f, 1.0f) % 2.0f)[0][1], -1.0f);
    }
    {
        using namespace shader_math::glsl;
        const vec3 x(0.5f, 1.5f, -0.25f);
        CHECK_NEAR(asinh(x)[0], std::asinh(0.5f));
        CHECK_NEAR(acosh(x)[1], std::acosh(1.5f));
        CHECK_NEAR(atanh(x)[2], std::atanh(-0.25f));
        const vec4 a(1.0f, 2.0f, 3.0f, 4.0f);
        const vec4 b(-1.0f, -2.0f, -3.0f, -4.0f);
        const vec4 m = mix(a, b, greaterThan(a, vec4(2.0f)));  // y where true
        CHECK_EQF(m[0], 1.0f);
        CHECK_EQF(m[3], -4.0f);
    }
}

// ---- scalar arguments ----------------------------------------------------------
template <class... As>
concept hlsl_max_ok = requires(const As&... as) { shader_math::hlsl::max(as...); };

static void test_funcs_scalar() {
    {
        using namespace shader_math::hlsl;
        const float3 n(0.0f, 1.0f, 0.0f);
        const float3 l(0.6f, 0.8f, 0.0f);
        CHECK_EQF(max(dot(n, l), 0.0f), 0.8f);
        CHECK_EQF(max(-dot(n, l), 0.0), 0.0f);  // double literal next to a float
        CHECK_EQF(saturate(1.5f), 1.0f);
        CHECK_EQF(saturate(dot(n, -l)), 0.0f);
        CHECK_EQF(abs(-2.5f), 2.5f);  // not the C int abs
        CHECK_EQF(lerp(1.0f, 3.0f, 0.25f), 1.5f);
        CHECK_EQF(clamp(5.0f, 0, 1), 1.0f);
        CHECK_EQF(step(0.5f, 0.25f), 0.0f);
        CHECK_EQF(smoothstep(0.0f, 1.0f, 0.5f), 0.5f);
        CHECK_EQF(frac(-0.75f), 0.25f);
        CHECK_EQF(rcp(4.0f), 0.25f);
        CHECK_EQF(rsqrt(16.0f), 0.25f);
        CHECK_EQF(fmod(-7.0f, 3.0f), -1.0f);
        // qualified: libc++ / MSVC also declare ::round(float), so the
        // unqualified call is ambiguous there
        CHECK_EQF(shader_math::hlsl::round(2.5f), 2.0f);
        CHECK_EQF(shader_math::hlsl::round(float2(1.0f, 2.5f).y), 2.0f);
        CHECK_EQF(shader_math::glsl::round(-0.5f), -0.0f);
        CHECK_EQF(sign(-3.0f), -1.0f);
        CHECK_NEAR(pow(2.0f, 10.0f), 1024.0f);
        CHECK_NEAR(atan2(1.0f, 1.0f), 0.78539816f);
        CHECK_EQF(mad(2.0f, 3.0f, 1.0f), 7.0f);
        CHECK_EQF(ldexp(1.5f, 2.0f), 6.0f);
        CHECK_EQF(length(-3.0f), 3.0f);
        CHECK_EQF(distance(1.0f, 4.0f), 3.0f);
        CHECK_EQF(dot(2.0f, 3.0f), 6.0f);
        float ip = 0.0f;
        CHECK_EQF(modf(-2.5f, ip), -0.5f);
        CHECK_EQF(ip, -2.0f);
        float e = 0.0f;
        CHECK_EQF(frexp(8.0f, e), 0.5f);
        CHECK_EQF(e, 4.0f);
        float s = 0.0f, c = 0.0f;
        sincos(0.0f, s, c);
        CHECK_EQF(s, 0.0f);
        CHECK_EQF(c, 1.0f);
        CHECK_EQF(select(true, 1.0f, 2.0f), 1.0f);

        // one-component swizzles and matrix elements are scalars
        const float4 v(-0.5f, 2.0f, 3.0f, 0.25f);
        CHECK_EQF(max(v.x, 0.0f), 0.0f);
        CHECK_EQF(saturate(v.y), 1.0f);
        CHECK_EQF(lerp(v.x, v.y, 0.5f), 0.75f);
        const float2x2 m(1.0f, -2.0f, 3.0f, 4.0f);
        CHECK_EQF(abs(m._12), 2.0f);
        CHECK_EQF(min(m._m00, m._m11), 1.0f);
    }
    {
        using namespace shader_math::glsl;
        CHECK_EQF(mix(1.0f, 3.0f, 0.25f), 1.5f);
        CHECK_EQF(mix(1.0f, 3.0f, true), 3.0f);  // y where the selector is true
        CHECK_EQF(fract(-0.75f), 0.25f);
        CHECK_EQF(mod(-7.0f, 3.0f), 2.0f);
        CHECK_EQF(inversesqrt(4.0f), 0.5f);
        CHECK_EQF(roundEven(2.5f), 2.0f);
        CHECK_NEAR(atan(1.0f, 1.0f), 0.78539816f);
        CHECK_NEAR(atan(1.0f), 0.78539816f);
        CHECK_EQF(fma(2.0f, 3.0f, 1.0f), 7.0f);
        CHECK_NEAR(asinh(0.5f), std::asinh(0.5f));
        CHECK_EQF(clamp(vec2(2.0f, -1.0f).x, 0.0f, 1.0f), 1.0f);
    }
#if defined(SHADER_MATH_HAS_HALF)
    {
        using namespace shader_math::hlsl;
        const half x = max(half(-1.0f), half(0.5f));
        CHECK_EQF(static_cast<float>(x), 0.5f);
        CHECK_EQF(static_cast<float>(saturate(half(2.0f))), 1.0f);
        static_assert(std::same_as<decltype(saturate(half(2.0f))), half>);
        static_assert(!hlsl_max_ok<half, float>);  // no implicit half / float mixing
    }
#endif
    static_assert(std::same_as<decltype(shader_math::hlsl::max(1.0f, 2)), float>);
    static_assert(!hlsl_max_ok<double, double>);  // at least one float / half
    static_assert(!hlsl_max_ok<int, int>);
}

// ---- NaN rules (D3D): min / max ignore a NaN operand -------------------------
static void test_funcs_nan_rules() {
    const float qn = std::numeric_limits<float>::quiet_NaN();
    {
        using namespace shader_math::hlsl;
        const float4 n(qn, 1.0f, qn, 2.0f);
        const float4 v(1.0f, qn, 3.0f, qn);
        CHECK_EQF(min(n, v)[0], 1.0f);
        CHECK_EQF(min(n, v)[1], 1.0f);
        CHECK_EQF(min(n, v)[2], 3.0f);
        CHECK_EQF(min(n, v)[3], 2.0f);
        CHECK_EQF(max(n, v)[0], 1.0f);
        CHECK_EQF(max(n, v)[1], 1.0f);
        CHECK_EQF(saturate(n)[0], 0.0f);
        CHECK_EQF(clamp(n, -1.0f, 1.0f)[0], -1.0f);
        CHECK_EQF(min(n, 0.5f)[0], 0.5f);
        CHECK(isnan(min(float2(qn, qn), float2(qn, qn)))[0]);  // both NaN stays NaN
        CHECK_EQF(max(qn, 1.0f), 1.0f);                         // scalars too
        CHECK_EQF(saturate(qn), 0.0f);
    }
    {
        using namespace shader_math::glsl;
        CHECK_EQF(max(vec2(qn, 1.0f), vec2(2.0f, qn))[0], 2.0f);
        CHECK_EQF(max(vec2(qn, 1.0f), vec2(2.0f, qn))[1], 1.0f);
    }
#if defined(SHADER_MATH_HAS_HALF)
    {
        using namespace shader_math::hlsl;
        const half3 n(qn, 1.0f, 0.0f);
        const half3 v(half(0.5f), half(qn), half(0.0f));
        CHECK_EQF(static_cast<float>(min(n, v)[0]), 0.5f);
        CHECK_EQF(static_cast<float>(max(n, v)[1]), 1.0f);
        CHECK_EQF(static_cast<float>(saturate(n)[0]), 0.0f);
    }
#endif
}

// ---- gaps from the flat review -------------------------------------------------
static void test_funcs_review_gaps() {
    const float qn = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();
    {
        using namespace shader_math::hlsl;
        // isnan / isinf / isfinite on scalars and one-component swizzles
        CHECK(isnan(qn) && !isnan(1.0f));
        CHECK(isinf(-inf) && !isinf(1.0f));
        CHECK(isfinite(1.0f) && !isfinite(inf) && !isfinite(qn));
        const float4 v(qn, 1.0f, inf, 0.0f);
        CHECK(isnan(v.x) && isinf(v.z) && isfinite(v.w));

        // all / any on numbers: components against zero, NaN is nonzero
        CHECK(all(float3(1.0f, -2.0f, 3.0f)));
        CHECK(!all(float3(1.0f, 0.0f, 3.0f)));
        CHECK(!any(float3(0.0f, -0.0f, 0.0f)));
        CHECK(any(float3(0.0f, qn, 0.0f)));
        CHECK(all(v.yx));
        CHECK(all(2.0f) && !all(0.0f) && any(-1.0f));
        CHECK(!all(float2x2(1.0f, 1.0f, 1.0f, 0.0f)));
        CHECK(any(float2x2(0.0f, 0.0f, 0.0f, 0.5f)));
        CHECK(!any(float2x2(0.0f, 0.0f, 0.0f, 0.0f)));

        // select with a scalar condition and vector operands
        const float3 a(1.0f, 2.0f, 3.0f);
        const float3 b(4.0f, 5.0f, 6.0f);
        CHECK_EQF(select(true, a, b)[2], 3.0f);
        CHECK_EQF(select(false, a, b)[0], 4.0f);
        CHECK_EQF(select(false, a, 9.0f)[1], 9.0f);

        // bool <-> float vectors, explicit only
        const float3 f(bool3(true, false, true));
        CHECK_EQF(f[0], 1.0f);
        CHECK_EQF(f[1], 0.0f);
        const bool3 t(float3(0.0f, 2.0f, qn));
        CHECK(!t[0] && t[1] && t[2]);
        CHECK(bool2(a.zx)[1]);

        // vector<T, N>
        static_assert(std::same_as<vector<float, 3>, float3>);
        static_assert(std::same_as<vector<>, float4>);
    }
    {
        using namespace shader_math::glsl;
        CHECK(isnan(qn) && isinf(inf));
        const vec3 r = vec3(lessThan(vec3(1.0f, 2.0f, 3.0f), vec3(2.0f)));
        CHECK_EQF(r[0], 1.0f);
        CHECK_EQF(r[2], 0.0f);
        CHECK(all(bvec3(vec3(1.0f, -1.0f, 0.5f))));
    }
#if defined(SHADER_MATH_HAS_HALF)
    {
        using namespace shader_math::hlsl;
        CHECK(isnan(half(qn)));
        CHECK(all(half3(half(1.0f), half(2.0f), half(3.0f))));
        CHECK_EQF(static_cast<float>(half3(bool3(false, true, false))[1]), 1.0f);
        static_assert(std::same_as<vector<half, 2>, half2>);
    }
#endif
}

static void test_funcs_matrix_hlsl() {
    using namespace shader_math::hlsl;

    // non-square on purpose: 2 rows x 3 columns
    const float2x3 m(-1.5f, 0.25f, 2.75f,
                     -0.5f, 1.0f, -3.25f);
    const float2x3 a = abs(m);
    CHECK_EQF(a[0][0], 1.5f);
    CHECK_EQF(a[1][2], 3.25f);
    CHECK_EQF(floor(m)[0][2], 2.0f);
    CHECK_EQF(floor(m)[1][0], -1.0f);
    CHECK_EQF(frac(m)[0][0], 0.5f);
    CHECK_EQF(saturate(m)[0][2], 1.0f);
    CHECK_EQF(saturate(m)[1][0], 0.0f);
    CHECK_EQF(sign(m)[1][2], -1.0f);

    CHECK_EQF(min(m, 0.0f)[0][1], 0.0f);         // matrix, scalar
    CHECK_EQF(max(m, a)[1][0], 0.5f);            // matrix, matrix
    CHECK_EQF(clamp(m, -1.0f, 1.0f)[0][2], 1.0f);
    CHECK_EQF(clamp(m, -a, a)[1][1], 1.0f);
    CHECK_EQF(step(0.0f, m)[0][1], 1.0f);        // edge first
    CHECK_EQF(step(m, a)[1][2], 1.0f);
    CHECK_NEAR(lerp(m, a, 0.5f)[0][0], 0.0f);
    CHECK_NEAR(lerp(m, a, float2x3(1.0f))[1][2], 3.25f);
    CHECK_NEAR(smoothstep(-1.0f, 1.0f, m)[1][0], 0.15625f);  // t = 0.25: t*t*(3 - 2t)
    CHECK_NEAR(mad(m, a, float2x3(1.0f))[0][1], 1.0625f);
    CHECK_NEAR(pow(a, 2.0f)[0][0], 2.25f);
    CHECK_NEAR(fmod(m, 2.0f)[1][2], -1.25f);
    CHECK_NEAR(atan2(m, a)[1][0], std::atan2(-0.5f, 0.5f));
    CHECK_NEAR(sin(m)[0][2], std::sin(2.75f));
    CHECK_NEAR(sqrt(a)[1][1], 1.0f);

    float2x3 special = m;
    special[0][1] = std::numeric_limits<float>::quiet_NaN();
    special[1][2] = std::numeric_limits<float>::infinity();
    const bool2x3 n = isnan(special);
    const bool2x3 i = isinf(special);
    const bool2x3 f = isfinite(special);
    CHECK(n[0][1] && !n[0][0] && !n[1][2]);
    CHECK(i[1][2] && !i[0][1]);
    CHECK(!f[0][1] && !f[1][2] && f[0][0] && f[1][1]);

#if defined(SHADER_MATH_HAS_HALF)
    const half2x2 hm(half(-1.5f), half(0.25f), half(2.0f), half(-0.75f));
    CHECK_EQF(static_cast<float>(abs(hm)[0][0]), 1.5f);
    CHECK_EQF(static_cast<float>(saturate(hm)[1][0]), 1.0f);
    CHECK_EQF(static_cast<float>(min(hm, half(0.0f))[0][1]), 0.0f);
#endif
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
    CHECK_EQF(static_cast<float>(round(v)[2]), 2.0f);  // halves to even
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
    test_funcs_matrix_hlsl();
    test_funcs_compat_gaps();
    test_funcs_scalar();
    test_funcs_nan_rules();
    test_funcs_review_gaps();
    test_funcs_half();
}
