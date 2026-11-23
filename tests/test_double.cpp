// Double-precision vectors and matrices in both language namespaces:
// storage/alignment, full 53-bit mantissa precision, fma single rounding,
// double -> int saturation, HLSL native vs float-lowered intrinsics,
// GLSL widening / packing / 64-bit bitcasts / transcendental rejection,
// mixed-precision operations, and explicit instantiations.
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

// Explicit instantiations catch member-signature or eager-instantiation bugs
// across toolchains (including 1-dimension HLSL matrices).
template struct shader_math::detail::fvec<1, double, shader_math::hlsl::lang_tag>;
template struct shader_math::detail::fvec<2, double, shader_math::hlsl::lang_tag>;
template struct shader_math::detail::fvec<3, double, shader_math::hlsl::lang_tag>;
template struct shader_math::detail::fvec<4, double, shader_math::hlsl::lang_tag>;
template struct shader_math::detail::fvec<2, double, shader_math::glsl::lang_tag>;
template struct shader_math::detail::fvec<3, double, shader_math::glsl::lang_tag>;
template struct shader_math::detail::fvec<4, double, shader_math::glsl::lang_tag>;
template struct shader_math::hlsl::matrix<double, 1, 1>;
template struct shader_math::hlsl::matrix<double, 1, 3>;
template struct shader_math::hlsl::matrix<double, 3, 1>;
template struct shader_math::hlsl::matrix<double, 2, 2>;
template struct shader_math::hlsl::matrix<double, 3, 3>;
template struct shader_math::hlsl::matrix<double, 4, 4>;
template struct shader_math::glsl::matrix<2, 2, double>;
template struct shader_math::glsl::matrix<2, 3, double>;
template struct shader_math::glsl::matrix<3, 3, double>;
template struct shader_math::glsl::matrix<4, 4, double>;

namespace {

inline bool near_d(double a, double b, double eps = 1e-12) {
    const double d = std::fabs(a - b);
    const double m = std::fmax(1.0, std::fmax(std::fabs(a), std::fabs(b)));
    return d <= eps * m;
}

#define CHECK_EQD(a, b)                                                             \
    do {                                                                            \
        ++g_checks;                                                                 \
        const double va_ = (a);                                                     \
        const double vb_ = (b);                                                     \
        if (!(va_ == vb_)) {                                                        \
            ++g_failures;                                                           \
            std::printf("FAIL %s:%d: %s == %s (%.17g vs %.17g)\n", __FILE__,        \
                        __LINE__, #a, #b, va_, vb_);                                \
        }                                                                           \
    } while (0)

#define CHECK_NEARD(a, b)                                                           \
    do {                                                                            \
        ++g_checks;                                                                 \
        const double va_ = (a);                                                     \
        const double vb_ = (b);                                                     \
        if (!near_d(va_, vb_)) {                                                    \
            ++g_failures;                                                           \
            std::printf("FAIL %s:%d: %s ~=d %s (%.17g vs %.17g)\n", __FILE__,       \
                        __LINE__, #a, #b, va_, vb_);                                \
        }                                                                           \
    } while (0)

template <class A, class B>
concept CanMod = requires(const A& a, const B& b) { a % b; };
template <class A, class B>
concept CanModAssign = requires(A& a, const B& b) { a %= b; };
template <class A, class B>
concept CanAddAssign = requires(A& a, const B& b) { a += b; };
template <class A, class B, class C>
concept CanHlslFma = requires(const A& a, const B& b, const C& c) {
    shader_math::hlsl::fma(a, b, c);
};
template <class A>
concept CanHlslAsUint1 = requires(const A& a) { shader_math::hlsl::asuint(a); };
template <class A>
concept CanGlslSin = requires(const A& a) { shader_math::glsl::sin(a); };
template <class A>
concept CanGlslExp = requires(const A& a) { shader_math::glsl::exp(a); };
template <class A, class B>
concept CanGlslPow = requires(const A& a, const B& b) { shader_math::glsl::pow(a, b); };
template <class A>
concept CanGlslRadians = requires(const A& a) { shader_math::glsl::radians(a); };
template <class A>
concept CanGlslAsinh = requires(const A& a) { shader_math::glsl::asinh(a); };

void test_layout_and_f2int() {
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;
    namespace d = shader_math::detail;

    static_assert(sizeof(h::double1) == 32);
    static_assert(sizeof(h::double2) == 32);
    static_assert(sizeof(h::double3) == 32);
    static_assert(sizeof(h::double4) == 32);
    static_assert(sizeof(h::double3x3) == 96);
    static_assert(sizeof(g::dvec2) == 32);
    static_assert(sizeof(g::dvec3) == 32);
    static_assert(sizeof(g::dvec4) == 32);
    static_assert(sizeof(g::dmat3) == 96);
    static_assert(std::is_same_v<g::float64_t, double>);
    static_assert(std::is_same_v<g::f64vec3, g::dvec3>);
    static_assert(std::is_same_v<g::f64mat3x4, g::dmat3x4>);

    // double -> int64/uint64 preserves >24-bit values without truncating through float
    const double big53 = 9007199254740991.0;  // 2^53 - 1
    CHECK(d::int_lane::f2int<std::int64_t>(big53) == 9007199254740991LL);
    CHECK(d::int_lane::f2int<std::int64_t>(-big53) == -9007199254740991LL);
    CHECK(d::int_lane::f2int<std::uint64_t>(big53) == 9007199254740991ULL);

    // Saturation and NaN -> 0
    const double qn = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    CHECK(d::int_lane::f2int<std::int64_t>(qn) == 0);
    CHECK(d::int_lane::f2int<std::uint64_t>(qn) == 0u);
    CHECK(d::int_lane::f2int<std::int64_t>(9223372036854775808.0) == INT64_MAX);
    CHECK(d::int_lane::f2int<std::int64_t>(inf) == INT64_MAX);
    CHECK(d::int_lane::f2int<std::int64_t>(-9223372036854775808.0) == INT64_MIN);
    CHECK(d::int_lane::f2int<std::int64_t>(-inf) == INT64_MIN);
    CHECK(d::int_lane::f2int<std::uint64_t>(18446744073709551616.0) == UINT64_MAX);
    CHECK(d::int_lane::f2int<std::uint64_t>(inf) == UINT64_MAX);
    CHECK(d::int_lane::f2int<std::uint64_t>(-1.0) == 0u);
    CHECK(d::int_lane::f2int<std::int32_t>(1e20) == INT32_MAX);
    CHECK(d::int_lane::f2int<std::int32_t>(-1e20) == INT32_MIN);
    CHECK(d::int_lane::f2int<std::uint32_t>(1e20) == UINT32_MAX);
    CHECK(d::int_lane::f2int<std::uint32_t>(-5.0) == 0u);
    CHECK(d::int_lane::f2int<std::int16_t>(100000.0) == INT16_MAX);
    CHECK(d::int_lane::f2int<std::uint16_t>(100000.0) == UINT16_MAX);

    // Vector conversion preserves 53-bit precision
    const h::double2 dv(big53, -big53);
    const h::int64_t2 iv(dv);
    CHECK(iv[0] == 9007199254740991LL);
    CHECK(iv[1] == -9007199254740991LL);
    const h::double2 back = iv;
    CHECK_EQD(back[0], big53);
    CHECK_EQD(back[1], -big53);
}

void test_hlsl_double() {
    using namespace shader_math::hlsl;

    // Implicit / explicit conversions
    static_assert(std::is_convertible_v<float3, double3>);
    static_assert(std::is_convertible_v<double3, float3>);
    static_assert(std::is_convertible_v<int3, double3>);
    static_assert(std::is_convertible_v<double3, int3>);
    static_assert(std::is_convertible_v<int64_t3, double3>);
    static_assert(std::is_convertible_v<double3, int64_t3>);
    static_assert(std::is_convertible_v<float3x3, double3x3>);
    static_assert(std::is_convertible_v<double3x3, float3x3>);
    static_assert(!std::is_convertible_v<bool3, double3>);
    static_assert(!std::is_convertible_v<double3, bool3>);
#if defined(SHADER_MATH_HAS_HALF)
    static_assert(!std::is_convertible_v<half3, double3>);
    static_assert(!std::is_convertible_v<double3, half3>);
    static_assert(!std::is_convertible_v<half3x3, double3x3>);
#endif

    // % and %= must be rejected for double vectors, matrices and double1
    static_assert(!CanMod<double3, double3>);
    static_assert(!CanMod<double3, double>);
    static_assert(!CanMod<double3, float3>);
    static_assert(!CanModAssign<double3, double3>);
    static_assert(!CanMod<double3x3, double3x3>);
    static_assert(!CanMod<double3x3, double>);
    static_assert(!CanModAssign<double3x3, double3x3>);
    static_assert(!CanMod<double1, double>);

    // fma is double-only in HLSL
    static_assert(CanHlslFma<double3, double3, double3>);
    static_assert(CanHlslFma<double3, double, double>);
    static_assert(CanHlslFma<double, double, double>);
    static_assert(CanHlslFma<double3x3, double3x3, double3x3>);
    static_assert(!CanHlslFma<float3, float3, float3>);
    static_assert(!CanHlslFma<double3, float3, double3>);
    static_assert(!CanHlslFma<float, float, float>);
    static_assert(!CanHlslAsUint1<double3>);

    // float3 + 1.0 stays float3 (critical rule for shader literals)
    static_assert(std::same_as<decltype(float3(1.0f) + 1.0), float3>);
    static_assert(std::same_as<decltype(double3(1.0) + 1.0f), double3>);
    static_assert(std::same_as<decltype(double3(1.0) + float3(1.0f)), double3>);
    static_assert(std::same_as<decltype(double3(1.0) + int3(1)), double3>);
    static_assert(std::same_as<decltype(double3(1.0) + 2), double3>);

    // Full 53-bit mantissa precision in native double ops
    const double eps = 1e-12;
    const double3 a(1.0 + eps, 2.0 - eps, 1e200);
    const double3 b(1.0, 2.0, 1e200);
    const double3 diff = a - b;
    CHECK_NEARD(diff[0], eps);
    CHECK_NEARD(diff[1], -eps);
    CHECK_EQD(diff[2], 0.0);
    CHECK(all(isfinite(a)));
    CHECK(!any(isinf(a)));
    CHECK(!any(isnan(a)));

    const double qn = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();
    const double3 special(qn, inf, -1e200);
    CHECK(isnan(special)[0] && !isnan(special)[1] && !isnan(special)[2]);
    CHECK(!isinf(special)[0] && isinf(special)[1] && !isinf(special)[2]);
    CHECK(!isfinite(special)[0] && !isfinite(special)[1] && isfinite(special)[2]);

    // min / max IEEE minNum / maxNum with NaN
    CHECK_EQD(min(double2(qn, 2.0), double2(3.0, qn))[0], 3.0);
    CHECK_EQD(min(double2(qn, 2.0), double2(3.0, qn))[1], 2.0);
    CHECK_EQD(max(double2(qn, 2.0), double2(3.0, qn))[0], 3.0);
    CHECK_EQD(max(double2(qn, 2.0), double2(3.0, qn))[1], 2.0);

    // Exact single-rounding fma: (1 + 2^-27)*(1 - 2^-27) - 1 == -2^-54
    const double p = 1.0 + std::ldexp(1.0, -27);
    const double q = 1.0 - std::ldexp(1.0, -27);
    const double expected_fma = -std::ldexp(1.0, -54);
    CHECK_EQD(hlsl::fma(p, q, -1.0), expected_fma);
    const double3 fv = fma(double3(p, p, 2.0), double3(q, q, 3.0), double3(-1.0, -1.0, 4.0));
    CHECK_EQD(fv[0], expected_fma);
    CHECK_EQD(fv[2], 10.0);
    const double3 fm_mix = fma(double3(p, 2.0, 3.0), q, -1.0);
    CHECK_EQD(fm_mix[0], expected_fma);

    // Native double intrinsics: abs, sign, clamp, saturate, rcp, mad, dot, select
    static_assert(std::same_as<decltype(abs(a)), double3>);
    static_assert(std::same_as<decltype(rcp(a)), double3>);
    static_assert(std::same_as<decltype(saturate(a)), double3>);
    static_assert(std::same_as<decltype(mad(a, b, a)), double3>);
    static_assert(std::same_as<decltype(dot(a, b)), double>);
    static_assert(std::same_as<decltype(max(a, float3(1.0f))), double3>);
    static_assert(std::same_as<decltype(select(bool3(true), a, float3(1.0f))), double3>);

    CHECK_EQD(abs(double3(-1.0 - eps, 0.0, 2.0))[0], 1.0 + eps);
    CHECK_EQD(rcp(double2(4.0, 3.0))[0], 0.25);
    CHECK_NEARD(rcp(double2(4.0, 3.0))[1], 1.0 / 3.0);
    CHECK_EQD(saturate(double3(-1.0, 0.5 + eps, 2.0))[1], 0.5 + eps);
    CHECK_NEARD(dot(double2(1.0 + eps, 1.0), double2(1.0, -1.0)), (1.0 + eps) - 1.0);
    CHECK_EQD(dot(double2(1.0 + std::ldexp(1.0, -40), 1.0), double2(1.0, -1.0)),
              std::ldexp(1.0, -40));
    CHECK_EQD(select(bool2(true, false), double2(1.0 + eps, 2.0), float2(3.0f, 4.0f))[0], 1.0 + eps);

    // asdouble / 3-arg asuint bit reinterpretation
    uint lo = 0, hi = 0;
    asuint(-2.5, lo, hi);
    CHECK(lo == 0x00000000u && hi == 0xC0040000u);
    CHECK_EQD(asdouble(lo, hi), -2.5);
    int ilo = 0, ihi = 0;
    asuint(1.0, ilo, ihi);
    CHECK(ilo == 0 && ihi == 0x3FF00000);
    CHECK_EQD(asdouble(ilo, ihi), 1.0);

    uint3 vlo, vhi;
    const double3 bits_in(1.0, -2.5, 3.141592653589793);
    asuint(bits_in, vlo, vhi);
    const double3 bits_out = asdouble(vlo, vhi);
    CHECK(all(bits_out == bits_in));
    int3 ivlo, ivhi;
    asuint(bits_in, ivlo, ivhi);
    CHECK(all(asdouble(ivlo, ivhi) == bits_in));

    // HLSL float-lowered functions return float / float3 / float3x3
    const double3 d(1.5, -2.25, 4.0);
    static_assert(std::same_as<decltype(floor(d)), float3>);
    static_assert(std::same_as<decltype(ceil(d)), float3>);
    static_assert(std::same_as<decltype(trunc(d)), float3>);
    static_assert(std::same_as<decltype(round(d)), float3>);
    static_assert(std::same_as<decltype(frac(d)), float3>);
    static_assert(std::same_as<decltype(sqrt(d)), float3>);
    static_assert(std::same_as<decltype(rsqrt(d)), float3>);
    static_assert(std::same_as<decltype(sin(d)), float3>);
    static_assert(std::same_as<decltype(lerp(d, d, 0.5)), float3>);
    static_assert(std::same_as<decltype(step(0.5, d)), float3>);
    static_assert(std::same_as<decltype(smoothstep(0.0, 1.0, d)), float3>);
    static_assert(std::same_as<decltype(fmod(d, d)), float3>);
    static_assert(std::same_as<decltype(cross(d, d)), float3>);
    static_assert(std::same_as<decltype(length(d)), float>);
    static_assert(std::same_as<decltype(distance(d, d)), float>);
    static_assert(std::same_as<decltype(normalize(d)), float3>);
    static_assert(std::same_as<decltype(reflect(d, d)), float3>);
    static_assert(std::same_as<decltype(refract(d, d, 0.5)), float3>);
    static_assert(std::same_as<decltype(faceforward(d, d, d)), float3>);
    static_assert(std::same_as<decltype(ldexp(d, double3(1.0))), float3>);
    CHECK_EQF(floor(d)[0], 1.0f);
    CHECK_EQF(floor(d)[1], -3.0f);
    CHECK_EQF(sqrt(d)[2], 2.0f);
    CHECK_EQF(hlsl::round(2.5), 2.0f);

    // modf / frexp / sincos accept both float3 and double3 out params
    float3 fip, fe, fsn, fcs;
    double3 dip, de, dsn, dcs;
    const float3 mfr1 = modf(d, fip);
    const float3 mfr2 = modf(d, dip);
    CHECK_EQF(mfr1[0], 0.5f);
    CHECK_EQF(fip[0], 1.0f);
    CHECK_EQF(mfr2[0], 0.5f);
    CHECK_EQD(dip[0], 1.0);
    const float3 ffr1 = frexp(d, fe);
    const float3 ffr2 = frexp(d, de);
    CHECK_EQF(ffr1[2], 0.5f);
    CHECK_EQF(fe[2], 3.0f);
    CHECK_EQF(ffr2[2], 0.5f);
    CHECK_EQD(de[2], 3.0);
    sincos(double3(0.0, 0.0, 0.0), fsn, fcs);
    sincos(double3(0.0, 0.0, 0.0), dsn, dcs);
    CHECK_EQF(fsn[0], 0.0f);
    CHECK_EQF(fcs[0], 1.0f);
    CHECK_EQD(dsn[0], 0.0);
    CHECK_EQD(dcs[0], 1.0);

    // Double matrices (including 1-dim) & mixed matrix ops
    const double2x2 dm2(1.0 + eps, 2.0, 3.0, 4.0);
    const float2x2 fm2(1.0f, 0.0f, 0.0f, 1.0f);
    static_assert(std::same_as<decltype(dm2 + fm2), double2x2>);
    static_assert(std::same_as<decltype(mul(dm2, fm2)), double2x2>);
    static_assert(std::same_as<decltype(mul(double2(1.0, 2.0), fm2)), double2>);
    static_assert(std::same_as<decltype(mul(float2(1.0f, 2.0f), dm2)), double2>);
    static_assert(std::same_as<decltype(determinant(dm2)), float>);
    static_assert(std::same_as<decltype(shader_math::ext::inverse(dm2)), double2x2>);
    static_assert(std::same_as<decltype(sqrt(dm2)), float2x2>);
    static_assert(std::same_as<decltype(abs(dm2)), double2x2>);
    static_assert(std::same_as<decltype(fma(dm2, dm2, dm2)), double2x2>);

    const double2x2 inv2 = shader_math::ext::inverse(dm2);
    const double2x2 prod2 = mul(dm2, inv2);
    CHECK_NEARD(prod2[0][0], 1.0);
    CHECK_NEARD(prod2[0][1], 0.0);
    CHECK_NEARD(prod2[1][0], 0.0);
    CHECK_NEARD(prod2[1][1], 1.0);

    // 1-dim double matrices
    const double1x3 r13(1.0 + eps, 2.0, 3.0);
    const double3x1 c31 = transpose(r13);
    CHECK_EQD(c31._m00, 1.0 + eps);
    CHECK_EQD(c31._m20, 3.0);
    const double1x1 s11 = mul(r13, c31);
    CHECK_NEARD(double(s11), (1.0 + eps) * (1.0 + eps) + 13.0);

    // Pad-lane NaN poisoning on double3
    const double3 poisoned(shader_math::detail::dops::set(1.0, 2.0, 2.0, qn));
    CHECK(all(poisoned == double3(1.0, 2.0, 2.0)));
    CHECK_EQD(dot(poisoned, double3(1.0, 1.0, 1.0)), 5.0);
}

void test_glsl_double() {
    using namespace shader_math::glsl;

    // Implicit widening vs explicit narrowing
    static_assert(std::is_convertible_v<vec3, dvec3>);
    static_assert(!std::is_convertible_v<dvec3, vec3>);
    static_assert(std::is_convertible_v<ivec3, dvec3>);
    static_assert(std::is_convertible_v<uvec3, dvec3>);
    static_assert(std::is_convertible_v<i16vec3, dvec3>);
    static_assert(std::is_convertible_v<u16vec3, dvec3>);
    static_assert(std::is_convertible_v<i64vec3, dvec3>);
    static_assert(std::is_convertible_v<u64vec3, dvec3>);
    static_assert(std::is_convertible_v<mat3, dmat3>);
    static_assert(!std::is_convertible_v<dmat3, mat3>);
    static_assert(CanAddAssign<dvec3, vec3>);
    static_assert(!CanAddAssign<vec3, dvec3>);
    static_assert(CanAddAssign<dmat3, mat3>);
    static_assert(!CanAddAssign<mat3, dmat3>);
#if defined(SHADER_MATH_HAS_HALF)
    static_assert(!std::is_convertible_v<f16vec3, dvec3>);
    static_assert(!std::is_convertible_v<f16mat3, dmat3>);
#endif

    // Transcendentals must be rejected for double in GLSL
    static_assert(!CanGlslSin<dvec3>);
    static_assert(!CanGlslExp<dvec3>);
    static_assert(!CanGlslPow<dvec3, dvec3>);
    static_assert(!CanGlslRadians<dvec3>);
    static_assert(!CanGlslAsinh<dvec3>);
    static_assert(!CanGlslSin<double>);
    static_assert(!CanGlslExp<double>);
    static_assert(!CanGlslPow<double, double>);

    // vec3 + 1.0 stays vec3; dvec3 + vec3 promotes to dvec3
    static_assert(std::same_as<decltype(vec3(1.0f) + 1.0), vec3>);
    static_assert(std::same_as<decltype(dvec3(1.0) + vec3(1.0f)), dvec3>);
    static_assert(std::same_as<decltype(dvec3(1.0) + i64vec3(1)), dvec3>);
    static_assert(std::same_as<decltype(dmat3(1.0) + mat3(1.0f)), dmat3>);
    static_assert(std::same_as<decltype(dmat3(1.0) * mat3(1.0f)), dmat3>);
    static_assert(std::same_as<decltype(mat3(1.0f) * dmat3(1.0)), dmat3>);
    static_assert(std::same_as<decltype(dmat3(1.0) * vec3(1.0f)), dvec3>);
    static_assert(std::same_as<decltype(vec3(1.0f) * dmat3(1.0)), dvec3>);
    static_assert(std::same_as<decltype(mat3(1.0f) * dvec3(1.0)), dvec3>);
    static_assert(std::same_as<decltype(dvec3(1.0) * mat3(1.0f)), dvec3>);
    static_assert(std::same_as<decltype(matrixCompMult(dmat3(1.0), mat3(1.0f))), dmat3>);
    static_assert(std::same_as<decltype(outerProduct(dvec3(1.0), vec3(1.0f))), dmat3>);

    // Full double precision in GLSL native functions
    const double eps = 1e-12;
    const dvec3 da(1.0 + eps, 2.0, 2.0);
    static_assert(std::same_as<decltype(length(da)), double>);
    static_assert(std::same_as<decltype(normalize(da)), dvec3>);
    static_assert(std::same_as<decltype(sqrt(da)), dvec3>);
    static_assert(std::same_as<decltype(inversesqrt(da)), dvec3>);
    static_assert(std::same_as<decltype(floor(da)), dvec3>);
    static_assert(std::same_as<decltype(round(da)), dvec3>);
    static_assert(std::same_as<decltype(roundEven(da)), dvec3>);
    static_assert(std::same_as<decltype(fract(da)), dvec3>);
    static_assert(std::same_as<decltype(mod(da, 1.0)), dvec3>);
    static_assert(std::same_as<decltype(mix(da, vec3(1.0f), 0.5)), dvec3>);
    static_assert(std::same_as<decltype(mix(da, vec3(1.0f), bvec3(true))), dvec3>);
    static_assert(std::same_as<decltype(step(vec3(1.0f), da)), dvec3>);
    static_assert(std::same_as<decltype(smoothstep(vec3(0.0f), vec3(2.0f), da)), dvec3>);
    static_assert(std::same_as<decltype(fma(da, vec3(2.0f), da)), dvec3>);

    CHECK_NEARD(length(da), std::sqrt((1.0 + eps) * (1.0 + eps) + 8.0));
    CHECK_NEARD(sqrt(dvec2(2.0, 3.0))[0], std::sqrt(2.0));
    CHECK_NEARD(inversesqrt(dvec2(2.0, 4.0))[0], 1.0 / std::sqrt(2.0));
    CHECK_EQD(glsl::round(2.5), 2.0);
    CHECK_EQD(glsl::round(3.5), 4.0);

    // fma exact single rounding
    const double p = 1.0 + std::ldexp(1.0, -27);
    const double q = 1.0 - std::ldexp(1.0, -27);
    const double expected_fma = -std::ldexp(1.0, -54);
    CHECK_EQD(fma(dvec2(p, 2.0), dvec2(q, 3.0), dvec2(-1.0, 4.0))[0], expected_fma);

    // refract in full double precision
    const dvec3 inc = normalize(dvec3(0.5, -0.8, 0.0));
    const dvec3 nrm(0.0, 1.0, 0.0);
    const dvec3 rf = refract(inc, nrm, 0.75);
    CHECK_NEARD(length(rf), 1.0);

    // modf / frexp / ldexp in full double precision
    dvec2 dip;
    const dvec2 dfr = modf(dvec2(1234567890.0 + eps, -3.25), dip);
    CHECK_EQD(dip[0], 1234567890.0);
    CHECK_NEARD(dfr[0], eps);
    CHECK_EQD(dip[1], -3.0);
    CHECK_EQD(dfr[1], -0.25);

    ivec2 ex;
    const dvec2 val(1.0 + eps, -1e200);
    const dvec2 mant = frexp(val, ex);
    const dvec2 rebuilt = ldexp(mant, ex);
    CHECK_EQD(rebuilt[0], val[0]);
    CHECK_EQD(rebuilt[1], val[1]);
    int sex = 0;
    const double smant = frexp(1e200, sex);
    CHECK_EQD(ldexp(smant, sex), 1e200);

    // packDouble2x32 / unpackDouble2x32 and 64-bit bitcasts
    const uvec2 parts = unpackDouble2x32(-2.5);
    CHECK(parts[0] == 0x00000000u && parts[1] == 0xC0040000u);
    CHECK_EQD(packDouble2x32(parts), -2.5);
    CHECK_EQD(packDouble2x32(ivec2(0, 0x3FF00000)), 1.0);
    CHECK(unpackDouble2x32(1.0f) == uvec2(0u, 0x3FF00000u));

    const dvec2 dv(1.0, -2.5);
    const i64vec2 di = doubleBitsToInt64(dv);
    const u64vec2 du = doubleBitsToUint64(dv);
    CHECK(di[0] == 0x3FF0000000000000LL);
    CHECK(du[1] == 0xC004000000000000ULL);
    CHECK(int64BitsToDouble(di) == dv);
    CHECK(uint64BitsToDouble(du) == dv);
    CHECK(doubleBitsToInt64(1.0) == 0x3FF0000000000000LL);
    CHECK(doubleBitsToUint64(1.0f) == 0x3FF0000000000000ULL);
    CHECK_EQD(int64BitsToDouble(int64_t(0x3FF0000000000000LL)), 1.0);
    CHECK_EQD(uint64BitsToDouble(uint64_t(0x3FF0000000000000ULL)), 1.0);

    // Mixed comparisons
    CHECK(dvec2(1.0, 2.0) == vec2(1.0f, 2.0f));
    CHECK(dvec2(1.0 + eps, 2.0) != vec2(1.0f, 2.0f));
    CHECK(all(equal(dvec2(1.0, 2.0), vec2(1.0f, 2.0f))));
    CHECK(lessThan(vec2(1.0f, 2.0f), dvec2(1.0 + eps, 2.0))[0]);
    CHECK(dmat2(1.0) == mat2(1.0f));

    // dmat3 determinant / inverse in full double precision
    const dmat3 dm(2.0 + eps, 1.0, 0.0,
                   0.0, 3.0, 1.0,
                   1.0, 0.0, 4.0);
    static_assert(std::same_as<decltype(determinant(dm)), double>);
    static_assert(std::same_as<decltype(inverse(dm)), dmat3>);
    CHECK_NEARD(determinant(dm), (2.0 + eps) * 12.0 + 1.0);
    const dmat3 id = dm * inverse(dm);
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            CHECK_NEARD(id[c][r], c == r ? 1.0 : 0.0);

    // Cross-language explicit conversion (transposed)
    const shader_math::hlsl::double3x2 hm(1.0, 2.0,
                                          3.0, 4.0,
                                          5.0, 6.0);
    const dmat2x3 gm(hm);
    CHECK_EQD(gm[0][0], 1.0);
    CHECK_EQD(gm[0][1], 3.0);
    CHECK_EQD(gm[0][2], 5.0);
    CHECK_EQD(gm[1][0], 2.0);
    CHECK_EQD(gm[1][2], 6.0);
    const shader_math::hlsl::double3x2 hm2(gm);
    CHECK(shader_math::hlsl::all(hm2 == hm));
}

void test_scalar_double_priority() {
    using namespace shader_math::glsl;
    const float s = 0.25f;
    const double ds = 0.25;
    static_assert(std::same_as<decltype(step(0.5, s)), float>);
    static_assert(std::same_as<decltype(clamp(s, 0.0, 1.0)), float>);
    static_assert(std::same_as<decltype(max(1.0, s)), float>);
    static_assert(std::same_as<decltype(max(1.0, 2.0)), double>);
    static_assert(std::same_as<decltype(min(ds, 2)), double>);
    static_assert(std::same_as<decltype(step(0.5, ds)), double>);
    static_assert(std::same_as<decltype(clamp(ds, 0.0, 1.0)), double>);
    CHECK_EQF(step(0.5, s), 0.0f);
    CHECK_EQD(max(1.0, 2.0), 2.0);
    CHECK_EQD(min(ds, 2), 0.25);
}

}  // namespace

void test_double() {
    test_layout_and_f2int();
    test_hlsl_double();
    test_glsl_double();
    test_scalar_double_priority();
}
