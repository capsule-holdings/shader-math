// HLSL language surface.
//   operator* on matrices is COMPONENT-WISE (Hadamard); mul() is the linear
//   algebra product. Comparisons return bool vectors / bool matrices which are
//   not implicitly convertible to bool: reduce with all()/any(), exactly like
//   HLSL. Import with `using namespace hlsl;` (one language per scope).
#pragma once

#include "matrix.hpp"

namespace shader_math {
namespace hlsl {

// ---- type aliases -----------------------------------------------------------
// vector<T, N> (HLSL's template spelling; vector<float, 3> is float3)
template <class T = float, int N = 4>
using vector = detail::fvec<N, T>;

using float2 = detail::fvec<2>;
using float3 = detail::fvec<3>;
using float4 = detail::fvec<4>;
using bool2 = detail::bvec<2>;
using bool3 = detail::bvec<3>;
using bool4 = detail::bvec<4>;

// Half vectors (HLSL half). Language-level packing: sizeof(half3) == 8
// (4 x binary16), just as sizeof(float3) == 16 — language compatibility, not
// a GPU ABI. Absent entirely on compilers without a half scalar type.
#if defined(SHADER_MATH_HAS_HALF)
using half = detail::half_t;
using half2 = detail::fvec<2, detail::half_t>;
using half3 = detail::fvec<3, detail::half_t>;
using half4 = detail::fvec<4, detail::half_t>;
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

// re-export shared functions so `using namespace hlsl;` provides them
using detail::all;
using detail::any;
using detail::cross;
using detail::dot;
using detail::length;
using detail::normalize;
using detail::distance;
using detail::reflect;
using detail::refract;
using detail::faceforward;

// component-wise intrinsics with identical names in both languages
using detail::abs;
using detail::acos;
using detail::asin;
using detail::atan;
using detail::cosh;
using detail::degrees;
using detail::isinf;
using detail::isnan;
using detail::modf;
using detail::radians;
using detail::sinh;
using detail::tanh;
using detail::ceil;
using detail::clamp;
using detail::cos;
using detail::exp;
using detail::exp2;
using detail::floor;
using detail::log;
using detail::log2;
using detail::max;
using detail::min;
using detail::pow;
using detail::round;
using detail::sign;
using detail::sin;
using detail::smoothstep;
using detail::sqrt;
using detail::step;
using detail::tan;
using detail::trunc;

// HLSL-only spellings (defined here, not in detail, so they never leak into
// glsl via ADL on the shared vector type)
template <class... As>
    requires requires(const As&... as) { detail::lerp_impl(as...); }
SHADER_MATH_INLINE auto lerp(const As&... as) {
    return detail::lerp_impl(as...);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto saturate(const A& a) {
    return detail::saturate_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto frac(const A& a) {
    return detail::fract_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto rcp(const A& a) {
    return detail::rcp_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto rsqrt(const A& a) {
    return detail::rsqrt_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto log10(const A& a) {
    return detail::log10_impl(a);
}
template <class A, class B>
    requires requires(const A& a, const B& b) { detail::ldexp_impl(a, b); }
SHADER_MATH_INLINE auto ldexp(const A& a, const B& b) {
    return detail::ldexp_impl(a, b);
}
template <class A, int N, class T>
    requires requires(const A& a, detail::fvec<N, T>& e) { detail::frexp_impl(a, e); }
SHADER_MATH_INLINE auto frexp(const A& a, detail::fvec<N, T>& e) {
    return detail::frexp_impl(a, e);
}
// select(cond, a, b) (HLSL 2021): a where cond, else b; scalar a / b broadcast
template <int N, class A, class B>
    requires requires(const detail::bvec<N>& c, const A& a, const B& b) {
        detail::select_impl(c, a, b);
    }
SHADER_MATH_INLINE auto select(const detail::bvec<N>& c, const A& a, const B& b) {
    return detail::select_impl(c, a, b);
}
// dst(a, b) = (1, a.y * b.y, a.z, b.w): the distance vector of the
// fixed-function lighting model (float4 only, like HLSL)
template <class A, class B>
    requires(detail::SameVec<A, B> && detail::vec_size_v<A> == 4)
SHADER_MATH_INLINE auto dst(const A& a0, const B& b0) {
    using T = detail::vec_elem_t<A>;
    const detail::fvec<4, T> a = detail::to_vec(a0);
    const detail::fvec<4, T> b = detail::to_vec(b0);
    return detail::fvec<4, T>(T(1.0f), a[1] * b[1], a[2], b[3]);
}
// lit(n_dot_l, n_dot_h, m) = (1, diffuse, specular, 1): scalar inputs, like
// HLSL; specular is 0 when either dot product is negative
template <class S0, class S1, class S2>
    requires(std::is_arithmetic_v<S0> && std::is_arithmetic_v<S1> && std::is_arithmetic_v<S2>)
SHADER_MATH_INLINE detail::fvec<4, float> lit(S0 n_dot_l, S1 n_dot_h, S2 m) {
    const float l = static_cast<float>(n_dot_l);
    const float h = static_cast<float>(n_dot_h);
    const float diffuse = l < 0.0f ? 0.0f : l;
    const float specular = (l < 0.0f || h < 0.0f) ? 0.0f : std::pow(h, static_cast<float>(m));
    return detail::fvec<4, float>(1.0f, diffuse, specular, 1.0f);
}
template <class A, class B>
    requires requires(const A& y, const B& x) { detail::atan2_impl(y, x); }
SHADER_MATH_INLINE auto atan2(const A& y, const B& x) {
    return detail::atan2_impl(y, x);
}
template <class A, class B>
    requires requires(const A& a, const B& b) { detail::fmod_impl(a, b); }
SHADER_MATH_INLINE auto fmod(const A& a, const B& b) {
    return detail::fmod_impl(a, b);
}
template <class A, class B, class C>
    requires requires(const A& a, const B& b, const C& c) { detail::mad_impl(a, b, c); }
SHADER_MATH_INLINE auto mad(const A& a, const B& b, const C& c) {
    return detail::mad_impl(a, b, c);
}
// fvec / swz parameters for the same reason as detail::isnan (MSVC UCRT)
template <int N, class T>
SHADER_MATH_INLINE detail::bvec<N> isfinite(const detail::fvec<N, T>& v) {
    return detail::isfinite_impl(v);
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
SHADER_MATH_INLINE auto isfinite(const detail::swz<T, N, Is...>& s) {
    return detail::isfinite_impl(detail::to_vec(s));
}
template <class A, int N, class T>
    requires requires(const A& a, detail::fvec<N, T>& s, detail::fvec<N, T>& c) {
        detail::sincos_impl(a, s, c);
    }
SHADER_MATH_INLINE void sincos(const A& a, detail::fvec<N, T>& s, detail::fvec<N, T>& c) {
    detail::sincos_impl(a, s, c);
}

// isfinite on scalars (by value, see detail::isnan)
template <class A>
    requires detail::ScalarCall<A>
SHADER_MATH_INLINE bool isfinite(A a) {
    return isfinite(detail::lane1<detail::scalar_call_t<A>>(a))[0];
}

// all / any on numeric vectors, matrices and scalars: HLSL tests the
// components against zero (NaN counts as nonzero); GLSL takes bool vectors
// only, so these live here and not in detail
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE bool all(const A& a) {
    return all(detail::bvec<detail::vec_size_v<A>>(a));
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE bool any(const A& a) {
    return any(detail::bvec<detail::vec_size_v<A>>(a));
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
    requires(N >= 2 && requires(const detail::bvec<N>& c, const A& a, const B& b) {
        detail::select_impl(c, a, b);
    })
SHADER_MATH_INLINE auto select(bool c, const A& a, const B& b) {
    return detail::select_impl(detail::bvec<N>(c), a, b);
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
    requires(detail::ScalarCall<A> && std::is_same_v<T, detail::scalar_call_t<A>>)
SHADER_MATH_INLINE T frexp(const A& a, T& e) {
    detail::fvec<2, T> ev;
    const T m = frexp(detail::lane1<T>(a), ev)[0];
    e = ev[0];
    return m;
}
template <class A, class T>
    requires(detail::ScalarCall<A> && std::is_same_v<T, detail::scalar_call_t<A>>)
SHADER_MATH_INLINE void sincos(const A& a, T& s, T& c) {
    detail::fvec<2, T> sv, cv;
    sincos(detail::lane1<T>(a), sv, cv);
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
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE matrix<R, C, T> operator sym(const matrix<R, C, T>& a, const S& s) { \
        matrix<R, C, T> o;                                                               \
        const auto sp = detail::vops<T>::splat(static_cast<T>(s));                       \
        detail::regs_fill<R>(o.row_,                                                     \
                             [&](int r) { return detail::vops<T>::fn(a.row_[r].q_, sp); }); \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::ScalarFor<S, T>                                                 \
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

#define SM_HLSL_MAT_COMPOUND(sym)                                                        \
    template <int R, int C, class T>                                                     \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a,              \
                                                        const matrix<R, C, T>& b) {      \
        a = a sym b;                                                                     \
        return a;                                                                        \
    }                                                                                    \
    template <int R, int C, class T, class S>                                            \
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE matrix<R, C, T>& operator sym##=(matrix<R, C, T>& a, const S& s) { \
        a = a sym s;                                                                     \
        return a;                                                                        \
    }

SM_HLSL_MAT_COMPOUND(+)
SM_HLSL_MAT_COMPOUND(-)
SM_HLSL_MAT_COMPOUND(*)
SM_HLSL_MAT_COMPOUND(/)
#undef SM_HLSL_MAT_COMPOUND

// ---- % on float vectors / matrices: fmod (trunc-based, sign of x) ----------
// HLSL defines % for floats; GLSL does not (its mod() is floor-based).
template <class A, class B>
    requires requires(const A& a, const B& b) { detail::fmod_impl(a, b); }
SHADER_MATH_INLINE auto operator%(const A& a, const B& b) {
    return detail::fmod_impl(a, b);
}
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> operator%(const matrix<R, C, T>& a, const matrix<R, C, T>& b) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = detail::fmod_impl(a.row_[r], b.row_[r]);
    return o;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<R, C, T> operator%(const matrix<R, C, T>& a, const S& s) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = detail::fmod_impl(a.row_[r], s);
    return o;
}

// ---- comparisons: per-component results (HLSL semantics) --------------------
// `if (a == b)` does not compile (bvec has no bool conversion); use all()/any().
#define SM_HLSL_VEC_CMP(sym, fn)                                                         \
    template <class A, class B>                                                          \
        requires detail::SameVec<A, B>                                                   \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                       \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n>(detail::vops<T>::fn(detail::to_vec(a).q_, detail::to_vec(b).q_)); \
    }                                                                                    \
    template <class A, class S>                                                          \
        requires(detail::VecLike<A> && detail::ScalarFor<S, detail::vec_elem_t<A>>)      \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                       \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n>(                                                          \
            detail::vops<T>::fn(detail::to_vec(a).q_, detail::vops<T>::splat(static_cast<T>(s)))); \
    }                                                                                    \
    template <class B, class S>                                                          \
        requires(detail::VecLike<B> && detail::ScalarFor<S, detail::vec_elem_t<B>>)      \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                       \
        constexpr int n = detail::vec_size_v<B>;                                         \
        using T = detail::vec_elem_t<B>;                                                 \
        return detail::bvec<n>(                                                          \
            detail::vops<T>::fn(detail::vops<T>::splat(static_cast<T>(s)), detail::to_vec(b).q_)); \
    }                                                                                    \
    template <int R, int C, class T>                                                     \
    SHADER_MATH_INLINE detail::bmat<R, C> operator sym(const matrix<R, C, T>& a,         \
                                                       const matrix<R, C, T>& b) {       \
        detail::bmat<R, C> o;                                                            \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] = detail::bvec<C>(detail::vops<T>::fn(a.row_[r].q_, b.row_[r].q_)); \
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
SHADER_MATH_INLINE detail::bvec<N> operator!(const detail::bvec<N>& a) {
    return detail::bvec<N>(detail::ops::m_not(a.m_));
}
template <int N>
SHADER_MATH_INLINE detail::bvec<N> operator==(const detail::bvec<N>& a,
                                              const detail::bvec<N>& b) {
    return detail::bvec<N>(detail::ops::m_not(detail::ops::m_xor(a.m_, b.m_)));
}
template <int N>
SHADER_MATH_INLINE detail::bvec<N> operator!=(const detail::bvec<N>& a,
                                              const detail::bvec<N>& b) {
    return detail::bvec<N>(detail::ops::m_xor(a.m_, b.m_));
}

// ---- mul: the 9 official overloads ------------------------------------------
SHADER_MATH_INLINE float mul(float a, float b) { return a * b; }
#if defined(SHADER_MATH_HAS_HALF)
SHADER_MATH_INLINE detail::half_t mul(detail::half_t a, detail::half_t b) { return a * b; }
#endif

template <class V, class S>
    requires(detail::VecLike<V> && detail::ScalarFor<S, detail::vec_elem_t<V>>)
SHADER_MATH_INLINE auto mul(const S& s, const V& v) {
    return s * detail::to_vec(v);
}
template <class V, class S>
    requires(detail::VecLike<V> && detail::ScalarFor<S, detail::vec_elem_t<V>>)
SHADER_MATH_INLINE auto mul(const V& v, const S& s) {
    return detail::to_vec(v) * s;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<R, C, T> mul(const S& s, const matrix<R, C, T>& m) {
    return s * m;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<R, C, T> mul(const matrix<R, C, T>& m, const S& s) {
    return m * s;
}

// vector x vector: equivalent to dot()
template <class A, class B>
    requires detail::SameVec<A, B>
SHADER_MATH_INLINE detail::vec_elem_t<A> mul(const A& a, const B& b) {
    return detail::dot(a, b);
}

// row vector x matrix
template <class V, int M, int N2, class T>
    requires(detail::vec_size_v<V> == M && std::is_same_v<detail::vec_elem_t<V>, T>)
SHADER_MATH_INLINE detail::fvec<N2, T> mul(const V& v0, const matrix<M, N2, T>& m) {
    const detail::fvec<M, T> v = detail::to_vec(v0);
    return detail::fvec<N2, T>(detail::mad_accum<M>(v.q_, m.row_));
}

// matrix x column vector
template <int M, int K, class T, class V>
    requires(detail::vec_size_v<V> == K && std::is_same_v<detail::vec_elem_t<V>, T>)
SHADER_MATH_INLINE detail::fvec<M, T> mul(const matrix<M, K, T>& m, const V& v0) {
    const detail::fvec<K, T> v = detail::to_vec(v0);
    T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int r = 0; r < M; ++r) t[r] = detail::dot(m.row_[r], v);
    return detail::fvec<M, T>(detail::vops<T>::load4(t));
}

// matrix x matrix: inner dimensions must match (enforced by the signature)
template <int M, int K, int N2, class T>
SHADER_MATH_INLINE matrix<M, N2, T> mul(const matrix<M, K, T>& a, const matrix<K, N2, T>& b) {
    matrix<M, N2, T> o;
    for (int r = 0; r < M; ++r)
        o.row_[r] = detail::fvec<N2, T>(detail::mad_accum<K>(a.row_[r].q_, b.row_));
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
SHADER_MATH_INLINE T determinant(const matrix<R, C, T>& m) {
    static_assert(R == C, "shader-math: determinant requires a square matrix");
    return detail::det_regs<R>(m.row_);
}

// Extension: not an official HLSL intrinsic. Singular input yields inf/NaN
// components (no exceptions), matching shader-style arithmetic. True fp16
// inputs underflow/overflow much sooner than float; keep half matrices well
// conditioned.
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> inverse(const matrix<R, C, T>& m) {
    static_assert(R == C, "shader-math: inverse requires a square matrix");
    matrix<R, C, T> o;
    detail::inv_regs<R>(m.row_, o.row_);
    return o;
}

// ---- component-wise intrinsics on matrices ----------------------------------
// HLSL applies its component-wise intrinsics to matrices too; GLSL's generic
// functions take genType (scalars / vectors) only, so these exist in hlsl
// alone. Each row goes through the vector intrinsic of the same name.
#define SM_HLSL_MAT_UNARY(name)                                                       \
    template <int R, int C, class T>                                                  \
    SHADER_MATH_INLINE matrix<R, C, T> name(const matrix<R, C, T>& m) {               \
        matrix<R, C, T> o;                                                            \
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
    SHADER_MATH_INLINE matrix<R, C, T> name(const matrix<R, C, T>& a,                 \
                                            const matrix<R, C, T>& b) {               \
        matrix<R, C, T> o;                                                            \
        for (int r = 0; r < R; ++r) o.row_[r] = name(a.row_[r], b.row_[r]);           \
        return o;                                                                     \
    }                                                                                 \
    template <int R, int C, class T, class S>                                         \
        requires detail::ScalarFor<S, T>                                              \
    SHADER_MATH_INLINE matrix<R, C, T> name(const matrix<R, C, T>& a, const S& s) {   \
        matrix<R, C, T> o;                                                            \
        for (int r = 0; r < R; ++r) o.row_[r] = name(a.row_[r], s);                   \
        return o;                                                                     \
    }
SM_HLSL_MAT_BINARY(min)
SM_HLSL_MAT_BINARY(max)
SM_HLSL_MAT_BINARY(pow)
SM_HLSL_MAT_BINARY(fmod)
#undef SM_HLSL_MAT_BINARY

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> atan2(const matrix<R, C, T>& y, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = atan2(y.row_[r], x.row_[r]);
    return o;
}

// step(edge, x): edge-first, so the scalar form is (scalar, matrix)
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> step(const matrix<R, C, T>& edge, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = step(edge.row_[r], x.row_[r]);
    return o;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<R, C, T> step(const S& edge, const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = step(edge, x.row_[r]);
    return o;
}

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> lerp(const matrix<R, C, T>& a, const matrix<R, C, T>& b,
                                        const matrix<R, C, T>& t) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = lerp(a.row_[r], b.row_[r], t.row_[r]);
    return o;
}
template <int R, int C, class T, class S>
    requires detail::ScalarFor<S, T>
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
SHADER_MATH_INLINE matrix<R, C, T> clamp(const matrix<R, C, T>& x, const SL& lo, const SH& hi) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = clamp(x.row_[r], lo, hi);
    return o;
}

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> smoothstep(const matrix<R, C, T>& lo,
                                              const matrix<R, C, T>& hi,
                                              const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = smoothstep(lo.row_[r], hi.row_[r], x.row_[r]);
    return o;
}
template <int R, int C, class T, class SL, class SH>
    requires(detail::ScalarFor<SL, T> && detail::ScalarFor<SH, T>)
SHADER_MATH_INLINE matrix<R, C, T> smoothstep(const SL& lo, const SH& hi,
                                              const matrix<R, C, T>& x) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = smoothstep(lo, hi, x.row_[r]);
    return o;
}

template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T> mad(const matrix<R, C, T>& a, const matrix<R, C, T>& b,
                                       const matrix<R, C, T>& c) {
    matrix<R, C, T> o;
    for (int r = 0; r < R; ++r) o.row_[r] = mad(a.row_[r], b.row_[r], c.row_[r]);
    return o;
}

// classification -> boolRxC
#define SM_HLSL_MAT_CLASSIFY(name)                                                    \
    template <int R, int C, class T>                                                  \
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
}  // namespace shader_math
