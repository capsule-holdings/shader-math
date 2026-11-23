// Vector types of both language namespaces.
//   detail::fvec<N, T, Lg> is one template for both languages; Lg is the
//   language tag (hlsl::lang_tag / glsl::lang_tag), so hlsl::float3 and
//   glsl::vec3 are distinct types with the same layout and arithmetic.
//   The tag also makes the language namespace an associated namespace: a
//   language's operators and functions are found by ADL on its own vectors
//   only. Conversion between the languages is explicit.
//   bvec<N, Lg> is the comparison-result type (mask register, no arithmetic);
//   it is shared by float and half vectors (half comparisons widen to f32).
// Full swizzle support via proxy members (read: implicit conversion,
// write: constrained operator=; duplicate/out-of-range writes do not compile).
// Storage is always a 4-lane register. Lanes >= N are unspecified (splats,
// scalar operands and division leave arbitrary values, NaN included), so
// every operation must read lanes [0, N) only (tests/test_pad_lanes.cpp).
#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>

#include "simd.hpp"

#if defined(min) || defined(max)
#  error "shader-math: min/max are defined as macros (windows.h?); define NOMINMAX first"
#endif

namespace shader_math {
namespace hlsl {
struct lang_tag {};
}
namespace glsl {
struct lang_tag {};
}
namespace detail {

template <int N, class T, class Lg>
struct fvec;
template <int N, class Lg>
struct bvec;
template <int N, class T = float>
using hvec = fvec<N, T, hlsl::lang_tag>;
template <int N, class T = float>
using gvec = fvec<N, T, glsl::lang_tag>;
template <class Lg>
inline constexpr bool is_glsl_v = std::is_same_v<Lg, glsl::lang_tag>;

// Element type -> register type / ops implementation.
template <class T>
struct vops_for;
template <>
struct vops_for<float> {
    using ops_t = ops;
    using reg = freg;
};
#if defined(SHADER_MATH_HAS_HALF)
template <>
struct vops_for<half_t> {
    using ops_t = hops;
    using reg = hreg;
};
#endif
template <>
struct vops_for<std::int32_t> {
    using ops_t = iops;
    using reg = ireg;
};
template <>
struct vops_for<std::uint32_t> {
    using ops_t = uops;
    using reg = ireg;
};
template <>
struct vops_for<std::int16_t> {
    using ops_t = narrow_ops<std::int16_t>;
    using reg = nreg<std::int16_t>;
};
template <>
struct vops_for<std::uint16_t> {
    using ops_t = narrow_ops<std::uint16_t>;
    using reg = nreg<std::uint16_t>;
};
template <>
struct vops_for<std::int64_t> {
    using ops_t = wide_ops<std::int64_t>;
    using reg = wreg<std::int64_t>;
};
template <>
struct vops_for<std::uint64_t> {
    using ops_t = wide_ops<std::uint64_t>;
    using reg = wreg<std::uint64_t>;
};
template <class T>
using vops = typename vops_for<T>::ops_t;
template <class T>
using vreg = typename vops_for<T>::reg;

// Element kinds: the floating-point functions take float and half vectors,
// the integer operators and bit functions the int / uint vectors of 16, 32
// and 64 bits (docs/design/int_vectors.md); each bit cast and packing
// function takes the width it reinterprets
template <class T>
inline constexpr bool is_int32_elem_v =
    std::is_same_v<T, std::int32_t> || std::is_same_v<T, std::uint32_t>;
template <class T>
inline constexpr bool is_int_elem_v =
    is_int32_elem_v<T> || std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t> ||
    std::is_same_v<T, std::int64_t> || std::is_same_v<T, std::uint64_t>;

// Implicit conversions between the integer and float element types, per
// language: HLSL converts between any of them either way; GLSL widens only
// (to an integer of more bits or of the same bits unsigned, and to float from
// 32 bits or fewer). half and bool always convert explicitly.
template <class T>
inline constexpr bool is_num32_v = std::is_same_v<T, float> || is_int32_elem_v<T>;
template <class T>
inline constexpr bool is_num_v = std::is_same_v<T, float> || is_int_elem_v<T>;
template <class U, class T, class Lg>
constexpr bool implicit_elem() {
    if constexpr (std::is_same_v<U, T> || !is_num_v<U> || !is_num_v<T>) {
        return false;
    } else if constexpr (is_glsl_v<Lg>) {
        if constexpr (std::is_same_v<U, float>)
            return false;
        else if constexpr (std::is_same_v<T, float>)
            return sizeof(U) <= 4;
        else
            return sizeof(T) > sizeof(U) ||
                   (sizeof(T) == sizeof(U) && std::is_signed_v<U> && std::is_unsigned_v<T>);
    } else {
        return true;
    }
}
template <class U, class T, class Lg>
inline constexpr bool implicit_elem_v = implicit_elem<U, T, Lg>();
// a function argument of element U where the function takes T
template <class U, class T, class Lg>
inline constexpr bool arg_elem_v = std::is_same_v<U, T> || implicit_elem_v<U, T, Lg>;
// The 32-bit integer a GLSL bitfield function (bitfieldExtract / Insert /
// Reverse, which have int and uint overloads only) computes in for arguments
// of elements Us: int when they all convert to int (glslang takes int for
// uint16 as well), else uint when they all convert to uint; void when neither
template <class Lg, class... Us>
using field_elem_t =
    std::conditional_t<(arg_elem_v<Us, std::int32_t, Lg> && ...), std::int32_t,
                       std::conditional_t<(arg_elem_v<Us, std::uint32_t, Lg> && ...), std::uint32_t, void>>;
// The type mixed operands compute in: HLSL takes float when either is float,
// else the wider integer, unsigned when either is (uint16 + int is a uint);
// GLSL converts one to the other implicitly (uint16 + int is an int, int64 +
// float does not compile). void where there is none.
template <class A, class B, class Lg>
struct common_elem {
    using type = void;
};
template <class A, class B, class Lg>
    requires(is_num_v<A> && is_num_v<B>)
struct common_elem<A, B, Lg> {
    static constexpr auto pick() {
        if constexpr (std::is_same_v<A, B>) {
            return std::type_identity<A>{};
        } else if constexpr (is_glsl_v<Lg>) {
            if constexpr (implicit_elem_v<A, B, Lg>)
                return std::type_identity<B>{};
            else if constexpr (implicit_elem_v<B, A, Lg>)
                return std::type_identity<A>{};
            else
                return std::type_identity<void>{};
        } else if constexpr (std::is_same_v<A, float> || std::is_same_v<B, float>) {
            return std::type_identity<float>{};
        } else {
            constexpr std::size_t w = sizeof(A) > sizeof(B) ? sizeof(A) : sizeof(B);
            constexpr bool u = std::is_unsigned_v<A> || std::is_unsigned_v<B>;
            using S = std::conditional_t<w == 2, std::int16_t,
                                         std::conditional_t<w == 4, std::int32_t, std::int64_t>>;
            return std::type_identity<std::conditional_t<u, std::make_unsigned_t<S>, S>>{};
        }
    }
    using type = typename decltype(pick())::type;
};
template <class A, class B, class Lg>
using common_elem_t = typename common_elem<A, B, Lg>::type;
// two different element types among the integers and float that combine in
// language Lg (mixed operands)
template <class T, class U, class Lg>
concept MixedElems = !std::is_same_v<T, U> && is_num_v<T> && is_num_v<U> &&
                     !std::is_void_v<common_elem_t<T, U, Lg>>;

// The value type of one constructor argument: itself, or the element type of
// a one-component swizzle (specialized after the swizzle types)
template <class U>
struct scalar_value {
    using type = U;
};
// One component converted for a constructor: float -> int / uint truncates,
// with NaN and out-of-range values given by int_lane::f2i / f2u (a plain
// static_cast is undefined there)
template <class T, class U>
SHADER_MATH_INLINE T elem_cast(const U& u) {
    using V = typename scalar_value<U>::type;
    if constexpr (is_int_elem_v<T> && !std::is_integral_v<V>) {
        return int_lane::f2int<T>(static_cast<float>(static_cast<V>(u)));
    } else {
        return static_cast<T>(u);
    }
}

// Constructor/assignment scalar rule: anything explicitly convertible to the
// element type is accepted and converted (HLSL/GLSL constructors convert).
// This is intentionally looser than the operator rule (ScalarFor below):
// half3 h(1.0f, 2.0f, 3.0f) works, half3 * 2.0f does not. Note that on GCC
// float -> _Float16 is not an implicit conversion (C++23 extended
// floating-point rank rules), so plain `T` parameters would reject floats.
template <class S, class T>
concept CvtScalar = requires(const S& s) { static_cast<T>(s); };
// Assignment to one component (v.x = s): what the constructors accept, but
// GLSL has no implicit float or bool -> int conversion (ivec.x = 2.5 fails)
template <class S, class T, class Lg>
concept CompAssign =
    CvtScalar<S, T> &&
    (!is_glsl_v<Lg> || !is_int_elem_v<T> ||
     (std::is_integral_v<typename scalar_value<std::remove_cvref_t<S>>::type> &&
      !std::is_same_v<typename scalar_value<std::remove_cvref_t<S>>::type, bool>));

// Vector-like operands (vectors and swizzles of two or more components) are
// described by vec_traits; its specializations follow the types below.
template <class V>
struct vec_traits {
    static constexpr int size = 0;
    using lang = void;
};
template <class V>
inline constexpr int vec_size_v = vec_traits<std::remove_cvref_t<V>>::size;
template <class V>
using vec_elem_t = typename vec_traits<std::remove_cvref_t<V>>::elem;
template <class V>
using vec_lang_t = typename vec_traits<std::remove_cvref_t<V>>::lang;

// Matrices as component-list arguments (GLSL vec4(s, m2), HLSL
// float3x3(m2, ...)): mat_traits gives their components in the language's
// constructor order (hlsl row by row, glsl column by column). Specialized for
// both matrix types in matrix.hpp.
template <class M>
struct mat_traits {
    static constexpr int size = 0;
};
template <class M>
inline constexpr int mat_size_v = mat_traits<std::remove_cvref_t<M>>::size;

// Component-list constructors (GLSL vec3(v2, w2) / mat2(v4), HLSL
// float2x2(v2, 3.0, 4.0)): the components an argument contributes, -1 when it
// is not a scalar, a vector of language Lg whose elements convert to T
// (constructors convert: float4(v2, h2) mixes float and half), or, where Mat
// allows it, such a matrix
template <class T, class Lg, bool Mat, class A>
constexpr int ctor_arg_size() {
    if constexpr (vec_size_v<A> >= 2) {
        return CvtScalar<vec_elem_t<A>, T> && std::is_same_v<vec_lang_t<A>, Lg>
                   ? vec_size_v<A>
                   : -1;
    } else if constexpr (mat_size_v<A> > 0) {
        using MT = mat_traits<std::remove_cvref_t<A>>;
        return Mat && CvtScalar<typename MT::elem, T> && std::is_same_v<typename MT::lang, Lg>
                   ? MT::size
                   : -1;
    } else {
        return CvtScalar<A, T> ? 1 : -1;
    }
}
// 1: the arguments supply exactly Need components; 2: more, every argument
// contributing and only the last one cut short (the GLSL rule); 0: neither
template <int Need, class T, class Lg, bool Mat, class... As>
constexpr int ctor_fill() {
    if constexpr (sizeof...(As) == 0) {
        return 0;
    } else {
        constexpr int sizes[] = {ctor_arg_size<T, Lg, Mat, As>()...};
        int total = 0;
        for (int n : sizes) {
            if (n < 0) return 0;
            total += n;
        }
        if (total == Need) return 1;
        return total > Need && total - sizes[sizeof...(As) - 1] < Need ? 2 : 0;
    }
}
// At least one vector argument (all-scalar lists have their own constructors)
template <class... As>
inline constexpr bool any_vector_arg = ((vec_size_v<As> >= 2) || ...);
template <class... As>
inline constexpr bool any_matrix_arg = ((mat_size_v<As> > 0) || ...);
// A vector or matrix argument whose element type is not T
template <class T, class A>
constexpr bool foreign_elem() {
    if constexpr (vec_size_v<A> >= 2) {
        return !std::is_same_v<vec_elem_t<A>, T>;
    } else if constexpr (mat_size_v<A> > 0) {
        return !std::is_same_v<typename mat_traits<std::remove_cvref_t<A>>::elem, T>;
    } else {
        return false;
    }
}
template <class T, class... As>
inline constexpr bool any_foreign_elem = (foreign_elem<T, As>() || ...);
// The first n components of the arguments, in order
template <class T, class A>
SHADER_MATH_INLINE void put_components(T* out, int& k, int n, const A& a) {
    if constexpr (vec_size_v<A> >= 2) {
        const auto v = to_vec(a);
        for (int i = 0; i < vec_size_v<A> && k < n; ++i) out[k++] = elem_cast<T>(v[i]);
    } else if constexpr (mat_size_v<A> > 0) {
        using MT = mat_traits<std::remove_cvref_t<A>>;
        for (int i = 0; i < MT::size && k < n; ++i) out[k++] = elem_cast<T>(MT::at(a, i));
    } else if (k < n) {
        out[k++] = elem_cast<T>(a);
    }
}
template <class T, class... As>
SHADER_MATH_INLINE void gather_components(T* out, int n, const As&... as) {
    int k = 0;
    (put_components<T>(out, k, n, as), ...);
}

template <int... Is>
constexpr bool swz_unique() {
    constexpr int n = static_cast<int>(sizeof...(Is));
    const int a[n] = {Is...};
    for (int i = 0; i < n; ++i)
        for (int j = i + 1; j < n; ++j)
            if (a[i] == a[j]) return false;
    return true;
}

// ---- swizzles of swizzles (v.xy.yx), opt-in -------------------------------
// GLSL / HLSL read any swizzle of a swizzle. Here the nested members exist
// only under SHADER_MATH_NESTED_SWIZZLE, and only on 2- and 3-component
// swizzles: they multiply the member count of every vector type (about 2x the
// front-end time of a TU that includes the library), and on 4-component ones
// another order of magnitude. Write vec4(v.wzyx).xy there.
#if defined(SHADER_MATH_NESTED_SWIZZLE)

// Read-only proxy for a nested swizzle; Js are lanes of the original
// register, already composed. Assignment is deleted: a trivial copy would
// overwrite the whole register.
template <class T, int N, class Lg, int... Js>
struct SHADER_MATH_MAY_ALIAS nswz {
    static constexpr int K = static_cast<int>(sizeof...(Js));
    T d[4];

    nswz() = default;
    nswz(const nswz&) = default;
    nswz& operator=(const nswz&) = delete;

    SHADER_MATH_INLINE operator T() const
        requires(K == 1)
    {
        constexpr int j[] = {Js...};
        return d[j[0]];
    }
    SHADER_MATH_INLINE operator fvec<(K >= 2 ? K : 2), T, Lg>() const
        requires(K >= 2)
    {
        return fvec<(K >= 2 ? K : 2), T, Lg>(d[Js]...);
    }
};

// Storage of a swizzle proxy: the register bytes it overlays, plus the nested
// members of 2- and 3-component swizzles. I0, I1 (, I2) are the register
// lanes of the enclosing swizzle; the members name them directly, since an
// array lookup there costs a constant evaluation per member (tens of
// thousands per TU).
template <int M, class T, int N, class Lg, int... Is>
struct SHADER_MATH_MAY_ALIAS swz_store {
    T d[4];
};

#  define SM_NW1(name, i0) ::shader_math::detail::nswz<T, N, Lg, I##i0> name;
#  define SM_NW2(name, i0, i1) ::shader_math::detail::nswz<T, N, Lg, I##i0, I##i1> name;
#  define SM_NW3(name, i0, i1, i2) \
      ::shader_math::detail::nswz<T, N, Lg, I##i0, I##i1, I##i2> name;
#  define SM_NW4(name, i0, i1, i2, i3) \
      ::shader_math::detail::nswz<T, N, Lg, I##i0, I##i1, I##i2, I##i3> name;
// two-letter set (L0, L1); the parameters never share a spelling with the
// pasted letters, which arrive as arguments
#  define SM_NA2_2(pre, i0, L0, L1) SM_NW2(pre##L0, i0, 0) SM_NW2(pre##L1, i0, 1)
#  define SM_NA2_3(pre, i0, i1, L0, L1) SM_NW3(pre##L0, i0, i1, 0) SM_NW3(pre##L1, i0, i1, 1)
#  define SM_NA2_P3(pre, i0, L0, L1) \
      SM_NA2_3(pre##L0, i0, 0, L0, L1) SM_NA2_3(pre##L1, i0, 1, L0, L1)
#  define SM_NA2_4(pre, i0, i1, i2, L0, L1) \
      SM_NW4(pre##L0, i0, i1, i2, 0) SM_NW4(pre##L1, i0, i1, i2, 1)
#  define SM_NA2_P4(pre, i0, i1, L0, L1) \
      SM_NA2_4(pre##L0, i0, i1, 0, L0, L1) SM_NA2_4(pre##L1, i0, i1, 1, L0, L1)
#  define SM_NA2_C4(pre, i0, L0, L1) \
      SM_NA2_P4(pre##L0, i0, 0, L0, L1) SM_NA2_P4(pre##L1, i0, 1, L0, L1)
#  define SM_NGEN2(L0, L1)                                \
      SM_NW1(L0, 0) SM_NW1(L1, 1)                         \
      SM_NA2_2(L0, 0, L0, L1) SM_NA2_2(L1, 1, L0, L1)     \
      SM_NA2_P3(L0, 0, L0, L1) SM_NA2_P3(L1, 1, L0, L1)   \
      SM_NA2_C4(L0, 0, L0, L1) SM_NA2_C4(L1, 1, L0, L1)
// three-letter set (L0, L1, L2)
#  define SM_NA3_2(pre, i0, L0, L1, L2) \
      SM_NW2(pre##L0, i0, 0) SM_NW2(pre##L1, i0, 1) SM_NW2(pre##L2, i0, 2)
#  define SM_NA3_3(pre, i0, i1, L0, L1, L2) \
      SM_NW3(pre##L0, i0, i1, 0) SM_NW3(pre##L1, i0, i1, 1) SM_NW3(pre##L2, i0, i1, 2)
#  define SM_NA3_P3(pre, i0, L0, L1, L2)                                        \
      SM_NA3_3(pre##L0, i0, 0, L0, L1, L2) SM_NA3_3(pre##L1, i0, 1, L0, L1, L2) \
      SM_NA3_3(pre##L2, i0, 2, L0, L1, L2)
#  define SM_NA3_4(pre, i0, i1, i2, L0, L1, L2)                     \
      SM_NW4(pre##L0, i0, i1, i2, 0) SM_NW4(pre##L1, i0, i1, i2, 1) \
      SM_NW4(pre##L2, i0, i1, i2, 2)
#  define SM_NA3_P4(pre, i0, i1, L0, L1, L2)                                            \
      SM_NA3_4(pre##L0, i0, i1, 0, L0, L1, L2) SM_NA3_4(pre##L1, i0, i1, 1, L0, L1, L2) \
      SM_NA3_4(pre##L2, i0, i1, 2, L0, L1, L2)
#  define SM_NA3_C4(pre, i0, L0, L1, L2)                                          \
      SM_NA3_P4(pre##L0, i0, 0, L0, L1, L2) SM_NA3_P4(pre##L1, i0, 1, L0, L1, L2) \
      SM_NA3_P4(pre##L2, i0, 2, L0, L1, L2)
#  define SM_NGEN3(L0, L1, L2)                                                             \
      SM_NW1(L0, 0) SM_NW1(L1, 1) SM_NW1(L2, 2)                                            \
      SM_NA3_2(L0, 0, L0, L1, L2) SM_NA3_2(L1, 1, L0, L1, L2) SM_NA3_2(L2, 2, L0, L1, L2)  \
      SM_NA3_P3(L0, 0, L0, L1, L2) SM_NA3_P3(L1, 1, L0, L1, L2)                            \
      SM_NA3_P3(L2, 2, L0, L1, L2)                                                         \
      SM_NA3_C4(L0, 0, L0, L1, L2) SM_NA3_C4(L1, 1, L0, L1, L2)                            \
      SM_NA3_C4(L2, 2, L0, L1, L2)
template <class T, int N, class Lg, int I0, int I1>
struct SHADER_MATH_MAY_ALIAS swz_store<2, T, N, Lg, I0, I1> {
    union {
        T d[4];
        SM_NGEN2(x, y) SM_NGEN2(r, g)
    };
};
template <class T, int N, class Lg, int I0, int I1, int I2>
struct SHADER_MATH_MAY_ALIAS swz_store<3, T, N, Lg, I0, I1, I2> {
    union {
        T d[4];
        SM_NGEN3(x, y, z) SM_NGEN3(r, g, b)
    };
};
template <class T, int N, int I0, int I1>
struct SHADER_MATH_MAY_ALIAS swz_store<2, T, N, glsl::lang_tag, I0, I1> {
    using Lg = glsl::lang_tag;
    union {
        T d[4];
        SM_NGEN2(x, y) SM_NGEN2(r, g) SM_NGEN2(s, t)
    };
};
template <class T, int N, int I0, int I1, int I2>
struct SHADER_MATH_MAY_ALIAS swz_store<3, T, N, glsl::lang_tag, I0, I1, I2> {
    using Lg = glsl::lang_tag;
    union {
        T d[4];
        SM_NGEN3(x, y, z) SM_NGEN3(r, g, b) SM_NGEN3(s, t, p)
    };
};
#  undef SM_NW1
#  undef SM_NW2
#  undef SM_NW3
#  undef SM_NW4
#  undef SM_NA2_2
#  undef SM_NA2_3
#  undef SM_NA2_P3
#  undef SM_NA2_4
#  undef SM_NA2_P4
#  undef SM_NA2_C4
#  undef SM_NGEN2
#  undef SM_NA3_2
#  undef SM_NA3_3
#  undef SM_NA3_P3
#  undef SM_NA3_4
#  undef SM_NA3_P4
#  undef SM_NA3_C4
#  undef SM_NGEN3
#endif

// Swizzle proxy. Lives inside fvec's union; d overlays the register storage.
// T = element type, N = parent vector width, Lg = language tag,
// Is... = selected component indices.
template <class T, int N, class Lg, int... Is>
struct SHADER_MATH_MAY_ALIAS swz
#if defined(SHADER_MATH_NESTED_SWIZZLE)
    : swz_store<static_cast<int>(sizeof...(Is)), T, N, Lg, Is...>
#endif
{
    static constexpr int M = static_cast<int>(sizeof...(Is));
    static constexpr int idx[M] = {Is...};
    static constexpr bool writable = swz_unique<Is...>();
#if defined(SHADER_MATH_NESTED_SWIZZLE)
    using swz_store<M, T, N, Lg, Is...>::d;
#else
    T d[4];
#endif

    // Trivial copy (a proxy passed by value, e.g. float3(v.x, v.y, v.z)) is
    // declared explicitly because the copy assignment below is user-defined.
    swz() = default;
    swz(const swz&) = default;

    // ---- reads --------------------------------------------------------
    SHADER_MATH_INLINE operator T() const
        requires(M == 1)
    {
        return d[idx[0]];
    }
    // Width is clamped in the *declaration* so that fvec<1, ...> is never named:
    // MSVC instantiates member declaration types while building the enclosing
    // class, which would trip fvec's width static_assert for M == 1 swizzles.
    // The requires-clause still limits the operator to M >= 2.
    SHADER_MATH_INLINE operator fvec<(M >= 2 ? M : 2), T, Lg>() const
        requires(M >= 2)
    {
        return fvec<(M >= 2 ? M : 2), T, Lg>(d[Is]...);
    }

    // ---- writes (constraint-based: misuse is a compile error) ----------
    template <class S>
        requires(M == 1 && writable && CompAssign<S, T, Lg>)
    SHADER_MATH_INLINE swz& operator=(const S& s) {
        d[idx[0]] = elem_cast<T>(s);
        return *this;
    }
    SHADER_MATH_INLINE swz& operator=(const fvec<(M >= 2 ? M : 2), T, Lg>& v)
        requires(M >= 2 && writable)
    {
        T t[4];
        vops<T>::store4(v.q_, t);
        int k = 0;
        ((d[Is] = t[k++]), ...);
        return *this;
    }
    SHADER_MATH_INLINE swz& operator=(const swz& o)
        requires(writable)
    {
        if constexpr (M == 1) {
            d[idx[0]] = o.d[idx[0]];
        } else {
            *this = fvec<M, T, Lg>(o);  // gather first: handles aliasing (v.xy = v.yx)
        }
        return *this;
    }
    template <int N2, int... Js>
    SHADER_MATH_INLINE swz& operator=(const swz<T, N2, Lg, Js...>& o)
        requires(static_cast<int>(sizeof...(Js)) == M && M >= 2 && writable)
    {
        return *this = fvec<M, T, Lg>(o);
    }

    // ---- compound assignment -------------------------------------------
#define SM_SWZ_COMPOUND(sym)                                     \
    template <class S>                                           \
        requires(M == 1 && writable && CompAssign<S, T, Lg>)     \
    SHADER_MATH_INLINE swz& operator sym##=(const S& s) {        \
        d[idx[0]] sym## = elem_cast<T>(s);                       \
        return *this;                                            \
    }                                                            \
    template <class V>                                           \
    SHADER_MATH_INLINE swz& operator sym##=(const V& v)          \
        requires(M >= 2 && writable)                             \
    {                                                            \
        *this = fvec<M, T, Lg>(*this) sym v;                     \
        return *this;                                            \
    }

    SM_SWZ_COMPOUND(+)
    SM_SWZ_COMPOUND(-)
    SM_SWZ_COMPOUND(*)
    SM_SWZ_COMPOUND(/)
#undef SM_SWZ_COMPOUND

    // ++ / -- add or subtract 1 in the selected components (v.x++, v.xy++)
#define SM_SWZ_STEP(sym)                                         \
    SHADER_MATH_INLINE swz& operator sym##sym()                  \
        requires writable                                        \
    {                                                            \
        *this sym## = T(1.0f);                                   \
        return *this;                                            \
    }                                                            \
    SHADER_MATH_INLINE auto operator sym##sym(int)               \
        requires writable                                        \
    {                                                            \
        if constexpr (M == 1) {                                  \
            const T old = d[idx[0]];                             \
            d[idx[0]] = old sym T(1.0f);                         \
            return old;                                          \
        } else {                                                 \
            const fvec<M, T, Lg> old(*this);                     \
            *this = old sym T(1.0f);                             \
            return old;                                          \
        }                                                        \
    }
    SM_SWZ_STEP(+)
    SM_SWZ_STEP(-)
#undef SM_SWZ_STEP
};

// ---- swizzle member generation ------------------------------------------
// A vector of width K declares every swizzle of 1 to 4 components over its
// first K letters of each set (xyzw, rgba and, for glsl, stpq), so a
// component past the width (float2(...).z) is not a member at all. Swizzles
// that select the same lanes share one proxy type, whichever set names them.
// The letters arrive as arguments L0..; the parameters (pre, i0..i2, L0..L3)
// never share a spelling with them.
#define SM_SW1(name, i0) ::shader_math::detail::swz<T, N, Lg, i0> name;
#define SM_SW2(name, i0, i1) ::shader_math::detail::swz<T, N, Lg, i0, i1> name;
#define SM_SW3(name, i0, i1, i2) ::shader_math::detail::swz<T, N, Lg, i0, i1, i2> name;
#define SM_SW4(name, i0, i1, i2, i3) ::shader_math::detail::swz<T, N, Lg, i0, i1, i2, i3> name;

#define SM_S2_R2(pre, i0, L0, L1) \
    SM_SW2(pre##L0, i0, 0)        \
    SM_SW2(pre##L1, i0, 1)
#define SM_S2_R3(pre, i0, i1, L0, L1) \
    SM_SW3(pre##L0, i0, i1, 0)        \
    SM_SW3(pre##L1, i0, i1, 1)
#define SM_S2_P3(pre, i0, L0, L1)    \
    SM_S2_R3(pre##L0, i0, 0, L0, L1) \
    SM_S2_R3(pre##L1, i0, 1, L0, L1)
#define SM_S2_R4(pre, i0, i1, i2, L0, L1) \
    SM_SW4(pre##L0, i0, i1, i2, 0)        \
    SM_SW4(pre##L1, i0, i1, i2, 1)
#define SM_S2_P4(pre, i0, i1, L0, L1)    \
    SM_S2_R4(pre##L0, i0, i1, 0, L0, L1) \
    SM_S2_R4(pre##L1, i0, i1, 1, L0, L1)
#define SM_S2_C4(pre, i0, L0, L1)    \
    SM_S2_P4(pre##L0, i0, 0, L0, L1) \
    SM_S2_P4(pre##L1, i0, 1, L0, L1)
#define SM_SGEN2(L0, L1)    \
    SM_SW1(L0, 0)           \
    SM_SW1(L1, 1)           \
    SM_S2_R2(L0, 0, L0, L1) \
    SM_S2_R2(L1, 1, L0, L1) \
    SM_S2_P3(L0, 0, L0, L1) \
    SM_S2_P3(L1, 1, L0, L1) \
    SM_S2_C4(L0, 0, L0, L1) \
    SM_S2_C4(L1, 1, L0, L1)

#define SM_S3_R2(pre, i0, L0, L1, L2) \
    SM_SW2(pre##L0, i0, 0)            \
    SM_SW2(pre##L1, i0, 1)            \
    SM_SW2(pre##L2, i0, 2)
#define SM_S3_R3(pre, i0, i1, L0, L1, L2) \
    SM_SW3(pre##L0, i0, i1, 0)            \
    SM_SW3(pre##L1, i0, i1, 1)            \
    SM_SW3(pre##L2, i0, i1, 2)
#define SM_S3_P3(pre, i0, L0, L1, L2)    \
    SM_S3_R3(pre##L0, i0, 0, L0, L1, L2) \
    SM_S3_R3(pre##L1, i0, 1, L0, L1, L2) \
    SM_S3_R3(pre##L2, i0, 2, L0, L1, L2)
#define SM_S3_R4(pre, i0, i1, i2, L0, L1, L2) \
    SM_SW4(pre##L0, i0, i1, i2, 0)            \
    SM_SW4(pre##L1, i0, i1, i2, 1)            \
    SM_SW4(pre##L2, i0, i1, i2, 2)
#define SM_S3_P4(pre, i0, i1, L0, L1, L2)    \
    SM_S3_R4(pre##L0, i0, i1, 0, L0, L1, L2) \
    SM_S3_R4(pre##L1, i0, i1, 1, L0, L1, L2) \
    SM_S3_R4(pre##L2, i0, i1, 2, L0, L1, L2)
#define SM_S3_C4(pre, i0, L0, L1, L2)    \
    SM_S3_P4(pre##L0, i0, 0, L0, L1, L2) \
    SM_S3_P4(pre##L1, i0, 1, L0, L1, L2) \
    SM_S3_P4(pre##L2, i0, 2, L0, L1, L2)
#define SM_SGEN3(L0, L1, L2)    \
    SM_SW1(L0, 0)               \
    SM_SW1(L1, 1)               \
    SM_SW1(L2, 2)               \
    SM_S3_R2(L0, 0, L0, L1, L2) \
    SM_S3_R2(L1, 1, L0, L1, L2) \
    SM_S3_R2(L2, 2, L0, L1, L2) \
    SM_S3_P3(L0, 0, L0, L1, L2) \
    SM_S3_P3(L1, 1, L0, L1, L2) \
    SM_S3_P3(L2, 2, L0, L1, L2) \
    SM_S3_C4(L0, 0, L0, L1, L2) \
    SM_S3_C4(L1, 1, L0, L1, L2) \
    SM_S3_C4(L2, 2, L0, L1, L2)

#define SM_S4_R2(pre, i0, L0, L1, L2, L3) \
    SM_SW2(pre##L0, i0, 0)                \
    SM_SW2(pre##L1, i0, 1)                \
    SM_SW2(pre##L2, i0, 2)                \
    SM_SW2(pre##L3, i0, 3)
#define SM_S4_R3(pre, i0, i1, L0, L1, L2, L3) \
    SM_SW3(pre##L0, i0, i1, 0)                \
    SM_SW3(pre##L1, i0, i1, 1)                \
    SM_SW3(pre##L2, i0, i1, 2)                \
    SM_SW3(pre##L3, i0, i1, 3)
#define SM_S4_P3(pre, i0, L0, L1, L2, L3)    \
    SM_S4_R3(pre##L0, i0, 0, L0, L1, L2, L3) \
    SM_S4_R3(pre##L1, i0, 1, L0, L1, L2, L3) \
    SM_S4_R3(pre##L2, i0, 2, L0, L1, L2, L3) \
    SM_S4_R3(pre##L3, i0, 3, L0, L1, L2, L3)
#define SM_S4_R4(pre, i0, i1, i2, L0, L1, L2, L3) \
    SM_SW4(pre##L0, i0, i1, i2, 0)                \
    SM_SW4(pre##L1, i0, i1, i2, 1)                \
    SM_SW4(pre##L2, i0, i1, i2, 2)                \
    SM_SW4(pre##L3, i0, i1, i2, 3)
#define SM_S4_P4(pre, i0, i1, L0, L1, L2, L3)    \
    SM_S4_R4(pre##L0, i0, i1, 0, L0, L1, L2, L3) \
    SM_S4_R4(pre##L1, i0, i1, 1, L0, L1, L2, L3) \
    SM_S4_R4(pre##L2, i0, i1, 2, L0, L1, L2, L3) \
    SM_S4_R4(pre##L3, i0, i1, 3, L0, L1, L2, L3)
#define SM_S4_C4(pre, i0, L0, L1, L2, L3)    \
    SM_S4_P4(pre##L0, i0, 0, L0, L1, L2, L3) \
    SM_S4_P4(pre##L1, i0, 1, L0, L1, L2, L3) \
    SM_S4_P4(pre##L2, i0, 2, L0, L1, L2, L3) \
    SM_S4_P4(pre##L3, i0, 3, L0, L1, L2, L3)
#define SM_SGEN4(L0, L1, L2, L3)    \
    SM_SW1(L0, 0)                   \
    SM_SW1(L1, 1)                   \
    SM_SW1(L2, 2)                   \
    SM_SW1(L3, 3)                   \
    SM_S4_R2(L0, 0, L0, L1, L2, L3) \
    SM_S4_R2(L1, 1, L0, L1, L2, L3) \
    SM_S4_R2(L2, 2, L0, L1, L2, L3) \
    SM_S4_R2(L3, 3, L0, L1, L2, L3) \
    SM_S4_P3(L0, 0, L0, L1, L2, L3) \
    SM_S4_P3(L1, 1, L0, L1, L2, L3) \
    SM_S4_P3(L2, 2, L0, L1, L2, L3) \
    SM_S4_P3(L3, 3, L0, L1, L2, L3) \
    SM_S4_C4(L0, 0, L0, L1, L2, L3) \
    SM_S4_C4(L1, 1, L0, L1, L2, L3) \
    SM_S4_C4(L2, 2, L0, L1, L2, L3) \
    SM_S4_C4(L3, 3, L0, L1, L2, L3)

// ---- fvec -----------------------------------------------------------------
// The register and the swizzle members share one union, so each language's
// swizzle set lives in the storage specialization for its tag. The copy
// operations are user-provided: implicit ones would make the compiler resolve
// the copy operations of every swizzle member (front-end time).
template <int N, class T, class Lg>
struct fvec_store;
#define SM_FVEC_STORE(W, LANG, MEMBERS)                                     \
    template <class T>                                                      \
    struct fvec_store<W, T, LANG> {                                         \
        static constexpr int N = W;                                         \
        using Lg = LANG;                                                    \
        union {                                                             \
            vreg<T> q_;                                                     \
            T comp_[4];                                                     \
            MEMBERS                                                         \
        };                                                                  \
        SHADER_MATH_INLINE explicit fvec_store(vreg<T> r) : q_(r) {}        \
        SHADER_MATH_INLINE fvec_store(const fvec_store& o) : q_(o.q_) {}    \
        SHADER_MATH_INLINE fvec_store& operator=(const fvec_store& o) {     \
            q_ = o.q_;                                                      \
            return *this;                                                   \
        }                                                                   \
    };
SM_FVEC_STORE(2, hlsl::lang_tag, SM_SGEN2(x, y) SM_SGEN2(r, g))
SM_FVEC_STORE(3, hlsl::lang_tag, SM_SGEN3(x, y, z) SM_SGEN3(r, g, b))
SM_FVEC_STORE(4, hlsl::lang_tag, SM_SGEN4(x, y, z, w) SM_SGEN4(r, g, b, a))
SM_FVEC_STORE(2, glsl::lang_tag, SM_SGEN2(x, y) SM_SGEN2(r, g) SM_SGEN2(s, t))
SM_FVEC_STORE(3, glsl::lang_tag, SM_SGEN3(x, y, z) SM_SGEN3(r, g, b) SM_SGEN3(s, t, p))
SM_FVEC_STORE(4, glsl::lang_tag,
              SM_SGEN4(x, y, z, w) SM_SGEN4(r, g, b, a) SM_SGEN4(s, t, p, q))
#undef SM_FVEC_STORE

template <int N, class T, class Lg>
struct fvec : fvec_store<N, T, Lg> {
    static_assert(N >= 2 && N <= 4, "shader-math: vector width must be 2..4");
    static constexpr int size_v = N;
    using elem_t = T;
    using lang_t = Lg;
    using ops_t = vops<T>;
    using reg_t = vreg<T>;
    using store_t = fvec_store<N, T, Lg>;
    using store_t::comp_;
    using store_t::q_;

    SHADER_MATH_INLINE fvec() : store_t(ops_t::zero()) {}
    SHADER_MATH_INLINE fvec(const fvec& o) : store_t(o.q_) {}
    SHADER_MATH_INLINE fvec& operator=(const fvec& o) {
        q_ = o.q_;
        return *this;
    }
    SHADER_MATH_INLINE explicit fvec(reg_t r) : store_t(r) {}
    // A scalar converts implicitly in HLSL (float3 v = 1.0f) and only
    // through a constructor in GLSL (vec3(1.0))
    template <class S>
        requires CvtScalar<S, T>
    SHADER_MATH_INLINE explicit(is_glsl_v<Lg>) fvec(const S& s)
        : store_t(ops_t::splat(elem_cast<T>(s))) {}

    template <class X, class Y>
        requires(N == 2 && CvtScalar<X, T> && CvtScalar<Y, T>)
    SHADER_MATH_INLINE fvec(X x, Y y)
        : store_t(ops_t::set(elem_cast<T>(x), elem_cast<T>(y), T(0.0f), T(0.0f))) {}
    template <class X, class Y, class Z>
        requires(N == 3 && CvtScalar<X, T> && CvtScalar<Y, T> && CvtScalar<Z, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, Z z)
        : store_t(
              ops_t::set(elem_cast<T>(x), elem_cast<T>(y), elem_cast<T>(z), T(0.0f))) {}
    template <class X, class Y, class Z, class W>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<Y, T> && CvtScalar<Z, T> &&
                 CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, Z z, W w)
        : store_t(ops_t::set(elem_cast<T>(x), elem_cast<T>(y), elem_cast<T>(z),
                             elem_cast<T>(w))) {}

    // composite constructors (HLSL/GLSL: concatenate vectors and scalars)
    template <class Z>
        requires(N == 3 && CvtScalar<Z, T>)
    SHADER_MATH_INLINE fvec(const fvec<2, T, Lg>& v, Z z)
        : store_t(ops_t::set(v[0], v[1], elem_cast<T>(z), T(0.0f))) {}
    template <class X>
        requires(N == 3 && CvtScalar<X, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<2, T, Lg>& v)
        : store_t(ops_t::set(elem_cast<T>(x), v[0], v[1], T(0.0f))) {}
    template <class Z, class W>
        requires(N == 4 && CvtScalar<Z, T> && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(const fvec<2, T, Lg>& v, Z z, W w)
        : store_t(ops_t::set(v[0], v[1], elem_cast<T>(z), elem_cast<T>(w))) {}
    template <class X, class W>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<2, T, Lg>& v, W w)
        : store_t(ops_t::set(elem_cast<T>(x), v[0], v[1], elem_cast<T>(w))) {}
    template <class X, class Y>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<Y, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, const fvec<2, T, Lg>& v)
        : store_t(ops_t::set(elem_cast<T>(x), elem_cast<T>(y), v[0], v[1])) {}
    SHADER_MATH_INLINE fvec(const fvec<2, T, Lg>& v, const fvec<2, T, Lg>& u)
        requires(N == 4)
        : store_t(ops_t::set(v[0], v[1], u[0], u[1])) {}
    template <class W>
        requires(N == 4 && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(const fvec<3, T, Lg>& v, W w)
        : store_t(ops_t::set(v[0], v[1], v[2], elem_cast<T>(w))) {}
    template <class X>
        requires(N == 4 && CvtScalar<X, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<3, T, Lg>& v)
        : store_t(ops_t::set(elem_cast<T>(x), v[0], v[1], v[2])) {}

    // Exact component lists with a vector of another element type
    // (float4(v2, h2)); the same-type lists have the constructors above
    template <class... As>
        requires(sizeof...(As) >= 2 && ctor_fill<N, T, Lg, false, As...>() == 1 &&
                 any_foreign_elem<T, As...>)
    SHADER_MATH_INLINE fvec(const As&... as) : store_t(gathered(as...)) {}

    // GLSL component lists longer than N (vec3(v2, w2), vec4(s, v4),
    // vec4(s, m2)): every argument contributes, the last one may be cut short
    template <class... As>
        requires(is_glsl_v<Lg> && sizeof...(As) >= 2 &&
                 ctor_fill<N, T, Lg, true, As...>() == 2)
    SHADER_MATH_INLINE explicit fvec(const As&... as) : store_t(gathered(as...)) {}

    // explicit truncation (HLSL cast / GLSL constructor shortening)
    template <int N2>
    SHADER_MATH_INLINE explicit fvec(const fvec<N2, T, Lg>& o)
        requires(N2 > N)
        : store_t(o.q_) {}

    // conversion to another element type or to the other language's vector of
    // the same width: implicit where the language converts implicitly
    // (implicit_elem_v: HLSL int <-> float, GLSL int -> float ...), explicit
    // otherwise (half <-> float, bool, the other language). Two constrained
    // constructors rather than explicit(cond): Apple Clang 21 crashes deducing
    // `vector r = v;` through the alias template with a conditional explicit.
    template <class U, class Lg2>
        requires(std::is_same_v<Lg2, Lg> && implicit_elem_v<U, T, Lg>)
    SHADER_MATH_INLINE fvec(const fvec<N, U, Lg2>& o) : store_t(convert(o)) {}
    template <class U, class Lg2>
        requires((!std::is_same_v<U, T> || !std::is_same_v<Lg2, Lg>) &&
                 !(std::is_same_v<Lg2, Lg> && implicit_elem_v<U, T, Lg>))
    SHADER_MATH_INLINE explicit fvec(const fvec<N, U, Lg2>& o) : store_t(convert(o)) {}
    // explicit conversion from a bool vector: 1 where true, 0 where false
    // (HLSL float3(b), GLSL vec3(b))
    template <int M>
        requires(M == N)
    SHADER_MATH_INLINE explicit fvec(const bvec<M, Lg>& b)
        : store_t(ops_t::sel(b.m_, ops_t::splat(T(1.0f)), ops_t::zero())) {}
    // ... and from a swizzle of the other element type or language
    // (half4(f.wzyx)): the proxy cannot deduce U through the overload above
    template <class U, int M, class Lg2, int... Is>
        requires(sizeof...(Is) == N && std::is_same_v<Lg2, Lg> && implicit_elem_v<U, T, Lg>)
    SHADER_MATH_INLINE fvec(const swz<U, M, Lg2, Is...>& s) : fvec(fvec<N, U, Lg2>(s)) {}
    template <class U, int M, class Lg2, int... Is>
        requires((!std::is_same_v<U, T> || !std::is_same_v<Lg2, Lg>) && sizeof...(Is) == N &&
                 !(std::is_same_v<Lg2, Lg> && implicit_elem_v<U, T, Lg>))
    SHADER_MATH_INLINE explicit fvec(const swz<U, M, Lg2, Is...>& s)
        : fvec(fvec<N, U, Lg2>(s)) {}

    // GLSL v.length(): the number of components
    static constexpr int length()
        requires is_glsl_v<Lg>
    {
        return N;
    }

    SHADER_MATH_INLINE T& operator[](int i) { return comp_[i]; }
    SHADER_MATH_INLINE T operator[](int i) const { return comp_[i]; }

    SHADER_MATH_INLINE void store(T* p) const {
        T t[4];
        ops_t::store4(q_, t);
        for (int i = 0; i < N; ++i) p[i] = t[i];
    }

private:
    template <class... As>
    static SHADER_MATH_INLINE reg_t gathered(const As&... as) {
        T t[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
        gather_components<T>(t, N, as...);
        return ops_t::load4(t);
    }
    template <class U, class Lg2>
    static SHADER_MATH_INLINE reg_t convert(const fvec<N, U, Lg2>& o) {
        if constexpr (std::is_same_v<U, T> || (is_int32_elem_v<U> && is_int32_elem_v<T>)) {
            return o.q_;  // same type, or 32-bit int <-> uint: the same register
        } else if constexpr (std::is_same_v<U, float> && is_int_elem_v<T>) {
            return ops_t::from_f32(o.q_);
        } else if constexpr (is_int_elem_v<U> && std::is_same_v<T, float>) {
            return vops<U>::to_f32(o.q_);
        } else {
            return ops_t::set(elem_cast<T>(o[0]), elem_cast<T>(o[1]),
                              N > 2 ? elem_cast<T>(o[2]) : T(0.0f),
                              N > 3 ? elem_cast<T>(o[3]) : T(0.0f));
        }
    }
};

#undef SM_SW1
#undef SM_SW2
#undef SM_SW3
#undef SM_SW4
#undef SM_S2_R2
#undef SM_S2_R3
#undef SM_S2_P3
#undef SM_S2_R4
#undef SM_S2_P4
#undef SM_S2_C4
#undef SM_SGEN2
#undef SM_S3_R2
#undef SM_S3_R3
#undef SM_S3_P3
#undef SM_S3_R4
#undef SM_S3_P4
#undef SM_S3_C4
#undef SM_SGEN3
#undef SM_S4_R2
#undef SM_S4_R3
#undef SM_S4_P3
#undef SM_S4_R4
#undef SM_S4_P4
#undef SM_S4_C4
#undef SM_SGEN4

// ---- one-component vectors (HLSL float1, int1, ...) ------------------------
// A scalar spelled as a vector: it converts to and from its element type
// implicitly and takes part in every operation as a scalar, the way a
// one-component swizzle does (float1 + float3 is a float3, sqrt(f1) takes it
// as a float); .x / .r, .xx to .xxxx and [0] read it.
template <class T, class Lg>
struct fvec<1, T, Lg> {
    static constexpr int size_v = 1;
    using elem_t = T;
    using lang_t = Lg;
    union {
        T comp_[4];
        swz<T, 1, Lg, 0> x, r;
        swz<T, 1, Lg, 0, 0> xx, rr;
        swz<T, 1, Lg, 0, 0, 0> xxx, rrr;
        swz<T, 1, Lg, 0, 0, 0, 0> xxxx, rrrr;
    };

    SHADER_MATH_INLINE fvec() : comp_{T(0), T(0), T(0), T(0)} {}
    SHADER_MATH_INLINE fvec(const fvec& o) : comp_{o.comp_[0], T(0), T(0), T(0)} {}
    SHADER_MATH_INLINE fvec& operator=(const fvec& o) {
        comp_[0] = o.comp_[0];
        return *this;
    }
    template <class S>
        requires CvtScalar<S, T>
    SHADER_MATH_INLINE fvec(const S& s) : comp_{elem_cast<T>(s), T(0), T(0), T(0)} {}
    template <class U>
        requires(!std::is_same_v<U, T> && !CvtScalar<fvec<1, U, Lg>, T>)
    SHADER_MATH_INLINE explicit fvec(const fvec<1, U, Lg>& o)
        : comp_{elem_cast<T>(o.comp_[0]), T(0), T(0), T(0)} {}
    SHADER_MATH_INLINE operator T() const { return comp_[0]; }
    SHADER_MATH_INLINE T& operator[](int) { return comp_[0]; }
    SHADER_MATH_INLINE const T& operator[](int) const { return comp_[0]; }

#define SM_VEC1_COMPOUND(sym)                                                    \
    template <class S>                                                         \
        requires requires(const T& t, const S& s) { elem_cast<T>(t sym s); }   \
    SHADER_MATH_INLINE fvec& operator sym##=(const S& s) {                     \
        comp_[0] = elem_cast<T>(comp_[0] sym s);                               \
        return *this;                                                          \
    }
    SM_VEC1_COMPOUND(+)
    SM_VEC1_COMPOUND(-)
    SM_VEC1_COMPOUND(*)
    SM_VEC1_COMPOUND(/)
#undef SM_VEC1_COMPOUND
    SHADER_MATH_INLINE fvec& operator++() {
        comp_[0] = static_cast<T>(comp_[0] + T(1));
        return *this;
    }
    SHADER_MATH_INLINE fvec& operator--() {
        comp_[0] = static_cast<T>(comp_[0] - T(1));
        return *this;
    }
    SHADER_MATH_INLINE fvec operator++(int) {
        const fvec o = *this;
        ++*this;
        return o;
    }
    SHADER_MATH_INLINE fvec operator--(int) {
        const fvec o = *this;
        --*this;
        return o;
    }
    // HLSL's % on a floating-point scalar (fmod: truncating, the sign of a);
    // C++ has no built-in one to fall back on
    template <class S>
        requires(!std::is_integral_v<T> && CvtScalar<S, T>)
    friend SHADER_MATH_INLINE T operator%(const fvec& a, const S& b) {
        return fmod_impl(fvec<2, T, Lg>(a.comp_[0]), fvec<2, T, Lg>(elem_cast<T>(b)))[0];
    }
    template <class S>
        requires(!std::is_integral_v<T> && CvtScalar<S, T> && !std::is_same_v<S, fvec>)
    friend SHADER_MATH_INLINE T operator%(const S& a, const fvec& b) {
        return fmod_impl(fvec<2, T, Lg>(elem_cast<T>(a)), fvec<2, T, Lg>(b.comp_[0]))[0];
    }
};

// ---- vec_traits / concepts -------------------------------------------------
template <int N, class T, class Lg>
struct vec_traits<fvec<N, T, Lg>> {
    static constexpr int size = N;
    using elem = T;
    using lang = Lg;
    static SHADER_MATH_INLINE const fvec<N, T, Lg>& get(const fvec<N, T, Lg>& v) { return v; }
};
template <class T, int N, class Lg, int... Is>
    requires(sizeof...(Is) >= 2)
struct vec_traits<swz<T, N, Lg, Is...>> {
    static constexpr int size = static_cast<int>(sizeof...(Is));
    using elem = T;
    using lang = Lg;
    static SHADER_MATH_INLINE fvec<size, T, Lg> get(const swz<T, N, Lg, Is...>& s) {
        return fvec<size, T, Lg>(s);
    }
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, class Lg, int... Is>
    requires(sizeof...(Is) >= 2)
struct vec_traits<nswz<T, N, Lg, Is...>> {
    static constexpr int size = static_cast<int>(sizeof...(Is));
    using elem = T;
    using lang = Lg;
    static SHADER_MATH_INLINE fvec<size, T, Lg> get(const nswz<T, N, Lg, Is...>& s) {
        return fvec<size, T, Lg>(s);
    }
};
#endif

// The vector type a vector-like operand (vector or swizzle) computes in;
// M overrides the width
template <class V, int M = vec_size_v<V>>
using vec_t = fvec<M, vec_elem_t<V>, vec_lang_t<V>>;
// V is a vector or swizzle computing as W (false for scalars)
template <class V, class W>
constexpr bool vec_is() {
    if constexpr (vec_size_v<V> >= 2) {
        return std::is_same_v<vec_t<V>, W>;
    } else {
        return false;
    }
}

template <class V>
concept VecLike = (vec_size_v<V> >= 2);
// A vector (or swizzle) of language Lg
template <class V, class Lg>
concept VecOf = VecLike<V> && std::is_same_v<vec_lang_t<V>, Lg>;
// Every vector operand belongs to language Lg (scalar operands are free)
template <class Lg, class... As>
concept ArgsOf = ((!VecLike<As> || std::is_same_v<vec_lang_t<As>, Lg>) && ...);
// Same width, element type and language: half and float vectors, and hlsl
// and glsl vectors, never mix implicitly (convert explicitly instead).
template <class A, class B>
concept SameVec = VecLike<A> && VecLike<B> && (vec_size_v<A> == vec_size_v<B>) &&
                  std::is_same_v<vec_elem_t<A>, vec_elem_t<B>> &&
                  std::is_same_v<vec_lang_t<A>, vec_lang_t<B>>;
// float / half vectors (the floating-point functions) and int / uint
// vectors (the integer operators)
template <class V>
concept FloatVec = VecLike<V> && !is_int_elem_v<vec_elem_t<V>>;
template <class V>
concept IntVec = VecLike<V> && is_int_elem_v<vec_elem_t<V>>;
template <class V>
concept Int32Vec = VecLike<V> && is_int32_elem_v<vec_elem_t<V>>;
template <class A, class B>
concept SameFloatVec = SameVec<A, B> && FloatVec<A>;
// a float vector of N components (the packing functions)
template <class A, int N>
concept FloatVecN = VecLike<A> && std::is_same_v<vec_elem_t<A>, float> && vec_size_v<A> == N;

// Scalar operand rules for vector-scalar operators:
//   float vectors: any arithmetic scalar (int / double literals keep working)
//   half vectors:  exactly half (no implicit float mixing: half3 * 2.0f fails)
//   int / uint vectors: any integer scalar but bool (int3 * 2.5f fails)
//   all:           a single-component swizzle of the same element type
template <class S>
struct swz_elem {
    using type = void;
};
template <class T, int N, class Lg, int I>
struct swz_elem<swz<T, N, Lg, I>> {
    using type = T;
};
template <class T, class Lg>
struct swz_elem<fvec<1, T, Lg>> {
    using type = T;
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, class Lg, int J>
struct swz_elem<nswz<T, N, Lg, J>> {
    using type = T;
};
#endif
template <class S>
inline constexpr bool is_int_scalar_v =
    std::is_integral_v<std::remove_cvref_t<S>> && !std::is_same_v<std::remove_cvref_t<S>, bool>;
// (a scalar here is a number, one component of a vector or matrix, or a
// one-component vector, taken by its value type)
template <class S, class T>
concept ScalarFor =
    std::is_same_v<std::remove_cvref_t<S>, T> ||
    (std::is_same_v<T, float> && std::is_arithmetic_v<typename scalar_value<std::remove_cvref_t<S>>::type>) ||
    (is_int_elem_v<T> && is_int_scalar_v<typename scalar_value<std::remove_cvref_t<S>>::type>) ||
    std::is_same_v<typename swz_elem<std::remove_cvref_t<S>>::type, T>;
template <class T, int N, class Lg, int I>
struct scalar_value<swz<T, N, Lg, I>> {
    using type = T;
};
template <class T, class Lg>
struct scalar_value<fvec<1, T, Lg>> {
    using type = T;
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, class Lg, int J>
struct scalar_value<nswz<T, N, Lg, J>> {
    using type = T;
};
#endif

// The type an integer scalar has next to an integer vector of T, in language
// Lg. GLSL types every integer (2 is an int, 2u a uint, one component its
// vector's element). HLSL types 2u as a uint and a component as its element,
// but an unsuffixed literal takes the vector's type, so a plain signed scalar
// counts as a T (an int variable cannot be told apart from the literal).
template <class S, class T, class Lg>
constexpr auto int_scalar_elem() {
    using V = typename scalar_value<std::remove_cvref_t<S>>::type;
    if constexpr (!std::is_integral_v<V> || std::is_same_v<V, bool>) {
        return std::type_identity<void>{};
    } else if constexpr (!is_glsl_v<Lg> && std::is_arithmetic_v<std::remove_cvref_t<S>> && std::is_signed_v<V>) {
        return std::type_identity<T>{};
    } else {
        using W = std::conditional_t<sizeof(V) == 2, std::int16_t,
                                     std::conditional_t<sizeof(V) == 8, std::int64_t, std::int32_t>>;
        return std::type_identity<std::conditional_t<std::is_signed_v<V>, W, std::make_unsigned_t<W>>>{};
    }
}
// The element type an integer vector A and integer scalars S... compute in:
// their common type (GLSL i16vec3 * 2 is an ivec3, HLSL and GLSL int3 * 2u a
// uint3, clamp(ivec3, 0, 5u) a uvec3); void where there is none
template <class Lg, class T, class... Us>
struct common_elems {
    using type = T;
};
template <class Lg, class T, class U, class... Us>
struct common_elems<Lg, T, U, Us...> : common_elems<Lg, common_elem_t<T, U, Lg>, Us...> {};
template <class A, class... S>
using int_scalar_op_t =
    typename common_elems<vec_lang_t<A>, vec_elem_t<A>,
                          typename decltype(int_scalar_elem<S, vec_elem_t<A>, vec_lang_t<A>>())::type...>::type;
// A scalar operand that keeps vector A's type, and integer scalars that take
// it to another element type
template <class S, class A>
concept VecScalar = VecLike<A> && ScalarFor<S, vec_elem_t<A>> &&
                    (!IntVec<A> || std::is_same_v<int_scalar_op_t<A, S>, vec_elem_t<A>>);
template <class A, class... S>
concept PromotingScalars = IntVec<A> && !std::is_void_v<int_scalar_op_t<A, S...>> &&
                           !std::is_same_v<int_scalar_op_t<A, S...>, vec_elem_t<A>>;
template <class S, class A>
concept PromotingScalar = PromotingScalars<A, S>;

template <class V>
SHADER_MATH_INLINE decltype(auto) to_vec(const V& t) {
    return vec_traits<std::remove_cvref_t<V>>::get(t);
}

// ---- component-wise vector operators (identical in HLSL and GLSL) ---------
#define SM_VEC_BINOP(sym, fn)                                                          \
    template <class A, class B>                                                        \
        requires SameVec<A, B>                                                         \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, to_vec(b).q_));                      \
    }                                                                                  \
    template <class A, class S>                                                        \
        requires VecScalar<S, A>                                                       \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, vops<T>::splat(static_cast<T>(s)))); \
    }                                                                                  \
    template <class B, class S>                                                        \
        requires VecScalar<S, B>                                                       \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                     \
        using T = vec_elem_t<B>;                                                       \
        return vec_t<B>(vops<T>::fn(vops<T>::splat(static_cast<T>(s)), to_vec(b).q_)); \
    }

SM_VEC_BINOP(+, add)
SM_VEC_BINOP(-, sub)
SM_VEC_BINOP(*, mul)
SM_VEC_BINOP(/, div)
#undef SM_VEC_BINOP

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto operator-(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::neg(to_vec(a).q_));
}
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto operator+(const A& a) {
    return vec_t<A>(to_vec(a));
}

// ++ / -- add or subtract 1 in every component (both languages)
template <int N, class T, class Lg>
    requires(N >= 2)
SHADER_MATH_INLINE fvec<N, T, Lg>& operator++(fvec<N, T, Lg>& a) {
    a = a + T(1.0f);
    return a;
}
template <int N, class T, class Lg>
    requires(N >= 2)
SHADER_MATH_INLINE fvec<N, T, Lg>& operator--(fvec<N, T, Lg>& a) {
    a = a - T(1.0f);
    return a;
}
template <int N, class T, class Lg>
    requires(N >= 2)
SHADER_MATH_INLINE fvec<N, T, Lg> operator++(fvec<N, T, Lg>& a, int) {
    const fvec<N, T, Lg> old = a;
    a = a + T(1.0f);
    return old;
}
template <int N, class T, class Lg>
    requires(N >= 2)
SHADER_MATH_INLINE fvec<N, T, Lg> operator--(fvec<N, T, Lg>& a, int) {
    const fvec<N, T, Lg> old = a;
    a = a - T(1.0f);
    return old;
}

#define SM_VEC_COMPOUND(sym)                                                            \
    template <int N, class T, class Lg, class B>                                        \
        requires(SameVec<fvec<N, T, Lg>, B>)                                            \
    SHADER_MATH_INLINE fvec<N, T, Lg>& operator sym##=(fvec<N, T, Lg>& a, const B& b) { \
        a = a sym b;                                                                    \
        return a;                                                                       \
    }                                                                                   \
    template <int N, class T, class Lg, class S>                                        \
        requires VecScalar<S, fvec<N, T, Lg>>                                           \
    SHADER_MATH_INLINE fvec<N, T, Lg>& operator sym##=(fvec<N, T, Lg>& a, const S& s) { \
        a = a sym s;                                                                    \
        return a;                                                                       \
    }

SM_VEC_COMPOUND(+)
SM_VEC_COMPOUND(-)
SM_VEC_COMPOUND(*)
SM_VEC_COMPOUND(/)
#undef SM_VEC_COMPOUND

// ---- mixed int / uint / float operands (both languages) ---------------------
// Both operands convert to common_elem_t (int3 + float3 is a float3, int3 +
// uint3 a uint3); an int / uint vector with a floating-point scalar computes
// in float (int3 * 2.5). Compound assignment keeps the left operand's type,
// so it needs the language's implicit conversion back (HLSL int3 += float3
// truncates; GLSL ivec3 += vec3 does not compile).
template <class A, class B>
concept MixedVec = VecLike<A> && VecLike<B> && (vec_size_v<A> == vec_size_v<B>) &&
                   std::is_same_v<vec_lang_t<A>, vec_lang_t<B>> &&
                   MixedElems<vec_elem_t<A>, vec_elem_t<B>, vec_lang_t<A>>;
template <class A, class B>
using mixed_t = fvec<vec_size_v<A>, common_elem_t<vec_elem_t<A>, vec_elem_t<B>, vec_lang_t<A>>,
                     vec_lang_t<A>>;
template <class A>
using float_vec_t = fvec<vec_size_v<A>, float, vec_lang_t<A>>;
template <class A, class S>
concept IntVecFloatScalar = IntVec<A> &&
                            std::is_floating_point_v<typename scalar_value<std::remove_cvref_t<S>>::type> &&
                            implicit_elem_v<vec_elem_t<A>, float, vec_lang_t<A>>;
#define SM_MIXED_BINOP(sym)                                                         \
    template <class A, class B>                                                     \
        requires MixedVec<A, B>                                                     \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                  \
        using R = mixed_t<A, B>;                                                    \
        return R(to_vec(a)) sym R(to_vec(b));                                       \
    }                                                                               \
    template <class A, class S>                                                     \
        requires IntVecFloatScalar<A, S>                                            \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                  \
        return float_vec_t<A>(to_vec(a)) sym static_cast<float>(s);                 \
    }                                                                               \
    template <class B, class S>                                                     \
        requires IntVecFloatScalar<B, S>                                            \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                  \
        return static_cast<float>(s) sym float_vec_t<B>(to_vec(b));                 \
    }                                                                               \
    template <class A, class S>                                                     \
        requires PromotingScalar<S, A>                                              \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                  \
        using R = int_scalar_op_t<A, S>;                                            \
        return fvec<vec_size_v<A>, R, vec_lang_t<A>>(to_vec(a)) sym static_cast<R>(s); \
    }                                                                               \
    template <class B, class S>                                                     \
        requires PromotingScalar<S, B>                                              \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                  \
        using R = int_scalar_op_t<B, S>;                                            \
        return static_cast<R>(s) sym fvec<vec_size_v<B>, R, vec_lang_t<B>>(to_vec(b)); \
    }                                                                               \
    template <int N, class T, class Lg, class B>                                    \
        requires((MixedVec<fvec<N, T, Lg>, B> || IntVecFloatScalar<fvec<N, T, Lg>, B> || \
                  PromotingScalar<B, fvec<N, T, Lg>>) &&                            \
                 requires(const fvec<N, T, Lg>& a, const B& b) {                    \
                     { a sym b } -> std::convertible_to<fvec<N, T, Lg>>;            \
                 })                                                                 \
    SHADER_MATH_INLINE fvec<N, T, Lg>& operator sym##=(fvec<N, T, Lg>& a, const B& b) { \
        a = a sym b;                                                                \
        return a;                                                                   \
    }
SM_MIXED_BINOP(+)
SM_MIXED_BINOP(-)
SM_MIXED_BINOP(*)
SM_MIXED_BINOP(/)
#undef SM_MIXED_BINOP

// ---- integer operators (int / uint vectors, both languages) ----------------
// % truncates like / (the sign follows the dividend); x / 0, x % 0 and
// INT_MIN / -1 have the results of detail/int_lane.hpp
#define SM_INT_BINOP(sym, fn)                                                          \
    template <class A, class B>                                                        \
        requires(SameVec<A, B> && IntVec<A>)                                           \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, to_vec(b).q_));                      \
    }                                                                                  \
    template <class A, class S>                                                        \
        requires(IntVec<A> && VecScalar<S, A>)                                         \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, vops<T>::splat(static_cast<T>(s)))); \
    }                                                                                  \
    template <class B, class S>                                                        \
        requires(IntVec<B> && VecScalar<S, B>)                                         \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                     \
        using T = vec_elem_t<B>;                                                       \
        return vec_t<B>(vops<T>::fn(vops<T>::splat(static_cast<T>(s)), to_vec(b).q_)); \
    }
SM_INT_BINOP(%, rem)
SM_INT_BINOP(&, band)
SM_INT_BINOP(|, bor)
SM_INT_BINOP(^, bxor)
#undef SM_INT_BINOP
// int with uint: both convert to uint
#define SM_MIXED_INT_BINOP(sym)                                                     \
    template <class A, class B>                                                     \
        requires(MixedVec<A, B> && IntVec<A> && IntVec<B>)                          \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                  \
        using R = mixed_t<A, B>;                                                    \
        return R(to_vec(a)) sym R(to_vec(b));                                       \
    }                                                                               \
    template <class A, class S>                                                     \
        requires PromotingScalar<S, A>                                              \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                  \
        using R = int_scalar_op_t<A, S>;                                            \
        return fvec<vec_size_v<A>, R, vec_lang_t<A>>(to_vec(a)) sym static_cast<R>(s); \
    }                                                                               \
    template <class B, class S>                                                     \
        requires PromotingScalar<S, B>                                              \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                  \
        using R = int_scalar_op_t<B, S>;                                            \
        return static_cast<R>(s) sym fvec<vec_size_v<B>, R, vec_lang_t<B>>(to_vec(b)); \
    }
SM_MIXED_INT_BINOP(%)
SM_MIXED_INT_BINOP(&)
SM_MIXED_INT_BINOP(|)
SM_MIXED_INT_BINOP(^)
#undef SM_MIXED_INT_BINOP

template <class A>
    requires IntVec<A>
SHADER_MATH_INLINE auto operator~(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::bnot(to_vec(a).q_));
}

// << / >>: the result has the left operand's type; the count is a vector of
// the same width and language (int or uint) or an integer scalar, taken
// modulo 32. >> is arithmetic on int, logical on uint.
template <class A, class B>
concept ShiftCount = IntVec<A> && IntVec<B> && vec_size_v<A> == vec_size_v<B> &&
                     std::is_same_v<vec_lang_t<A>, vec_lang_t<B>> &&
                     sizeof(vec_elem_t<A>) == sizeof(vec_elem_t<B>);
#define SM_INT_SHIFT(sym, fn)                                                          \
    template <class A, class B>                                                        \
        requires ShiftCount<A, B>                                                      \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& n) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, to_vec(n).q_));                      \
    }                                                                                  \
    template <class A, class S>                                                        \
        requires(IntVec<A> && is_int_scalar_v<S>)                                      \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& n) {                     \
        using T = vec_elem_t<A>;                                                       \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_, vops<T>::splat(static_cast<T>(n)))); \
    }
SM_INT_SHIFT(<<, shl)
SM_INT_SHIFT(>>, shr)
#undef SM_INT_SHIFT

#define SM_INT_COMPOUND(sym)                                                            \
    template <int N, class T, class Lg, class B>                                        \
        requires(is_int_elem_v<T> && requires(const fvec<N, T, Lg>& a, const B& b) {    \
            { a sym b } -> std::convertible_to<fvec<N, T, Lg>>;                         \
        })                                                                              \
    SHADER_MATH_INLINE fvec<N, T, Lg>& operator sym##=(fvec<N, T, Lg>& a, const B& b) { \
        a = a sym b;                                                                    \
        return a;                                                                       \
    }
SM_INT_COMPOUND(%)
SM_INT_COMPOUND(&)
SM_INT_COMPOUND(|)
SM_INT_COMPOUND(^)
SM_INT_COMPOUND(<<)
SM_INT_COMPOUND(>>)
#undef SM_INT_COMPOUND

// ---- shared vector functions (same meaning in both languages) --------------
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE vec_elem_t<A> dot(const A& a0, const B& b0) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const vec_t<A> a = to_vec(a0);
    const vec_t<A> b = to_vec(b0);
    T t[4];
    vops<T>::store4(vops<T>::mul(a.q_, b.q_), t);
    T s = t[0] + t[1];
    if constexpr (n >= 3) s = s + t[2];
    if constexpr (n == 4) s = s + t[3];
    return s;
}

template <class A, class B>
    requires(SameFloatVec<A, B> && vec_size_v<A> == 3)
SHADER_MATH_INLINE auto cross(const A& a0, const B& b0) {
    const vec_t<A> a = to_vec(a0);
    const vec_t<A> b = to_vec(b0);
    return vec_t<A>(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                    a[0] * b[1] - a[1] * b[0]);
}

template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE vec_elem_t<A> length(const A& a) {
    using T = vec_elem_t<A>;
    return static_cast<T>(std::sqrt(static_cast<float>(dot(a, a))));
}

template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto normalize(const A& a) {
    const vec_t<A> v = to_vec(a);
    return v / length(v);
}

// ---- component-wise functions (same meaning in both languages) --------------
// abs / min / max / clamp take every vector kind; sign takes float, half and
// int vectors (as in both languages, not uint)
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto abs(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::abs(to_vec(a).q_));
}
// Language-specific spellings (hlsl lerp/frac/rcp/rsqrt/saturate, glsl
// mix/fract/inversesqrt/roundEven) forward to the *_impl cores below from
// their own namespaces so they never leak across languages via ADL.

#define SM_VEC_UNARY_FN(name, fn)                                    \
    template <class A>                                               \
        requires FloatVec<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        using T = vec_elem_t<A>;                                     \
        return vec_t<A>(vops<T>::fn(to_vec(a).q_));                  \
    }

SM_VEC_UNARY_FN(sqrt, sqrt)
SM_VEC_UNARY_FN(floor, rnd_floor)
SM_VEC_UNARY_FN(ceil, rnd_ceil)
SM_VEC_UNARY_FN(trunc, rnd_trunc)
// round sends halves to even, as DXC compiles HLSL round (Round_ne in DXIL,
// RoundEven in SPIR-V). GLSL leaves the halfway direction to the
// implementation, so glsl::round is roundEven too.
SM_VEC_UNARY_FN(round, rnd_even)
SM_VEC_UNARY_FN(round_even_impl, rnd_even)  // glsl::roundEven
#undef SM_VEC_UNARY_FN

#define SM_VEC_MINMAX_FN(name)                                                 \
    template <class A, class B>                                                \
        requires SameVec<A, B>                                                 \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                     \
        using T = vec_elem_t<A>;                                               \
        return vec_t<A>(vops<T>::name(to_vec(a).q_, to_vec(b).q_));            \
    }                                                                          \
    template <class A, class S>                                                \
        requires VecScalar<S, A>                                               \
    SHADER_MATH_INLINE auto name(const A& a, const S& s) {                     \
        using T = vec_elem_t<A>;                                               \
        return vec_t<A>(                                                       \
            vops<T>::name(to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));   \
    }                                                                          \
    template <class A, class S>                                                \
        requires PromotingScalar<S, A>                                         \
    SHADER_MATH_INLINE auto name(const A& a, const S& s) {                     \
        using R = int_scalar_op_t<A, S>;                                       \
        return name(fvec<vec_size_v<A>, R, vec_lang_t<A>>(to_vec(a)), static_cast<R>(s)); \
    }

SM_VEC_MINMAX_FN(min)
SM_VEC_MINMAX_FN(max)
#undef SM_VEC_MINMAX_FN

// sign: -1 / 0 / +1 as the same vector type in both languages (HLSL's int
// return type is intentionally not replicated). sign(NaN) == 0.
template <class A>
    requires(VecLike<A> && !std::is_same_v<vec_elem_t<A>, std::uint32_t>)
SHADER_MATH_INLINE auto sign(const A& a) {
    using T = vec_elem_t<A>;
    const auto v = to_vec(a).q_;
    const auto zero = vops<T>::zero();
    const auto one = vops<T>::splat(T(1.0f));
    return vec_t<A>(vops<T>::sub(vops<T>::sel(vops<T>::cmp_gt(v, zero), one, zero),
                                   vops<T>::sel(vops<T>::cmp_lt(v, zero), one, zero)));
}

template <class A, class B, class C>
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto clamp(const A& x, const B& lo, const C& hi) {
    using T = vec_elem_t<A>;
    return vec_t<A>(
        vops<T>::min(vops<T>::max(to_vec(x).q_, to_vec(lo).q_), to_vec(hi).q_));
}
template <class A, class SL, class SH>
    requires(VecScalar<SL, A> && VecScalar<SH, A>)
SHADER_MATH_INLINE auto clamp(const A& x, const SL& lo, const SH& hi) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::min(
        vops<T>::max(to_vec(x).q_, vops<T>::splat(static_cast<T>(lo))),
        vops<T>::splat(static_cast<T>(hi))));
}
template <class A, class SL, class SH>
    requires(ScalarFor<SL, vec_elem_t<A>> && ScalarFor<SH, vec_elem_t<A>> && PromotingScalars<A, SL, SH>)
SHADER_MATH_INLINE auto clamp(const A& x, const SL& lo, const SH& hi) {
    using R = int_scalar_op_t<A, SL, SH>;
    return clamp(fvec<vec_size_v<A>, R, vec_lang_t<A>>(to_vec(x)), static_cast<R>(lo), static_cast<R>(hi));
}

// hlsl::saturate (GLSL has no saturate; use clamp(x, 0.0, 1.0))
template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto saturate_impl(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::min(vops<T>::max(to_vec(a).q_, vops<T>::zero()),
                                   vops<T>::splat(T(1.0f))));
}

// hlsl::lerp: a + t*(b - a), the form DXC emits for DXIL; fmadd path (rounding
// differs with FMA)
template <class A, class B, class C>
    requires(SameFloatVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto lerp_impl(const A& a, const B& b, const C& t) {
    using T = vec_elem_t<A>;
    const auto av = to_vec(a).q_;
    return vec_t<A>(vops<T>::fmadd(to_vec(t).q_, vops<T>::sub(to_vec(b).q_, av), av));
}
template <class A, class B, class S>
    requires(SameFloatVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto lerp_impl(const A& a, const B& b, const S& t) {
    using T = vec_elem_t<A>;
    const auto av = to_vec(a).q_;
    return vec_t<A>(vops<T>::fmadd(vops<T>::splat(static_cast<T>(t)),
                                     vops<T>::sub(to_vec(b).q_, av), av));
}

// glsl::mix: x * (1 - a) + y * a, the formula GLSL defines mix by. Exact at
// both ends (mix(x, y, 1) = y even when |x| is much larger than |y|).
template <class T>
SHADER_MATH_INLINE vreg<T> mix_core(vreg<T> x, vreg<T> y, vreg<T> a) {
    return vops<T>::add(vops<T>::mul(x, vops<T>::sub(vops<T>::splat(T(1.0f)), a)),
                        vops<T>::mul(y, a));
}
template <class A, class B, class C>
    requires(SameFloatVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto mix_impl(const A& x, const B& y, const C& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(mix_core<T>(to_vec(x).q_, to_vec(y).q_, to_vec(a).q_));
}
template <class A, class B, class S>
    requires(SameFloatVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto mix_impl(const A& x, const B& y, const S& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(
        mix_core<T>(to_vec(x).q_, to_vec(y).q_, vops<T>::splat(static_cast<T>(a))));
}

// step(edge, x): x >= edge ? 1 : 0 (note the edge-first argument order)
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto step(const A& edge, const B& x) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::sel(vops<T>::cmp_ge(to_vec(x).q_, to_vec(edge).q_),
                                   vops<T>::splat(T(1.0f)), vops<T>::zero()));
}
template <class S, class B>
    requires(FloatVec<B> && ScalarFor<S, vec_elem_t<B>>)
SHADER_MATH_INLINE auto step(const S& edge, const B& x) {
    using T = vec_elem_t<B>;
    return vec_t<B>(
        vops<T>::sel(vops<T>::cmp_ge(to_vec(x).q_, vops<T>::splat(static_cast<T>(edge))),
                     vops<T>::splat(T(1.0f)), vops<T>::zero()));
}

// smoothstep(lo, hi, x): undefined for lo == hi (division by zero)
template <class T>
SHADER_MATH_INLINE vreg<T> smoothstep_core(vreg<T> lo, vreg<T> hi, vreg<T> x) {
    const vreg<T> d = vops<T>::div(vops<T>::sub(x, lo), vops<T>::sub(hi, lo));
    const vreg<T> t =
        vops<T>::min(vops<T>::max(d, vops<T>::zero()), vops<T>::splat(T(1.0f)));
    // t*t*(3 - 2t)
    return vops<T>::mul(vops<T>::mul(t, t),
                        vops<T>::fmadd(vops<T>::splat(T(-2.0f)), t, vops<T>::splat(T(3.0f))));
}
template <class A, class B, class C>
    requires(SameFloatVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto smoothstep(const A& lo, const B& hi, const C& x) {
    using T = vec_elem_t<A>;
    return vec_t<A>(smoothstep_core<T>(to_vec(lo).q_, to_vec(hi).q_, to_vec(x).q_));
}
template <class SL, class SH, class C>
    requires(FloatVec<C> && ScalarFor<SL, vec_elem_t<C>> && ScalarFor<SH, vec_elem_t<C>>)
SHADER_MATH_INLINE auto smoothstep(const SL& lo, const SH& hi, const C& x) {
    using T = vec_elem_t<C>;
    return vec_t<C>(smoothstep_core<T>(vops<T>::splat(static_cast<T>(lo)),
                                         vops<T>::splat(static_cast<T>(hi)), to_vec(x).q_));
}

// hlsl::frac / glsl::fract: x - floor(x)
template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto fract_impl(const A& a) {
    using T = vec_elem_t<A>;
    const auto v = to_vec(a).q_;
    return vec_t<A>(vops<T>::sub(v, vops<T>::rnd_floor(v)));
}

// hlsl::rcp: exact 1/x (the approximate rcpps is NOT used: its precision is
// implementation-defined even across GPUs, which would make results
// impossible to cross-check)
template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto rcp_impl(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::div(vops<T>::splat(T(1.0f)), to_vec(a).q_));
}

// hlsl::rsqrt / glsl::inversesqrt: exact 1/sqrt(x) (rsqrtps not used, as above)
template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto rsqrt_impl(const A& a) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::div(vops<T>::splat(T(1.0f)), vops<T>::sqrt(to_vec(a).q_)));
}

// ---- transcendental functions: per-lane libm --------------------------------
// No SIMD polynomial approximations: correctness and cross-checkability first.
// half computes in f32 and narrows.
template <int N, class T, class Lg, class F>
SHADER_MATH_INLINE fvec<N, T, Lg> map_lanes(const fvec<N, T, Lg>& v, F&& f) {
    T in[4];
    vops<T>::store4(v.q_, in);
    T out[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) out[i] = static_cast<T>(f(static_cast<float>(in[i])));
    return fvec<N, T, Lg>(vops<T>::load4(out));
}
template <int N, class T, class Lg, class F>
SHADER_MATH_INLINE fvec<N, T, Lg> map_lanes2(const fvec<N, T, Lg>& a, const fvec<N, T, Lg>& b,
                                             F&& f) {
    T ta[4];
    T tb[4];
    vops<T>::store4(a.q_, ta);
    vops<T>::store4(b.q_, tb);
    T out[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i)
        out[i] = static_cast<T>(f(static_cast<float>(ta[i]), static_cast<float>(tb[i])));
    return fvec<N, T, Lg>(vops<T>::load4(out));
}

#define SM_VEC_LIBM_FN(name, call)                                   \
    template <class A>                                               \
        requires FloatVec<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        const vec_t<A> v = to_vec(a);                                \
        return map_lanes(v, [](float x) { return call; });           \
    }

SM_VEC_LIBM_FN(exp, std::exp(x))
SM_VEC_LIBM_FN(exp2, std::exp2(x))
SM_VEC_LIBM_FN(log, std::log(x))
SM_VEC_LIBM_FN(log2, std::log2(x))
SM_VEC_LIBM_FN(sin, std::sin(x))
SM_VEC_LIBM_FN(cos, std::cos(x))
SM_VEC_LIBM_FN(tan, std::tan(x))
SM_VEC_LIBM_FN(asin, std::asin(x))
SM_VEC_LIBM_FN(acos, std::acos(x))
SM_VEC_LIBM_FN(atan, std::atan(x))
SM_VEC_LIBM_FN(sinh, std::sinh(x))
SM_VEC_LIBM_FN(cosh, std::cosh(x))
SM_VEC_LIBM_FN(tanh, std::tanh(x))
#undef SM_VEC_LIBM_FN

// pow(x < 0, non-integer) is NaN per libm (undefined in the shader languages)
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto pow(const A& a, const B& b) {
    const vec_t<A> av = to_vec(a);
    const vec_t<A> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float y) { return std::pow(x, y); });
}
template <class A, class S>
    requires(FloatVec<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto pow(const A& a, const S& s) {
    using T = vec_elem_t<A>;
    const vec_t<A> av = to_vec(a);
    const float y = static_cast<float>(static_cast<T>(s));
    return map_lanes(av, [y](float x) { return std::pow(x, y); });
}

// language-specific unary libm cores (exposed under their names by hlsl.hpp /
// glsl.hpp only)
#define SM_VEC_LIBM_IMPL(name, call)                                 \
    template <class A>                                               \
        requires FloatVec<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        const vec_t<A> v = to_vec(a);                                \
        return map_lanes(v, [](float x) { return call; });           \
    }
SM_VEC_LIBM_IMPL(log10_impl, std::log10(x))  // hlsl::log10
SM_VEC_LIBM_IMPL(asinh_impl, std::asinh(x))  // glsl::asinh
SM_VEC_LIBM_IMPL(acosh_impl, std::acosh(x))  // glsl::acosh
SM_VEC_LIBM_IMPL(atanh_impl, std::atanh(x))  // glsl::atanh
#undef SM_VEC_LIBM_IMPL

// hlsl::ldexp(x, e) = x * 2^e with a float exponent (HLSL takes floats; GLSL's
// int-exponent ldexp is out of scope with the int types)
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto ldexp_impl(const A& a, const B& b) {
    const vec_t<A> av = to_vec(a);
    const vec_t<A> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float e) { return x * std::exp2(e); });
}

// hlsl::frexp(x, out e): mantissa in [0.5, 1) with the sign of x, e as float
template <class A, int N, class T, class Lg>
    requires(FloatVec<A> && std::is_same_v<vec_t<A>, fvec<N, T, Lg>>)
SHADER_MATH_INLINE fvec<N, T, Lg> frexp_impl(const A& a, fvec<N, T, Lg>& e) {
    T in[4];
    vops<T>::store4(to_vec(a).q_, in);
    T m[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    T ex[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) {
        int k = 0;
        m[i] = static_cast<T>(std::frexp(static_cast<float>(in[i]), &k));
        ex[i] = static_cast<T>(static_cast<float>(k));
    }
    e = fvec<N, T, Lg>(vops<T>::load4(ex));
    return fvec<N, T, Lg>(vops<T>::load4(m));
}

// hlsl::atan2(y, x) / glsl::atan(y, x): note the y-first argument order
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto atan2_impl(const A& y, const B& x) {
    const vec_t<A> yv = to_vec(y);
    const vec_t<A> xv = to_vec(x);
    return map_lanes2(yv, xv, [](float a, float b) { return std::atan2(a, b); });
}

// hlsl::fmod: x - y * trunc(x / y), sign of x (std::fmod, exact)
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto fmod_impl(const A& a, const B& b) {
    const vec_t<A> av = to_vec(a);
    const vec_t<A> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float y) { return std::fmod(x, y); });
}
template <class A, class S>
    requires(FloatVec<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto fmod_impl(const A& a, const S& s) {
    using T = vec_elem_t<A>;
    return fmod_impl(a, vec_t<A>(static_cast<T>(s)));
}

// glsl::mod: x - y * floor(x / y), sign of y (the GLSL definition, evaluated
// as written; differs from hlsl::fmod for operands of opposite sign)
template <class T>
SHADER_MATH_INLINE vreg<T> mod_core(vreg<T> x, vreg<T> y) {
    return vops<T>::sub(x, vops<T>::mul(y, vops<T>::rnd_floor(vops<T>::div(x, y))));
}
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto mod_impl(const A& a, const B& b) {
    using T = vec_elem_t<A>;
    return vec_t<A>(mod_core<T>(to_vec(a).q_, to_vec(b).q_));
}
template <class A, class S>
    requires(FloatVec<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto mod_impl(const A& a, const S& s) {
    using T = vec_elem_t<A>;
    return vec_t<A>(mod_core<T>(to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));
}

// hlsl::mad / glsl::fma: a * b + c through ops::fmadd (fused where the
// backend has FMA; neither language guarantees a single rounding unless the
// result is `precise`)
template <class A, class B, class C>
    requires(SameFloatVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto mad_impl(const A& a, const B& b, const C& c) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::fmadd(to_vec(a).q_, to_vec(b).q_, to_vec(c).q_));
}

template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto radians(const A& a) {
    using T = vec_elem_t<A>;
    return to_vec(a) * T(0.017453292519943295f);
}
template <class A>
    requires FloatVec<A>
SHADER_MATH_INLINE auto degrees(const A& a) {
    using T = vec_elem_t<A>;
    return to_vec(a) * T(57.29577951308232f);
}

// modf(x, out ip): fractional part with the sign of x; ip = integer part.
// std::modf per lane, so +-inf gives a +-0 fraction rather than NaN.
template <class A, int N, class T, class Lg>
    requires(FloatVec<A> && std::is_same_v<vec_t<A>, fvec<N, T, Lg>>)
SHADER_MATH_INLINE fvec<N, T, Lg> modf(const A& a, fvec<N, T, Lg>& ip) {
    T in[4];
    vops<T>::store4(to_vec(a).q_, in);
    T fr[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    T ir[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) {
        float whole = 0.0f;
        fr[i] = static_cast<T>(std::modf(static_cast<float>(in[i]), &whole));
        ir[i] = static_cast<T>(whole);
    }
    ip = fvec<N, T, Lg>(vops<T>::load4(ir));
    return fvec<N, T, Lg>(vops<T>::load4(fr));
}

// hlsl::sincos(x, out s, out c)
template <class A, int N, class T, class Lg>
    requires(FloatVec<A> && std::is_same_v<vec_t<A>, fvec<N, T, Lg>>)
SHADER_MATH_INLINE void sincos_impl(const A& a, fvec<N, T, Lg>& s, fvec<N, T, Lg>& c) {
    s = sin(a);
    c = cos(a);
}

// ---- geometric functions (same meaning in both languages) ------------------
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE vec_elem_t<A> distance(const A& a, const B& b) {
    return length(to_vec(a) - to_vec(b));
}

// reflect(i, n) = i - 2 * dot(n, i) * n   (n is expected to be normalized)
template <class A, class B>
    requires SameFloatVec<A, B>
SHADER_MATH_INLINE auto reflect(const A& i0, const B& n0) {
    using T = vec_elem_t<A>;
    const auto i = to_vec(i0);
    const auto nv = to_vec(n0);
    return i - nv * (T(2.0f) * dot(nv, i));
}

// refract(i, n, eta): zero vector on total internal reflection (k < 0)
template <class A, class B, class S>
    requires(SameFloatVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto refract(const A& i0, const B& n0, const S& eta0) {
    using T = vec_elem_t<A>;
    const vec_t<A> i = to_vec(i0);
    const vec_t<A> nv = to_vec(n0);
    const T eta = static_cast<T>(eta0);
    const T d = dot(nv, i);
    const T k = T(1.0f) - eta * eta * (T(1.0f) - d * d);
    if (k < T(0.0f)) return vec_t<A>();
    return i * eta - nv * (eta * d + static_cast<T>(std::sqrt(static_cast<float>(k))));
}

// faceforward(n, i, nref) = dot(nref, i) < 0 ? n : -n
template <class A, class B, class C>
    requires(SameFloatVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto faceforward(const A& n0, const B& i, const C& nref) {
    using T = vec_elem_t<A>;
    const auto nv = to_vec(n0);
    return dot(nref, i) < T(0.0f) ? nv : -nv;
}

// int / uint dot product (HLSL only; GLSL's dot takes floats): the sum wraps
// like the vector arithmetic
template <class A, class B>
    requires(SameVec<A, B> && IntVec<A>)
SHADER_MATH_INLINE vec_elem_t<A> idot(const A& a0, const B& b0) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    T t[4];
    vops<T>::store4(vops<T>::mul(to_vec(a0).q_, to_vec(b0).q_), t);
    using S = std::conditional_t<(sizeof(T) > 4), std::uint64_t, std::uint32_t>;
    S s = 0u;
    for (int i = 0; i < n; ++i) s += static_cast<S>(static_cast<std::make_unsigned_t<T>>(t[i]));
    return static_cast<T>(s);
}

// f applied to each int / uint lane of a, giving R lanes (the bit functions)
template <class R, class A, class F>
SHADER_MATH_INLINE fvec<vec_size_v<A>, R, vec_lang_t<A>> map_int_lanes(const A& a, F f) {
    constexpr int n = vec_size_v<A>;
    const vec_t<A> v = to_vec(a);
    R out[4] = {R(0), R(0), R(0), R(0)};
    for (int i = 0; i < n; ++i) out[i] = f(v[i]);
    return fvec<n, R, vec_lang_t<A>>(vops<R>::load4(out));
}

// The argument of an HLSL bit function: int / uint lanes of any width as they
// are, float and half lanes converted to int (HLSL converts them implicitly;
// countbits(float3), and reversebits(float3) is an int3)
template <class T>
using bits_elem_t = std::conditional_t<is_int_elem_v<T>, T, std::int32_t>;
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto bits_vec(const A& a) {
    return fvec<vec_size_v<A>, bits_elem_t<vec_elem_t<A>>, vec_lang_t<A>>(to_vec(a));
}
// ... and of a scalar (an integer, a floating-point value, or one component):
// integers of 16 or 64 bits keep their width, other integers are 32-bit
template <class S>
inline constexpr bool is_bits_scalar_v =
    !VecLike<S> &&
    ((std::is_integral_v<typename scalar_value<std::remove_cvref_t<S>>::type> &&
      !std::is_same_v<typename scalar_value<std::remove_cvref_t<S>>::type, bool>) ||
     std::is_floating_point_v<typename scalar_value<std::remove_cvref_t<S>>::type>);
template <class S>
    requires is_bits_scalar_v<S>
SHADER_MATH_INLINE auto bits_scalar(const S& s) {
    using V = typename scalar_value<std::remove_cvref_t<S>>::type;
    const V v = static_cast<V>(s);
    if constexpr (std::is_floating_point_v<V>) {
        return elem_cast<std::int32_t>(static_cast<float>(v));
    } else {
        using W = std::conditional_t<sizeof(V) == 2, std::int16_t,
                                     std::conditional_t<sizeof(V) == 8, std::int64_t, std::int32_t>>;
        return static_cast<std::conditional_t<std::is_signed_v<V>, W, std::make_unsigned_t<W>>>(v);
    }
}

// Lanes of the bit functions (hlsl countbits / firstbithigh / firstbitlow /
// reversebits, glsl bitCount / findMSB / findLSB / bitfieldExtract ...), on
// integers of every width
struct popcount_fn {
    SHADER_MATH_INLINE std::uint32_t operator()(auto x) const {
        return int_lane::popcount(static_cast<std::make_unsigned_t<decltype(x)>>(x));
    }
};
// on signed integers, the highest bit that differs from the sign bit
struct high_fn {
    SHADER_MATH_INLINE std::uint32_t operator()(auto x) const {
        if constexpr (std::is_signed_v<decltype(x)>)
            return static_cast<std::uint32_t>(int_lane::msb_s(x));
        else
            return static_cast<std::uint32_t>(int_lane::msb(x));
    }
};
struct low_fn {
    SHADER_MATH_INLINE std::uint32_t operator()(auto x) const {
        return static_cast<std::uint32_t>(int_lane::lsb(static_cast<std::make_unsigned_t<decltype(x)>>(x)));
    }
};
struct reverse_fn {
    SHADER_MATH_INLINE auto operator()(auto x) const {
        using T = decltype(x);
        return static_cast<T>(int_lane::reverse(static_cast<std::make_unsigned_t<T>>(x)));
    }
};
template <class T>
SHADER_MATH_INLINE int find_msb_lane(T x) {
    if constexpr (std::is_signed_v<T>)
        return int_lane::msb_s(x);
    else
        return int_lane::msb(x);
}
template <class T>
SHADER_MATH_INLINE T extract_lane(T x, int offset, int bits) {
    if constexpr (std::is_same_v<T, std::int32_t>)
        return int_lane::extract_s(x, offset, bits);
    else
        return int_lane::extract_u(x, offset, bits);
}
// an int or uint scalar, or one component of an int / uint vector
template <class S>
inline constexpr bool is_int32_scalar_v =
    !VecLike<S> && is_int32_elem_v<typename scalar_value<std::remove_cvref_t<S>>::type>;
// ... of 16, 32 or 64 bits
template <class S>
inline constexpr bool is_int_lane_scalar_v =
    !VecLike<S> && is_int_elem_v<typename scalar_value<std::remove_cvref_t<S>>::type>;
// a scalar (or one component) and an N-component vector argument of language
// Lg where the function takes T
template <class S, class T, class Lg>
concept ScalarArg = !VecLike<S> && arg_elem_v<typename scalar_value<std::remove_cvref_t<S>>::type, T, Lg>;
template <class A, class T, int N, class Lg>
concept VecArg = VecOf<A, Lg> && vec_size_v<A> == N && arg_elem_v<vec_elem_t<A>, T, Lg>;
// the 64-bit products of int / uint lanes, split into high and low halves
template <class A, class B, class T = vec_elem_t<A>>
    requires(IntVec<A> && SameVec<A, B>)
SHADER_MATH_INLINE void mul_extended(const A& x, const B& y, vec_t<A>& msb, vec_t<A>& lsb) {
    using W = std::conditional_t<std::is_same_v<T, std::int32_t>, std::int64_t, std::uint64_t>;
    const vec_t<A> a = to_vec(x);
    const vec_t<A> b = to_vec(y);
    T hi[4] = {T(0), T(0), T(0), T(0)};
    T lo[4] = {T(0), T(0), T(0), T(0)};
    for (int i = 0; i < vec_size_v<A>; ++i) {
        const auto p = static_cast<std::uint64_t>(static_cast<W>(a[i]) * static_cast<W>(b[i]));
        hi[i] = static_cast<T>(static_cast<std::uint32_t>(p >> 32));
        lo[i] = static_cast<T>(static_cast<std::uint32_t>(p));
    }
    msb = vec_t<A>(vops<T>::load4(hi));
    lsb = vec_t<A>(vops<T>::load4(lo));
}

// Integer vector arguments of a floating-point function as float vectors (HLSL
// converts them implicitly: sqrt(int3) is a float3); other arguments as is
template <class A>
    requires(!IntVec<A>)
SHADER_MATH_INLINE const A& float_arg(const A& a) {
    return a;
}
template <class A>
    requires IntVec<A>
SHADER_MATH_INLINE float_vec_t<A> float_arg(const A& a) {
    return float_vec_t<A>(to_vec(a));
}

// ---- bit reinterpretation (hlsl asint / asuint / asfloat, glsl
// floatBitsToInt / floatBitsToUint / intBitsToFloat / uintBitsToFloat) -------
template <class T>
inline constexpr bool is_bits32_v = std::is_same_v<T, float> || is_int32_elem_v<T>;
template <class U, class A>
    requires(VecLike<A> && is_bits32_v<vec_elem_t<A>> && is_bits32_v<U>)
SHADER_MATH_INLINE auto bitcast_vec(const A& a) {
    using T = vec_elem_t<A>;
    using R = fvec<vec_size_v<A>, U, vec_lang_t<A>>;
    const auto q = to_vec(a).q_;
    if constexpr (std::is_same_v<T, U> || (is_int_elem_v<T> && is_int_elem_v<U>))
        return R(q);
    else if constexpr (std::is_same_v<U, float>)
        return R(vops<T>::bits_to_f32(q));
    else
        return R(vops<U>::bits_from_f32(q));
}
// a float, int or uint scalar, or one component of such a vector
template <class S>
inline constexpr bool is_bits32_scalar_v =
    is_bits32_v<typename scalar_value<std::remove_cvref_t<S>>::type>;
template <class U, class S>
    requires(is_bits32_scalar_v<S> && is_bits32_v<U>)
SHADER_MATH_INLINE U bitcast_scalar(const S& s) {
    const auto v = static_cast<typename scalar_value<std::remove_cvref_t<S>>::type>(s);
    U u;
    std::memcpy(&u, &v, sizeof(u));
    return u;
}
// one lane as another type of the same size (the 16-bit casts: hlsl asint16 /
// asuint16 / asfloat16, glsl float16BitsToInt16 ...)
template <class T>
inline constexpr bool is_bits16_v =
#if defined(SHADER_MATH_HAS_HALF)
    std::is_same_v<T, half_t> ||
#endif
    std::is_same_v<T, std::int16_t> || std::is_same_v<T, std::uint16_t>;
template <class U, class V>
    requires(sizeof(U) == sizeof(V))
SHADER_MATH_INLINE U bitcast_lane(const V& v) {
    U u;
    std::memcpy(&u, &v, sizeof(u));
    return u;
}

// The lanes of v side by side in an integer R, the first in the low bits
// (glsl packInt2x16, packFloat2x16 ...), and back
template <class R, int N, class T, class Lg>
SHADER_MATH_INLINE R pack_lanes(const fvec<N, T, Lg>& v) {
    using U = std::make_unsigned_t<R>;
    using B = std::conditional_t<sizeof(T) == 2, std::uint16_t, std::uint32_t>;
    U r = 0u;
    for (int i = 0; i < N; ++i) r |= static_cast<U>(bitcast_lane<B>(v[i])) << (i * 8 * sizeof(T));
    return static_cast<R>(r);
}
template <class T, int N, class Lg, class P>
SHADER_MATH_INLINE fvec<N, T, Lg> unpack_lanes(P p) {
    using B = std::conditional_t<sizeof(T) == 2, std::uint16_t, std::uint32_t>;
    const auto u = static_cast<std::make_unsigned_t<P>>(p);
    T t[4] = {T(0), T(0), T(0), T(0)};
    for (int i = 0; i < N; ++i) t[i] = bitcast_lane<T>(static_cast<B>(u >> (i * 8 * sizeof(T))));
    return fvec<N, T, Lg>(vops<T>::load4(t));
}

// ---- bvec: comparison result type ------------------------------------------
// No arithmetic and no implicit bool conversion; reduce with all()/any().
// Shared by float and half comparisons (half comparisons widen to f32, which
// is exact for every pair of half values).
// m with lane i set to v
SHADER_MATH_INLINE mreg mask_with_lane(mreg m, int i, bool v) {
    float t[4];
    for (int k = 0; k < 4; ++k) t[k] = ops::lane_b(m, k) ? 1.0f : 0.0f;
    t[i] = v ? 1.0f : 0.0f;
    return ops::cmp_ne(ops::load4(t), ops::zero());
}

template <int N, int I>
struct bswz;

// b[i] on a non-const bool vector: reads as a bool, assignment writes lane i
// (from a bool or another component; not an int, as bool3(1) does not
// compile either)
class bref {
public:
    SHADER_MATH_INLINE bref(mreg& m, int i) : m_(m), i_(i) {}
    bref(const bref&) = default;
    SHADER_MATH_INLINE operator bool() const { return ops::lane_b(m_, i_); }
    template <class B>
        requires std::is_same_v<B, bool>
    SHADER_MATH_INLINE bref& operator=(B v) {
        m_ = mask_with_lane(m_, i_, v);
        return *this;
    }
    SHADER_MATH_INLINE bref& operator=(const bref& o) {
        return *this = static_cast<bool>(o);
    }
    template <int N, int I>
    SHADER_MATH_INLINE bref& operator=(const bswz<N, I>& o) {
        return *this = static_cast<bool>(o);
    }

private:
    mreg& m_;
    int i_;
};

// Single-component accessor (b.x, b.a) overlaying the mask register, like
// the float swizzle proxies; out-of-range lanes fail at use. Assignment
// writes the one lane, from a bool or another component (not an int); the
// copy assignment is user-provided because a trivial one would copy the
// whole register.
template <int N, int I>
struct SHADER_MATH_MAY_ALIAS bswz {
    mreg d;

    bswz() = default;
    bswz(const bswz&) = default;

    SHADER_MATH_INLINE operator bool() const {
        static_assert(I < N, "shader-math: bool vector component out of range");
        return ops::lane_b(d, I);
    }
    template <class B>
        requires std::is_same_v<B, bool>
    SHADER_MATH_INLINE bswz& operator=(B v) {
        static_assert(I < N, "shader-math: bool vector component out of range");
        d = mask_with_lane(d, I, v);
        return *this;
    }
    SHADER_MATH_INLINE bswz& operator=(const bswz& o) {
        return *this = static_cast<bool>(o);
    }
    template <int J>
    SHADER_MATH_INLINE bswz& operator=(const bswz<N, J>& o) {
        return *this = static_cast<bool>(o);
    }
    SHADER_MATH_INLINE bswz& operator=(const bref& o) {
        return *this = static_cast<bool>(o);
    }
};

// The mask register and the component accessors share one union; glsl adds
// .stpq, as for its float vectors.
template <int N, class Lg>
struct bvec_store {
    union {
        mreg m_;
        bswz<N, 0> x, r;
        bswz<N, 1> y, g;
        bswz<N, 2> z, b;
        bswz<N, 3> w, a;
    };
    SHADER_MATH_INLINE explicit bvec_store(mreg m) : m_(m) {}
    SHADER_MATH_INLINE bvec_store(const bvec_store& o) : m_(o.m_) {}
    SHADER_MATH_INLINE bvec_store& operator=(const bvec_store& o) {
        m_ = o.m_;
        return *this;
    }
};
template <int N>
struct bvec_store<N, glsl::lang_tag> {
    union {
        mreg m_;
        bswz<N, 0> x, r, s;
        bswz<N, 1> y, g, t;
        bswz<N, 2> z, b, p;
        bswz<N, 3> w, a, q;
    };
    SHADER_MATH_INLINE explicit bvec_store(mreg m) : m_(m) {}
    SHADER_MATH_INLINE bvec_store(const bvec_store& o) : m_(o.m_) {}
    SHADER_MATH_INLINE bvec_store& operator=(const bvec_store& o) {
        m_ = o.m_;
        return *this;
    }
};

template <int N, class Lg>
struct bvec : bvec_store<N, Lg> {
    static_assert(N >= 2 && N <= 4);
    static constexpr int size_v = N;
    using lang_t = Lg;
    using store_t = bvec_store<N, Lg>;
    using store_t::m_;

    SHADER_MATH_INLINE bvec() : store_t(mreg()) {}
    SHADER_MATH_INLINE explicit bvec(mreg m) : store_t(m) {}
    // bool3(true, false, true) / bool3(true): exactly bool arguments, so an
    // int or a float does not silently become a mask
    template <class... Bs>
        requires(sizeof...(Bs) == N && (std::is_same_v<Bs, bool> && ...))
    SHADER_MATH_INLINE bvec(Bs... bs) : store_t(from_bools({bs...})) {}
    // a bool splats implicitly in HLSL (bool3 b = true), only through a
    // constructor in GLSL (bvec3(true))
    template <class B>
        requires std::is_same_v<B, bool>
    SHADER_MATH_INLINE explicit(is_glsl_v<Lg>) bvec(B s) : store_t(from_bools({s, s, s, s})) {}
    // explicit conversion from a numeric vector or swizzle: true where the
    // component is not zero (NaN included), like HLSL bool3(v) / GLSL bvec3(v)
    template <class V>
        requires(VecLike<V> && vec_size_v<V> == N)
    SHADER_MATH_INLINE explicit bvec(const V& v)
        : store_t(vops<vec_elem_t<V>>::cmp_ne(to_vec(v).q_, vops<vec_elem_t<V>>::zero())) {}
    // ... and from the other language's bool vector
    template <class Lg2>
        requires(!std::is_same_v<Lg2, Lg>)
    SHADER_MATH_INLINE explicit bvec(const bvec<N, Lg2>& o) : store_t(o.m_) {}
    SHADER_MATH_INLINE bvec(const bvec& o) : store_t(o.m_) {}
    SHADER_MATH_INLINE bvec& operator=(const bvec& o) {
        m_ = o.m_;
        return *this;
    }

    SHADER_MATH_INLINE bool operator[](int i) const { return ops::lane_b(m_, i); }
    SHADER_MATH_INLINE bref operator[](int i) { return bref(m_, i); }

    // GLSL b.length(): the number of components
    static constexpr int length()
        requires is_glsl_v<Lg>
    {
        return N;
    }

private:
    static SHADER_MATH_INLINE mreg from_bools(std::initializer_list<bool> bs) {
        float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        int i = 0;
        for (bool v : bs) t[i++] = v ? 1.0f : 0.0f;
        return ops::cmp_ne(ops::load4(t), ops::zero());
    }
};

// +b is b (both languages accept unary + on bool vectors)
template <int N, class Lg>
SHADER_MATH_INLINE bvec<N, Lg> operator+(const bvec<N, Lg>& b) {
    return b;
}

template <int N, class Lg>
SHADER_MATH_INLINE bool all(const bvec<N, Lg>& b) {
    return ops::all_n(b.m_, N);
}
template <int N, class Lg>
SHADER_MATH_INLINE bool any(const bvec<N, Lg>& b) {
    return ops::any_n(b.m_, N);
}

// isnan / isinf (both languages), hlsl::isfinite.
// The parameters name fvec / swz explicitly instead of a generic VecLike: the
// MSVC UCRT declares unconstrained global templates isnan(_Ty) / isinf(_Ty) /
// isfinite(_Ty), and only a more specialized parameter wins overload
// resolution against them (a constrained generic `const A&` is ambiguous).
template <int N, class T, class Lg>
    requires(N >= 2 && !is_int_elem_v<T>)
SHADER_MATH_INLINE bvec<N, Lg> isnan(const fvec<N, T, Lg>& v) {
    return bvec<N, Lg>(vops<T>::cmp_ne(v.q_, v.q_));
}
template <int N, class T, class Lg>
    requires(N >= 2 && !is_int_elem_v<T>)
SHADER_MATH_INLINE bvec<N, Lg> isinf(const fvec<N, T, Lg>& v) {
    const auto inf = vops<T>::splat(static_cast<T>(std::numeric_limits<float>::infinity()));
    return bvec<N, Lg>(vops<T>::cmp_eq(vops<T>::abs(v.q_), inf));
}
template <int N, class T, class Lg>
    requires(N >= 2 && !is_int_elem_v<T>)
SHADER_MATH_INLINE bvec<N, Lg> isfinite_impl(const fvec<N, T, Lg>& v) {
    const auto inf = vops<T>::splat(static_cast<T>(std::numeric_limits<float>::infinity()));
    return bvec<N, Lg>(vops<T>::cmp_lt(vops<T>::abs(v.q_), inf));
}
template <class T, int N, class Lg, int... Is>
    requires(sizeof...(Is) >= 2 && !is_int_elem_v<T>)
SHADER_MATH_INLINE auto isnan(const swz<T, N, Lg, Is...>& s) {
    return isnan(to_vec(s));
}
template <class T, int N, class Lg, int... Is>
    requires(sizeof...(Is) >= 2 && !is_int_elem_v<T>)
SHADER_MATH_INLINE auto isinf(const swz<T, N, Lg, Is...>& s) {
    return isinf(to_vec(s));
}

// hlsl::select(cond, a, b) / glsl::mix(x, y, bvec): per-lane choice by a
// comparison mask (note the opposite operand order: select picks a where
// true, mix picks y where true)
template <int N, class Lg, class A, class B>
    requires(SameVec<A, B> && VecOf<A, Lg> && vec_size_v<A> == N)
SHADER_MATH_INLINE auto select_impl(const bvec<N, Lg>& c, const A& a, const B& b) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::sel(c.m_, to_vec(a).q_, to_vec(b).q_));
}
template <int N, class Lg, class A, class S>
    requires(VecOf<A, Lg> && vec_size_v<A> == N && VecScalar<S, A>)
SHADER_MATH_INLINE auto select_impl(const bvec<N, Lg>& c, const A& a, const S& s) {
    using T = vec_elem_t<A>;
    return vec_t<A>(vops<T>::sel(c.m_, to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));
}
template <int N, class Lg, class S, class B>
    requires(VecOf<B, Lg> && vec_size_v<B> == N && VecScalar<S, B>)
SHADER_MATH_INLINE auto select_impl(const bvec<N, Lg>& c, const S& s, const B& b) {
    using T = vec_elem_t<B>;
    return vec_t<B>(vops<T>::sel(c.m_, vops<T>::splat(static_cast<T>(s)), to_vec(b).q_));
}
template <int N, class Lg, class A, class S>
    requires(VecOf<A, Lg> && vec_size_v<A> == N && PromotingScalar<S, A>)
SHADER_MATH_INLINE auto select_impl(const bvec<N, Lg>& c, const A& a, const S& s) {
    using R = int_scalar_op_t<A, S>;
    return select_impl(c, fvec<N, R, Lg>(to_vec(a)), static_cast<R>(s));
}
template <int N, class Lg, class S, class B>
    requires(VecOf<B, Lg> && vec_size_v<B> == N && PromotingScalar<S, B>)
SHADER_MATH_INLINE auto select_impl(const bvec<N, Lg>& c, const S& s, const B& b) {
    using R = int_scalar_op_t<B, S>;
    return select_impl(c, static_cast<R>(s), fvec<N, R, Lg>(to_vec(b)));
}

// ---- scalar arguments ---------------------------------------------------------
// Both languages apply their component-wise functions to scalars as well
// (max(dot(n, l), 0.0)). A scalar call has no vector argument and at least
// one float / half element: a float, a half, or a one-component swizzle or
// matrix element (v.x, m._11). The other arguments follow the vector-scalar
// operand rules (any arithmetic type next to float, exactly half next to
// half). The call runs the vector function on a one-lane splat, so scalars
// and vectors share every rounding and edge-case rule.
template <class S>
struct scalar_elem {
    using type = void;
};
template <>
struct scalar_elem<float> {
    using type = float;
};
#if defined(SHADER_MATH_HAS_HALF)
template <>
struct scalar_elem<half_t> {
    using type = half_t;
};
#endif
template <class T, int N, class Lg, int I>
struct scalar_elem<swz<T, N, Lg, I>> {
    using type = T;
};
template <class T, class Lg>
struct scalar_elem<fvec<1, T, Lg>> {
    using type = T;
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, class Lg, int J>
struct scalar_elem<nswz<T, N, Lg, J>> {
    using type = T;
};
#endif

template <class... S>
struct scalar_call_elem {
    using type = void;
};
template <class S0, class... S>
struct scalar_call_elem<S0, S...> {
    using first = typename scalar_elem<std::remove_cvref_t<S0>>::type;
    using type = std::conditional_t<std::is_void_v<first>, typename scalar_call_elem<S...>::type,
                                    first>;
};
template <class... S>
using scalar_call_t = typename scalar_call_elem<S...>::type;

template <class S, class T>
concept ScalarArgFor =
    std::is_same_v<typename scalar_elem<std::remove_cvref_t<S>>::type, T> ||
    (std::is_same_v<T, float> && std::is_arithmetic_v<std::remove_cvref_t<S>>) ||
    (is_int_elem_v<T> && is_int_scalar_v<S>);
template <class... S>
concept ScalarCall = !std::is_void_v<scalar_call_t<S...>> &&
                     (ScalarArgFor<S, scalar_call_t<S...>> && ...);
// ... with a float / half element (the floating-point functions)
template <class... S>
concept FloatScalarCall = ScalarCall<S...> && !is_int_elem_v<scalar_call_t<S...>>;

// Lg is the language whose functions run on the lane: each language
// namespace uses its own tag (lang_tag, found unqualified by the macros
// below); the shared functions here do not depend on it and use hlsl lanes.
template <class T, class Lg, class S>
SHADER_MATH_INLINE fvec<2, T, Lg> lane1(const S& s) {
    return fvec<2, T, Lg>(static_cast<T>(s));
}
// lane1 for the element type of a scalar call on arguments S...
template <class Lg, class... S, class X>
SHADER_MATH_INLINE fvec<2, scalar_call_t<S...>, Lg> lane_of(const X& x) {
    return lane1<scalar_call_t<S...>, Lg>(x);
}
using lang_tag = hlsl::lang_tag;

}  // namespace detail
}  // namespace shader_math

// A shared function of detail under a language namespace's name, for that
// language's arguments only (hlsl::dot rejects glsl vectors). The enclosing
// namespace declares lang_tag. Unqualified calls also reach detail's own
// overloads through ADL; either one runs the same detail code.
#define SHADER_MATH_LANG_FORWARD(name)                                                   \
    template <class... As>                                                               \
        requires(::shader_math::detail::ArgsOf<lang_tag, std::remove_cvref_t<As>...> &&  \
                 requires(As&&... as) {                                                  \
                     ::shader_math::detail::name(static_cast<As&&>(as)...);              \
                 })                                                                      \
    SHADER_MATH_INLINE decltype(auto) name(As&&... as) {                                 \
        return ::shader_math::detail::name(static_cast<As&&>(as)...);                   \
    }

// Scalar overloads of a component-wise function `name` that has vector
// overloads in the enclosing namespace, which declares lang_tag.
#define SHADER_MATH_SCALAR_LANE(T_, x) ::shader_math::detail::lane1<T_, lang_tag>(x)
// The overloads exist only where the vector function takes the element type
// (sqrt(v.x) on an int vector does not compile, like sqrt on an int vector).
#define SHADER_MATH_SCALAR_FN1(name)                                                  \
    template <class A>                                                                \
        requires(::shader_math::detail::ScalarCall<A> && requires(const A& a) {       \
            name(::shader_math::detail::lane_of<lang_tag, A>(a)); \
        })                                                                            \
    SHADER_MATH_INLINE auto name(const A& a) {                                        \
        using T_ = ::shader_math::detail::scalar_call_t<A>;                           \
        return name(SHADER_MATH_SCALAR_LANE(T_, a))[0];                               \
    }
#define SHADER_MATH_SCALAR_FN2(name)                                                    \
    template <class A, class B>                                                         \
        requires(::shader_math::detail::ScalarCall<A, B> && requires(const A& a, const B& b) { \
            name(::shader_math::detail::lane_of<lang_tag, A, B>(a),  \
                 ::shader_math::detail::lane_of<lang_tag, A, B>(b)); \
        })                                                                              \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                              \
        using T_ = ::shader_math::detail::scalar_call_t<A, B>;                          \
        return name(SHADER_MATH_SCALAR_LANE(T_, a), SHADER_MATH_SCALAR_LANE(T_, b))[0]; \
    }
#define SHADER_MATH_SCALAR_FN3(name)                                                  \
    template <class A, class B, class C>                                              \
        requires(::shader_math::detail::ScalarCall<A, B, C> &&                        \
                 requires(const A& a, const B& b, const C& c) {                       \
                     name(::shader_math::detail::lane_of<lang_tag, A, B, C>(a), \
                          ::shader_math::detail::lane_of<lang_tag, A, B, C>(b), \
                          ::shader_math::detail::lane_of<lang_tag, A, B, C>(c)); \
                 })                                                                   \
    SHADER_MATH_INLINE auto name(const A& a, const B& b, const C& c) {                \
        using T_ = ::shader_math::detail::scalar_call_t<A, B, C>;                     \
        return name(SHADER_MATH_SCALAR_LANE(T_, a), SHADER_MATH_SCALAR_LANE(T_, b),   \
                    SHADER_MATH_SCALAR_LANE(T_, c))[0];                               \
    }

namespace shader_math {
namespace detail {

SHADER_MATH_SCALAR_FN1(abs)
SHADER_MATH_SCALAR_FN1(acos)
SHADER_MATH_SCALAR_FN1(asin)
SHADER_MATH_SCALAR_FN1(atan)
SHADER_MATH_SCALAR_FN1(ceil)
SHADER_MATH_SCALAR_FN1(cos)
SHADER_MATH_SCALAR_FN1(cosh)
SHADER_MATH_SCALAR_FN1(degrees)
SHADER_MATH_SCALAR_FN1(exp)
SHADER_MATH_SCALAR_FN1(exp2)
SHADER_MATH_SCALAR_FN1(floor)
SHADER_MATH_SCALAR_FN1(log)
SHADER_MATH_SCALAR_FN1(log2)
SHADER_MATH_SCALAR_FN1(radians)
SHADER_MATH_SCALAR_FN1(sign)
SHADER_MATH_SCALAR_FN1(sin)
SHADER_MATH_SCALAR_FN1(sinh)
SHADER_MATH_SCALAR_FN1(sqrt)
SHADER_MATH_SCALAR_FN1(tan)
SHADER_MATH_SCALAR_FN1(tanh)
SHADER_MATH_SCALAR_FN1(trunc)
SHADER_MATH_SCALAR_FN2(max)
SHADER_MATH_SCALAR_FN2(min)
SHADER_MATH_SCALAR_FN2(pow)
SHADER_MATH_SCALAR_FN2(step)
SHADER_MATH_SCALAR_FN3(clamp)
SHADER_MATH_SCALAR_FN3(smoothstep)

// Scalar round of the language namespaces (round(float) / round(half) live
// there, not here: a second, identical overload in detail would make a call
// on a component (round(v.x)) ambiguous through ADL). On the register
// directly: a vector here would be instantiated by every translation unit
// that includes the library.
template <class T>
SHADER_MATH_INLINE T round_lane(T a) {
    T t[4];
    vops<T>::store4(vops<T>::rnd_even(vops<T>::splat(a)), t);
    return t[0];
}

// geometric functions on scalars (one-component vectors)
template <class A, class B>
    requires FloatScalarCall<A, B>
SHADER_MATH_INLINE auto dot(const A& a, const B& b) {
    using T = scalar_call_t<A, B>;
    return static_cast<T>(static_cast<T>(a) * static_cast<T>(b));
}
template <class A>
    requires FloatScalarCall<A>
SHADER_MATH_INLINE auto length(const A& a) {
    return abs(a);
}
template <class A, class B>
    requires FloatScalarCall<A, B>
SHADER_MATH_INLINE auto distance(const A& a, const B& b) {
    using T = scalar_call_t<A, B>;
    return abs(static_cast<T>(static_cast<T>(a) - static_cast<T>(b)));
}
// normalize / reflect / refract / faceforward compute in scalars: on a splat
// lane the vector forms would count the value once per lane
template <class A>
    requires FloatScalarCall<A>
SHADER_MATH_INLINE auto normalize(const A& a) {
    using T = scalar_call_t<A>;
    const T v = static_cast<T>(a);
    return static_cast<T>(v / abs(v));
}
template <class A, class B>
    requires FloatScalarCall<A, B>
SHADER_MATH_INLINE auto reflect(const A& i0, const B& n0) {
    using T = scalar_call_t<A, B>;
    const T i = static_cast<T>(i0);
    const T n = static_cast<T>(n0);
    return static_cast<T>(i - n * (T(2.0f) * (n * i)));
}
template <class A, class B, class S>
    requires FloatScalarCall<A, B, S>
SHADER_MATH_INLINE auto refract(const A& i0, const B& n0, const S& eta0) {
    using T = scalar_call_t<A, B, S>;
    const T i = static_cast<T>(i0);
    const T n = static_cast<T>(n0);
    const T eta = static_cast<T>(eta0);
    const T d = n * i;
    const T k = T(1.0f) - eta * eta * (T(1.0f) - d * d);
    if (k < T(0.0f)) return T(0.0f);
    const T root = static_cast<T>(std::sqrt(static_cast<float>(k)));
    return static_cast<T>(i * eta - n * (eta * d + root));
}
template <class A, class B, class C>
    requires FloatScalarCall<A, B, C>
SHADER_MATH_INLINE auto faceforward(const A& n0, const B& i, const C& nref) {
    using T = scalar_call_t<A, B, C>;
    const T n = static_cast<T>(n0);
    return static_cast<T>(static_cast<T>(nref) * static_cast<T>(i) < T(0.0f) ? n : -n);
}
// modf(x, out ip)
template <class A, class T>
    requires(FloatScalarCall<A> && std::is_same_v<T, scalar_call_t<A>>)
SHADER_MATH_INLINE T modf(const A& a, T& ip) {
    hvec<2, T> whole;
    const T fr = modf(lane1<T, lang_tag>(a), whole)[0];
    ip = whole[0];
    return fr;
}

// compile-time loop helper (used by matrix code)
template <int NN, class F>
SHADER_MATH_INLINE void unroll(F&& f) {
    [&]<int... I>(std::integer_sequence<int, I...>) {
        (f.template operator()<I>(), ...);
    }(std::make_integer_sequence<int, NN>{});
}

}  // namespace detail
}  // namespace shader_math
