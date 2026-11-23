// HLSL language surface.
//   operator* on matrices is COMPONENT-WISE (Hadamard); mul() is the linear
//   algebra product. Comparisons return bool vectors / bool matrices which are
//   not implicitly convertible to bool: reduce with all()/any(), exactly like
//   HLSL. Import with `using namespace hlsl;` (one language per scope).
#pragma once

#include <concepts>

#include "matrix.hpp"

namespace shader_math {
namespace hlsl {

// ---- type aliases -----------------------------------------------------------
// vector<T, N> (HLSL's template spelling; vector<float, 3> is float3)
template <class T = float, int N = 4>
using vector = detail::fvec<N, T, lang_tag>;

using float2 = vector<float, 2>;
using float3 = vector<float, 3>;
using float4 = vector<float, 4>;
using bool2 = detail::bvec<2, lang_tag>;
using bool3 = detail::bvec<3, lang_tag>;
using bool4 = detail::bvec<4, lang_tag>;

// 32-bit integer vectors: arithmetic wraps; x / 0, shifts by 32 or more and
// out-of-range float -> int conversions have defined results
// (docs/design/int_vectors.md)
static_assert(sizeof(int) == 4, "shader-math: int must be 32 bits");
using uint = unsigned int;
using dword = unsigned int;
using int2 = vector<int, 2>;
using int3 = vector<int, 3>;
using int4 = vector<int, 4>;
using uint2 = vector<uint, 2>;
using uint3 = vector<uint, 3>;
using uint4 = vector<uint, 4>;
// 16- and 64-bit integers (DXC: -enable-16bit-types for the 16-bit ones);
// 16-bit lanes compute in 32 bits and wrap, 64-bit lanes one at a time
using int16_t = std::int16_t;
using uint16_t = std::uint16_t;
using int64_t = std::int64_t;
using uint64_t = std::uint64_t;
using int16_t2 = vector<int16_t, 2>;
using int16_t3 = vector<int16_t, 3>;
using int16_t4 = vector<int16_t, 4>;
using uint16_t2 = vector<uint16_t, 2>;
using uint16_t3 = vector<uint16_t, 3>;
using uint16_t4 = vector<uint16_t, 4>;
using int64_t2 = vector<int64_t, 2>;
using int64_t3 = vector<int64_t, 3>;
using int64_t4 = vector<int64_t, 4>;
using uint64_t2 = vector<uint64_t, 2>;
using uint64_t3 = vector<uint64_t, 3>;
using uint64_t4 = vector<uint64_t, 4>;

// Half vectors (HLSL half). Language-level packing: sizeof(half3) == 8
// (4 x binary16), just as sizeof(float3) == 16 — language compatibility, not
// a GPU ABI. Absent entirely on compilers without a half scalar type.
#if defined(SHADER_MATH_HAS_HALF)
using half = detail::half_t;
using half2 = vector<half, 2>;
using half3 = vector<half, 3>;
using half4 = vector<half, 4>;
#endif

using float2x2 = matrix<2, 2>;
using float2x3 = matrix<2, 3>;
using float2x4 = matrix<2, 4>;
using float3x2 = matrix<3, 2>;
using float3x3 = matrix<3, 3>;
using float3x4 = matrix<3, 4>;
using float4x2 = matrix<4, 2>;
using float4x3 = matrix<4, 3>;
using float4x4 = matrix<4, 4>;

// Half matrices (HLSL halfRxC); same ctor order and operator contract as
// floatRxC, no implicit half/float mixing (conversion is explicit).
#if defined(SHADER_MATH_HAS_HALF)
using half2x2 = matrix<2, 2, detail::half_t>;
using half2x3 = matrix<2, 3, detail::half_t>;
using half2x4 = matrix<2, 4, detail::half_t>;
using half3x2 = matrix<3, 2, detail::half_t>;
using half3x3 = matrix<3, 3, detail::half_t>;
using half3x4 = matrix<3, 4, detail::half_t>;
using half4x2 = matrix<4, 2, detail::half_t>;
using half4x3 = matrix<4, 3, detail::half_t>;
using half4x4 = matrix<4, 4, detail::half_t>;
#endif

using bool2x2 = detail::bmat<2, 2>;
using bool2x3 = detail::bmat<2, 3>;
using bool2x4 = detail::bmat<2, 4>;
using bool3x2 = detail::bmat<3, 2>;
using bool3x3 = detail::bmat<3, 3>;
using bool3x4 = detail::bmat<3, 4>;
using bool4x2 = detail::bmat<4, 2>;
using bool4x3 = detail::bmat<4, 3>;
using bool4x4 = detail::bmat<4, 4>;

// Integer matrices (HLSL only; GLSL has none): the int / uint vector rules
// row by row, and mul / transpose as for float
using int2x2 = matrix<2, 2, int>;
using int2x3 = matrix<2, 3, int>;
using int2x4 = matrix<2, 4, int>;
using int3x2 = matrix<3, 2, int>;
using int3x3 = matrix<3, 3, int>;
using int3x4 = matrix<3, 4, int>;
using int4x2 = matrix<4, 2, int>;
using int4x3 = matrix<4, 3, int>;
using int4x4 = matrix<4, 4, int>;
using uint2x2 = matrix<2, 2, uint>;
using uint2x3 = matrix<2, 3, uint>;
using uint2x4 = matrix<2, 4, uint>;
using uint3x2 = matrix<3, 2, uint>;
using uint3x3 = matrix<3, 3, uint>;
using uint3x4 = matrix<3, 4, uint>;
using uint4x2 = matrix<4, 2, uint>;
using uint4x3 = matrix<4, 3, uint>;
using uint4x4 = matrix<4, 4, uint>;

// shared functions under hlsl:: names, for hlsl arguments only
SHADER_MATH_LANG_FORWARD(cross)
SHADER_MATH_LANG_FORWARD(dot)
SHADER_MATH_LANG_FORWARD(length)
SHADER_MATH_LANG_FORWARD(normalize)
SHADER_MATH_LANG_FORWARD(distance)
SHADER_MATH_LANG_FORWARD(reflect)
SHADER_MATH_LANG_FORWARD(refract)
SHADER_MATH_LANG_FORWARD(faceforward)
SHADER_MATH_LANG_FORWARD(abs)
SHADER_MATH_LANG_FORWARD(acos)
SHADER_MATH_LANG_FORWARD(asin)
SHADER_MATH_LANG_FORWARD(atan)
SHADER_MATH_LANG_FORWARD(cosh)
SHADER_MATH_LANG_FORWARD(degrees)
SHADER_MATH_LANG_FORWARD(modf)
SHADER_MATH_LANG_FORWARD(radians)
SHADER_MATH_LANG_FORWARD(sinh)
SHADER_MATH_LANG_FORWARD(tanh)
SHADER_MATH_LANG_FORWARD(ceil)
SHADER_MATH_LANG_FORWARD(clamp)
SHADER_MATH_LANG_FORWARD(cos)
SHADER_MATH_LANG_FORWARD(exp)
SHADER_MATH_LANG_FORWARD(exp2)
SHADER_MATH_LANG_FORWARD(floor)
SHADER_MATH_LANG_FORWARD(log)
SHADER_MATH_LANG_FORWARD(log2)
SHADER_MATH_LANG_FORWARD(max)
SHADER_MATH_LANG_FORWARD(min)
SHADER_MATH_LANG_FORWARD(pow)
SHADER_MATH_LANG_FORWARD(sign)
SHADER_MATH_LANG_FORWARD(sin)
SHADER_MATH_LANG_FORWARD(smoothstep)
SHADER_MATH_LANG_FORWARD(sqrt)
SHADER_MATH_LANG_FORWARD(step)
SHADER_MATH_LANG_FORWARD(tan)
SHADER_MATH_LANG_FORWARD(trunc)

// int / uint vector arguments of the floating-point functions convert to
// float (sqrt(int3) is a float3), as HLSL converts them implicitly; GLSL does
// not. Functions that take integer vectors themselves (abs, min, max, clamp,
// sign, dot) keep them.
#define SM_HLSL_INT_PROMOTE(name, impl)                                                   \
    template <class... As>                                                                \
        requires(detail::ArgsOf<lang_tag, As...> && (detail::IntVec<As> || ...) &&        \
                 !requires(const As&... as) { ::shader_math::detail::impl(as...); } &&    \
                 requires(const As&... as) {                                              \
                     ::shader_math::detail::impl(::shader_math::detail::float_arg(as)...); \
                 })                                                                       \
    SHADER_MATH_INLINE auto name(const As&... as) {                                       \
        return ::shader_math::detail::impl(::shader_math::detail::float_arg(as)...);       \
    }
SM_HLSL_INT_PROMOTE(cross, cross)
SM_HLSL_INT_PROMOTE(length, length)
SM_HLSL_INT_PROMOTE(normalize, normalize)
SM_HLSL_INT_PROMOTE(distance, distance)
SM_HLSL_INT_PROMOTE(reflect, reflect)
SM_HLSL_INT_PROMOTE(refract, refract)
SM_HLSL_INT_PROMOTE(faceforward, faceforward)
SM_HLSL_INT_PROMOTE(acos, acos)
SM_HLSL_INT_PROMOTE(asin, asin)
SM_HLSL_INT_PROMOTE(atan, atan)
SM_HLSL_INT_PROMOTE(cosh, cosh)
SM_HLSL_INT_PROMOTE(degrees, degrees)
SM_HLSL_INT_PROMOTE(radians, radians)
SM_HLSL_INT_PROMOTE(sinh, sinh)
SM_HLSL_INT_PROMOTE(tanh, tanh)
SM_HLSL_INT_PROMOTE(ceil, ceil)
SM_HLSL_INT_PROMOTE(cos, cos)
SM_HLSL_INT_PROMOTE(exp, exp)
SM_HLSL_INT_PROMOTE(exp2, exp2)
SM_HLSL_INT_PROMOTE(floor, floor)
SM_HLSL_INT_PROMOTE(log, log)
SM_HLSL_INT_PROMOTE(log2, log2)
SM_HLSL_INT_PROMOTE(pow, pow)
SM_HLSL_INT_PROMOTE(sin, sin)
SM_HLSL_INT_PROMOTE(smoothstep, smoothstep)
SM_HLSL_INT_PROMOTE(sqrt, sqrt)
SM_HLSL_INT_PROMOTE(step, step)
SM_HLSL_INT_PROMOTE(tan, tan)
SM_HLSL_INT_PROMOTE(trunc, trunc)
SM_HLSL_INT_PROMOTE(round, round)
SM_HLSL_INT_PROMOTE(frac, fract_impl)
SM_HLSL_INT_PROMOTE(saturate, saturate_impl)
SM_HLSL_INT_PROMOTE(rcp, rcp_impl)
SM_HLSL_INT_PROMOTE(rsqrt, rsqrt_impl)
SM_HLSL_INT_PROMOTE(log10, log10_impl)
SM_HLSL_INT_PROMOTE(lerp, lerp_impl)
SM_HLSL_INT_PROMOTE(atan2, atan2_impl)
SM_HLSL_INT_PROMOTE(fmod, fmod_impl)
#undef SM_HLSL_INT_PROMOTE

// isnan / isinf / round / all / any have explicit signatures. The vector
// forms name the vector type because the MSVC UCRT declares unconstrained
// isnan(_Ty) / isinf(_Ty) templates that only a more specialized parameter
// beats (see detail::isnan). The scalar isnan / isinf take their argument by
// value so that against those templates the constraint decides; where the C
// library has a plain isnan(float) it wins, with the same result. Scalar
// round is a plain function: libc++ and MSVC declare ::round(float) (halves
// away from zero), which a template would quietly lose to; this way an
// unqualified round(x) is ambiguous there and qualified calls always resolve.
template <int N, class T>
    requires(!detail::is_int_elem_v<T>)
SHADER_MATH_INLINE detail::bvec<N, lang_tag> isnan(const vector<T, N>& v) {
    return detail::isnan(v);
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
SHADER_MATH_INLINE auto isnan(const detail::swz<T, N, lang_tag, Is...>& s) {
    return detail::isnan(s);
}
template <class A>
    requires detail::FloatScalarCall<A>
SHADER_MATH_INLINE bool isnan(A a) {
    return detail::isnan(detail::lane1<detail::scalar_call_t<A>, lang_tag>(a))[0];
}
template <int N, class T>
    requires(!detail::is_int_elem_v<T>)
SHADER_MATH_INLINE detail::bvec<N, lang_tag> isinf(const vector<T, N>& v) {
    return detail::isinf(v);
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
SHADER_MATH_INLINE auto isinf(const detail::swz<T, N, lang_tag, Is...>& s) {
    return detail::isinf(s);
}
template <class A>
    requires detail::FloatScalarCall<A>
SHADER_MATH_INLINE bool isinf(A a) {
    return detail::isinf(detail::lane1<detail::scalar_call_t<A>, lang_tag>(a))[0];
}
SHADER_MATH_INLINE float round(float a) {
    return detail::round_lane(a);
}
#if defined(SHADER_MATH_HAS_HALF)
SHADER_MATH_INLINE detail::half_t round(detail::half_t a) {
    return detail::round_lane(a);
}
#endif
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto round(const A& a) {
    return detail::round(a);
}
template <int N>
SHADER_MATH_INLINE bool all(const detail::bvec<N, lang_tag>& b) {
    return detail::all(b);
}
SHADER_MATH_INLINE bool all(bool b) {
    return b;
}
SHADER_MATH_INLINE bool any(bool b) {
    return b;
}
template <int N>
SHADER_MATH_INLINE bool any(const detail::bvec<N, lang_tag>& b) {
    return detail::any(b);
}
template <int R, int C>
SHADER_MATH_INLINE bool all(const detail::bmat<R, C>& b) {
    for (int r = 0; r < R; ++r)
        if (!all(b.row_[r])) return false;
    return true;
}
template <int R, int C>
SHADER_MATH_INLINE bool any(const detail::bmat<R, C>& b) {
    for (int r = 0; r < R; ++r)
        if (any(b.row_[r])) return true;
    return false;
}

// HLSL-only spellings (defined here, not in detail, so they never leak into
// glsl; the language tag makes ADL find them on hlsl vectors)
template <class... As>
    requires(detail::ArgsOf<lang_tag, As...> &&
             requires(const As&... as) { detail::lerp_impl(as...); })
SHADER_MATH_INLINE auto lerp(const As&... as) {
    return detail::lerp_impl(as...);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto saturate(const A& a) {
    return detail::saturate_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto frac(const A& a) {
    return detail::fract_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto rcp(const A& a) {
    return detail::rcp_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto rsqrt(const A& a) {
    return detail::rsqrt_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto log10(const A& a) {
    return detail::log10_impl(a);
}
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& a, const B& b) { detail::ldexp_impl(a, b); })
SHADER_MATH_INLINE auto ldexp(const A& a, const B& b) {
    return detail::ldexp_impl(a, b);
}
template <class A, int N, class T>
    requires requires(const A& a, vector<T, N>& e) { detail::frexp_impl(a, e); }
SHADER_MATH_INLINE auto frexp(const A& a, vector<T, N>& e) {
    return detail::frexp_impl(a, e);
}
// select(cond, a, b) (HLSL 2021): a where cond, else b; scalar a / b broadcast
template <int N, class A, class B>
    requires requires(const detail::bvec<N, lang_tag>& c, const A& a, const B& b) {
        detail::select_impl(c, a, b);
    }
SHADER_MATH_INLINE auto select(const detail::bvec<N, lang_tag>& c, const A& a, const B& b) {
    return detail::select_impl(c, a, b);
}
// dst(a, b) = (1, a.y * b.y, a.z, b.w): the distance vector of the
// fixed-function lighting model (float4 only, like HLSL)
template <class A, class B>
    requires(detail::SameVec<A, B> && detail::VecOf<A, lang_tag> && detail::vec_size_v<A> == 4)
SHADER_MATH_INLINE auto dst(const A& a0, const B& b0) {
    using T = detail::vec_elem_t<A>;
    const detail::vec_t<A> a = detail::to_vec(a0);
    const detail::vec_t<A> b = detail::to_vec(b0);
    return detail::vec_t<A>(T(1.0f), a[1] * b[1], a[2], b[3]);
}
// lit(n_dot_l, n_dot_h, m) = (1, diffuse, specular, 1): scalar inputs, like
// HLSL; specular is 0 when either dot product is negative
template <class S0, class S1, class S2>
    requires(std::is_arithmetic_v<S0> && std::is_arithmetic_v<S1> && std::is_arithmetic_v<S2>)
SHADER_MATH_INLINE float4 lit(S0 n_dot_l, S1 n_dot_h, S2 m) {
    const float l = static_cast<float>(n_dot_l);
    const float h = static_cast<float>(n_dot_h);
    const float diffuse = l < 0.0f ? 0.0f : l;
    const float specular = (l < 0.0f || h < 0.0f) ? 0.0f : std::pow(h, static_cast<float>(m));
    return float4(1.0f, diffuse, specular, 1.0f);
}
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& y, const B& x) { detail::atan2_impl(y, x); })
SHADER_MATH_INLINE auto atan2(const A& y, const B& x) {
    return detail::atan2_impl(y, x);
}
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& a, const B& b) { detail::fmod_impl(a, b); })
SHADER_MATH_INLINE auto fmod(const A& a, const B& b) {
    return detail::fmod_impl(a, b);
}
template <class A, class B, class C>
    requires(detail::ArgsOf<lang_tag, A, B, C> &&
             requires(const A& a, const B& b, const C& c) { detail::mad_impl(a, b, c); })
SHADER_MATH_INLINE auto mad(const A& a, const B& b, const C& c) {
    return detail::mad_impl(a, b, c);
}
// fvec / swz parameters for the same reason as detail::isnan (MSVC UCRT)
template <int N, class T>
    requires(!detail::is_int_elem_v<T>)
SHADER_MATH_INLINE detail::bvec<N, lang_tag> isfinite(const vector<T, N>& v) {
    return detail::isfinite_impl(v);
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2 && !detail::is_int_elem_v<T>)
SHADER_MATH_INLINE auto isfinite(const detail::swz<T, N, lang_tag, Is...>& s) {
    return detail::isfinite_impl(detail::to_vec(s));
}
template <class A, int N, class T>
    requires requires(const A& a, vector<T, N>& s, vector<T, N>& c) {
        detail::sincos_impl(a, s, c);
    }
SHADER_MATH_INLINE void sincos(const A& a, vector<T, N>& s, vector<T, N>& c) {
    detail::sincos_impl(a, s, c);
}

// isfinite on scalars (by value, see detail::isnan)
template <class A>
    requires detail::FloatScalarCall<A>
SHADER_MATH_INLINE bool isfinite(A a) {
    return isfinite(detail::lane1<detail::scalar_call_t<A>, lang_tag>(a))[0];
}

// all / any on numeric vectors, matrices and scalars: HLSL tests the
// components against zero (NaN counts as nonzero); GLSL takes bool vectors
// only, so these live here and not in detail
template <class A>
    requires detail::VecOf<A, lang_tag>
SHADER_MATH_INLINE bool all(const A& a) {
    return all(detail::bvec<detail::vec_size_v<A>, lang_tag>(a));
}
template <class A>
    requires detail::VecOf<A, lang_tag>
SHADER_MATH_INLINE bool any(const A& a) {
    return any(detail::bvec<detail::vec_size_v<A>, lang_tag>(a));
}
template <class A>
    requires detail::ScalarCall<A>
SHADER_MATH_INLINE bool all(A a) {
    using T = detail::scalar_call_t<A>;
    return static_cast<T>(a) != T(0.0f);
}
template <class A>
    requires detail::ScalarCall<A>
SHADER_MATH_INLINE bool any(A a) {
    return all(a);
}

// select(bool, a, b) with a vector operand: the condition applies to every
// component
template <class A, class B,
          int N = (detail::vec_size_v<A> > detail::vec_size_v<B> ? detail::vec_size_v<A>
                                                                  : detail::vec_size_v<B>)>
    requires(N >= 2 && requires(const detail::bvec<N, lang_tag>& c, const A& a, const B& b) {
        detail::select_impl(c, a, b);
    })
SHADER_MATH_INLINE auto select(bool c, const A& a, const B& b) {
    return detail::select_impl(detail::bvec<N, lang_tag>(c), a, b);
}

// scalar arguments (detail::ScalarCall): HLSL-only spellings
SHADER_MATH_SCALAR_FN3(lerp)
SHADER_MATH_SCALAR_FN1(saturate)
SHADER_MATH_SCALAR_FN1(frac)
SHADER_MATH_SCALAR_FN1(rcp)
SHADER_MATH_SCALAR_FN1(rsqrt)
SHADER_MATH_SCALAR_FN1(log10)
SHADER_MATH_SCALAR_FN2(atan2)
SHADER_MATH_SCALAR_FN2(fmod)
SHADER_MATH_SCALAR_FN2(ldexp)
SHADER_MATH_SCALAR_FN3(mad)
template <class A, class T>
    requires(detail::FloatScalarCall<A> && std::is_same_v<T, detail::scalar_call_t<A>>)
SHADER_MATH_INLINE T frexp(const A& a, T& e) {
    vector<T, 2> ev;
    const T m = frexp(detail::lane1<T, lang_tag>(a), ev)[0];
    e = ev[0];
    return m;
}
template <class A, class T>
    requires(detail::FloatScalarCall<A> && std::is_same_v<T, detail::scalar_call_t<A>>)
SHADER_MATH_INLINE void sincos(const A& a, T& s, T& c) {
    vector<T, 2> sv, cv;
    sincos(detail::lane1<T, lang_tag>(a), sv, cv);
    s = sv[0];
    c = cv[0];
}
template <class A, class B>
    requires detail::ScalarCall<A, B>
SHADER_MATH_INLINE auto select(bool c, const A& a, const B& b) {
    using T = detail::scalar_call_t<A, B>;
    return c ? static_cast<T>(a) : static_cast<T>(b);
}

// ---- component-wise matrix operators ----------------------------------------
// Same-shape matrices only; `float3x4 * float4x3` does not compile (use mul).
#define SM_HLSL_MAT_BINOP(sym, fn)                                                       \
    template <int R, int C, class T>                                                     \
    SHADER_MATH_INLINE matrix<R, C, T> operator sym(const matrix<R, C, T>& a,            \
                                                    const matrix<R, C, T>& b) {          \
        matrix<R, C, T> o;                                                               \
        detail::regs_fill<R>(o.row_, [&](int r) {                                        \
            return detail::vops<T>::fn(a.row_[r].q_, b.row_[r].q_);                      \
        });                                                                              \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::VecScalar<S, detail::hvec<C, T>>                                \
    SHADER_MATH_INLINE matrix<R, C, T> operator sym(const matrix<R, C, T>& a, const S& s) { \
        matrix<R, C, T> o;                                                               \
        const auto sp = detail::vops<T>::splat(static_cast<T>(s));                       \
        detail::regs_fill<R>(o.row_,                                                     \
                             [&](int r) { return detail::vops<T>::fn(a.row_[r].q_, sp); }); \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::VecScalar<S, detail::hvec<C, T>>                                \
    SHADER_MATH_INLINE matrix<R, C, T> operator sym(const S& s, const matrix<R, C, T>& b) { \
        matrix<R, C, T> o;                                                               \
        const auto sp = detail::vops<T>::splat(static_cast<T>(s));                       \
        detail::regs_fill<R>(o.row_,                                                     \
                             [&](int r) { return detail::vops<T>::fn(sp, b.row_[r].q_); }); \
        return o;                                                                        \
    }

SM_HLSL_MAT_BINOP(+, add)
SM_HLSL_MAT_BINOP(-, sub)
SM_HLSL_MAT_BINOP(*, mul)  // Hadamard, NOT the matrix product
SM_HLSL_MAT_BINOP(/, div)
#undef SM_HLSL_MAT_BINOP

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator-(const matrix<R, C, T>& a) {
    matrix<R, C, T> o;
    detail::regs_fill<R>(o.row_, [&](int r) { return detail::vops<T>::neg(a.row_[r].q_); });
    return o;
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator+(const matrix<R, C, T>& a) {
    return a;
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T>& operator++(matrix<R, C, T>& a) {
    a = a + T(1.0f);
    return a;
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T>& operator--(matrix<R, C, T>& a) {
    a = a - T(1.0f);
    return a;
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator++(matrix<R, C, T>& a, int) {
    const matrix<R, C, T> old = a;
    a = a + T(1.0f);
    return old;
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator--(matrix<R, C, T>& a, int) {
    const matrix<R, C, T> old = a;
    a = a - T(1.0f);
    return old;
}

#define SM_HLSL_MAT_COMPOUND(sym)                                                        \
    template <int R, int C, class T>                                                     \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a,              \
                                                        const matrix<R, C, T>& b) {      \
        a = a sym b;                                                                     \
        return a;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::VecScalar<S, detail::hvec<C, T>>                                \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a, const S& s) { \
        a = a sym s;                                                                     \
        return a;                                                                        \
    }

SM_HLSL_MAT_COMPOUND(+)
SM_HLSL_MAT_COMPOUND(-)
SM_HLSL_MAT_COMPOUND(*)
SM_HLSL_MAT_COMPOUND(/)
#undef SM_HLSL_MAT_COMPOUND

// ---- integer matrices: bitwise operators and shifts, row by row -------------
#define SM_HLSL_IMAT_BINOP(sym)                                                          \
    template <int R, int C, class T, class B>                                            \
        requires(detail::is_int_elem_v<T> &&                                             \
                 (std::is_same_v<B, matrix<R, C, T>> || detail::is_int_scalar_v<B>))      \
    SHADER_MATH_INLINE auto operator sym(const matrix<R, C, T>& a, const B& b) {         \
        if constexpr (std::is_same_v<B, matrix<R, C, T>>) {                              \
            matrix<R, C, T> o;                                                           \
            for (int r = 0; r < R; ++r) o.row_[r] = a.row_[r] sym b.row_[r];             \
            return o;                                                                    \
        } else {                                                                         \
            detail::hmat_of_rows<R, decltype(a.row_[0] sym b)> o;                        \
            for (int r = 0; r < R; ++r) o.row_[r] = a.row_[r] sym b;                     \
            return o;                                                                    \
        }                                                                                \
    }                                                                                    \
    template <int R, int C, class T, class B>                                            \
        requires(detail::is_int_elem_v<T> && requires(const matrix<R, C, T>& a, const B& b) { \
            { a sym b } -> std::convertible_to<matrix<R, C, T>>;                         \
        })                                                                               \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a, const B& b) { \
        a = a sym b;                                                                     \
        return a;                                                                        \
    }
SM_HLSL_IMAT_BINOP(&)
SM_HLSL_IMAT_BINOP(|)
SM_HLSL_IMAT_BINOP(^)
SM_HLSL_IMAT_BINOP(<<)
SM_HLSL_IMAT_BINOP(>>)
#undef SM_HLSL_IMAT_BINOP
template <int R, int C, class T>
    requires detail::is_int_elem_v<T>
SHADER_MATH_INLINE matrix<R, C, T> operator~(const matrix<R, C, T>& a) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = ~a.row_[r];
    return o;
}

// ---- mixed int / uint / float matrices: the common type, as for vectors -----
// (int3x3 + float3x3 is a float3x3, an int matrix with a floating-point
// scalar computes in float); compound assignment converts back implicitly
#define SM_HLSL_MIXED_MAT_BINOP(sym)                                                     \
    template <int R, int C, class T, class U>                                            \
        requires detail::MixedElems<T, U, lang_tag>                                                        \
    SHADER_MATH_INLINE auto operator sym(const matrix<R, C, T>& a, const matrix<R, C, U>& b) { \
        using E = detail::common_elem_t<T, U, lang_tag>;                                           \
        return matrix<R, C, E>(a) sym matrix<R, C, E>(b);                                \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires(detail::is_int_elem_v<T> && std::is_floating_point_v<S>)                \
    SHADER_MATH_INLINE auto operator sym(const matrix<R, C, T>& a, const S& s) {         \
        return matrix<R, C, float>(a) sym static_cast<float>(s);                         \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires(detail::is_int_elem_v<T> && std::is_floating_point_v<S>)                \
    SHADER_MATH_INLINE auto operator sym(const S& s, const matrix<R, C, T>& b) {         \
        return static_cast<float>(s) sym matrix<R, C, float>(b);                         \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::PromotingScalar<S, detail::hvec<C, T>>                          \
    SHADER_MATH_INLINE auto operator sym(const matrix<R, C, T>& a, const S& s) {         \
        using E = detail::int_scalar_op_t<detail::hvec<C, T>, S>;                        \
        return matrix<R, C, E>(a) sym static_cast<E>(s);                                 \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::PromotingScalar<S, detail::hvec<C, T>>                          \
    SHADER_MATH_INLINE auto operator sym(const S& s, const matrix<R, C, T>& b) {         \
        using E = detail::int_scalar_op_t<detail::hvec<C, T>, S>;                        \
        return static_cast<E>(s) sym matrix<R, C, E>(b);                                 \
    }                                                                                    \
    template <int R, int C, class T, class B>                                            \
        requires(((detail::is_int_elem_v<T> && std::is_floating_point_v<B>) ||           \
                  detail::PromotingScalar<B, detail::hvec<C, T>> ||                      \
                  requires { requires detail::MixedElems<T, typename B::elem_t, lang_tag>; }) &&           \
                 requires(const matrix<R, C, T>& a, const B& b) {                        \
                     { a sym b } -> std::convertible_to<matrix<R, C, T>>;                \
                 })                                                                      \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a, const B& b) { \
        a = a sym b;                                                                     \
        return a;                                                                        \
    }
SM_HLSL_MIXED_MAT_BINOP(+)
SM_HLSL_MIXED_MAT_BINOP(-)
SM_HLSL_MIXED_MAT_BINOP(*)
SM_HLSL_MIXED_MAT_BINOP(/)
#undef SM_HLSL_MIXED_MAT_BINOP

// ---- % on float vectors / matrices: fmod (trunc-based, sign of x) ----------
// HLSL defines % for floats; GLSL does not (its mod() is floor-based).
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& a, const B& b) { detail::fmod_impl(a, b); })
SHADER_MATH_INLINE auto operator%(const A& a, const B& b) {
    return detail::fmod_impl(a, b);
}
template <class A, class B>
    requires(detail::MixedVec<A, B> && detail::VecOf<A, lang_tag> &&
             std::is_same_v<detail::common_elem_t<detail::vec_elem_t<A>, detail::vec_elem_t<B>, lang_tag>, float>)
SHADER_MATH_INLINE auto operator%(const A& a, const B& b) {
    using M = detail::mixed_t<A, B>;
    return detail::fmod_impl(M(detail::to_vec(a)), M(detail::to_vec(b)));
}
// matrices: row by row (fmod on float rows, the truncating % on int rows)
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator%(const matrix<R, C, T>& a, const matrix<R, C, T>& b) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = a.row_[r] % b.row_[r];
    return o;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE auto operator%(const matrix<R, C, T>& a, const S& s) {
    detail::hmat_of_rows<R, decltype(a.row_[0] % s)> o;
    for (int r = 0; r < R; ++r) o.row_[r] = a.row_[r] % s;
    return o;
}
template <int N, class T, class B>
    requires(!detail::is_int_elem_v<T> && requires(const vector<T, N>& a, const B& b) {
        { a % b } -> std::same_as<vector<T, N>>;
    })
SHADER_MATH_INLINE vector<T, N>& operator%=(vector<T, N>& a, const B& b) {
    a = a % b;
    return a;
}
template <int R, int C, class T, class B>
    requires requires(const matrix<R, C, T>& a, const B& b) {
        { a % b } -> std::convertible_to<matrix<R, C, T>>;
    }
SHADER_MATH_INLINE matrix<R, C, T>& operator%=(matrix<R, C, T>& a, const B& b) {
    a = a % b;
    return a;
}

// ---- comparisons: per-component results (HLSL semantics) --------------------
// `if (a == b)` does not compile (bvec has no bool conversion); use all()/any().
#define SM_HLSL_VEC_CMP(sym, fn)                                                         \
    template <class A, class B>                                                          \
        requires(detail::SameVec<A, B> && detail::VecOf<A, lang_tag>)                    \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                       \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n, lang_tag>(                                                \
            detail::vops<T>::fn(detail::to_vec(a).q_, detail::to_vec(b).q_));            \
    }                                                                                    \
    template <class A, class S>                                                          \
        requires(detail::VecOf<A, lang_tag> && detail::VecScalar<S, A>)                  \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                       \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n, lang_tag>(                                                \
            detail::vops<T>::fn(detail::to_vec(a).q_, detail::vops<T>::splat(static_cast<T>(s)))); \
    }                                                                                    \
    template <class B, class S>                                                          \
        requires(detail::VecOf<B, lang_tag> && detail::VecScalar<S, B>)                  \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                       \
        constexpr int n = detail::vec_size_v<B>;                                         \
        using T = detail::vec_elem_t<B>;                                                 \
        return detail::bvec<n, lang_tag>(                                                \
            detail::vops<T>::fn(detail::vops<T>::splat(static_cast<T>(s)), detail::to_vec(b).q_)); \
    }                                                                                    \
    template <class A, class B>                                                          \
        requires(detail::MixedVec<A, B> && detail::VecOf<A, lang_tag>)                   \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                       \
        using M = detail::mixed_t<A, B>;                                                 \
        return M(detail::to_vec(a)) sym M(detail::to_vec(b));                            \
    }                                                                                    \
    template <class A, class S>                                                          \
        requires(detail::VecOf<A, lang_tag> && detail::IntVecFloatScalar<A, S>)          \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                       \
        return detail::float_vec_t<A>(detail::to_vec(a)) sym static_cast<float>(s);      \
    }                                                                                    \
    template <class B, class S>                                                          \
        requires(detail::VecOf<B, lang_tag> && detail::IntVecFloatScalar<B, S>)          \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                       \
        return static_cast<float>(s) sym detail::float_vec_t<B>(detail::to_vec(b));      \
    }                                                                                    \
    template <class A, class S>                                                          \
        requires(detail::VecOf<A, lang_tag> && detail::PromotingScalar<S, A>)            \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                       \
        using R = detail::int_scalar_op_t<A, S>;                                         \
        return detail::hvec<detail::vec_size_v<A>, R>(detail::to_vec(a)) sym static_cast<R>(s); \
    }                                                                                    \
    template <class B, class S>                                                          \
        requires(detail::VecOf<B, lang_tag> && detail::PromotingScalar<S, B>)            \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                       \
        using R = detail::int_scalar_op_t<B, S>;                                         \
        return static_cast<R>(s) sym detail::hvec<detail::vec_size_v<B>, R>(detail::to_vec(b)); \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE detail::bmat<R, C> operator sym(const matrix<R, C, T>& a, const S& s) { \
        detail::bmat<R, C> o;                                                            \
        for (int r = 0; r < R; ++r) o.row_[r] = a.row_[r] sym s;                         \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE detail::bmat<R, C> operator sym(const S& s, const matrix<R, C, T>& b) { \
        detail::bmat<R, C> o;                                                            \
        for (int r = 0; r < R; ++r) o.row_[r] = s sym b.row_[r];                         \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C, class T>                                                     \
    SHADER_MATH_INLINE detail::bmat<R, C> operator sym(const matrix<R, C, T>& a,         \
                                                       const matrix<R, C, T>& b) {       \
        detail::bmat<R, C> o;                                                            \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] =                                                                  \
                detail::bvec<C, lang_tag>(detail::vops<T>::fn(a.row_[r].q_, b.row_[r].q_)); \
        return o;                                                                        \
    }

SM_HLSL_VEC_CMP(==, cmp_eq)
SM_HLSL_VEC_CMP(!=, cmp_ne)
SM_HLSL_VEC_CMP(<, cmp_lt)
SM_HLSL_VEC_CMP(<=, cmp_le)
SM_HLSL_VEC_CMP(>, cmp_gt)
SM_HLSL_VEC_CMP(>=, cmp_ge)
#undef SM_HLSL_VEC_CMP

// bool-vector logic (HLSL): !, ==, != are per-component
template <int N>
SHADER_MATH_INLINE detail::bvec<N, lang_tag> operator!(const detail::bvec<N, lang_tag>& a) {
    return detail::bvec<N, lang_tag>(detail::ops::m_not(a.m_));
}
template <int N>
SHADER_MATH_INLINE detail::bvec<N, lang_tag> operator==(const detail::bvec<N, lang_tag>& a,
                                                        const detail::bvec<N, lang_tag>& b) {
    return detail::bvec<N, lang_tag>(detail::ops::m_not(detail::ops::m_xor(a.m_, b.m_)));
}
template <int N>
SHADER_MATH_INLINE detail::bvec<N, lang_tag> operator!=(const detail::bvec<N, lang_tag>& a,
                                                        const detail::bvec<N, lang_tag>& b) {
    return detail::bvec<N, lang_tag>(detail::ops::m_xor(a.m_, b.m_));
}

// ! on int / uint vectors: true where the component is zero
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::IntVec<A>)
SHADER_MATH_INLINE auto operator!(const A& a) {
    using T = detail::vec_elem_t<A>;
    return detail::bvec<detail::vec_size_v<A>, lang_tag>(
        detail::vops<T>::cmp_eq(detail::to_vec(a).q_, detail::vops<T>::zero()));
}

// scalar << vector / scalar >> vector (HLSL; GLSL needs a vector on the left)
template <class S, class B>
    requires(detail::VecOf<B, lang_tag> && detail::IntVec<B> && detail::is_int_scalar_v<S>)
SHADER_MATH_INLINE auto operator<<(const S& s, const B& n) {
    using T = detail::vec_elem_t<B>;
    return detail::vec_t<B>(static_cast<T>(s)) << n;
}
template <class S, class B>
    requires(detail::VecOf<B, lang_tag> && detail::IntVec<B> && detail::is_int_scalar_v<S>)
SHADER_MATH_INLINE auto operator>>(const S& s, const B& n) {
    using T = detail::vec_elem_t<B>;
    return detail::vec_t<B>(static_cast<T>(s)) >> n;
}

// dot on int / uint vectors (the float dot is the shared one)
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && detail::SameVec<A, B> && detail::IntVec<A>)
SHADER_MATH_INLINE auto dot(const A& a, const B& b) {
    return detail::idot(a, b);
}

// asint / asuint / asfloat: the same bits as another 32-bit type (float,
// int, uint vectors and scalars)
#define SM_HLSL_BITCAST(name, U)                                                         \
    template <class A>                                                                   \
        requires(detail::VecOf<A, lang_tag> &&                                           \
                 requires(const A& a) { detail::bitcast_vec<U>(a); })                    \
    SHADER_MATH_INLINE auto name(const A& a) {                                           \
        return detail::bitcast_vec<U>(a);                                                \
    }                                                                                    \
    template <class S>                                                                   \
        requires(!detail::VecLike<S> && detail::is_bits32_scalar_v<S>)                   \
    SHADER_MATH_INLINE U name(const S& s) {                                              \
        return detail::bitcast_scalar<U>(s);                                             \
    }
SM_HLSL_BITCAST(asint, int)
SM_HLSL_BITCAST(asuint, uint)
SM_HLSL_BITCAST(asfloat, float)
#undef SM_HLSL_BITCAST

// asint16 / asuint16 / asfloat16: the same bits as another 16-bit type
// (int16_t, uint16_t, half vectors and scalars)
#define SM_HLSL_BITCAST16(name, U)                                                       \
    template <class A>                                                                   \
        requires(detail::VecOf<A, lang_tag> && detail::is_bits16_v<detail::vec_elem_t<A>>) \
    SHADER_MATH_INLINE auto name(const A& a) {                                           \
        return detail::map_int_lanes<U>(a, [](auto x) { return detail::bitcast_lane<U>(x); }); \
    }                                                                                    \
    template <class S>                                                                   \
        requires(!detail::VecLike<S> &&                                                  \
                 detail::is_bits16_v<typename detail::scalar_value<std::remove_cvref_t<S>>::type>) \
    SHADER_MATH_INLINE U name(const S& s) {                                              \
        return detail::bitcast_lane<U>(                                                  \
            static_cast<typename detail::scalar_value<std::remove_cvref_t<S>>::type>(s)); \
    }
SM_HLSL_BITCAST16(asint16, int16_t)
SM_HLSL_BITCAST16(asuint16, uint16_t)
#if defined(SHADER_MATH_HAS_HALF)
SM_HLSL_BITCAST16(asfloat16, detail::half_t)
#endif
#undef SM_HLSL_BITCAST16

// countbits / firstbithigh / firstbitlow / reversebits on integer vectors and
// scalars of 16, 32 and 64 bits (a float or half argument converts to int, as
// in HLSL). countbits / firstbithigh / firstbitlow give uint; firstbithigh on
// a signed integer is the highest bit that differs from the sign bit, and
// both give 0xFFFFFFFF where there is no such bit. reversebits keeps the
// integer type.
#define SM_HLSL_BITFN(name, fn)                                                          \
    template <class A>                                                                   \
        requires detail::VecOf<A, lang_tag>                                              \
    SHADER_MATH_INLINE auto name(const A& a) {                                           \
        return detail::map_int_lanes<uint>(detail::bits_vec(a), detail::fn{});           \
    }                                                                                    \
    template <class S>                                                                   \
        requires detail::is_bits_scalar_v<S>                                             \
    SHADER_MATH_INLINE uint name(const S& s) {                                           \
        return detail::fn{}(detail::bits_scalar(s));                                     \
    }
SM_HLSL_BITFN(countbits, popcount_fn)
SM_HLSL_BITFN(firstbithigh, high_fn)
SM_HLSL_BITFN(firstbitlow, low_fn)
#undef SM_HLSL_BITFN
template <class A>
    requires detail::VecOf<A, lang_tag>
SHADER_MATH_INLINE auto reversebits(const A& a) {
    using T = detail::bits_elem_t<detail::vec_elem_t<A>>;
    return detail::map_int_lanes<T>(detail::bits_vec(a), detail::reverse_fn{});
}
template <class S>
    requires detail::is_bits_scalar_v<S>
SHADER_MATH_INLINE auto reversebits(const S& s) {
    return detail::reverse_fn{}(detail::bits_scalar(s));
}

// f32tof16 / f16tof32: the binary16 bits of a float in the low 16 bits of a
// uint (rounded to nearest even), and back from the low 16 bits; vectors and
// scalars
template <class A>
    requires(detail::VecOf<A, lang_tag> && std::is_same_v<detail::vec_elem_t<A>, float>)
SHADER_MATH_INLINE auto f32tof16(const A& a) {
    return detail::map_int_lanes<uint>(
        a, [](float x) { return static_cast<uint>(detail::half_bits::from_f32(x)); });
}
template <class S>
    requires(!detail::VecLike<S> && std::is_arithmetic_v<typename detail::scalar_value<std::remove_cvref_t<S>>::type> &&
             !std::is_same_v<typename detail::scalar_value<std::remove_cvref_t<S>>::type, bool>)
SHADER_MATH_INLINE uint f32tof16(const S& s) {
    using V = typename detail::scalar_value<std::remove_cvref_t<S>>::type;
    return detail::half_bits::from_f32(static_cast<float>(static_cast<V>(s)));
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::Int32Vec<A>)
SHADER_MATH_INLINE auto f16tof32(const A& a) {
    return detail::map_int_lanes<float>(a, [](auto x) {
        return detail::half_bits::to_f32(static_cast<unsigned short>(static_cast<uint>(x) & 0xFFFFu));
    });
}
template <class S>
    requires detail::is_int32_scalar_v<S>
SHADER_MATH_INLINE float f16tof32(const S& s) {
    return detail::half_bits::to_f32(static_cast<unsigned short>(static_cast<uint>(s) & 0xFFFFu));
}

// ---- mul: the 9 official overloads ------------------------------------------
SHADER_MATH_INLINE float mul(float a, float b) { return a * b; }
#if defined(SHADER_MATH_HAS_HALF)
SHADER_MATH_INLINE detail::half_t mul(detail::half_t a, detail::half_t b) { return a * b; }
#endif

template <class V, class S>
    requires(detail::VecOf<V, lang_tag> && detail::ScalarFor<S, detail::vec_elem_t<V>>)
SHADER_MATH_INLINE auto mul(const S& s, const V& v) {
    return s * detail::to_vec(v);
}
template <class V, class S>
    requires(detail::VecOf<V, lang_tag> && detail::ScalarFor<S, detail::vec_elem_t<V>>)
SHADER_MATH_INLINE auto mul(const V& v, const S& s) {
    return detail::to_vec(v) * s;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE auto mul(const S& s, const matrix<R, C, T>& m) {
    return s * m;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE auto mul(const matrix<R, C, T>& m, const S& s) {
    return m * s;
}

// vector x vector: equivalent to dot()
template <class A, class B>
    requires(detail::SameVec<A, B> && detail::VecOf<A, lang_tag>)
SHADER_MATH_INLINE detail::vec_elem_t<A> mul(const A& a, const B& b) {
    if constexpr (detail::IntVec<A>)
        return detail::idot(a, b);
    else
        return detail::dot(a, b);
}

// row vector x matrix
template <class V, int M, int N2, class T>
    requires std::is_same_v<detail::vec_t<V>, vector<T, M>>
SHADER_MATH_INLINE vector<T, N2> mul(const V& v0, const matrix<M, N2, T>& m) {
    const vector<T, M> v = detail::to_vec(v0);
    return vector<T, N2>(detail::mad_accum<M>(v.q_, m.row_));
}

// matrix x column vector
template <int M, int K, class T, class V>
    requires std::is_same_v<detail::vec_t<V>, vector<T, K>>
SHADER_MATH_INLINE vector<T, M> mul(const matrix<M, K, T>& m, const V& v0) {
    const vector<T, K> v = detail::to_vec(v0);
    T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int r = 0; r < M; ++r) t[r] = mul(m.row_[r], v);
    return vector<T, M>(detail::vops<T>::load4(t));
}

// matrix x matrix: inner dimensions must match (enforced by the signature)
template <int M, int K, int N2, class T>
SHADER_MATH_INLINE matrix<M, N2, T> mul(const matrix<M, K, T>& a, const matrix<K, N2, T>& b) {
    matrix<M, N2, T> o;
    for (int r = 0; r < M; ++r)
        o.row_[r] = vector<T, N2>(detail::mad_accum<K>(a.row_[r].q_, b.row_));
    return o;
}

#if defined(SHADER_MATH_HAS_AVX_2ROW)
// float 4x4 product on the shared YMM kernel (AVX1 + FMA3, bit-identical to
// the 128-bit path); this non-template overload simply wins overload
// resolution when available.
SHADER_MATH_INLINE matrix<4, 4> mul(const matrix<4, 4>& a, const matrix<4, 4>& b) {
    matrix<4, 4> o;
    detail::avx2row::mul4x4(a.row_, b.row_, o.row_);
    return o;
}
#endif

// all / any on numeric matrices: every / some element is nonzero
template <int R, int C, class T>
SHADER_MATH_INLINE bool all(const matrix<R, C, T>& m) {
    for (int r = 0; r < R; ++r)
        if (!all(m.row_[r])) return false;
    return true;
}
template <int R, int C, class T>
SHADER_MATH_INLINE bool any(const matrix<R, C, T>& m) {
    for (int r = 0; r < R; ++r)
        if (any(m.row_[r])) return true;
    return false;
}

// ---- transpose / determinant / inverse ---------------------------------------
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<C, R, T> transpose(const matrix<R, C, T>& m) {
    matrix<C, R, T> o;
    detail::transpose_regs<R, C>(m.row_, o.row_);
    return o;
}

template <int R, int C, class T>
SHADER_MATH_INLINE auto determinant(const matrix<R, C, T>& m) {
    static_assert(R == C, "shader-math: determinant requires a square matrix");
    if constexpr (detail::is_int_elem_v<T>)  // converted to float, as HLSL does
        return detail::det_regs<R>(matrix<R, C, float>(m).row_);
    else
        return detail::det_regs<R>(m.row_);
}


// ---- component-wise intrinsics on matrices ----------------------------------
// HLSL applies its component-wise intrinsics to matrices too; GLSL's generic
// functions take genType (scalars / vectors) only, so these exist in hlsl
// alone. Each row goes through the vector intrinsic of the same name; an
// overload exists only where the row function does, and the result element is
// the row result's (sqrt(int3x3) is a float3x3, as in HLSL).
#define SM_HLSL_MAT_UNARY(name)                                                       \
    template <int R, int C, class T>                                                  \
        requires requires(const detail::hvec<C, T>& v) { name(v); }                   \
    SHADER_MATH_INLINE auto name(const matrix<R, C, T>& m) {                          \
        detail::hmat_of_rows<R, decltype(name(m.row_[0]))> o;                                  \
        for (int r = 0; r < R; ++r) o.row_[r] = name(m.row_[r]);                      \
        return o;                                                                     \
    }
SM_HLSL_MAT_UNARY(abs)
SM_HLSL_MAT_UNARY(sign)
SM_HLSL_MAT_UNARY(floor)
SM_HLSL_MAT_UNARY(ceil)
SM_HLSL_MAT_UNARY(trunc)
SM_HLSL_MAT_UNARY(round)
SM_HLSL_MAT_UNARY(frac)
SM_HLSL_MAT_UNARY(saturate)
SM_HLSL_MAT_UNARY(sqrt)
SM_HLSL_MAT_UNARY(rsqrt)
SM_HLSL_MAT_UNARY(rcp)
SM_HLSL_MAT_UNARY(exp)
SM_HLSL_MAT_UNARY(exp2)
SM_HLSL_MAT_UNARY(log)
SM_HLSL_MAT_UNARY(log2)
SM_HLSL_MAT_UNARY(sin)
SM_HLSL_MAT_UNARY(cos)
SM_HLSL_MAT_UNARY(tan)
SM_HLSL_MAT_UNARY(asin)
SM_HLSL_MAT_UNARY(acos)
SM_HLSL_MAT_UNARY(atan)
SM_HLSL_MAT_UNARY(sinh)
SM_HLSL_MAT_UNARY(cosh)
SM_HLSL_MAT_UNARY(tanh)
SM_HLSL_MAT_UNARY(radians)
SM_HLSL_MAT_UNARY(degrees)
#undef SM_HLSL_MAT_UNARY

// matrix (op) matrix of the same shape, and matrix (op) scalar
#define SM_HLSL_MAT_BINARY(name)                                                      \
    template <int R, int C, class T>                                                  \
        requires requires(const detail::hvec<C, T>& v) { name(v, v); }                \
    SHADER_MATH_INLINE auto name(const matrix<R, C, T>& a, const matrix<R, C, T>& b) { \
        detail::hmat_of_rows<R, decltype(name(a.row_[0], b.row_[0]))> o;                       \
        for (int r = 0; r < R; ++r) o.row_[r] = name(a.row_[r], b.row_[r]);           \
        return o;                                                                     \
    }                                                                                 \
    template <int R, int C, class T, class S>                                         \
        requires(detail::ScalarFor<S, T> &&                                           \
                 requires(const detail::hvec<C, T>& v, const S& s) { name(v, s); })   \
    SHADER_MATH_INLINE auto name(const matrix<R, C, T>& a, const S& s) {              \
        detail::hmat_of_rows<R, decltype(name(a.row_[0], s))> o;                               \
        for (int r = 0; r < R; ++r) o.row_[r] = name(a.row_[r], s);                   \
        return o;                                                                     \
    }
SM_HLSL_MAT_BINARY(min)
SM_HLSL_MAT_BINARY(max)
SM_HLSL_MAT_BINARY(pow)
SM_HLSL_MAT_BINARY(fmod)
#undef SM_HLSL_MAT_BINARY

template <int R, int C, class T>
    requires requires(const detail::hvec<C, T>& v) { atan2(v, v); }
SHADER_MATH_INLINE matrix<R, C, T> atan2(const matrix<R, C, T>& y, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = atan2(y.row_[r], x.row_[r]);
    return o;
}

// step(edge, x): edge-first, so the scalar form is (scalar, matrix)
template <int R, int C, class T>
    requires requires(const detail::hvec<C, T>& v) { step(v, v); }
SHADER_MATH_INLINE matrix<R, C, T> step(const matrix<R, C, T>& edge, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = step(edge.row_[r], x.row_[r]);
    return o;
}
template <int R, int C, class T, class S>
    requires(detail::ScalarFor<S, T> && requires(const detail::hvec<C, T>& v) { step(v, v); })
SHADER_MATH_INLINE matrix<R, C, T> step(const S& edge, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = step(edge, x.row_[r]);
    return o;
}

template <int R, int C, class T>
    requires requires(const detail::hvec<C, T>& v) { lerp(v, v, v); }
SHADER_MATH_INLINE matrix<R, C, T> lerp(const matrix<R, C, T>& a, const matrix<R, C, T>& b,
                                        const matrix<R, C, T>& t) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = lerp(a.row_[r], b.row_[r], t.row_[r]);
    return o;
}
template <int R, int C, class T, class S>
    requires(detail::ScalarFor<S, T> && requires(const detail::hvec<C, T>& v) { lerp(v, v, v); })
SHADER_MATH_INLINE matrix<R, C, T> lerp(const matrix<R, C, T>& a, const matrix<R, C, T>& b,
                                        const S& t) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = lerp(a.row_[r], b.row_[r], t);
    return o;
}

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> clamp(const matrix<R, C, T>& x, const matrix<R, C, T>& lo,
                                         const matrix<R, C, T>& hi) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = clamp(x.row_[r], lo.row_[r], hi.row_[r]);
    return o;
}
template <int R, int C, class T, class SL, class SH>
    requires(detail::ScalarFor<SL, T> && detail::ScalarFor<SH, T>)
SHADER_MATH_INLINE auto clamp(const matrix<R, C, T>& x, const SL& lo, const SH& hi) {
    detail::hmat_of_rows<R, decltype(clamp(x.row_[0], lo, hi))> o;
    for (int r = 0; r < R; ++r) o.row_[r] = clamp(x.row_[r], lo, hi);
    return o;
}

template <int R, int C, class T>
    requires requires(const detail::hvec<C, T>& v) { smoothstep(v, v, v); }
SHADER_MATH_INLINE matrix<R, C, T> smoothstep(const matrix<R, C, T>& lo,
                                              const matrix<R, C, T>& hi,
                                              const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = smoothstep(lo.row_[r], hi.row_[r], x.row_[r]);
    return o;
}
template <int R, int C, class T, class SL, class SH>
    requires(detail::ScalarFor<SL, T> && detail::ScalarFor<SH, T> &&
             requires(const detail::hvec<C, T>& v) { smoothstep(v, v, v); })
SHADER_MATH_INLINE matrix<R, C, T> smoothstep(const SL& lo, const SH& hi,
                                              const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = smoothstep(lo, hi, x.row_[r]);
    return o;
}

template <int R, int C, class T>
    requires requires(const detail::hvec<C, T>& v) { mad(v, v, v); }
SHADER_MATH_INLINE matrix<R, C, T> mad(const matrix<R, C, T>& a, const matrix<R, C, T>& b,
                                       const matrix<R, C, T>& c) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = mad(a.row_[r], b.row_[r], c.row_[r]);
    return o;
}

// classification -> boolRxC
#define SM_HLSL_MAT_CLASSIFY(name)                                                    \
    template <int R, int C, class T>                                                  \
        requires requires(const detail::hvec<C, T>& v) { name(v); }                   \
    SHADER_MATH_INLINE detail::bmat<R, C> name(const matrix<R, C, T>& m) {            \
        detail::bmat<R, C> o;                                                         \
        for (int r = 0; r < R; ++r) o.row_[r] = name(m.row_[r]);                      \
        return o;                                                                     \
    }
SM_HLSL_MAT_CLASSIFY(isnan)
SM_HLSL_MAT_CLASSIFY(isinf)
SM_HLSL_MAT_CLASSIFY(isfinite)
#undef SM_HLSL_MAT_CLASSIFY

}  // namespace hlsl

// ---- extensions: in neither language ----------------------------------------
// Only reachable qualified (shader_math::ext::inverse(m)), so shader code that
// uses them does not compile by accident.
namespace ext {

// Inverse of an hlsl matrix (HLSL has no inverse; glsl::inverse is GLSL's
// own). Singular input yields inf/NaN components (no exceptions), matching
// shader-style arithmetic. True fp16 inputs underflow/overflow much sooner
// than float; keep half matrices well conditioned.
template <int R, int C, class T>
    requires(!detail::is_int_elem_v<T>)
SHADER_MATH_INLINE hlsl::matrix<R, C, T> inverse(const hlsl::matrix<R, C, T>& m) {
    static_assert(R == C, "shader-math: inverse requires a square matrix");
    hlsl::matrix<R, C, T> o;
    detail::inv_regs<R>(m.row_, o.row_);
    return o;
}

}  // namespace ext
}  // namespace shader_math
