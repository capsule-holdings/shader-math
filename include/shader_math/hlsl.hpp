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

using float2x2 = matrix<2, 2>;
using float2x3 = matrix<2, 3>;
using float2x4 = matrix<2, 4>;
using float3x2 = matrix<3, 2>;
using float3x3 = matrix<3, 3>;
using float3x4 = matrix<3, 4>;
using float4x2 = matrix<4, 2>;
using float4x3 = matrix<4, 3>;
using float4x4 = matrix<4, 4>;

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

// ---- component-wise matrix operators ----------------------------------------
// Same-shape matrices only; `float3x4 * float4x3` does not compile (use mul).
#define SM_HLSL_MAT_BINOP(sym, fn)                                                       \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE matrix<R, C> operator sym(const matrix<R, C>& a,                  \
                                                 const matrix<R, C>& b) {                \
        matrix<R, C> o;                                                                  \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] = detail::fvec<C>(detail::ops::fn(a.row_[r].q_, b.row_[r].q_));    \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE matrix<R, C> operator sym(const matrix<R, C>& a, float s) {       \
        matrix<R, C> o;                                                                  \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] = detail::fvec<C>(detail::ops::fn(a.row_[r].q_, detail::ops::splat(s))); \
        return o;                                                                        \
    }                                                                                    \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE matrix<R, C> operator sym(float s, const matrix<R, C>& b) {       \
        matrix<R, C> o;                                                                  \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] = detail::fvec<C>(detail::ops::fn(detail::ops::splat(s), b.row_[r].q_)); \
        return o;                                                                        \
    }

SM_HLSL_MAT_BINOP(+, add)
SM_HLSL_MAT_BINOP(-, sub)
SM_HLSL_MAT_BINOP(*, mul)  // Hadamard, NOT the matrix product
SM_HLSL_MAT_BINOP(/, div)
#undef SM_HLSL_MAT_BINOP

template <int R, int C>
SHADER_MATH_INLINE matrix<R, C> operator-(const matrix<R, C>& a) {
    matrix<R, C> o;
    for (int r = 0; r < R; ++r) o.row_[r] = detail::fvec<C>(detail::ops::neg(a.row_[r].q_));
    return o;
}

#define SM_HLSL_MAT_COMPOUND(sym)                                                        \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE matrix<R, C>& operator sym##=(matrix<R, C>& a,                    \
                                                     const matrix<R, C>& b) {            \
        a = a sym b;                                                                     \
        return a;                                                                        \
    }                                                                                    \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE matrix<R, C>& operator sym##=(matrix<R, C>& a, float s) {         \
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
        return detail::bvec<n>(detail::ops::fn(detail::to_vec(a).q_, detail::to_vec(b).q_)); \
    }                                                                                    \
    template <class A>                                                                   \
        requires detail::VecLike<A>                                                      \
    SHADER_MATH_INLINE auto operator sym(const A& a, float s) {                          \
        constexpr int n = detail::vec_size_v<A>;                                         \
        return detail::bvec<n>(detail::ops::fn(detail::to_vec(a).q_, detail::ops::splat(s))); \
    }                                                                                    \
    template <class B>                                                                   \
        requires detail::VecLike<B>                                                      \
    SHADER_MATH_INLINE auto operator sym(float s, const B& b) {                          \
        constexpr int n = detail::vec_size_v<B>;                                         \
        return detail::bvec<n>(detail::ops::fn(detail::ops::splat(s), detail::to_vec(b).q_)); \
    }                                                                                    \
    template <int R, int C>                                                              \
    SHADER_MATH_INLINE detail::bmat<R, C> operator sym(const matrix<R, C>& a,            \
                                                       const matrix<R, C>& b) {          \
        detail::bmat<R, C> o;                                                            \
        for (int r = 0; r < R; ++r)                                                      \
            o.row_[r] = detail::bvec<C>(detail::ops::fn(a.row_[r].q_, b.row_[r].q_));    \
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

template <class V>
    requires detail::VecLike<V>
SHADER_MATH_INLINE auto mul(float s, const V& v) {
    return s * detail::to_vec(v);
}
template <class V>
    requires detail::VecLike<V>
SHADER_MATH_INLINE auto mul(const V& v, float s) {
    return detail::to_vec(v) * s;
}
template <int R, int C>
SHADER_MATH_INLINE matrix<R, C> mul(float s, const matrix<R, C>& m) {
    return s * m;
}
template <int R, int C>
SHADER_MATH_INLINE matrix<R, C> mul(const matrix<R, C>& m, float s) {
    return m * s;
}

// vector x vector: equivalent to dot()
template <class A, class B>
    requires detail::SameVec<A, B>
SHADER_MATH_INLINE float mul(const A& a, const B& b) {
    return detail::dot(a, b);
}

// row vector x matrix
template <class V, int M, int N2>
    requires(detail::vec_size_v<V> == M)
SHADER_MATH_INLINE detail::fvec<N2> mul(const V& v0, const matrix<M, N2>& m) {
    const detail::fvec<M> v = detail::to_vec(v0);
    detail::freg acc = detail::ops::zero();
    detail::unroll<M>([&]<int J>() {
        acc = detail::ops::fmadd(detail::ops::bcast<J>(v.q_), m.row_[J].q_, acc);
    });
    return detail::fvec<N2>(acc);
}

// matrix x column vector
template <int M, int K, class V>
    requires(detail::vec_size_v<V> == K)
SHADER_MATH_INLINE detail::fvec<M> mul(const matrix<M, K>& m, const V& v0) {
    const detail::fvec<K> v = detail::to_vec(v0);
    float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for (int r = 0; r < M; ++r) t[r] = detail::dot(m.row_[r], v);
    return detail::fvec<M>(detail::ops::load4(t));
}

// matrix x matrix: inner dimensions must match (enforced by the signature)
template <int M, int K, int N2>
SHADER_MATH_INLINE matrix<M, N2> mul(const matrix<M, K>& a, const matrix<K, N2>& b) {
    matrix<M, N2> o;
    for (int r = 0; r < M; ++r) {
        detail::freg acc = detail::ops::zero();
        detail::unroll<K>([&]<int J>() {
            acc = detail::ops::fmadd(detail::ops::bcast<J>(a.row_[r].q_), b.row_[J].q_, acc);
        });
        o.row_[r] = detail::fvec<N2>(acc);
    }
    return o;
}

// ---- transpose / determinant / inverse ---------------------------------------
template <int R, int C>
SHADER_MATH_INLINE matrix<C, R> transpose(const matrix<R, C>& m) {
    matrix<C, R> o;
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c) o.row_[c][r] = m.row_[r][c];
    return o;
}

template <int R, int C>
SHADER_MATH_INLINE float determinant(const matrix<R, C>& m) {
    static_assert(R == C, "shader-math: determinant requires a square matrix");
    float p[R * C];
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c) p[r * C + c] = m.row_[r][c];
    if constexpr (R == 2) return detail::det2(p);
    if constexpr (R == 3) return detail::det3(p);
    if constexpr (R == 4) return detail::det4(p);
}

// Extension: not an official HLSL intrinsic. Singular input yields inf/NaN
// components (no exceptions), matching shader-style arithmetic.
template <int R, int C>
SHADER_MATH_INLINE matrix<R, C> inverse(const matrix<R, C>& m) {
    static_assert(R == C, "shader-math: inverse requires a square matrix");
    float p[R * C];
    float q[R * C];
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c) p[r * C + c] = m.row_[r][c];
    if constexpr (R == 2) detail::inv2(p, q);
    if constexpr (R == 3) detail::inv3(p, q);
    if constexpr (R == 4) detail::inv4(p, q);
    matrix<R, C> o;
    for (int r = 0; r < R; ++r)
        for (int c = 0; c < C; ++c) o.row_[r][c] = q[r * C + c];
    return o;
}

}  // namespace hlsl
}  // namespace shader_math
