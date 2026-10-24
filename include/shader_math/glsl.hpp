// GLSL language surface.
//   operator* is the linear algebra product (matrix*matrix, matrix*vector,
//   vector*matrix, scalar scaling). Component-wise multiply of matrices is
//   matrixCompMult(). == / != return a single bool; per-component comparisons
//   are equal()/lessThan()/... returning bvec. Relational operators (<, > ...)
//   are NOT defined for vectors, exactly like GLSL.
//   GLSL's not() is spelled not_() (`not` is a C++ keyword).
#pragma once

#include "matrix.hpp"

namespace shader_math {
namespace glsl {

// ---- type aliases -----------------------------------------------------------
using vec2 = detail::fvec<2>;
using vec3 = detail::fvec<3>;
using vec4 = detail::fvec<4>;
using bvec2 = detail::bvec<2>;
using bvec3 = detail::bvec<3>;
using bvec4 = detail::bvec<4>;

// Half vectors (GLSL GL_EXT_shader_explicit_arithmetic_types float16_t /
// f16vecN). Language-level packing: sizeof(f16vec3) == 8 (4 x binary16),
// language compatibility, not a GPU ABI. Absent entirely on compilers
// without a half scalar type.
#if defined(SHADER_MATH_HAS_HALF)
using float16_t = detail::half_t;
using f16vec2 = detail::fvec<2, detail::half_t>;
using f16vec3 = detail::fvec<3, detail::half_t>;
using f16vec4 = detail::fvec<4, detail::half_t>;
#endif

using mat2 = matrix<2, 2>;
using mat3 = matrix<3, 3>;
using mat4 = matrix<4, 4>;
using mat2x2 = matrix<2, 2>;
using mat2x3 = matrix<2, 3>;
using mat2x4 = matrix<2, 4>;
using mat3x2 = matrix<3, 2>;
using mat3x3 = matrix<3, 3>;
using mat3x4 = matrix<3, 4>;
using mat4x2 = matrix<4, 2>;
using mat4x3 = matrix<4, 3>;
using mat4x4 = matrix<4, 4>;

// Half matrices (GLSL f16matCxR); non-square shape correspondence with the
// HLSL halfRxC aliases is the same as for float (f16mat4x3 == half3x4 math).
#if defined(SHADER_MATH_HAS_HALF)
using f16mat2 = matrix<2, 2, detail::half_t>;
using f16mat3 = matrix<3, 3, detail::half_t>;
using f16mat4 = matrix<4, 4, detail::half_t>;
using f16mat2x2 = matrix<2, 2, detail::half_t>;
using f16mat2x3 = matrix<2, 3, detail::half_t>;
using f16mat2x4 = matrix<2, 4, detail::half_t>;
using f16mat3x2 = matrix<3, 2, detail::half_t>;
using f16mat3x3 = matrix<3, 3, detail::half_t>;
using f16mat3x4 = matrix<3, 4, detail::half_t>;
using f16mat4x2 = matrix<4, 2, detail::half_t>;
using f16mat4x3 = matrix<4, 3, detail::half_t>;
using f16mat4x4 = matrix<4, 4, detail::half_t>;
#endif

// re-export shared functions so `using namespace glsl;` provides them
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

// component-wise functions with identical names in both languages
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

// GLSL-only spellings (defined here, not in detail, so they never leak into
// hlsl via ADL on the shared vector type)
template <class... As>
    requires requires(const As&... as) { detail::lerp_impl(as...); }
SHADER_MATH_INLINE auto mix(const As&... as) {
    return detail::lerp_impl(as...);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto fract(const A& a) {
    return detail::fract_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto inversesqrt(const A& a) {
    return detail::rsqrt_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto roundEven(const A& a) {
    return detail::round_even_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto asinh(const A& a) {
    return detail::asinh_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto acosh(const A& a) {
    return detail::acosh_impl(a);
}
template <class A>
    requires detail::VecLike<A>
SHADER_MATH_INLINE auto atanh(const A& a) {
    return detail::atanh_impl(a);
}
// mix(x, y, bvec): y where the selector is true, else x (no blending)
template <class A, class B, int N>
    requires requires(const A& x, const B& y, const detail::bvec<N>& c) {
        detail::select_impl(c, y, x);
    }
SHADER_MATH_INLINE auto mix(const A& x, const B& y, const detail::bvec<N>& c) {
    return detail::select_impl(c, y, x);
}
// atan(y, x): GLSL spells the two-argument arctangent as an atan overload
template <class A, class B>
    requires requires(const A& y, const B& x) { detail::atan2_impl(y, x); }
SHADER_MATH_INLINE auto atan(const A& y, const B& x) {
    return detail::atan2_impl(y, x);
}
template <class A, class B>
    requires requires(const A& a, const B& b) { detail::mod_impl(a, b); }
SHADER_MATH_INLINE auto mod(const A& a, const B& b) {
    return detail::mod_impl(a, b);
}
template <class A, class B, class C>
    requires requires(const A& a, const B& b, const C& c) { detail::mad_impl(a, b, c); }
SHADER_MATH_INLINE auto fma(const A& a, const B& b, const C& c) {
    return detail::mad_impl(a, b, c);
}

// scalar arguments (detail::ScalarCall): GLSL-only spellings
SHADER_MATH_SCALAR_FN3(mix)
SHADER_MATH_SCALAR_FN1(fract)
SHADER_MATH_SCALAR_FN1(inversesqrt)
SHADER_MATH_SCALAR_FN1(roundEven)
SHADER_MATH_SCALAR_FN1(asinh)
SHADER_MATH_SCALAR_FN1(acosh)
SHADER_MATH_SCALAR_FN1(atanh)
SHADER_MATH_SCALAR_FN2(atan)
SHADER_MATH_SCALAR_FN2(mod)
SHADER_MATH_SCALAR_FN3(fma)
template <class A, class B>
    requires detail::ScalarCall<A, B>
SHADER_MATH_INLINE auto mix(const A& x, const B& y, bool c) {
    using T = detail::scalar_call_t<A, B>;
    return c ? static_cast<T>(y) : static_cast<T>(x);
}

// ---- component-wise matrix operators (+ - / and scalar forms) ---------------
#define SM_GLSL_MAT_BINOP(sym, fn)                                                       \
    template <int Cn, int Rn, class T>                                                   \
    SHADER_MATH_INLINE matrix<Cn, Rn, T> operator sym(const matrix<Cn, Rn, T>& a,        \
                                                      const matrix<Cn, Rn, T>& b) {      \
        matrix<Cn, Rn, T> o;                                                             \
        detail::regs_fill<Cn>(o.col_, [&](int c) {                                       \
            return detail::vops<T>::fn(a.col_[c].q_, b.col_[c].q_);                      \
        });                                                                              \
        return o;                                                                        \
    }                                                                                    \
    template <int Cn, int Rn, class T, class S>                                          \
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE matrix<Cn, Rn, T> operator sym(const matrix<Cn, Rn, T>& a,        \
                                                      const S& s) {                     \
        matrix<Cn, Rn, T> o;                                                             \
        const auto sp = detail::vops<T>::splat(static_cast<T>(s));                       \
        detail::regs_fill<Cn>(o.col_,                                                    \
                              [&](int c) { return detail::vops<T>::fn(a.col_[c].q_, sp); }); \
        return o;                                                                        \
    }                                                                                    \
    template <int Cn, int Rn, class T, class S>                                          \
        requires detail::ScalarFor<S, T>                                                 \
    SHADER_MATH_INLINE matrix<Cn, Rn, T> operator sym(const S& s,                        \
                                                      const matrix<Cn, Rn, T>& b) {     \
        matrix<Cn, Rn, T> o;                                                             \
        const auto sp = detail::vops<T>::splat(static_cast<T>(s));                       \
        detail::regs_fill<Cn>(o.col_,                                                    \
                              [&](int c) { return detail::vops<T>::fn(sp, b.col_[c].q_); }); \
        return o;                                                                        \
    }

SM_GLSL_MAT_BINOP(+, add)
SM_GLSL_MAT_BINOP(-, sub)
SM_GLSL_MAT_BINOP(/, div)
#undef SM_GLSL_MAT_BINOP

template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator-(const matrix<Cn, Rn, T>& a) {
    matrix<Cn, Rn, T> o;
    detail::regs_fill<Cn>(o.col_, [&](int c) { return detail::vops<T>::neg(a.col_[c].q_); });
    return o;
}

// ---- operator*: linear algebra (GLSL semantics) ------------------------------
// matrix * matrix: inner dimensions must match (enforced by the signature).
template <int C1, int R1, int C2, class T>
SHADER_MATH_INLINE matrix<C2, R1, T> operator*(const matrix<C1, R1, T>& a,
                                               const matrix<C2, C1, T>& b) {
    matrix<C2, R1, T> o;
    for (int j = 0; j < C2; ++j)
        o.col_[j] = detail::fvec<R1, T>(detail::mad_accum<C1>(b.col_[j].q_, a.col_));
    return o;
}

#if defined(SHADER_MATH_HAS_AVX_2ROW)
// float mat4 product on the shared YMM kernel (AVX1 + FMA3, bit-identical to
// the 128-bit path). Columns swap roles versus HLSL: pack b's columns,
// broadcast a's columns.
SHADER_MATH_INLINE matrix<4, 4> operator*(const matrix<4, 4>& a, const matrix<4, 4>& b) {
    matrix<4, 4> o;
    detail::avx2row::mul4x4(b.col_, a.col_, o.col_);
    return o;
}
#endif

// matrix * column vector
template <int Cn, int Rn, class T, class V>
    requires(detail::vec_size_v<V> == Cn && std::is_same_v<detail::vec_elem_t<V>, T>)
SHADER_MATH_INLINE detail::fvec<Rn, T> operator*(const matrix<Cn, Rn, T>& m, const V& v0) {
    const detail::fvec<Cn, T> v = detail::to_vec(v0);
    return detail::fvec<Rn, T>(detail::mad_accum<Cn>(v.q_, m.col_));
}

// row vector * matrix
template <class V, int Cn, int Rn, class T>
    requires(detail::vec_size_v<V> == Rn && std::is_same_v<detail::vec_elem_t<V>, T>)
SHADER_MATH_INLINE detail::fvec<Cn, T> operator*(const V& v0, const matrix<Cn, Rn, T>& m) {
    const detail::fvec<Rn, T> v = detail::to_vec(v0);
    T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int c = 0; c < Cn; ++c) t[c] = detail::dot(v, m.col_[c]);
    return detail::fvec<Cn, T>(detail::vops<T>::load4(t));
}

// scalar scaling
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator*(const matrix<Cn, Rn, T>& a, const S& s) {
    matrix<Cn, Rn, T> o;
    const auto sp = detail::vops<T>::splat(static_cast<T>(s));
    detail::regs_fill<Cn>(o.col_, [&](int c) { return detail::vops<T>::mul(a.col_[c].q_, sp); });
    return o;
}
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator*(const S& s, const matrix<Cn, Rn, T>& b) {
    return b * s;
}

// compound assignment
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator+=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a + b;
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator-=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a - b;
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator/=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a / b;
    return a;
}
// m *= n requires the product to keep m's shape, i.e. square n (GLSL rule)
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator*=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Cn, T>& b) {
    a = a * b;
    return a;
}
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator+=(matrix<Cn, Rn, T>& a, const S& s) {
    a = a + s;
    return a;
}
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator-=(matrix<Cn, Rn, T>& a, const S& s) {
    a = a - s;
    return a;
}
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator*=(matrix<Cn, Rn, T>& a, const S& s) {
    a = a * s;
    return a;
}
template <int Cn, int Rn, class T, class S>
    requires detail::ScalarFor<S, T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator/=(matrix<Cn, Rn, T>& a, const S& s) {
    a = a / s;
    return a;
}

// ---- == / != : single bool (GLSL semantics) ----------------------------------
template <class A, class B>
    requires detail::SameVec<A, B>
SHADER_MATH_INLINE bool operator==(const A& a, const B& b) {
    constexpr int n = detail::vec_size_v<A>;
    using T = detail::vec_elem_t<A>;
    return detail::ops::all_n(detail::vops<T>::cmp_eq(detail::to_vec(a).q_, detail::to_vec(b).q_),
                              n);
}
template <class A, class B>
    requires detail::SameVec<A, B>
SHADER_MATH_INLINE bool operator!=(const A& a, const B& b) {
    return !(a == b);
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE bool operator==(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, T>& b) {
    for (int c = 0; c < Cn; ++c)
        if (!detail::ops::all_n(detail::vops<T>::cmp_eq(a.col_[c].q_, b.col_[c].q_), Rn))
            return false;
    return true;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE bool operator!=(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, T>& b) {
    return !(a == b);
}
template <int N>
SHADER_MATH_INLINE bool operator==(const detail::bvec<N>& a, const detail::bvec<N>& b) {
    for (int i = 0; i < N; ++i)
        if (a[i] != b[i]) return false;
    return true;
}
template <int N>
SHADER_MATH_INLINE bool operator!=(const detail::bvec<N>& a, const detail::bvec<N>& b) {
    return !(a == b);
}

// ---- per-component comparison functions (GLSL 8.7) ---------------------------
#define SM_GLSL_CMP_FN(name, fn)                                                         \
    template <class A, class B>                                                          \
        requires detail::SameVec<A, B>                                                   \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                               \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n>(detail::vops<T>::fn(detail::to_vec(a).q_, detail::to_vec(b).q_)); \
    }

SM_GLSL_CMP_FN(equal, cmp_eq)
SM_GLSL_CMP_FN(notEqual, cmp_ne)
SM_GLSL_CMP_FN(lessThan, cmp_lt)
SM_GLSL_CMP_FN(lessThanEqual, cmp_le)
SM_GLSL_CMP_FN(greaterThan, cmp_gt)
SM_GLSL_CMP_FN(greaterThanEqual, cmp_ge)
#undef SM_GLSL_CMP_FN

// GLSL not(bvec). `not` is a C++ keyword, hence the trailing underscore.
template <int N>
SHADER_MATH_INLINE detail::bvec<N> not_(const detail::bvec<N>& a) {
    return detail::bvec<N>(detail::ops::m_not(a.m_));
}

// ---- matrix functions (GLSL 8.6) ----------------------------------------------
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> matrixCompMult(const matrix<Cn, Rn, T>& a,
                                                    const matrix<Cn, Rn, T>& b) {
    matrix<Cn, Rn, T> o;
    detail::regs_fill<Cn>(
        o.col_, [&](int c) { return detail::vops<T>::mul(a.col_[c].q_, b.col_[c].q_); });
    return o;
}

// outerProduct(c, r): c is a column (Rx1), r is a row (1xC) -> matCxR
template <class A, class B>
    requires(detail::VecLike<A> && detail::VecLike<B> &&
             std::is_same_v<detail::vec_elem_t<A>, detail::vec_elem_t<B>>)
SHADER_MATH_INLINE matrix<detail::vec_size_v<B>, detail::vec_size_v<A>, detail::vec_elem_t<A>>
outerProduct(const A& c0, const B& r0) {
    constexpr int RR = detail::vec_size_v<A>;
    constexpr int CC = detail::vec_size_v<B>;
    using T = detail::vec_elem_t<A>;
    const detail::fvec<RR, T> c = detail::to_vec(c0);
    const detail::fvec<CC, T> r = detail::to_vec(r0);
    matrix<CC, RR, T> o;
    detail::regs_fill<CC>(o.col_, [&](int j) {
        return detail::vops<T>::mul(c.q_, detail::vops<T>::splat(r[j]));
    });
    return o;
}

template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Rn, Cn, T> transpose(const matrix<Cn, Rn, T>& m) {
    matrix<Rn, Cn, T> o;
    detail::transpose_regs<Cn, Rn>(m.col_, o.col_);
    return o;
}

template <int Cn, int Rn, class T>
SHADER_MATH_INLINE T determinant(const matrix<Cn, Rn, T>& m) {
    static_assert(Cn == Rn, "shader-math: determinant requires a square matrix");
    // Column-major pack; det(A) == det(A^T) so the row-major core applies as-is.
    return detail::det_regs<Cn>(m.col_);
}

// GLSL official inverse(). Singular input yields inf/NaN (undefined in GLSL).
// True fp16 inputs underflow/overflow much sooner than float; keep half
// matrices well conditioned.
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> inverse(const matrix<Cn, Rn, T>& m) {
    static_assert(Cn == Rn, "shader-math: inverse requires a square matrix");
    // The packed array is column-major = row-major of A^T. The core returns
    // inv(A^T) = inv(A)^T in row-major, which is inv(A) in column-major.
    matrix<Cn, Rn, T> o;
    detail::inv_regs<Cn>(m.col_, o.col_);
    return o;
}

}  // namespace glsl
}  // namespace shader_math
