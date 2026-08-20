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
struct mswz {
    static constexpr bool valid = (RI >= 0 && RI < R && CI >= 0 && CI < C);
    T d[R * 4];

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

// N registers of L lanes <-> dense array p[i * L + j] = regs[i][j]
template <int N, int L, class T>
SHADER_MATH_INLINE void pack_dense(const fvec<L, T>* regs, T* p) {
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < L; ++j) p[i * L + j] = regs[i][j];
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
struct matrix {
    static_assert(R >= 2 && R <= 4 && C >= 2 && C <= 4,
                  "shader-math: matrix dimensions must be 2..4");
    static constexpr int rows_v = R;
    static constexpr int cols_v = C;
    using elem_t = T;

    union {
        detail::fvec<C, T> row_[R];
        T flat_[R * 4];
        // zero-based element accessors
        detail::mswz<T, R, C, 0, 0> _m00; detail::mswz<T, R, C, 0, 1> _m01;
        detail::mswz<T, R, C, 0, 2> _m02; detail::mswz<T, R, C, 0, 3> _m03;
        detail::mswz<T, R, C, 1, 0> _m10; detail::mswz<T, R, C, 1, 1> _m11;
        detail::mswz<T, R, C, 1, 2> _m12; detail::mswz<T, R, C, 1, 3> _m13;
        detail::mswz<T, R, C, 2, 0> _m20; detail::mswz<T, R, C, 2, 1> _m21;
        detail::mswz<T, R, C, 2, 2> _m22; detail::mswz<T, R, C, 2, 3> _m23;
        detail::mswz<T, R, C, 3, 0> _m30; detail::mswz<T, R, C, 3, 1> _m31;
        detail::mswz<T, R, C, 3, 2> _m32; detail::mswz<T, R, C, 3, 3> _m33;
        // one-based element accessors
        detail::mswz<T, R, C, 0, 0> _11; detail::mswz<T, R, C, 0, 1> _12;
        detail::mswz<T, R, C, 0, 2> _13; detail::mswz<T, R, C, 0, 3> _14;
        detail::mswz<T, R, C, 1, 0> _21; detail::mswz<T, R, C, 1, 1> _22;
        detail::mswz<T, R, C, 1, 2> _23; detail::mswz<T, R, C, 1, 3> _24;
        detail::mswz<T, R, C, 2, 0> _31; detail::mswz<T, R, C, 2, 1> _32;
        detail::mswz<T, R, C, 2, 2> _33; detail::mswz<T, R, C, 2, 3> _34;
        detail::mswz<T, R, C, 3, 0> _41; detail::mswz<T, R, C, 3, 1> _42;
        detail::mswz<T, R, C, 3, 2> _43; detail::mswz<T, R, C, 3, 3> _44;
    };

    SHADER_MATH_INLINE matrix() : row_{} {}
    SHADER_MATH_INLINE matrix(const matrix& o) : row_{} {
        for (int r = 0; r < R; ++r) row_[r] = o.row_[r];
    }
    SHADER_MATH_INLINE matrix& operator=(const matrix& o) {
        for (int r = 0; r < R; ++r) row_[r] = o.row_[r];
        return *this;
    }

    // HLSL: scalar constructs a matrix with ALL components set to s (splat).
    template <class S>
        requires detail::CvtScalar<S, T>
    SHADER_MATH_INLINE matrix(const S& s) : row_{} {
        for (int r = 0; r < R; ++r) row_[r] = detail::fvec<C, T>(static_cast<T>(s));
    }

    // R*C scalars, row-major fill (HLSL constructor order).
    template <class... S>
        requires(sizeof...(S) == R * C && R * C > 1 && (detail::CvtScalar<S, T> && ...))
    SHADER_MATH_INLINE matrix(S... s) : row_{} {
        const T a[R * C] = {static_cast<T>(s)...};
        for (int r = 0; r < R; ++r) {
            T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
            for (int c = 0; c < C; ++c) t[c] = a[r * C + c];
            row_[r] = detail::fvec<C, T>(detail::vops<T>::load4(t));
        }
    }

    // R row vectors.
    template <class... V>
        requires(sizeof...(V) == R && (std::is_convertible_v<V, detail::fvec<C, T>> && ...))
    SHADER_MATH_INLINE matrix(const V&... rows) : row_{} {
        const detail::fvec<C, T> rr[R] = {detail::fvec<C, T>(rows)...};
        for (int r = 0; r < R; ++r) row_[r] = rr[r];
    }

    // HLSL cast-style shrink from a bigger matrix (upper-left cut). Growth is
    // not provided (HLSL has no widening matrix cast).
    template <int R2, int C2>
        requires(R2 >= R && C2 >= C && !(R2 == R && C2 == C))
    SHADER_MATH_INLINE explicit matrix(const matrix<R2, C2, T>& o) : row_{} {
        for (int r = 0; r < R; ++r) {
            T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
            for (int c = 0; c < C; ++c) t[c] = o.row_[r][c];
            row_[r] = detail::fvec<C, T>(detail::vops<T>::load4(t));
        }
    }

    // Explicit element-type conversion (half <-> float); no implicit mixing.
    template <class U>
        requires(!std::is_same_v<U, T>)
    SHADER_MATH_INLINE explicit matrix(const matrix<R, C, U>& o) : row_{} {
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
        for (int c = 0; c < Cn; ++c) {
            T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
            for (int r = 0; r < Rn; ++r) t[r] = a[c * Rn + r];
            col_[c] = detail::fvec<Rn, T>(detail::vops<T>::load4(t));
        }
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
namespace hlsl {
template <int R, int C, class T>
SHADER_MATH_INLINE matrix<R, C, T>::matrix(const glsl::matrix<C, R, T>& g) : row_{} {
    for (int r = 0; r < R; ++r) {
        T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
        for (int c = 0; c < C; ++c) t[c] = g.col_[c][r];
        row_[r] = detail::fvec<C, T>(detail::vops<T>::load4(t));
    }
}
}  // namespace hlsl

namespace glsl {
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>::matrix(const hlsl::matrix<Rn, Cn, T>& h) {
    for (int c = 0; c < Cn; ++c) {
        T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
        for (int r = 0; r < Rn; ++r) t[r] = h.row_[r][c];
        col_[c] = detail::fvec<Rn, T>(detail::vops<T>::load4(t));
    }
}
}  // namespace glsl

}  // namespace shader_math
