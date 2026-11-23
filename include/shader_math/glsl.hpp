// GLSL language surface.
//   operator* is the linear algebra product (matrix*matrix, matrix*vector,
//   vector*matrix, scalar scaling). Component-wise multiply of matrices is
//   matrixCompMult(). == / != return a single bool; per-component comparisons
//   are equal()/lessThan()/... returning bvec. Relational operators (<, > ...)
//   are NOT defined for vectors, exactly like GLSL.
//   GLSL's not() is spelled not_() (`not` is a C++ alternative token).
#pragma once

#include "matrix.hpp"

namespace shader_math {
namespace glsl {

// ---- type aliases -----------------------------------------------------------
using vec2 = detail::gvec<2>;
using vec3 = detail::gvec<3>;
using vec4 = detail::gvec<4>;
using float64_t = double;
using dvec2 = detail::gvec<2, double>;
using dvec3 = detail::gvec<3, double>;
using dvec4 = detail::gvec<4, double>;
using f64vec2 = dvec2;
using f64vec3 = dvec3;
using f64vec4 = dvec4;
using bvec2 = detail::bvec<2, lang_tag>;
using bvec3 = detail::bvec<3, lang_tag>;
using bvec4 = detail::bvec<4, lang_tag>;

// 32-bit integer vectors: arithmetic wraps; x / 0, shifts by 32 or more and
// out-of-range float -> int conversions have defined results
// (docs/design/int_vectors.md)
static_assert(sizeof(int) == 4, "shader-math: int must be 32 bits");
using uint = unsigned int;
using ivec2 = detail::gvec<2, int>;
using ivec3 = detail::gvec<3, int>;
using ivec4 = detail::gvec<4, int>;
using uvec2 = detail::gvec<2, uint>;
using uvec3 = detail::gvec<3, uint>;
using uvec4 = detail::gvec<4, uint>;
// 16- and 64-bit integers (GL_EXT_shader_explicit_arithmetic_types); 16-bit
// lanes compute in 32 bits and wrap, 64-bit lanes one at a time
using int16_t = std::int16_t;
using uint16_t = std::uint16_t;
using int64_t = std::int64_t;
using uint64_t = std::uint64_t;
using i16vec2 = detail::gvec<2, int16_t>;
using i16vec3 = detail::gvec<3, int16_t>;
using i16vec4 = detail::gvec<4, int16_t>;
using u16vec2 = detail::gvec<2, uint16_t>;
using u16vec3 = detail::gvec<3, uint16_t>;
using u16vec4 = detail::gvec<4, uint16_t>;
using i64vec2 = detail::gvec<2, int64_t>;
using i64vec3 = detail::gvec<3, int64_t>;
using i64vec4 = detail::gvec<4, int64_t>;
using u64vec2 = detail::gvec<2, uint64_t>;
using u64vec3 = detail::gvec<3, uint64_t>;
using u64vec4 = detail::gvec<4, uint64_t>;

// Half vectors (GLSL GL_EXT_shader_explicit_arithmetic_types float16_t /
// f16vecN). Language-level packing: sizeof(f16vec3) == 8 (4 x binary16),
// language compatibility, not a GPU ABI. Absent entirely on compilers
// without a half scalar type.
#if defined(SHADER_MATH_HAS_HALF)
using float16_t = detail::half_t;
using f16vec2 = detail::gvec<2, detail::half_t>;
using f16vec3 = detail::gvec<3, detail::half_t>;
using f16vec4 = detail::gvec<4, detail::half_t>;
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

using dmat2 = matrix<2, 2, double>;
using dmat3 = matrix<3, 3, double>;
using dmat4 = matrix<4, 4, double>;
using dmat2x2 = matrix<2, 2, double>;
using dmat2x3 = matrix<2, 3, double>;
using dmat2x4 = matrix<2, 4, double>;
using dmat3x2 = matrix<3, 2, double>;
using dmat3x3 = matrix<3, 3, double>;
using dmat3x4 = matrix<3, 4, double>;
using dmat4x2 = matrix<4, 2, double>;
using dmat4x3 = matrix<4, 3, double>;
using dmat4x4 = matrix<4, 4, double>;
using f64mat2 = dmat2;
using f64mat3 = dmat3;
using f64mat4 = dmat4;
using f64mat2x2 = dmat2x2;
using f64mat2x3 = dmat2x3;
using f64mat2x4 = dmat2x4;
using f64mat3x2 = dmat3x2;
using f64mat3x3 = dmat3x3;
using f64mat3x4 = dmat3x4;
using f64mat4x2 = dmat4x2;
using f64mat4x3 = dmat4x3;
using f64mat4x4 = dmat4x4;

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

// shared functions under glsl:: names, for glsl arguments only
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
SHADER_MATH_INLINE detail::bvec<N, lang_tag> isnan(const detail::gvec<N, T>& v) {
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
SHADER_MATH_INLINE detail::bvec<N, lang_tag> isinf(const detail::gvec<N, T>& v) {
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
SHADER_MATH_INLINE double round(double a) {
    return detail::round_lane(a);
}
#if defined(SHADER_MATH_HAS_HALF)
SHADER_MATH_INLINE detail::half_t round(detail::half_t a) {
    return detail::round_lane(a);
}
#endif
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::GlslDoubleOrFloatVec<A>)
SHADER_MATH_INLINE auto round(const A& a) {
    return detail::round(a);
}
template <int N>
SHADER_MATH_INLINE bool all(const detail::bvec<N, lang_tag>& b) {
    return detail::all(b);
}
template <int N>
SHADER_MATH_INLINE bool any(const detail::bvec<N, lang_tag>& b) {
    return detail::any(b);
}

// GLSL-only spellings (defined here, not in detail, so they never leak into
// hlsl; the language tag makes ADL find them on glsl vectors)
template <class... As>
    requires(detail::ArgsOf<lang_tag, As...> &&
             requires(const As&... as) { detail::mix_impl(as...); })
SHADER_MATH_INLINE auto mix(const As&... as) {
    return detail::mix_impl(as...);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto fract(const A& a) {
    return detail::fract_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto inversesqrt(const A& a) {
    return detail::rsqrt_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A>)
SHADER_MATH_INLINE auto roundEven(const A& a) {
    return detail::round_even_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::NonDoubleFloatVec<A>)
SHADER_MATH_INLINE auto asinh(const A& a) {
    return detail::asinh_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::NonDoubleFloatVec<A>)
SHADER_MATH_INLINE auto acosh(const A& a) {
    return detail::acosh_impl(a);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::NonDoubleFloatVec<A>)
SHADER_MATH_INLINE auto atanh(const A& a) {
    return detail::atanh_impl(a);
}
// mix(x, y, bvec): y where the selector is true, else x (no blending)
template <class A, class B, int N>
    requires requires(const A& x, const B& y, const detail::bvec<N, lang_tag>& c) {
        detail::select_impl(c, y, x);
    }
SHADER_MATH_INLINE auto mix(const A& x, const B& y, const detail::bvec<N, lang_tag>& c) {
    return detail::select_impl(c, y, x);
}
// atan(y, x): GLSL spells the two-argument arctangent as an atan overload
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& y, const B& x) { detail::atan2_impl(y, x); })
SHADER_MATH_INLINE auto atan(const A& y, const B& x) {
    return detail::atan2_impl(y, x);
}
template <class A, class B>
    requires(detail::ArgsOf<lang_tag, A, B> &&
             requires(const A& a, const B& b) { detail::mod_impl(a, b); })
SHADER_MATH_INLINE auto mod(const A& a, const B& b) {
    return detail::mod_impl(a, b);
}
template <class A, class B, class C>
    requires(detail::ArgsOf<lang_tag, A, B, C> &&
             requires(const A& a, const B& b, const C& c) { detail::mad_impl(a, b, c); })
SHADER_MATH_INLINE auto fma(const A& a, const B& b, const C& c) {
    return detail::mad_impl(a, b, c);
}

// frexp(x, out e): mantissa in [0.5, 1) with the sign of x, e as int (GLSL's
// int exponent; HLSL's frexp returns it as float)
template <class A, int N>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A> && detail::vec_size_v<A> == N)
SHADER_MATH_INLINE auto frexp(const A& a, detail::gvec<N, int>& e) {
    using T = detail::vec_elem_t<A>;
    const detail::vec_t<A> v = detail::to_vec(a);
    T m[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    int ex[4] = {0, 0, 0, 0};
    for (int i = 0; i < N; ++i) {
        if constexpr (std::is_same_v<T, double>)
            m[i] = std::frexp(v[i], &ex[i]);
        else
            m[i] = static_cast<T>(std::frexp(static_cast<float>(v[i]), &ex[i]));
    }
    e = detail::gvec<N, int>(detail::vops<int>::load4(ex));
    return detail::vec_t<A>(detail::vops<T>::load4(m));
}
template <class A, class T = detail::scalar_call_t<A>>
    requires detail::FloatScalarCall<A>
SHADER_MATH_INLINE T frexp(const A& a, int& e) {
    if constexpr (std::is_same_v<T, double>)
        return std::frexp(static_cast<double>(a), &e);
    else
        return static_cast<T>(std::frexp(static_cast<float>(static_cast<T>(a)), &e));
}
// ldexp(x, e) = x * 2^e with an int exponent
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVec<A> && detail::VecOf<B, lang_tag> &&
             detail::arg_elem_v<detail::vec_elem_t<B>, int, lang_tag> &&
             detail::vec_size_v<A> == detail::vec_size_v<B>)
SHADER_MATH_INLINE auto ldexp(const A& a, const B& b) {
    using T = detail::vec_elem_t<A>;
    const detail::vec_t<A> v = detail::to_vec(a);
    const detail::gvec<detail::vec_size_v<A>, int> e(detail::to_vec(b));
    T r[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < detail::vec_size_v<A>; ++i) {
        if constexpr (std::is_same_v<T, double>)
            r[i] = std::ldexp(v[i], e[i]);
        else
            r[i] = static_cast<T>(std::ldexp(static_cast<float>(v[i]), e[i]));
    }
    return detail::vec_t<A>(detail::vops<T>::load4(r));
}
template <class A, class I, class T = detail::scalar_call_t<A>>
    requires(detail::FloatScalarCall<A> && detail::is_int_scalar_v<I>)
SHADER_MATH_INLINE T ldexp(const A& a, I e) {
    if constexpr (std::is_same_v<T, double>)
        return std::ldexp(static_cast<double>(a), static_cast<int>(e));
    else
        return static_cast<T>(std::ldexp(static_cast<float>(static_cast<T>(a)), static_cast<int>(e)));
}

// floatBitsToInt / floatBitsToUint / intBitsToFloat / uintBitsToFloat: the
// same bits as another 32-bit type; an argument converts to the type the
// function takes as GLSL converts implicitly (intBitsToFloat(i16vec3))
#define SM_GLSL_BITCAST(name, From, U)                                                   \
    template <class A>                                                                   \
        requires(detail::VecOf<A, lang_tag> &&                                           \
                 detail::arg_elem_v<detail::vec_elem_t<A>, From, lang_tag>)               \
    SHADER_MATH_INLINE auto name(const A& a) {                                           \
        return detail::bitcast_vec<U>(detail::gvec<detail::vec_size_v<A>, From>(detail::to_vec(a))); \
    }                                                                                    \
    template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type> \
        requires(!detail::VecLike<S> && detail::arg_elem_v<V, From, lang_tag>)           \
    SHADER_MATH_INLINE U name(const S& s) {                                              \
        return detail::bitcast_scalar<U>(static_cast<From>(static_cast<V>(s)));          \
    }
SM_GLSL_BITCAST(floatBitsToInt, float, int)
SM_GLSL_BITCAST(floatBitsToUint, float, uint)
SM_GLSL_BITCAST(intBitsToFloat, int, float)
SM_GLSL_BITCAST(uintBitsToFloat, uint, float)
#undef SM_GLSL_BITCAST

#if defined(SHADER_MATH_HAS_HALF)
// float16BitsToInt16 / float16BitsToUint16 / int16BitsToFloat16 /
// uint16BitsToFloat16 (GL_EXT_shader_explicit_arithmetic_types): the same
// bits as another 16-bit type
#define SM_GLSL_BITCAST16(name, From, U)                                                 \
    template <class A>                                                                   \
        requires(detail::VecOf<A, lang_tag> &&                                           \
                 detail::arg_elem_v<detail::vec_elem_t<A>, From, lang_tag>)               \
    SHADER_MATH_INLINE auto name(const A& a) {                                           \
        return detail::map_int_lanes<U>(detail::gvec<detail::vec_size_v<A>, From>(detail::to_vec(a)), \
                                        [](From x) { return detail::bitcast_lane<U>(x); }); \
    }                                                                                    \
    template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type> \
        requires(!detail::VecLike<S> && detail::arg_elem_v<V, From, lang_tag>)           \
    SHADER_MATH_INLINE U name(const S& s) {                                              \
        return detail::bitcast_lane<U>(static_cast<From>(static_cast<V>(s)));            \
    }
SM_GLSL_BITCAST16(float16BitsToInt16, float16_t, int16_t)
SM_GLSL_BITCAST16(float16BitsToUint16, float16_t, uint16_t)
SM_GLSL_BITCAST16(int16BitsToFloat16, int16_t, float16_t)
SM_GLSL_BITCAST16(uint16BitsToFloat16, uint16_t, float16_t)
#undef SM_GLSL_BITCAST16
#endif

// doubleBitsToInt64 / doubleBitsToUint64 / int64BitsToDouble /
// uint64BitsToDouble (GL_EXT_shader_explicit_arithmetic_types): the same
// bits as another 64-bit type
#define SM_GLSL_BITCAST64(name, From, U)                                                       \
    template <class A>                                                                         \
        requires(detail::VecOf<A, lang_tag> &&                                                 \
                 detail::arg_elem_v<detail::vec_elem_t<A>, From, lang_tag>)                    \
    SHADER_MATH_INLINE auto name(const A& a) {                                                 \
        return detail::map_int_lanes<U>(                                                       \
            detail::gvec<detail::vec_size_v<A>, From>(detail::to_vec(a)),                      \
            [](From x) { return detail::bitcast_lane<U>(x); });                                \
    }                                                                                          \
    template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type>  \
        requires(!detail::VecLike<S> && detail::arg_elem_v<V, From, lang_tag>)                 \
    SHADER_MATH_INLINE U name(const S& s) {                                                    \
        return detail::bitcast_lane<U>(static_cast<From>(static_cast<V>(s)));                  \
    }
SM_GLSL_BITCAST64(doubleBitsToInt64, double, int64_t)
SM_GLSL_BITCAST64(doubleBitsToUint64, double, uint64_t)
SM_GLSL_BITCAST64(int64BitsToDouble, int64_t, double)
SM_GLSL_BITCAST64(uint64BitsToDouble, uint64_t, double)
#undef SM_GLSL_BITCAST64

// ---- integer functions (GLSL 8.8) -------------------------------------------
// bitCount / findMSB / findLSB take integers of 16, 32 and 64 bits and give a
// signed integer of the same width (findMSB on a signed integer: the highest
// bit that differs from the sign bit; -1 where there is no such bit).
// bitfieldReverse, bitfieldExtract and bitfieldInsert have int and uint
// overloads only: they keep a 32-bit type and take a 16-bit one as int, as
// GLSL converts it implicitly (offset and bits are int; a field outside the
// 32 bits, undefined in GLSL, is cut at bit 31 and negative values count as
// 0). uaddCarry / usubBorrow / umulExtended take uint, imulExtended int.
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::IntVec<A>)
SHADER_MATH_INLINE auto bitCount(const A& a) {
    using R = std::make_signed_t<detail::vec_elem_t<A>>;
    return detail::map_int_lanes<R>(a, [](auto x) { return static_cast<R>(detail::popcount_fn{}(x)); });
}
template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type>
    requires detail::is_int_lane_scalar_v<S>
SHADER_MATH_INLINE auto bitCount(const S& s) {
    return static_cast<std::make_signed_t<V>>(detail::popcount_fn{}(static_cast<V>(s)));
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::IntVec<A>)
SHADER_MATH_INLINE auto findMSB(const A& a) {
    using R = std::make_signed_t<detail::vec_elem_t<A>>;
    return detail::map_int_lanes<R>(a, [](auto x) { return static_cast<R>(detail::find_msb_lane(x)); });
}
template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type>
    requires detail::is_int_lane_scalar_v<S>
SHADER_MATH_INLINE auto findMSB(const S& s) {
    return static_cast<std::make_signed_t<V>>(detail::find_msb_lane(static_cast<V>(s)));
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::IntVec<A>)
SHADER_MATH_INLINE auto findLSB(const A& a) {
    using R = std::make_signed_t<detail::vec_elem_t<A>>;
    return detail::map_int_lanes<R>(a, [](auto x) {
        return static_cast<R>(detail::int_lane::lsb(static_cast<std::make_unsigned_t<decltype(x)>>(x)));
    });
}
template <class S, class V = typename detail::scalar_value<std::remove_cvref_t<S>>::type>
    requires detail::is_int_lane_scalar_v<S>
SHADER_MATH_INLINE auto findLSB(const S& s) {
    return static_cast<std::make_signed_t<V>>(detail::int_lane::lsb(static_cast<std::make_unsigned_t<V>>(s)));
}
template <class A, class T = detail::field_elem_t<lang_tag, detail::vec_elem_t<A>>>
    requires(detail::VecOf<A, lang_tag> && !std::is_void_v<T>)
SHADER_MATH_INLINE auto bitfieldReverse(const A& a) {
    return detail::map_int_lanes<T>(detail::gvec<detail::vec_size_v<A>, T>(detail::to_vec(a)),
                                    detail::reverse_fn{});
}
template <class S, class T = detail::field_elem_t<lang_tag, typename detail::scalar_value<std::remove_cvref_t<S>>::type>>
    requires(!detail::VecLike<S> && !std::is_void_v<T>)
SHADER_MATH_INLINE T bitfieldReverse(const S& s) {
    return detail::reverse_fn{}(static_cast<T>(s));
}
template <class A, class O, class B, class T = detail::field_elem_t<lang_tag, detail::vec_elem_t<A>>>
    requires(detail::VecOf<A, lang_tag> && !std::is_void_v<T> && std::same_as<O, int> && std::same_as<B, int>)
SHADER_MATH_INLINE auto bitfieldExtract(const A& a, O offset, B bits) {
    return detail::map_int_lanes<T>(detail::gvec<detail::vec_size_v<A>, T>(detail::to_vec(a)),
                                    [=](T x) { return detail::extract_lane(x, offset, bits); });
}
template <class S, class O, class B,
          class T = detail::field_elem_t<lang_tag, typename detail::scalar_value<std::remove_cvref_t<S>>::type>>
    requires(!detail::VecLike<S> && !std::is_void_v<T> && std::same_as<O, int> && std::same_as<B, int>)
SHADER_MATH_INLINE T bitfieldExtract(const S& s, O offset, B bits) {
    return detail::extract_lane(static_cast<T>(s), offset, bits);
}
template <class A, class I, class O, class B,
          class T = detail::field_elem_t<lang_tag, detail::vec_elem_t<A>, detail::vec_elem_t<I>>>
    requires(detail::VecOf<A, lang_tag> && detail::VecOf<I, lang_tag> &&
             detail::vec_size_v<A> == detail::vec_size_v<I> && !std::is_void_v<T> &&
             std::same_as<O, int> && std::same_as<B, int>)
SHADER_MATH_INLINE auto bitfieldInsert(const A& base, const I& insert, O offset, B bits) {
    constexpr int n = detail::vec_size_v<A>;
    const detail::gvec<n, T> b(detail::to_vec(base));
    const detail::gvec<n, T> v(detail::to_vec(insert));
    T r[4] = {T(0), T(0), T(0), T(0)};
    for (int i = 0; i < n; ++i)
        r[i] = static_cast<T>(detail::int_lane::insert(static_cast<uint>(b[i]), static_cast<uint>(v[i]),
                                                       offset, bits));
    return detail::gvec<n, T>(detail::vops<T>::load4(r));
}
template <class S, class I, class O, class B,
          class T = detail::field_elem_t<lang_tag, typename detail::scalar_value<std::remove_cvref_t<S>>::type,
                                         typename detail::scalar_value<std::remove_cvref_t<I>>::type>>
    requires(!detail::VecLike<S> && !detail::VecLike<I> && !std::is_void_v<T> && std::same_as<O, int> &&
             std::same_as<B, int>)
SHADER_MATH_INLINE T bitfieldInsert(const S& base, const I& insert, O offset, B bits) {
    return static_cast<T>(detail::int_lane::insert(static_cast<uint>(static_cast<T>(base)),
                                                   static_cast<uint>(static_cast<T>(insert)), offset, bits));
}

// uaddCarry(x, y, out carry) = x + y modulo 2^32, carry 1 where it overflows;
// usubBorrow(x, y, out borrow) = x - y modulo 2^32, borrow 1 where x < y;
// umulExtended / imulExtended(x, y, out msb, out lsb): the 64-bit product
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && std::is_same_v<detail::vec_elem_t<A>, uint> && detail::SameVec<A, B>)
SHADER_MATH_INLINE auto uaddCarry(const A& x, const B& y, detail::vec_t<A>& carry) {
    const detail::vec_t<A> s = detail::to_vec(x) + detail::to_vec(y);
    carry = detail::vec_t<A>(lessThan(s, detail::to_vec(x)));
    return s;
}
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && std::is_same_v<detail::vec_elem_t<A>, uint> && detail::SameVec<A, B>)
SHADER_MATH_INLINE auto usubBorrow(const A& x, const B& y, detail::vec_t<A>& borrow) {
    borrow = detail::vec_t<A>(lessThan(x, y));
    return detail::to_vec(x) - detail::to_vec(y);
}
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && std::is_same_v<detail::vec_elem_t<A>, uint> && detail::SameVec<A, B>)
SHADER_MATH_INLINE void umulExtended(const A& x, const B& y, detail::vec_t<A>& msb,
                                     detail::vec_t<A>& lsb) {
    detail::mul_extended(x, y, msb, lsb);
}
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && std::is_same_v<detail::vec_elem_t<A>, int> && detail::SameVec<A, B>)
SHADER_MATH_INLINE void imulExtended(const A& x, const B& y, detail::vec_t<A>& msb,
                                     detail::vec_t<A>& lsb) {
    detail::mul_extended(x, y, msb, lsb);
}
// scalar forms
template <class X, class Y>
    requires(detail::is_int32_scalar_v<X> && detail::is_int32_scalar_v<Y>)
SHADER_MATH_INLINE uint uaddCarry(const X& x0, const Y& y0, uint& carry) {
    const uint x = static_cast<uint>(x0);
    const uint y = static_cast<uint>(y0);
    const uint s = x + y;
    carry = s < x ? 1u : 0u;
    return s;
}
template <class X, class Y>
    requires(detail::is_int32_scalar_v<X> && detail::is_int32_scalar_v<Y>)
SHADER_MATH_INLINE uint usubBorrow(const X& x0, const Y& y0, uint& borrow) {
    const uint x = static_cast<uint>(x0);
    const uint y = static_cast<uint>(y0);
    borrow = x < y ? 1u : 0u;
    return x - y;
}
template <class X, class Y>
    requires(detail::is_int32_scalar_v<X> && detail::is_int32_scalar_v<Y>)
SHADER_MATH_INLINE void umulExtended(const X& x, const Y& y, uint& msb, uint& lsb) {
    const std::uint64_t p = static_cast<std::uint64_t>(static_cast<uint>(x)) * static_cast<uint>(y);
    msb = static_cast<uint>(p >> 32);
    lsb = static_cast<uint>(p);
}
template <class X, class Y>
    requires(std::is_same_v<typename detail::scalar_value<X>::type, int> &&
             std::is_same_v<typename detail::scalar_value<Y>::type, int>)
SHADER_MATH_INLINE void imulExtended(const X& x, const Y& y, int& msb, int& lsb) {
    const auto p = static_cast<std::uint64_t>(static_cast<std::int64_t>(static_cast<int>(x)) *
                                              static_cast<int>(y));
    msb = static_cast<int>(static_cast<uint>(p >> 32));
    lsb = static_cast<int>(static_cast<uint>(p));
}

// ---- packing (GLSL 8.4) ------------------------------------------------------
// packUnorm*: round(clamp(c, 0, 1) * (2^n - 1)); packSnorm*: round(clamp(c,
// -1, 1) * (2^(n-1) - 1)), ties to even; the first component in the low bits.
// unpack* divide back (Snorm clamped to -1). packHalf2x16 rounds to nearest
// even binary16.
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVecN<A, 2>)
SHADER_MATH_INLINE uint packUnorm2x16(const A& a) {
    const vec2 v = detail::to_vec(a);
    return detail::int_lane::unorm(v[0], 65535.0f) | (detail::int_lane::unorm(v[1], 65535.0f) << 16);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVecN<A, 2>)
SHADER_MATH_INLINE uint packSnorm2x16(const A& a) {
    const vec2 v = detail::to_vec(a);
    const auto lo = static_cast<uint>(detail::int_lane::snorm(v[0], 32767.0f)) & 0xFFFFu;
    const auto hi = static_cast<uint>(detail::int_lane::snorm(v[1], 32767.0f)) & 0xFFFFu;
    return lo | (hi << 16);
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVecN<A, 4>)
SHADER_MATH_INLINE uint packUnorm4x8(const A& a) {
    const vec4 v = detail::to_vec(a);
    uint r = 0u;
    for (int i = 0; i < 4; ++i) r |= detail::int_lane::unorm(v[i], 255.0f) << (8 * i);
    return r;
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVecN<A, 4>)
SHADER_MATH_INLINE uint packSnorm4x8(const A& a) {
    const vec4 v = detail::to_vec(a);
    uint r = 0u;
    for (int i = 0; i < 4; ++i)
        r |= (static_cast<uint>(detail::int_lane::snorm(v[i], 127.0f)) & 0xFFu) << (8 * i);
    return r;
}
template <class A>
    requires(detail::VecOf<A, lang_tag> && detail::FloatVecN<A, 2>)
SHADER_MATH_INLINE uint packHalf2x16(const A& a) {
    const vec2 v = detail::to_vec(a);
    return static_cast<uint>(detail::half_bits::from_f32(v[0])) |
           (static_cast<uint>(detail::half_bits::from_f32(v[1])) << 16);
}
template <class P>
    requires detail::ScalarArg<P, uint, lang_tag>
SHADER_MATH_INLINE vec2 unpackUnorm2x16(const P& p0) {
    const auto p = static_cast<uint>(p0);
    return vec2(static_cast<float>(p & 0xFFFFu) / 65535.0f, static_cast<float>(p >> 16) / 65535.0f);
}
template <class P>
    requires detail::ScalarArg<P, uint, lang_tag>
SHADER_MATH_INLINE vec2 unpackSnorm2x16(const P& p0) {
    const auto p = static_cast<uint>(p0);
    auto f = [](uint h) {
        const float v = static_cast<float>(static_cast<std::int16_t>(static_cast<std::uint16_t>(h))) / 32767.0f;
        return v < -1.0f ? -1.0f : v;
    };
    return vec2(f(p & 0xFFFFu), f(p >> 16));
}
template <class P>
    requires detail::ScalarArg<P, uint, lang_tag>
SHADER_MATH_INLINE vec4 unpackUnorm4x8(const P& p0) {
    const auto p = static_cast<uint>(p0);
    return vec4(static_cast<float>(p & 0xFFu) / 255.0f, static_cast<float>((p >> 8) & 0xFFu) / 255.0f,
                static_cast<float>((p >> 16) & 0xFFu) / 255.0f, static_cast<float>(p >> 24) / 255.0f);
}
template <class P>
    requires detail::ScalarArg<P, uint, lang_tag>
SHADER_MATH_INLINE vec4 unpackSnorm4x8(const P& p0) {
    const auto p = static_cast<uint>(p0);
    auto f = [](uint b) {
        const float v = static_cast<float>(static_cast<std::int8_t>(static_cast<std::uint8_t>(b))) / 127.0f;
        return v < -1.0f ? -1.0f : v;
    };
    return vec4(f(p & 0xFFu), f((p >> 8) & 0xFFu), f((p >> 16) & 0xFFu), f(p >> 24));
}
template <class P>
    requires detail::ScalarArg<P, uint, lang_tag>
SHADER_MATH_INLINE vec2 unpackHalf2x16(const P& p0) {
    const auto p = static_cast<uint>(p0);
    return vec2(detail::half_bits::to_f32(static_cast<unsigned short>(p & 0xFFFFu)),
                detail::half_bits::to_f32(static_cast<unsigned short>(p >> 16)));
}

// packInt2x16 / packUint2x16 / packInt4x16 / packUint4x16 / packInt2x32 /
// packUint2x32 / packFloat2x16 (GL_EXT_shader_explicit_arithmetic_types): the
// components' bits side by side, the first in the low bits; unpack* split
// them back
#define SM_GLSL_PACK(name, T, N, R)                                                      \
    template <class A>                                                                   \
        requires detail::VecArg<A, T, N, lang_tag>                                       \
    SHADER_MATH_INLINE R name(const A& a) {                                              \
        return detail::pack_lanes<R>(detail::gvec<N, T>(detail::to_vec(a)));             \
    }
#define SM_GLSL_UNPACK(name, P, T, N)                                                    \
    template <class S>                                                                   \
        requires detail::ScalarArg<S, P, lang_tag>                                       \
    SHADER_MATH_INLINE detail::gvec<N, T> name(const S& s) {                             \
        return detail::unpack_lanes<T, N, lang_tag>(static_cast<P>(s));                  \
    }
SM_GLSL_PACK(packInt2x16, int16_t, 2, int)
SM_GLSL_PACK(packUint2x16, uint16_t, 2, uint)
SM_GLSL_PACK(packInt4x16, int16_t, 4, int64_t)
SM_GLSL_PACK(packUint4x16, uint16_t, 4, uint64_t)
SM_GLSL_PACK(packInt2x32, int, 2, int64_t)
SM_GLSL_PACK(packUint2x32, uint, 2, uint64_t)
SM_GLSL_UNPACK(unpackInt2x16, int, int16_t, 2)
SM_GLSL_UNPACK(unpackUint2x16, uint, uint16_t, 2)
SM_GLSL_UNPACK(unpackInt4x16, int64_t, int16_t, 4)
SM_GLSL_UNPACK(unpackUint4x16, uint64_t, uint16_t, 4)
SM_GLSL_UNPACK(unpackInt2x32, int64_t, int, 2)
SM_GLSL_UNPACK(unpackUint2x32, uint64_t, uint, 2)
#if defined(SHADER_MATH_HAS_HALF)
SM_GLSL_PACK(packFloat2x16, float16_t, 2, uint)
SM_GLSL_UNPACK(unpackFloat2x16, uint, float16_t, 2)
#endif
#undef SM_GLSL_PACK
#undef SM_GLSL_UNPACK

// packDouble2x32 / unpackDouble2x32 (GLSL 8.4): low 32 bits in x, high in y
template <class A>
    requires detail::VecArg<A, uint, 2, lang_tag>
SHADER_MATH_INLINE double packDouble2x32(const A& a) {
    return detail::bitcast_lane<double>(
        detail::pack_lanes<uint64_t>(detail::gvec<2, uint>(detail::to_vec(a))));
}
template <class S>
    requires detail::ScalarArg<S, double, lang_tag>
SHADER_MATH_INLINE uvec2 unpackDouble2x32(const S& s) {
    return detail::unpack_lanes<uint, 2, lang_tag>(
        detail::bitcast_lane<uint64_t>(static_cast<double>(s)));
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
    template <int Cn, int Rn, class T, class U>                                          \
        requires detail::MixedElems<T, U, lang_tag>                                      \
    SHADER_MATH_INLINE auto operator sym(const matrix<Cn, Rn, T>& a,                     \
                                         const matrix<Cn, Rn, U>& b) {                   \
        using E = detail::common_elem_t<T, U, lang_tag>;                                 \
        return matrix<Cn, Rn, E>(a) sym matrix<Cn, Rn, E>(b);                            \
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
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator+(const matrix<Cn, Rn, T>& a) {
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator++(matrix<Cn, Rn, T>& a) {
    a = a + T(1.0f);
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator--(matrix<Cn, Rn, T>& a) {
    a = a - T(1.0f);
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator++(matrix<Cn, Rn, T>& a, int) {
    const matrix<Cn, Rn, T> old = a;
    a = a + T(1.0f);
    return old;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T> operator--(matrix<Cn, Rn, T>& a, int) {
    const matrix<Cn, Rn, T> old = a;
    a = a - T(1.0f);
    return old;
}

// ---- operator*: linear algebra (GLSL semantics) ------------------------------
// matrix * matrix: inner dimensions must match (enforced by the signature).
template <int C1, int R1, int C2, class T>
SHADER_MATH_INLINE matrix<C2, R1, T> operator*(const matrix<C1, R1, T>& a,
                                               const matrix<C2, C1, T>& b) {
    matrix<C2, R1, T> o;
    for (int j = 0; j < C2; ++j)
        o.col_[j] = detail::gvec<R1, T>(detail::mad_accum<C1>(b.col_[j].q_, a.col_));
    return o;
}
template <int C1, int R1, int C2, class T, class U>
    requires detail::MixedElems<T, U, lang_tag>
SHADER_MATH_INLINE auto operator*(const matrix<C1, R1, T>& a, const matrix<C2, C1, U>& b) {
    using E = detail::common_elem_t<T, U, lang_tag>;
    return matrix<C1, R1, E>(a) * matrix<C2, C1, E>(b);
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
    requires std::is_same_v<detail::vec_t<V>, detail::gvec<Cn, T>>
SHADER_MATH_INLINE detail::gvec<Rn, T> operator*(const matrix<Cn, Rn, T>& m, const V& v0) {
    const detail::gvec<Cn, T> v = detail::to_vec(v0);
    return detail::gvec<Rn, T>(detail::mad_accum<Cn>(v.q_, m.col_));
}
template <int Cn, int Rn, class T, class V>
    requires(detail::VecOf<V, lang_tag> && detail::vec_size_v<V> == Cn &&
             detail::MixedElems<T, detail::vec_elem_t<V>, lang_tag>)
SHADER_MATH_INLINE auto operator*(const matrix<Cn, Rn, T>& m, const V& v) {
    using E = detail::common_elem_t<T, detail::vec_elem_t<V>, lang_tag>;
    return matrix<Cn, Rn, E>(m) * detail::gvec<Cn, E>(detail::to_vec(v));
}

// row vector * matrix
template <class V, int Cn, int Rn, class T>
    requires std::is_same_v<detail::vec_t<V>, detail::gvec<Rn, T>>
SHADER_MATH_INLINE detail::gvec<Cn, T> operator*(const V& v0, const matrix<Cn, Rn, T>& m) {
    const detail::gvec<Rn, T> v = detail::to_vec(v0);
    T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int c = 0; c < Cn; ++c) t[c] = detail::dot(v, m.col_[c]);
    return detail::gvec<Cn, T>(detail::vops<T>::load4(t));
}
template <class V, int Cn, int Rn, class T>
    requires(detail::VecOf<V, lang_tag> && detail::vec_size_v<V> == Rn &&
             detail::MixedElems<detail::vec_elem_t<V>, T, lang_tag>)
SHADER_MATH_INLINE auto operator*(const V& v, const matrix<Cn, Rn, T>& m) {
    using E = detail::common_elem_t<detail::vec_elem_t<V>, T, lang_tag>;
    return detail::gvec<Rn, E>(detail::to_vec(v)) * matrix<Cn, Rn, E>(m);
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

// v *= m is v = v * m, which keeps v's width only for a square m
template <int N, class T>
SHADER_MATH_INLINE detail::gvec<N, T>& operator*=(detail::gvec<N, T>& v,
                                                 const matrix<N, N, T>& m) {
    v = v * m;
    return v;
}
template <int N, class T, class U>
    requires detail::implicit_elem_v<U, T, lang_tag>
SHADER_MATH_INLINE detail::gvec<N, T>& operator*=(detail::gvec<N, T>& v,
                                                 const matrix<N, N, U>& m) {
    v = v * matrix<N, N, T>(m);
    return v;
}

// compound assignment
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator+=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a + b;
    return a;
}
template <int Cn, int Rn, class T, class U>
    requires detail::implicit_elem_v<U, T, lang_tag>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator+=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, U>& b) {
    a = a + matrix<Cn, Rn, T>(b);
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator-=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a - b;
    return a;
}
template <int Cn, int Rn, class T, class U>
    requires detail::implicit_elem_v<U, T, lang_tag>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator-=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, U>& b) {
    a = a - matrix<Cn, Rn, T>(b);
    return a;
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator/=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, T>& b) {
    a = a / b;
    return a;
}
template <int Cn, int Rn, class T, class U>
    requires detail::implicit_elem_v<U, T, lang_tag>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator/=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Rn, U>& b) {
    a = a / matrix<Cn, Rn, T>(b);
    return a;
}
// m *= n requires the product to keep m's shape, i.e. square n (GLSL rule)
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator*=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Cn, T>& b) {
    a = a * b;
    return a;
}
template <int Cn, int Rn, class T, class U>
    requires detail::implicit_elem_v<U, T, lang_tag>
SHADER_MATH_INLINE matrix<Cn, Rn, T>& operator*=(matrix<Cn, Rn, T>& a,
                                                 const matrix<Cn, Cn, U>& b) {
    a = a * matrix<Cn, Cn, T>(b);
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
    requires(detail::SameVec<A, B> && detail::VecOf<A, lang_tag>)
SHADER_MATH_INLINE bool operator==(const A& a, const B& b) {
    constexpr int n = detail::vec_size_v<A>;
    using T = detail::vec_elem_t<A>;
    return detail::ops::all_n(detail::vops<T>::cmp_eq(detail::to_vec(a).q_, detail::to_vec(b).q_),
                              n);
}
template <class A, class B>
    requires(detail::MixedVec<A, B> && detail::VecOf<A, lang_tag>)
SHADER_MATH_INLINE bool operator==(const A& a, const B& b) {
    using M = detail::mixed_t<A, B>;
    return M(detail::to_vec(a)) == M(detail::to_vec(b));
}
template <class A, class B>
    requires((detail::SameVec<A, B> || detail::MixedVec<A, B>) && detail::VecOf<A, lang_tag>)
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
template <int Cn, int Rn, class T, class U>
    requires detail::MixedElems<T, U, lang_tag>
SHADER_MATH_INLINE bool operator==(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, U>& b) {
    using E = detail::common_elem_t<T, U, lang_tag>;
    return matrix<Cn, Rn, E>(a) == matrix<Cn, Rn, E>(b);
}
template <int Cn, int Rn, class T>
SHADER_MATH_INLINE bool operator!=(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, T>& b) {
    return !(a == b);
}
template <int Cn, int Rn, class T, class U>
    requires detail::MixedElems<T, U, lang_tag>
SHADER_MATH_INLINE bool operator!=(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, U>& b) {
    return !(a == b);
}
template <int N>
SHADER_MATH_INLINE bool operator==(const detail::bvec<N, lang_tag>& a,
                                   const detail::bvec<N, lang_tag>& b) {
    for (int i = 0; i < N; ++i)
        if (a[i] != b[i]) return false;
    return true;
}
template <int N>
SHADER_MATH_INLINE bool operator!=(const detail::bvec<N, lang_tag>& a,
                                   const detail::bvec<N, lang_tag>& b) {
    return !(a == b);
}

// ---- per-component comparison functions (GLSL 8.7) ---------------------------
#define SM_GLSL_CMP_FN(name, fn)                                                         \
    template <class A, class B>                                                          \
        requires(detail::SameVec<A, B> && detail::VecOf<A, lang_tag>)                    \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                               \
        constexpr int n = detail::vec_size_v<A>;                                         \
        using T = detail::vec_elem_t<A>;                                                 \
        return detail::bvec<n, lang_tag>(                                                \
            detail::vops<T>::fn(detail::to_vec(a).q_, detail::to_vec(b).q_));            \
    }                                                                                    \
    template <class A, class B>                                                          \
        requires(detail::MixedVec<A, B> && detail::VecOf<A, lang_tag>)                   \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                               \
        using M = detail::mixed_t<A, B>;                                                 \
        return name(M(detail::to_vec(a)), M(detail::to_vec(b)));                         \
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
SHADER_MATH_INLINE detail::bvec<N, lang_tag> not_(const detail::bvec<N, lang_tag>& a) {
    return detail::bvec<N, lang_tag>(detail::ops::m_not(a.m_));
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
template <int Cn, int Rn, class T, class U>
    requires detail::MixedElems<T, U, lang_tag>
SHADER_MATH_INLINE auto matrixCompMult(const matrix<Cn, Rn, T>& a, const matrix<Cn, Rn, U>& b) {
    using E = detail::common_elem_t<T, U, lang_tag>;
    return matrixCompMult(matrix<Cn, Rn, E>(a), matrix<Cn, Rn, E>(b));
}

// outerProduct(c, r): c is a column (Rx1), r is a row (1xC) -> matCxR
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && detail::VecOf<B, lang_tag> && detail::FloatVec<A> &&
             std::is_same_v<detail::vec_elem_t<A>, detail::vec_elem_t<B>>)
SHADER_MATH_INLINE matrix<detail::vec_size_v<B>, detail::vec_size_v<A>, detail::vec_elem_t<A>>
outerProduct(const A& c0, const B& r0) {
    constexpr int RR = detail::vec_size_v<A>;
    constexpr int CC = detail::vec_size_v<B>;
    using T = detail::vec_elem_t<A>;
    const detail::gvec<RR, T> c = detail::to_vec(c0);
    const detail::gvec<CC, T> r = detail::to_vec(r0);
    matrix<CC, RR, T> o;
    detail::regs_fill<CC>(o.col_, [&](int j) {
        return detail::vops<T>::mul(c.q_, detail::vops<T>::splat(r[j]));
    });
    return o;
}
template <class A, class B>
    requires(detail::VecOf<A, lang_tag> && detail::VecOf<B, lang_tag> &&
             detail::MixedElems<detail::vec_elem_t<A>, detail::vec_elem_t<B>, lang_tag> &&
             !detail::is_int_elem_v<
                 detail::common_elem_t<detail::vec_elem_t<A>, detail::vec_elem_t<B>, lang_tag>>)
SHADER_MATH_INLINE auto outerProduct(const A& c, const B& r) {
    using E = detail::common_elem_t<detail::vec_elem_t<A>, detail::vec_elem_t<B>, lang_tag>;
    return outerProduct(detail::gvec<detail::vec_size_v<A>, E>(detail::to_vec(c)),
                        detail::gvec<detail::vec_size_v<B>, E>(detail::to_vec(r)));
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
