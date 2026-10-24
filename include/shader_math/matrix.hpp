// Matrix types.
//   hlsl::matrix<R,C,T> : R rows x C cols, row-register storage, row-major ctor fill.
//   glsl::matrix<C,R,T> : C cols x R rows, column-register storage, column-major fill.
// The two are distinct types (operator* differs); conversion is explicit.
// hlsl floatRxC corresponds to glsl matCxR (same mathematical object); the
// same shape correspondence holds for the half aliases (halfRxC / f16matCxR).
#pragma once

#include "vector.hpp"

namespace shader_math {

namespace glsl {
template <int Cn, int Rn, class T = float>
struct matrix;
}

namespace detail {

// Single-element accessor proxy for HLSL _mRC / _RC members.
// Declared for all 16 positions on every matrix; out-of-range positions fail
// at the point of use.
template <class T, int R, int C, int RI, int CI>
struct SHADER_MATH_MAY_ALIAS mswz {
    static constexpr bool valid = (RI >= 0 && RI < R && CI >= 0 && CI < C);
    T d[R * 4];

    // Trivial copy (a proxy passed by value, e.g. float2(m._11, m._22)) is
    // declared explicitly because the copy assignment below is user-defined.
    mswz() = default;
    mswz(const mswz&) = default;

    SHADER_MATH_INLINE operator T() const {
        static_assert(valid, "shader-math: matrix element accessor out of range for this matrix size");
        return d[RI * 4 + CI];
    }
    template <class S>
        requires(valid && CvtScalar<S, T>)
    SHADER_MATH_INLINE mswz& operator=(const S& s) {
        d[RI * 4 + CI] = static_cast<T>(s);
        return *this;
    }
    SHADER_MATH_INLINE mswz& operator=(const mswz& o)
        requires(valid)
    {
        d[RI * 4 + CI] = o.d[RI * 4 + CI];
        return *this;
    }
    template <class T2, int R2, int C2, int RJ, int CJ>
    SHADER_MATH_INLINE mswz& operator=(const mswz<T2, R2, C2, RJ, CJ>& o)
        requires(valid && mswz<T2, R2, C2, RJ, CJ>::valid && std::is_same_v<T2, T>)
    {
        d[RI * 4 + CI] = static_cast<T>(o);
        return *this;
    }
#define SM_MSWZ_COMPOUND(sym)                                \
    template <class S>                                       \
        requires(valid && CvtScalar<S, T>)                   \
    SHADER_MATH_INLINE mswz& operator sym##=(const S& s) {   \
        d[RI * 4 + CI] sym## = static_cast<T>(s);            \
        return *this;                                        \
    }
    SM_MSWZ_COMPOUND(+)
    SM_MSWZ_COMPOUND(-)
    SM_MSWZ_COMPOUND(*)
    SM_MSWZ_COMPOUND(/)
#undef SM_MSWZ_COMPOUND
};

// Members shared by hlsl::matrix and, under SHADER_MATH_MATRIX_SWIZZLE, its
// storage base: the row registers, a flat view, and the 16 single-element
// accessors (declared for every shape; out-of-range ones fail where used).
#define SM_HMAT_COMMON(R, C)                                                         \
    ::shader_math::detail::fvec<C, T> row_[R];                                       \
    T flat_[R * 4];                                                                  \
    /* zero-based element accessors */                                               \
    ::shader_math::detail::mswz<T, R, C, 0, 0> _m00;                                 \
    ::shader_math::detail::mswz<T, R, C, 0, 1> _m01;                                 \
    ::shader_math::detail::mswz<T, R, C, 0, 2> _m02;                                 \
    ::shader_math::detail::mswz<T, R, C, 0, 3> _m03;                                 \
    ::shader_math::detail::mswz<T, R, C, 1, 0> _m10;                                 \
    ::shader_math::detail::mswz<T, R, C, 1, 1> _m11;                                 \
    ::shader_math::detail::mswz<T, R, C, 1, 2> _m12;                                 \
    ::shader_math::detail::mswz<T, R, C, 1, 3> _m13;                                 \
    ::shader_math::detail::mswz<T, R, C, 2, 0> _m20;                                 \
    ::shader_math::detail::mswz<T, R, C, 2, 1> _m21;                                 \
    ::shader_math::detail::mswz<T, R, C, 2, 2> _m22;                                 \
    ::shader_math::detail::mswz<T, R, C, 2, 3> _m23;                                 \
    ::shader_math::detail::mswz<T, R, C, 3, 0> _m30;                                 \
    ::shader_math::detail::mswz<T, R, C, 3, 1> _m31;                                 \
    ::shader_math::detail::mswz<T, R, C, 3, 2> _m32;                                 \
    ::shader_math::detail::mswz<T, R, C, 3, 3> _m33;                                 \
    /* one-based element accessors */                                                \
    ::shader_math::detail::mswz<T, R, C, 0, 0> _11;                                  \
    ::shader_math::detail::mswz<T, R, C, 0, 1> _12;                                  \
    ::shader_math::detail::mswz<T, R, C, 0, 2> _13;                                  \
    ::shader_math::detail::mswz<T, R, C, 0, 3> _14;                                  \
    ::shader_math::detail::mswz<T, R, C, 1, 0> _21;                                  \
    ::shader_math::detail::mswz<T, R, C, 1, 1> _22;                                  \
    ::shader_math::detail::mswz<T, R, C, 1, 2> _23;                                  \
    ::shader_math::detail::mswz<T, R, C, 1, 3> _24;                                  \
    ::shader_math::detail::mswz<T, R, C, 2, 0> _31;                                  \
    ::shader_math::detail::mswz<T, R, C, 2, 1> _32;                                  \
    ::shader_math::detail::mswz<T, R, C, 2, 2> _33;                                  \
    ::shader_math::detail::mswz<T, R, C, 2, 3> _34;                                  \
    ::shader_math::detail::mswz<T, R, C, 3, 0> _41;                                  \
    ::shader_math::detail::mswz<T, R, C, 3, 1> _42;                                  \
    ::shader_math::detail::mswz<T, R, C, 3, 2> _43;                                  \
    ::shader_math::detail::mswz<T, R, C, 3, 3> _44;

#if defined(SHADER_MATH_MATRIX_SWIZZLE3) && !defined(SHADER_MATH_MATRIX_SWIZZLE)
#  define SHADER_MATH_MATRIX_SWIZZLE
#endif
#if defined(SHADER_MATH_MATRIX_SWIZZLE)
// ---- multi-element accessors (_m00_m11, _11_22_33), opt-in -----------------
// HLSL reads any list of 2 to 4 elements and writes one without repeats.
// Here the lists are opt-in and limited to the matrix's own shape:
// SHADER_MATH_MATRIX_SWIZZLE declares the 2-element lists (a 4x4 gets
// 2 x 16^2 members), SHADER_MATH_MATRIX_SWIZZLE3 adds the 3-element ones
// (2 x 16^3 more, an order of magnitude more front-end time on gcc).
// 4-element lists would multiply that by 16 again; build those vectors from
// rows or single elements (float4(m._m00, m._m11, m._m22, m._m33)).

// F = flat register offsets (row * 4 + column) of the selected elements.
template <class T, int R, int... F>
struct SHADER_MATH_MAY_ALIAS mmswz {
    static constexpr int K = static_cast<int>(sizeof...(F));
    static constexpr bool writable = swz_unique<F...>();
    T d[R * 4];

    mmswz() = default;
    mmswz(const mmswz&) = default;

    SHADER_MATH_INLINE operator fvec<K, T>() const { return fvec<K, T>(d[F]...); }

    SHADER_MATH_INLINE mmswz& operator=(const fvec<K, T>& v)
        requires(writable)
    {
        int k = 0;
        ((d[F] = v[k++]), ...);
        return *this;
    }
    SHADER_MATH_INLINE mmswz& operator=(const mmswz& o)
        requires(writable)
    {
        return *this = fvec<K, T>(o);
    }
    template <class V>
        requires(writable && !std::is_same_v<V, mmswz> && vec_traits<V>::size == K &&
                 std::is_same_v<typename vec_traits<V>::elem, T>)
    SHADER_MATH_INLINE mmswz& operator=(const V& o) {
        return *this = fvec<K, T>(vec_traits<V>::get(o));  // gather first: m._m00_m01 = m._m01_m00
    }

#  define SM_MMSWZ_COMPOUND(sym)                             \
    template <class V>                                       \
    SHADER_MATH_INLINE mmswz& operator sym##=(const V& v)    \
        requires(writable)                                   \
    {                                                        \
        *this = fvec<K, T>(*this) sym v;                     \
        return *this;                                        \
    }
    SM_MMSWZ_COMPOUND(+)
    SM_MMSWZ_COMPOUND(-)
    SM_MMSWZ_COMPOUND(*)
    SM_MMSWZ_COMPOUND(/)
#  undef SM_MMSWZ_COMPOUND
};

template <class T, int R, int... F>
struct vec_traits<mmswz<T, R, F...>> {
    static constexpr int size = static_cast<int>(sizeof...(F));
    using elem = T;
    static SHADER_MATH_INLINE fvec<size, T> get(const mmswz<T, R, F...>& s) {
        return fvec<size, T>(s);
    }
};

// Member generation: three nested loops over the shape's positions. A
// position is four tokens (zero-based row and column, one-based row and
// column); each level has its own loop macros because a macro does not
// expand inside its own expansion.
#  define SM_MX_RA2(R, C, F) SM_MX_CA##C(R, C, F, 0, 1) SM_MX_CA##C(R, C, F, 1, 2)
#  define SM_MX_RA3(R, C, F) SM_MX_RA2(R, C, F) SM_MX_CA##C(R, C, F, 2, 3)
#  define SM_MX_RA4(R, C, F) SM_MX_RA3(R, C, F) SM_MX_CA##C(R, C, F, 3, 4)
#  define SM_MX_CA2(R, C, F, r, s) F(R, C, r, 0, s, 1) F(R, C, r, 1, s, 2)
#  define SM_MX_CA3(R, C, F, r, s) SM_MX_CA2(R, C, F, r, s) F(R, C, r, 2, s, 3)
#  define SM_MX_CA4(R, C, F, r, s) SM_MX_CA3(R, C, F, r, s) F(R, C, r, 3, s, 4)

#  define SM_MX_RB2(R, C, F, a, b, a1, b1) \
      SM_MX_CB##C(R, C, F, a, b, a1, b1, 0, 1) SM_MX_CB##C(R, C, F, a, b, a1, b1, 1, 2)
#  define SM_MX_RB3(R, C, F, a, b, a1, b1) \
      SM_MX_RB2(R, C, F, a, b, a1, b1) SM_MX_CB##C(R, C, F, a, b, a1, b1, 2, 3)
#  define SM_MX_RB4(R, C, F, a, b, a1, b1) \
      SM_MX_RB3(R, C, F, a, b, a1, b1) SM_MX_CB##C(R, C, F, a, b, a1, b1, 3, 4)
#  define SM_MX_CB2(R, C, F, a, b, a1, b1, r, s) \
      F(R, C, a, b, a1, b1, r, 0, s, 1) F(R, C, a, b, a1, b1, r, 1, s, 2)
#  define SM_MX_CB3(R, C, F, a, b, a1, b1, r, s) \
      SM_MX_CB2(R, C, F, a, b, a1, b1, r, s) F(R, C, a, b, a1, b1, r, 2, s, 3)
#  define SM_MX_CB4(R, C, F, a, b, a1, b1, r, s) \
      SM_MX_CB3(R, C, F, a, b, a1, b1, r, s) F(R, C, a, b, a1, b1, r, 3, s, 4)

#  define SM_MX_RC2(R, C, F, a, b, a1, b1, e, f, e1, f1)        \
      SM_MX_CC##C(R, C, F, a, b, a1, b1, e, f, e1, f1, 0, 1)    \
      SM_MX_CC##C(R, C, F, a, b, a1, b1, e, f, e1, f1, 1, 2)
#  define SM_MX_RC3(R, C, F, a, b, a1, b1, e, f, e1, f1)        \
      SM_MX_RC2(R, C, F, a, b, a1, b1, e, f, e1, f1)            \
      SM_MX_CC##C(R, C, F, a, b, a1, b1, e, f, e1, f1, 2, 3)
#  define SM_MX_RC4(R, C, F, a, b, a1, b1, e, f, e1, f1)        \
      SM_MX_RC3(R, C, F, a, b, a1, b1, e, f, e1, f1)            \
      SM_MX_CC##C(R, C, F, a, b, a1, b1, e, f, e1, f1, 3, 4)
#  define SM_MX_CC2(R, C, F, a, b, a1, b1, e, f, e1, f1, r, s)  \
      F(R, C, a, b, a1, b1, e, f, e1, f1, r, 0, s, 1)           \
      F(R, C, a, b, a1, b1, e, f, e1, f1, r, 1, s, 2)
#  define SM_MX_CC3(R, C, F, a, b, a1, b1, e, f, e1, f1, r, s)  \
      SM_MX_CC2(R, C, F, a, b, a1, b1, e, f, e1, f1, r, s)      \
      F(R, C, a, b, a1, b1, e, f, e1, f1, r, 2, s, 3)
#  define SM_MX_CC4(R, C, F, a, b, a1, b1, e, f, e1, f1, r, s)  \
      SM_MX_CC3(R, C, F, a, b, a1, b1, e, f, e1, f1, r, s)      \
      F(R, C, a, b, a1, b1, e, f, e1, f1, r, 3, s, 4)

// level 1 picks the first element, level 2 declares the pairs (and with
// SHADER_MATH_MATRIX_SWIZZLE3 recurses), level 3 declares the triples; each
// list in both spellings
#  if defined(SHADER_MATH_MATRIX_SWIZZLE3)
#    define SM_MX_TRIPLES(R, C, a, b, a1, b1, e, f, e1, f1) \
        SM_MX_RC##R(R, C, SM_MX_AT3, a, b, a1, b1, e, f, e1, f1)
#  else
#    define SM_MX_TRIPLES(R, C, a, b, a1, b1, e, f, e1, f1)
#  endif
// gcc's front-end time grows faster than linearly with the member count of
// one class scope, so on gcc each first element gets its own nested
// anonymous union (thousands of members per scope become hundreds). Nested
// anonymous unions are an extension: gcc accepts them silently, clang warns
// under -Wpedantic and gets slower, so other compilers keep one flat union.
#  if defined(__GNUC__) && !defined(__clang__)
#    define SM_MX_GROUP(...) union { __VA_ARGS__ };
#  else
#    define SM_MX_GROUP(...) __VA_ARGS__
#  endif
#  define SM_MX_AT1(R, C, a, b, a1, b1) SM_MX_GROUP(SM_MX_RB##R(R, C, SM_MX_AT2, a, b, a1, b1))
#  define SM_MX_AT2(R, C, a, b, a1, b1, e, f, e1, f1)                          \
      ::shader_math::detail::mmswz<T, R, a * 4 + b, e * 4 + f>                 \
          _m##a##b##_m##e##f, _##a1##b1##_##e1##f1;                            \
      SM_MX_TRIPLES(R, C, a, b, a1, b1, e, f, e1, f1)
#  define SM_MX_AT3(R, C, a, b, a1, b1, e, f, e1, f1, g, h, g1, h1)            \
      ::shader_math::detail::mmswz<T, R, a * 4 + b, e * 4 + f, g * 4 + h>      \
          _m##a##b##_m##e##f##_m##g##h, _##a1##b1##_##e1##f1##_##g1##h1;
#  define SM_MX_MULTI(R, C) SM_MX_RA##R(R, C, SM_MX_AT1)

// Storage of hlsl::matrix<R, C, T> with the multi-element members of its
// shape (the base keeps the union; the matrix names row_ / flat_ from it).
template <int R, int C, class T>
struct hmat_store;
#  define SM_MX_STORE(R, C)                                \
      template <class T>                                   \
      struct SHADER_MATH_MAY_ALIAS hmat_store<R, C, T> {   \
          union {                                          \
              SM_HMAT_COMMON(R, C)                         \
              SM_MX_MULTI(R, C)                            \
          };                                               \
          SHADER_MATH_INLINE hmat_store() : row_{} {}      \
      };
SM_MX_STORE(2, 2)
SM_MX_STORE(2, 3)
SM_MX_STORE(2, 4)
SM_MX_STORE(3, 2)
SM_MX_STORE(3, 3)
SM_MX_STORE(3, 4)
SM_MX_STORE(4, 2)
SM_MX_STORE(4, 3)
SM_MX_STORE(4, 4)
#  undef SM_MX_STORE
#  undef SM_MX_MULTI
#  undef SM_MX_AT1
#  undef SM_MX_GROUP
#  undef SM_MX_AT2
#  undef SM_MX_AT3
#  undef SM_MX_TRIPLES
#  undef SM_MX_RA2
#  undef SM_MX_RA3
#  undef SM_MX_RA4
#  undef SM_MX_CA2
#  undef SM_MX_CA3
#  undef SM_MX_CA4
#  undef SM_MX_RB2
#  undef SM_MX_RB3
#  undef SM_MX_RB4
#  undef SM_MX_CB2
#  undef SM_MX_CB3
#  undef SM_MX_CB4
#  undef SM_MX_RC2
#  undef SM_MX_RC3
#  undef SM_MX_RC4
#  undef SM_MX_CC2
#  undef SM_MX_CC3
#  undef SM_MX_CC4
#endif

// ---- determinant / inverse cores on packed dense row-major arrays ----------
// Templated on the element type; every operation rounds in T (true fp16 on
// FEAT_FP16 halves, promoted-per-op elsewhere).
template <class T>
SHADER_MATH_INLINE T det2(const T* m) {
    return m[0] * m[3] - m[1] * m[2];
}

template <class T>
SHADER_MATH_INLINE T det3(const T* m) {
    return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) +
           m[2] * (m[3] * m[7] - m[4] * m[6]);
}

template <class T>
SHADER_MATH_INLINE T det4(const T* m) {
    const T s0 = m[0] * m[5] - m[1] * m[4];
    const T s1 = m[0] * m[6] - m[2] * m[4];
    const T s2 = m[0] * m[7] - m[3] * m[4];
    const T s3 = m[1] * m[6] - m[2] * m[5];
    const T s4 = m[1] * m[7] - m[3] * m[5];
    const T s5 = m[2] * m[7] - m[3] * m[6];
    const T c5 = m[10] * m[15] - m[11] * m[14];
    const T c4 = m[9] * m[15] - m[11] * m[13];
    const T c3 = m[9] * m[14] - m[10] * m[13];
    const T c2 = m[8] * m[15] - m[11] * m[12];
    const T c1 = m[8] * m[14] - m[10] * m[12];
    const T c0 = m[8] * m[13] - m[9] * m[12];
    return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

template <class T>
SHADER_MATH_INLINE void inv2(const T* m, T* o) {
    const T id = T(1.0f) / det2(m);
    o[0] = m[3] * id;
    o[1] = -m[1] * id;
    o[2] = -m[2] * id;
    o[3] = m[0] * id;
}

template <class T>
SHADER_MATH_INLINE void inv3(const T* m, T* o) {
    const T id = T(1.0f) / det3(m);
    o[0] = (m[4] * m[8] - m[5] * m[7]) * id;
    o[1] = (m[2] * m[7] - m[1] * m[8]) * id;
    o[2] = (m[1] * m[5] - m[2] * m[4]) * id;
    o[3] = (m[5] * m[6] - m[3] * m[8]) * id;
    o[4] = (m[0] * m[8] - m[2] * m[6]) * id;
    o[5] = (m[2] * m[3] - m[0] * m[5]) * id;
    o[6] = (m[3] * m[7] - m[4] * m[6]) * id;
    o[7] = (m[1] * m[6] - m[0] * m[7]) * id;
    o[8] = (m[0] * m[4] - m[1] * m[3]) * id;
}

template <class T>
SHADER_MATH_INLINE void inv4(const T* m, T* o) {
    const T s0 = m[0] * m[5] - m[1] * m[4];
    const T s1 = m[0] * m[6] - m[2] * m[4];
    const T s2 = m[0] * m[7] - m[3] * m[4];
    const T s3 = m[1] * m[6] - m[2] * m[5];
    const T s4 = m[1] * m[7] - m[3] * m[5];
    const T s5 = m[2] * m[7] - m[3] * m[6];
    const T c5 = m[10] * m[15] - m[11] * m[14];
    const T c4 = m[9] * m[15] - m[11] * m[13];
    const T c3 = m[9] * m[14] - m[10] * m[13];
    const T c2 = m[8] * m[15] - m[11] * m[12];
    const T c1 = m[8] * m[14] - m[10] * m[12];
    const T c0 = m[8] * m[13] - m[9] * m[12];
    const T id = T(1.0f) / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

    o[0] = (m[5] * c5 - m[6] * c4 + m[7] * c3) * id;
    o[1] = (-m[1] * c5 + m[2] * c4 - m[3] * c3) * id;
    o[2] = (m[13] * s5 - m[14] * s4 + m[15] * s3) * id;
    o[3] = (-m[9] * s5 + m[10] * s4 - m[11] * s3) * id;
    o[4] = (-m[4] * c5 + m[6] * c2 - m[7] * c1) * id;
    o[5] = (m[0] * c5 - m[2] * c2 + m[3] * c1) * id;
    o[6] = (-m[12] * s5 + m[14] * s2 - m[15] * s1) * id;
    o[7] = (m[8] * s5 - m[10] * s2 + m[11] * s1) * id;
    o[8] = (m[4] * c4 - m[5] * c2 + m[7] * c0) * id;
    o[9] = (-m[0] * c4 + m[1] * c2 - m[3] * c0) * id;
    o[10] = (m[12] * s4 - m[13] * s2 + m[15] * s0) * id;
    o[11] = (-m[8] * s4 + m[9] * s2 - m[11] * s0) * id;
    o[12] = (-m[4] * c3 + m[5] * c1 - m[6] * c0) * id;
    o[13] = (m[0] * c3 - m[1] * c1 + m[2] * c0) * id;
    o[14] = (-m[12] * s3 + m[13] * s1 - m[14] * s0) * id;
    o[15] = (m[8] * s3 - m[9] * s1 + m[10] * s0) * id;
}

// ---- register-array cores shared by both language surfaces -----------------
// hlsl::matrix stores rows, glsl::matrix stores columns, but both are plain
// arrays of fvec<L, T>. Everything below is storage-direction agnostic; the
// mathematical correctness of reusing the row-major det/inv cores for the
// column-major pack is det(A) == det(A^T) and inv(A^T) == inv(A)^T.

// acc = sum_J bcast<J>(coeff) * regs[J]   (the matrix-product accumulator)
template <int K, int L, class T>
SHADER_MATH_INLINE vreg<T> mad_accum(vreg<T> coeff, const fvec<L, T>* regs) {
    vreg<T> acc = vops<T>::zero();
    unroll<K>([&]<int J>() {
        acc = vops<T>::fmadd(vops<T>::template bcast<J>(coeff), regs[J].q_, acc);
    });
    return acc;
}

// o[i] = fvec(f(i)) for i in [0, N): the component-wise matrix loop shared by
// both language surfaces (binary/scalar operators, negation, matrixCompMult).
// f receives the register index and returns a vreg<T>.
template <int N, int L, class T, class F>
SHADER_MATH_INLINE void regs_fill(fvec<L, T>* o, F&& f) {
    for (int i = 0; i < N; ++i) o[i] = fvec<L, T>(f(i));
}

// N registers of L lanes <-> dense array p[i * L + j] = regs[i][j].
// On NEON, 4-lane rows are read out as whole registers (store4 into the dense
// array): gcc's lane-by-lane reads made the 4x4 inverse slower than the
// scalar backend there, while on x86 the lane reads measured faster.
// Writes stay lane by lane through comp_: a vector store read back as float
// elements is the union type-punning CONTEXT.md describes, and gcc on
// AArch64 (float32x4_t is not may_alias) dropped such a store at -O2.
template <int N, int L, class T>
SHADER_MATH_INLINE void pack_dense(const fvec<L, T>* regs, T* p) {
    for (int i = 0; i < N; ++i) {
#if defined(SHADER_MATH_ISA_NEON)
        constexpr bool whole_rows = (L == 4);
#else
        constexpr bool whole_rows = false;
#endif
        if constexpr (whole_rows) {
            vops<T>::store4(regs[i].q_, p + i * 4);
        } else {
            for (int j = 0; j < L; ++j) p[i * L + j] = regs[i][j];
        }
    }
}
template <int N, int L, class T>
SHADER_MATH_INLINE void unpack_dense(const T* q, fvec<L, T>* regs) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < L; ++j) regs[i][j] = q[i * L + j];
}

template <int N, class T>
SHADER_MATH_INLINE T det_regs(const fvec<N, T>* regs) {
    T p[N * N];
    pack_dense<N, N>(regs, p);
    if constexpr (N == 2) return det2(p);
    if constexpr (N == 3) return det3(p);
    if constexpr (N == 4) return det4(p);
}

template <int N, class T>
SHADER_MATH_INLINE void inv_regs(const fvec<N, T>* in, fvec<N, T>* out) {
    T p[N * N];
    T q[N * N];
    pack_dense<N, N>(in, p);
    if constexpr (N == 2) inv2(p, q);
    if constexpr (N == 3) inv3(p, q);
    if constexpr (N == 4) inv4(p, q);
    unpack_dense<N, N>(q, out);
}

// out[j][i] = in[i][j]  (in: NI regs x LI lanes, out: LI regs x NI lanes)
template <int NI, int LI, class T>
SHADER_MATH_INLINE void transpose_regs(const fvec<LI, T>* in, fvec<NI, T>* out) {
    for (int i = 0; i < NI; ++i)
        for (int j = 0; j < LI; ++j) out[j][i] = in[i][j];
}

}  // namespace detail

// ============================ hlsl::matrix ==================================
namespace hlsl {

template <int R, int C, class T = float>
struct matrix
#if defined(SHADER_MATH_MATRIX_SWIZZLE)
    : detail::hmat_store<R, C, T>
#endif
{
    static_assert(R >= 2 && R <= 4 && C >= 2 && C <= 4,
                  "shader-math: matrix dimensions must be 2..4");
    static constexpr int rows_v = R;
    static constexpr int cols_v = C;
    using elem_t = T;

#if defined(SHADER_MATH_MATRIX_SWIZZLE)
    using detail::hmat_store<R, C, T>::row_;
    using detail::hmat_store<R, C, T>::flat_;
#  define SM_HMAT_INIT
#else
    union {
        SM_HMAT_COMMON(R, C)
    };
#  define SM_HMAT_INIT : row_{}
#endif

    SHADER_MATH_INLINE matrix() SM_HMAT_INIT {}
    SHADER_MATH_INLINE matrix(const matrix& o) SM_HMAT_INIT {
        for (int r = 0; r < R; ++r) row_[r] = o.row_[r];
    }
    SHADER_MATH_INLINE matrix& operator=(const matrix& o) {
        for (int r = 0; r < R; ++r) row_[r] = o.row_[r];
        return *this;
    }

    // HLSL: scalar constructs a matrix with ALL components set to s (splat).
    template <class S>
        requires detail::CvtScalar<S, T>
    SHADER_MATH_INLINE matrix(const S& s) SM_HMAT_INIT {
        for (int r = 0; r < R; ++r) row_[r] = detail::fvec<C, T>(static_cast<T>(s));
    }

    // R*C scalars, row-major fill (HLSL constructor order).
    template <class... S>
        requires(sizeof...(S) == R * C && R * C > 1 && (detail::CvtScalar<S, T> && ...))
    SHADER_MATH_INLINE matrix(S... s) SM_HMAT_INIT {
        const T a[R * C] = {static_cast<T>(s)...};
        detail::unpack_dense<R, C>(a, row_);
    }

    // R row vectors.
    template <class... V>
        requires(sizeof...(V) == R && (std::is_convertible_v<V, detail::fvec<C, T>> && ...))
    SHADER_MATH_INLINE matrix(const V&... rows) SM_HMAT_INIT {
        const detail::fvec<C, T> rr[R] = {detail::fvec<C, T>(rows)...};
        for (int r = 0; r < R; ++r) row_[r] = rr[r];
    }

    // HLSL cast-style shrink from a bigger matrix (upper-left cut). Growth is
    // not provided (HLSL has no widening matrix cast).
    template <int R2, int C2>
        requires(R2 >= R && C2 >= C && !(R2 == R && C2 == C))
    SHADER_MATH_INLINE explicit matrix(const matrix<R2, C2, T>& o) SM_HMAT_INIT {
        for (int r = 0; r < R; ++r) {
            T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
            for (int c = 0; c < C; ++c) t[c] = o.row_[r][c];
            row_[r] = detail::fvec<C, T>(detail::vops<T>::load4(t));
        }
    }

    // Explicit element-type conversion (half <-> float); no implicit mixing.
    template <class U>
        requires(!std::is_same_v<U, T>)
    SHADER_MATH_INLINE explicit matrix(const matrix<R, C, U>& o) SM_HMAT_INIT {
        for (int r = 0; r < R; ++r) row_[r] = detail::fvec<C, T>(o.row_[r]);
    }

    // Explicit conversion from the corresponding GLSL matrix (same math object).
    SHADER_MATH_INLINE explicit matrix(const glsl::matrix<C, R, T>& g);

    // Extension (not an HLSL intrinsic): identity for convenience.
    static SHADER_MATH_INLINE matrix identity() {
        matrix m;
        constexpr int d = R < C ? R : C;
        for (int i = 0; i < d; ++i) m.row_[i][i] = T(1.0f);
        return m;
    }

    SHADER_MATH_INLINE detail::fvec<C, T>& operator[](int r) { return row_[r]; }
    SHADER_MATH_INLINE const detail::fvec<C, T>& operator[](int r) const { return row_[r]; }
};

}  // namespace hlsl

// ============================ glsl::matrix ==================================
namespace glsl {

// Cn columns x Rn rows. glsl::matCxR. Column-register storage.
template <int Cn, int Rn, class T>
struct matrix {
    static_assert(Cn >= 2 && Cn <= 4 && Rn >= 2 && Rn <= 4,
                  "shader-math: matrix dimensions must be 2..4");
    static constexpr int cols_v = Cn;
    static constexpr int rows_v = Rn;
    using elem_t = T;

    detail::fvec<Rn, T> col_[Cn];

    SHADER_MATH_INLINE matrix() = default;

    // GLSL: single scalar puts s on the diagonal, zero elsewhere. mat3(1.0) is
    // the identity. Explicit: GLSL has no implicit scalar->matrix conversion.
    template <class S>
        requires detail::CvtScalar<S, T>
    SHADER_MATH_INLINE explicit matrix(const S& s) {
        constexpr int d = Cn < Rn ? Cn : Rn;
        for (int i = 0; i < d; ++i) col_[i][i] = static_cast<T>(s);
    }

    // Cn*Rn scalars, column-major fill (GLSL constructor order).
    template <class... S>
        requires(sizeof...(S) == Cn * Rn && Cn * Rn > 1 && (detail::CvtScalar<S, T> && ...))
    SHADER_MATH_INLINE matrix(S... s) {
        const T a[Cn * Rn] = {static_cast<T>(s)...};
        detail::unpack_dense<Cn, Rn>(a, col_);
    }

    // Cn column vectors.
    template <class... V>
        requires(sizeof...(V) == Cn && (std::is_convertible_v<V, detail::fvec<Rn, T>> && ...))
    SHADER_MATH_INLINE matrix(const V&... cols) {
        const detail::fvec<Rn, T> cc[Cn] = {detail::fvec<Rn, T>(cols)...};
        for (int c = 0; c < Cn; ++c) col_[c] = cc[c];
    }

    // GLSL matrix-from-matrix resize: result starts as identity, then the
    // overlapping upper-left region is copied (grow fills from identity,
    // shrink cuts the upper-left submatrix).
    template <int C2, int R2>
        requires(!(C2 == Cn && R2 == Rn))
    SHADER_MATH_INLINE explicit matrix(const matrix<C2, R2, T>& o) {
        constexpr int d = Cn < Rn ? Cn : Rn;
        for (int i = 0; i < d; ++i) col_[i][i] = T(1.0f);
        constexpr int cc = Cn < C2 ? Cn : C2;
        constexpr int rr = Rn < R2 ? Rn : R2;
        for (int c = 0; c < cc; ++c)
            for (int r = 0; r < rr; ++r) col_[c][r] = o.col_[c][r];
    }

    // Explicit element-type conversion (half <-> float); no implicit mixing.
    template <class U>
        requires(!std::is_same_v<U, T>)
    SHADER_MATH_INLINE explicit matrix(const matrix<Cn, Rn, U>& o) {
        for (int c = 0; c < Cn; ++c) col_[c] = detail::fvec<Rn, T>(o.col_[c]);
    }

    // Explicit conversion from the corresponding HLSL matrix (same math object).
    SHADER_MATH_INLINE explicit matrix(const hlsl::matrix<Rn, Cn, T>& h);

    SHADER_MATH_INLINE detail::fvec<Rn, T>& operator[](int c) { return col_[c]; }
    SHADER_MATH_INLINE const detail::fvec<Rn, T>& operator[](int c) const { return col_[c]; }
};

}  // namespace glsl

// ---- cross-namespace conversions (definitions) -----------------------------
// Crossing the storage direction (rows <-> columns of the same mathematical
// object) is exactly a register-array transpose.
namespace hlsl {
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T>::matrix(const glsl::matrix<C, R, T>& g) SM_HMAT_INIT {
    detail::transpose_regs<C, R>(g.col_, row_);
}
}  // namespace hlsl
#undef SM_HMAT_INIT
#undef SM_HMAT_COMMON

namespace glsl {
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>::matrix(const hlsl::matrix<Rn, Cn, T>& h) {
    detail::transpose_regs<Rn, Cn>(h.row_, col_);
}
}  // namespace glsl

}  // namespace shader_math
