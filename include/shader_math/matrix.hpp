// Matrix types.
//   hlsl::matrix<R,C> : R rows x C cols, row-register storage, row-major ctor fill.
//   glsl::matrix<C,R> : C cols x R rows, column-register storage, column-major fill.
// The two are distinct types (operator* differs); conversion is explicit.
// hlsl floatRxC corresponds to glsl matCxR (same mathematical object).
#pragma once

#include "vector.hpp"

namespace shader_math {

namespace glsl {
template <int Cn, int Rn>
struct matrix;
}

namespace detail {

// Single-element accessor proxy for HLSL _mRC / _RC members.
// Declared for all 16 positions on every matrix; out-of-range positions fail
// at the point of use.
template <int R, int C, int RI, int CI>
struct mswz {
    static constexpr bool valid = (RI >= 0 && RI < R && CI >= 0 && CI < C);
    float d[R * 4];

    SHADER_MATH_INLINE operator float() const {
        static_assert(valid, "shader-math: matrix element accessor out of range for this matrix size");
        return d[RI * 4 + CI];
    }
    SHADER_MATH_INLINE mswz& operator=(float s)
        requires(valid)
    {
        d[RI * 4 + CI] = s;
        return *this;
    }
    SHADER_MATH_INLINE mswz& operator=(const mswz& o)
        requires(valid)
    {
        d[RI * 4 + CI] = o.d[RI * 4 + CI];
        return *this;
    }
    template <int R2, int C2, int RJ, int CJ>
    SHADER_MATH_INLINE mswz& operator=(const mswz<R2, C2, RJ, CJ>& o)
        requires(valid && mswz<R2, C2, RJ, CJ>::valid)
    {
        d[RI * 4 + CI] = static_cast<float>(o);
        return *this;
    }
#define SM_MSWZ_COMPOUND(sym)                          \
    SHADER_MATH_INLINE mswz& operator sym##=(float s)  \
        requires(valid)                                \
    {                                                  \
        d[RI * 4 + CI] sym## = s;                      \
        return *this;                                  \
    }
    SM_MSWZ_COMPOUND(+)
    SM_MSWZ_COMPOUND(-)
    SM_MSWZ_COMPOUND(*)
    SM_MSWZ_COMPOUND(/)
#undef SM_MSWZ_COMPOUND
};

// ---- determinant / inverse cores on packed dense row-major arrays ----------
SHADER_MATH_INLINE float det2(const float* m) { return m[0] * m[3] - m[1] * m[2]; }

SHADER_MATH_INLINE float det3(const float* m) {
    return m[0] * (m[4] * m[8] - m[5] * m[7]) - m[1] * (m[3] * m[8] - m[5] * m[6]) +
           m[2] * (m[3] * m[7] - m[4] * m[6]);
}

SHADER_MATH_INLINE float det4(const float* m) {
    const float s0 = m[0] * m[5] - m[1] * m[4];
    const float s1 = m[0] * m[6] - m[2] * m[4];
    const float s2 = m[0] * m[7] - m[3] * m[4];
    const float s3 = m[1] * m[6] - m[2] * m[5];
    const float s4 = m[1] * m[7] - m[3] * m[5];
    const float s5 = m[2] * m[7] - m[3] * m[6];
    const float c5 = m[10] * m[15] - m[11] * m[14];
    const float c4 = m[9] * m[15] - m[11] * m[13];
    const float c3 = m[9] * m[14] - m[10] * m[13];
    const float c2 = m[8] * m[15] - m[11] * m[12];
    const float c1 = m[8] * m[14] - m[10] * m[12];
    const float c0 = m[8] * m[13] - m[9] * m[12];
    return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
}

SHADER_MATH_INLINE void inv2(const float* m, float* o) {
    const float id = 1.0f / det2(m);
    o[0] = m[3] * id;
    o[1] = -m[1] * id;
    o[2] = -m[2] * id;
    o[3] = m[0] * id;
}

SHADER_MATH_INLINE void inv3(const float* m, float* o) {
    const float id = 1.0f / det3(m);
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

SHADER_MATH_INLINE void inv4(const float* m, float* o) {
    const float s0 = m[0] * m[5] - m[1] * m[4];
    const float s1 = m[0] * m[6] - m[2] * m[4];
    const float s2 = m[0] * m[7] - m[3] * m[4];
    const float s3 = m[1] * m[6] - m[2] * m[5];
    const float s4 = m[1] * m[7] - m[3] * m[5];
    const float s5 = m[2] * m[7] - m[3] * m[6];
    const float c5 = m[10] * m[15] - m[11] * m[14];
    const float c4 = m[9] * m[15] - m[11] * m[13];
    const float c3 = m[9] * m[14] - m[10] * m[13];
    const float c2 = m[8] * m[15] - m[11] * m[12];
    const float c1 = m[8] * m[14] - m[10] * m[12];
    const float c0 = m[8] * m[13] - m[9] * m[12];
    const float id = 1.0f / (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);

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

}  // namespace detail

// ============================ hlsl::matrix ==================================
namespace hlsl {

template <int R, int C>
struct matrix {
    static_assert(R >= 2 && R <= 4 && C >= 2 && C <= 4,
                  "shader-math: matrix dimensions must be 2..4");
    static constexpr int rows_v = R;
    static constexpr int cols_v = C;

    union {
        detail::fvec<C> row_[R];
        float flat_[R * 4];
        // zero-based element accessors
        detail::mswz<R, C, 0, 0> _m00; detail::mswz<R, C, 0, 1> _m01;
        detail::mswz<R, C, 0, 2> _m02; detail::mswz<R, C, 0, 3> _m03;
        detail::mswz<R, C, 1, 0> _m10; detail::mswz<R, C, 1, 1> _m11;
        detail::mswz<R, C, 1, 2> _m12; detail::mswz<R, C, 1, 3> _m13;
        detail::mswz<R, C, 2, 0> _m20; detail::mswz<R, C, 2, 1> _m21;
        detail::mswz<R, C, 2, 2> _m22; detail::mswz<R, C, 2, 3> _m23;
        detail::mswz<R, C, 3, 0> _m30; detail::mswz<R, C, 3, 1> _m31;
        detail::mswz<R, C, 3, 2> _m32; detail::mswz<R, C, 3, 3> _m33;
        // one-based element accessors
        detail::mswz<R, C, 0, 0> _11; detail::mswz<R, C, 0, 1> _12;
        detail::mswz<R, C, 0, 2> _13; detail::mswz<R, C, 0, 3> _14;
        detail::mswz<R, C, 1, 0> _21; detail::mswz<R, C, 1, 1> _22;
        detail::mswz<R, C, 1, 2> _23; detail::mswz<R, C, 1, 3> _24;
        detail::mswz<R, C, 2, 0> _31; detail::mswz<R, C, 2, 1> _32;
        detail::mswz<R, C, 2, 2> _33; detail::mswz<R, C, 2, 3> _34;
        detail::mswz<R, C, 3, 0> _41; detail::mswz<R, C, 3, 1> _42;
        detail::mswz<R, C, 3, 2> _43; detail::mswz<R, C, 3, 3> _44;
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
    SHADER_MATH_INLINE matrix(float s) : row_{} {
        for (int r = 0; r < R; ++r) row_[r] = detail::fvec<C>(s);
    }

    // R*C scalars, row-major fill (HLSL constructor order).
    template <class... T>
        requires(sizeof...(T) == R * C && (std::is_convertible_v<T, float> && ...))
    SHADER_MATH_INLINE matrix(T... s) : row_{} {
        const float a[R * C] = {static_cast<float>(s)...};
        for (int r = 0; r < R; ++r) {
            float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            for (int c = 0; c < C; ++c) t[c] = a[r * C + c];
            row_[r] = detail::fvec<C>(detail::ops::load4(t));
        }
    }

    // R row vectors.
    template <class... V>
        requires(sizeof...(V) == R && (std::is_convertible_v<V, detail::fvec<C>> && ...))
    SHADER_MATH_INLINE matrix(const V&... rows) : row_{} {
        const detail::fvec<C> rr[R] = {detail::fvec<C>(rows)...};
        for (int r = 0; r < R; ++r) row_[r] = rr[r];
    }

    // HLSL cast-style shrink from a bigger matrix (upper-left cut). Growth is
    // not provided (HLSL has no widening matrix cast).
    template <int R2, int C2>
        requires(R2 >= R && C2 >= C && !(R2 == R && C2 == C))
    SHADER_MATH_INLINE explicit matrix(const matrix<R2, C2>& o) : row_{} {
        for (int r = 0; r < R; ++r) {
            float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            for (int c = 0; c < C; ++c) t[c] = o.row_[r][c];
            row_[r] = detail::fvec<C>(detail::ops::load4(t));
        }
    }

    // Explicit conversion from the corresponding GLSL matrix (same math object).
    SHADER_MATH_INLINE explicit matrix(const glsl::matrix<C, R>& g);

    // Extension (not an HLSL intrinsic): identity for convenience.
    static SHADER_MATH_INLINE matrix identity() {
        matrix m;
        constexpr int d = R < C ? R : C;
        for (int i = 0; i < d; ++i) m.row_[i][i] = 1.0f;
        return m;
    }

    SHADER_MATH_INLINE detail::fvec<C>& operator[](int r) { return row_[r]; }
    SHADER_MATH_INLINE const detail::fvec<C>& operator[](int r) const { return row_[r]; }
};

}  // namespace hlsl

// ============================ glsl::matrix ==================================
namespace glsl {

// Cn columns x Rn rows. glsl::matCxR. Column-register storage.
template <int Cn, int Rn>
struct matrix {
    static_assert(Cn >= 2 && Cn <= 4 && Rn >= 2 && Rn <= 4,
                  "shader-math: matrix dimensions must be 2..4");
    static constexpr int cols_v = Cn;
    static constexpr int rows_v = Rn;

    detail::fvec<Rn> col_[Cn];

    SHADER_MATH_INLINE matrix() = default;

    // GLSL: single scalar puts s on the diagonal, zero elsewhere. mat3(1.0) is
    // the identity. Explicit: GLSL has no implicit scalar->matrix conversion.
    SHADER_MATH_INLINE explicit matrix(float s) {
        constexpr int d = Cn < Rn ? Cn : Rn;
        for (int i = 0; i < d; ++i) col_[i][i] = s;
    }

    // Cn*Rn scalars, column-major fill (GLSL constructor order).
    template <class... T>
        requires(sizeof...(T) == Cn * Rn && (std::is_convertible_v<T, float> && ...))
    SHADER_MATH_INLINE matrix(T... s) {
        const float a[Cn * Rn] = {static_cast<float>(s)...};
        for (int c = 0; c < Cn; ++c) {
            float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
            for (int r = 0; r < Rn; ++r) t[r] = a[c * Rn + r];
            col_[c] = detail::fvec<Rn>(detail::ops::load4(t));
        }
    }

    // Cn column vectors.
    template <class... V>
        requires(sizeof...(V) == Cn && (std::is_convertible_v<V, detail::fvec<Rn>> && ...))
    SHADER_MATH_INLINE matrix(const V&... cols) {
        const detail::fvec<Rn> cc[Cn] = {detail::fvec<Rn>(cols)...};
        for (int c = 0; c < Cn; ++c) col_[c] = cc[c];
    }

    // GLSL matrix-from-matrix resize: result starts as identity, then the
    // overlapping upper-left region is copied (grow fills from identity,
    // shrink cuts the upper-left submatrix).
    template <int C2, int R2>
        requires(!(C2 == Cn && R2 == Rn))
    SHADER_MATH_INLINE explicit matrix(const matrix<C2, R2>& o) {
        constexpr int d = Cn < Rn ? Cn : Rn;
        for (int i = 0; i < d; ++i) col_[i][i] = 1.0f;
        constexpr int cc = Cn < C2 ? Cn : C2;
        constexpr int rr = Rn < R2 ? Rn : R2;
        for (int c = 0; c < cc; ++c)
            for (int r = 0; r < rr; ++r) col_[c][r] = o.col_[c][r];
    }

    // Explicit conversion from the corresponding HLSL matrix (same math object).
    SHADER_MATH_INLINE explicit matrix(const hlsl::matrix<Rn, Cn>& h);

    SHADER_MATH_INLINE detail::fvec<Rn>& operator[](int c) { return col_[c]; }
    SHADER_MATH_INLINE const detail::fvec<Rn>& operator[](int c) const { return col_[c]; }
};

}  // namespace glsl

// ---- cross-namespace conversions (definitions) -----------------------------
namespace hlsl {
template <int R, int C>
SHADER_MATH_INLINE matrix<R, C>::matrix(const glsl::matrix<C, R>& g) : row_{} {
    for (int r = 0; r < R; ++r) {
        float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        for (int c = 0; c < C; ++c) t[c] = g.col_[c][r];
        row_[r] = detail::fvec<C>(detail::ops::load4(t));
    }
}
}  // namespace hlsl

namespace glsl {
template <int Cn, int Rn>
SHADER_MATH_INLINE matrix<Cn, Rn>::matrix(const hlsl::matrix<Rn, Cn>& h) {
    for (int c = 0; c < Cn; ++c) {
        float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        for (int r = 0; r < Rn; ++r) t[r] = h.row_[r][c];
        col_[c] = detail::fvec<Rn>(detail::ops::load4(t));
    }
}
}  // namespace glsl

}  // namespace shader_math
