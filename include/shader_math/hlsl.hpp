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

}  // namespace hlsl
}  // namespace shader_math
