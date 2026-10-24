// Vectors shared by both language namespaces.
//   hlsl::floatN and glsl::vecN are aliases of detail::fvec<N> (same type).
//   hlsl::halfN and glsl::f16vecN are aliases of detail::fvec<N, half_t>.
//   bvec<N> is the comparison-result type (mask register, no arithmetic);
//   it is shared by float and half vectors (half comparisons widen to f32).
// Full swizzle support via proxy members (read: implicit conversion,
// write: constrained operator=; duplicate/out-of-range writes do not compile).
// Storage is always a 4-lane register. Lanes >= N are unspecified (splats,
// scalar operands and division leave arbitrary values, NaN included), so
// every operation must read lanes [0, N) only (tests/test_pad_lanes.cpp).
#pragma once

#include <cmath>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <utility>

#include "simd.hpp"

#if defined(min) || defined(max)
#  error "shader-math: min/max are defined as macros (windows.h?); define NOMINMAX first"
#endif

namespace shader_math {
namespace detail {

template <int N, class T = float>
struct fvec;
template <int N>
struct bvec;

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
template <class T>
using vops = typename vops_for<T>::ops_t;
template <class T>
using vreg = typename vops_for<T>::reg;

// Constructor/assignment scalar rule: anything explicitly convertible to the
// element type is accepted and converted (HLSL/GLSL constructors convert).
// This is intentionally looser than the operator rule (ScalarFor below):
// half3 h(1.0f, 2.0f, 3.0f) works, half3 * 2.0f does not. Note that on GCC
// float -> _Float16 is not an implicit conversion (C++23 extended
// floating-point rank rules), so plain `T` parameters would reject floats.
template <class S, class T>
concept CvtScalar = requires(const S& s) { static_cast<T>(s); };

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
template <class T, int N, int... Js>
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
    SHADER_MATH_INLINE operator fvec<(K >= 2 ? K : 2), T>() const
        requires(K >= 2)
    {
        return fvec<(K >= 2 ? K : 2), T>(d[Js]...);
    }
};

// Storage of a swizzle proxy: the register bytes it overlays, plus the nested
// members of 2- and 3-component swizzles. I0, I1 (, I2) are the register
// lanes of the enclosing swizzle; the members name them directly, since an
// array lookup there costs a constant evaluation per member (tens of
// thousands per TU).
template <int M, class T, int N, int... Is>
struct SHADER_MATH_MAY_ALIAS swz_store {
    T d[4];
};

#  define SM_NW1(name, i0) ::shader_math::detail::nswz<T, N, I##i0> name;
#  define SM_NW2(name, i0, i1) ::shader_math::detail::nswz<T, N, I##i0, I##i1> name;
#  define SM_NW3(name, i0, i1, i2) \
      ::shader_math::detail::nswz<T, N, I##i0, I##i1, I##i2> name;
#  define SM_NW4(name, i0, i1, i2, i3) \
      ::shader_math::detail::nswz<T, N, I##i0, I##i1, I##i2, I##i3> name;
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
#  if defined(SHADER_MATH_GLSL_STPQ)
#    define SM_NGEN2_STPQ SM_NGEN2(s, t)
#    define SM_NGEN3_STPQ SM_NGEN3(s, t, p)
#  else
#    define SM_NGEN2_STPQ
#    define SM_NGEN3_STPQ
#  endif

template <class T, int N, int I0, int I1>
struct SHADER_MATH_MAY_ALIAS swz_store<2, T, N, I0, I1> {
    union {
        T d[4];
        SM_NGEN2(x, y) SM_NGEN2(r, g) SM_NGEN2_STPQ
    };
};
template <class T, int N, int I0, int I1, int I2>
struct SHADER_MATH_MAY_ALIAS swz_store<3, T, N, I0, I1, I2> {
    union {
        T d[4];
        SM_NGEN3(x, y, z) SM_NGEN3(r, g, b) SM_NGEN3_STPQ
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
#  undef SM_NGEN2_STPQ
#  undef SM_NGEN3_STPQ
#endif

// Swizzle proxy. Lives inside fvec's union; d overlays the register storage.
// T = element type, N = parent vector width, Is... = selected component indices.
template <class T, int N, int... Is>
struct SHADER_MATH_MAY_ALIAS swz
#if defined(SHADER_MATH_NESTED_SWIZZLE)
    : swz_store<static_cast<int>(sizeof...(Is)), T, N, Is...>
#endif
{
    static constexpr int M = static_cast<int>(sizeof...(Is));
    static constexpr int idx[M] = {Is...};
    static constexpr bool in_range = ((Is >= 0 && Is < N) && ...);
    static constexpr bool writable = in_range && swz_unique<Is...>();
#if defined(SHADER_MATH_NESTED_SWIZZLE)
    using swz_store<M, T, N, Is...>::d;
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
        static_assert(in_range,
                      "shader-math: swizzle component out of range for this vector width");
        return d[idx[0]];
    }
    // Width is clamped in the *declaration* so that fvec<1, T> is never named:
    // MSVC instantiates member declaration types while building the enclosing
    // class, which would trip fvec's width static_assert for M == 1 swizzles.
    // The requires-clause still limits the operator to M >= 2.
    SHADER_MATH_INLINE operator fvec<(M >= 2 ? M : 2), T>() const
        requires(M >= 2)
    {
        static_assert(in_range,
                      "shader-math: swizzle component out of range for this vector width");
        return fvec<(M >= 2 ? M : 2), T>(d[Is]...);
    }

    // ---- writes (constraint-based: misuse is a compile error) ----------
    template <class S>
        requires(M == 1 && writable && CvtScalar<S, T>)
    SHADER_MATH_INLINE swz& operator=(const S& s) {
        d[idx[0]] = static_cast<T>(s);
        return *this;
    }
    SHADER_MATH_INLINE swz& operator=(const fvec<(M >= 2 ? M : 2), T>& v)
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
            *this = fvec<M, T>(o);  // gather first: handles aliasing (v.xy = v.yx)
        }
        return *this;
    }
    template <int N2, int... Js>
    SHADER_MATH_INLINE swz& operator=(const swz<T, N2, Js...>& o)
        requires(static_cast<int>(sizeof...(Js)) == M && M >= 2 && writable)
    {
        return *this = fvec<M, T>(o);
    }

    // ---- compound assignment -------------------------------------------
#define SM_SWZ_COMPOUND(sym)                                     \
    template <class S>                                           \
        requires(M == 1 && writable && CvtScalar<S, T>)          \
    SHADER_MATH_INLINE swz& operator sym##=(const S& s) {        \
        d[idx[0]] sym## = static_cast<T>(s);                     \
        return *this;                                            \
    }                                                            \
    template <class V>                                           \
    SHADER_MATH_INLINE swz& operator sym##=(const V& v)          \
        requires(M >= 2 && writable)                             \
    {                                                            \
        *this = fvec<M, T>(*this) sym v;                         \
        return *this;                                            \
    }

    SM_SWZ_COMPOUND(+)
    SM_SWZ_COMPOUND(-)
    SM_SWZ_COMPOUND(*)
    SM_SWZ_COMPOUND(/)
#undef SM_SWZ_COMPOUND
};

// ---- swizzle member generation ------------------------------------------
// All names over {x,y,z,w} / {r,g,b,a} are declared for every vector width;
// out-of-range components fail at the point of use.
#define SM_SW1(name, i0) ::shader_math::detail::swz<T, N, i0> name;
#define SM_SW2(name, i0, i1) ::shader_math::detail::swz<T, N, i0, i1> name;
#define SM_SW3(name, i0, i1, i2) ::shader_math::detail::swz<T, N, i0, i1, i2> name;
#define SM_SW4(name, i0, i1, i2, i3) ::shader_math::detail::swz<T, N, i0, i1, i2, i3> name;

// NOTE: index parameters are named i0/i1/i2 — they must never collide with the
// pasted letter tokens (x/y/z/w/r/g/b/a).
#define SM_ROW2(p, i0) \
    SM_SW2(p##x, i0, 0) SM_SW2(p##y, i0, 1) SM_SW2(p##z, i0, 2) SM_SW2(p##w, i0, 3)
#define SM_ROW3(p, i0, i1)                                                       \
    SM_SW3(p##x, i0, i1, 0) SM_SW3(p##y, i0, i1, 1) SM_SW3(p##z, i0, i1, 2)      \
    SM_SW3(p##w, i0, i1, 3)
#define SM_PL3(p, i0) \
    SM_ROW3(p##x, i0, 0) SM_ROW3(p##y, i0, 1) SM_ROW3(p##z, i0, 2) SM_ROW3(p##w, i0, 3)
#define SM_ROW4(p, i0, i1, i2)                                                          \
    SM_SW4(p##x, i0, i1, i2, 0) SM_SW4(p##y, i0, i1, i2, 1) SM_SW4(p##z, i0, i1, i2, 2) \
    SM_SW4(p##w, i0, i1, i2, 3)
#define SM_PL4(p, i0, i1)                                                       \
    SM_ROW4(p##x, i0, i1, 0) SM_ROW4(p##y, i0, i1, 1) SM_ROW4(p##z, i0, i1, 2)  \
    SM_ROW4(p##w, i0, i1, 3)
#define SM_CU4(p, i0) \
    SM_PL4(p##x, i0, 0) SM_PL4(p##y, i0, 1) SM_PL4(p##z, i0, 2) SM_PL4(p##w, i0, 3)

#define SM_GEN_XYZW                                                     \
    SM_SW1(x, 0) SM_SW1(y, 1) SM_SW1(z, 2) SM_SW1(w, 3)                 \
    SM_ROW2(x, 0) SM_ROW2(y, 1) SM_ROW2(z, 2) SM_ROW2(w, 3)             \
    SM_PL3(x, 0) SM_PL3(y, 1) SM_PL3(z, 2) SM_PL3(w, 3)                 \
    SM_CU4(x, 0) SM_CU4(y, 1) SM_CU4(z, 2) SM_CU4(w, 3)

#define SM_RROW2(p, i0) \
    SM_SW2(p##r, i0, 0) SM_SW2(p##g, i0, 1) SM_SW2(p##b, i0, 2) SM_SW2(p##a, i0, 3)
#define SM_RROW3(p, i0, i1)                                                      \
    SM_SW3(p##r, i0, i1, 0) SM_SW3(p##g, i0, i1, 1) SM_SW3(p##b, i0, i1, 2)      \
    SM_SW3(p##a, i0, i1, 3)
#define SM_RPL3(p, i0) \
    SM_RROW3(p##r, i0, 0) SM_RROW3(p##g, i0, 1) SM_RROW3(p##b, i0, 2) SM_RROW3(p##a, i0, 3)
#define SM_RROW4(p, i0, i1, i2)                                                         \
    SM_SW4(p##r, i0, i1, i2, 0) SM_SW4(p##g, i0, i1, i2, 1) SM_SW4(p##b, i0, i1, i2, 2) \
    SM_SW4(p##a, i0, i1, i2, 3)
#define SM_RPL4(p, i0, i1)                                                       \
    SM_RROW4(p##r, i0, i1, 0) SM_RROW4(p##g, i0, i1, 1) SM_RROW4(p##b, i0, i1, 2) \
    SM_RROW4(p##a, i0, i1, 3)
#define SM_RCU4(p, i0) \
    SM_RPL4(p##r, i0, 0) SM_RPL4(p##g, i0, 1) SM_RPL4(p##b, i0, 2) SM_RPL4(p##a, i0, 3)

#define SM_GEN_RGBA                                                     \
    SM_SW1(r, 0) SM_SW1(g, 1) SM_SW1(b, 2) SM_SW1(a, 3)                 \
    SM_RROW2(r, 0) SM_RROW2(g, 1) SM_RROW2(b, 2) SM_RROW2(a, 3)         \
    SM_RPL3(r, 0) SM_RPL3(g, 1) SM_RPL3(b, 2) SM_RPL3(a, 3)             \
    SM_RCU4(r, 0) SM_RCU4(g, 1) SM_RCU4(b, 2) SM_RCU4(a, 3)

// GLSL's third swizzle set (texture coordinates), opt-in: the vector type is
// shared with hlsl, which has no stpq, so enabling it lets hlsl code accept
// .stp too. SHADER_MATH_GLSL_STPQ must be defined identically in every
// translation unit (it changes the class definition). The prefix parameter
// is `pre`, not `p`, so it never collides with the pasted letter p.
#if defined(SHADER_MATH_GLSL_STPQ)
#  define SM_SROW2(pre, i0) \
    SM_SW2(pre##s, i0, 0) SM_SW2(pre##t, i0, 1) SM_SW2(pre##p, i0, 2) SM_SW2(pre##q, i0, 3)
#  define SM_SROW3(pre, i0, i1)                                                  \
    SM_SW3(pre##s, i0, i1, 0) SM_SW3(pre##t, i0, i1, 1) SM_SW3(pre##p, i0, i1, 2) \
    SM_SW3(pre##q, i0, i1, 3)
#  define SM_SPL3(pre, i0)                                                       \
    SM_SROW3(pre##s, i0, 0) SM_SROW3(pre##t, i0, 1) SM_SROW3(pre##p, i0, 2)      \
    SM_SROW3(pre##q, i0, 3)
#  define SM_SROW4(pre, i0, i1, i2)                                                       \
    SM_SW4(pre##s, i0, i1, i2, 0) SM_SW4(pre##t, i0, i1, i2, 1) SM_SW4(pre##p, i0, i1, i2, 2) \
    SM_SW4(pre##q, i0, i1, i2, 3)
#  define SM_SPL4(pre, i0, i1)                                                   \
    SM_SROW4(pre##s, i0, i1, 0) SM_SROW4(pre##t, i0, i1, 1) SM_SROW4(pre##p, i0, i1, 2) \
    SM_SROW4(pre##q, i0, i1, 3)
#  define SM_SCU4(pre, i0)                                                       \
    SM_SPL4(pre##s, i0, 0) SM_SPL4(pre##t, i0, 1) SM_SPL4(pre##p, i0, 2)         \
    SM_SPL4(pre##q, i0, 3)
#  define SM_GEN_STPQ                                                   \
    SM_SW1(s, 0) SM_SW1(t, 1) SM_SW1(p, 2) SM_SW1(q, 3)                 \
    SM_SROW2(s, 0) SM_SROW2(t, 1) SM_SROW2(p, 2) SM_SROW2(q, 3)         \
    SM_SPL3(s, 0) SM_SPL3(t, 1) SM_SPL3(p, 2) SM_SPL3(q, 3)             \
    SM_SCU4(s, 0) SM_SCU4(t, 1) SM_SCU4(p, 2) SM_SCU4(q, 3)
#else
#  define SM_GEN_STPQ
#endif

// ---- fvec -----------------------------------------------------------------
template <int N, class T>
struct fvec {
    static_assert(N >= 2 && N <= 4, "shader-math: vector width must be 2..4");
    static constexpr int size_v = N;
    using elem_t = T;
    using ops_t = vops<T>;
    using reg_t = vreg<T>;

    union {
        reg_t q_;
        T comp_[4];
        SM_GEN_XYZW
        SM_GEN_RGBA
        SM_GEN_STPQ
    };

    SHADER_MATH_INLINE fvec() : q_(ops_t::zero()) {}
    SHADER_MATH_INLINE fvec(const fvec& o) : q_(o.q_) {}
    SHADER_MATH_INLINE fvec& operator=(const fvec& o) {
        q_ = o.q_;
        return *this;
    }
    SHADER_MATH_INLINE explicit fvec(reg_t r) : q_(r) {}
    template <class S>
        requires CvtScalar<S, T>
    SHADER_MATH_INLINE fvec(const S& s) : q_(ops_t::splat(static_cast<T>(s))) {}

    template <class X, class Y>
        requires(N == 2 && CvtScalar<X, T> && CvtScalar<Y, T>)
    SHADER_MATH_INLINE fvec(X x, Y y)
        : q_(ops_t::set(static_cast<T>(x), static_cast<T>(y), T(0.0f), T(0.0f))) {}
    template <class X, class Y, class Z>
        requires(N == 3 && CvtScalar<X, T> && CvtScalar<Y, T> && CvtScalar<Z, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, Z z)
        : q_(ops_t::set(static_cast<T>(x), static_cast<T>(y), static_cast<T>(z), T(0.0f))) {}
    template <class X, class Y, class Z, class W>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<Y, T> && CvtScalar<Z, T> &&
                 CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, Z z, W w)
        : q_(ops_t::set(static_cast<T>(x), static_cast<T>(y), static_cast<T>(z),
                        static_cast<T>(w))) {}

    // composite constructors (HLSL/GLSL: concatenate vectors and scalars)
    template <class Z>
        requires(N == 3 && CvtScalar<Z, T>)
    SHADER_MATH_INLINE fvec(const fvec<2, T>& v, Z z)
        : q_(ops_t::set(v[0], v[1], static_cast<T>(z), T(0.0f))) {}
    template <class X>
        requires(N == 3 && CvtScalar<X, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<2, T>& v)
        : q_(ops_t::set(static_cast<T>(x), v[0], v[1], T(0.0f))) {}
    template <class Z, class W>
        requires(N == 4 && CvtScalar<Z, T> && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(const fvec<2, T>& v, Z z, W w)
        : q_(ops_t::set(v[0], v[1], static_cast<T>(z), static_cast<T>(w))) {}
    template <class X, class W>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<2, T>& v, W w)
        : q_(ops_t::set(static_cast<T>(x), v[0], v[1], static_cast<T>(w))) {}
    template <class X, class Y>
        requires(N == 4 && CvtScalar<X, T> && CvtScalar<Y, T>)
    SHADER_MATH_INLINE fvec(X x, Y y, const fvec<2, T>& v)
        : q_(ops_t::set(static_cast<T>(x), static_cast<T>(y), v[0], v[1])) {}
    SHADER_MATH_INLINE fvec(const fvec<2, T>& v, const fvec<2, T>& u)
        requires(N == 4)
        : q_(ops_t::set(v[0], v[1], u[0], u[1])) {}
    template <class W>
        requires(N == 4 && CvtScalar<W, T>)
    SHADER_MATH_INLINE fvec(const fvec<3, T>& v, W w)
        : q_(ops_t::set(v[0], v[1], v[2], static_cast<T>(w))) {}
    template <class X>
        requires(N == 4 && CvtScalar<X, T>)
    SHADER_MATH_INLINE fvec(X x, const fvec<3, T>& v)
        : q_(ops_t::set(static_cast<T>(x), v[0], v[1], v[2])) {}

    // explicit truncation (HLSL cast / GLSL constructor shortening)
    template <int N2>
    SHADER_MATH_INLINE explicit fvec(const fvec<N2, T>& o)
        requires(N2 > N)
        : q_(o.q_) {}

    // explicit element-type conversion (half <-> float); no implicit mixing
    template <class U>
    SHADER_MATH_INLINE explicit fvec(const fvec<N, U>& o)
        requires(!std::is_same_v<U, T>)
        : q_(ops_t::set(static_cast<T>(o[0]), static_cast<T>(o[1]),
                        N > 2 ? static_cast<T>(o[2]) : T(0.0f),
                        N > 3 ? static_cast<T>(o[3]) : T(0.0f))) {}
    // explicit conversion from a bool vector: 1 where true, 0 where false
    // (HLSL float3(b), GLSL vec3(b))
    template <int M>
        requires(M == N)
    SHADER_MATH_INLINE explicit fvec(const bvec<M>& b)
        : q_(ops_t::sel(b.m_, ops_t::splat(T(1.0f)), ops_t::zero())) {}
    // ... and from a swizzle of the other element type (half4(f.wzyx)): the
    // proxy cannot deduce U through the overload above
    template <class U, int M, int... Is>
        requires(!std::is_same_v<U, T> && sizeof...(Is) == N)
    SHADER_MATH_INLINE explicit fvec(const swz<U, M, Is...>& s) : fvec(fvec<N, U>(s)) {}

    SHADER_MATH_INLINE T& operator[](int i) { return comp_[i]; }
    SHADER_MATH_INLINE T operator[](int i) const { return comp_[i]; }

    SHADER_MATH_INLINE void store(T* p) const {
        T t[4];
        ops_t::store4(q_, t);
        for (int i = 0; i < N; ++i) p[i] = t[i];
    }
};

#undef SM_SW1
#undef SM_SW2
#undef SM_SW3
#undef SM_SW4
#undef SM_ROW2
#undef SM_ROW3
#undef SM_PL3
#undef SM_ROW4
#undef SM_PL4
#undef SM_CU4
#undef SM_GEN_XYZW
#undef SM_RROW2
#undef SM_RROW3
#undef SM_RPL3
#undef SM_RROW4
#undef SM_RPL4
#undef SM_RCU4
#undef SM_GEN_RGBA
#if defined(SHADER_MATH_GLSL_STPQ)
#  undef SM_SROW2
#  undef SM_SROW3
#  undef SM_SPL3
#  undef SM_SROW4
#  undef SM_SPL4
#  undef SM_SCU4
#endif
#undef SM_GEN_STPQ

// ---- vec_traits / concepts -------------------------------------------------
template <class V>
struct vec_traits {
    static constexpr int size = 0;
};
template <int N, class T>
struct vec_traits<fvec<N, T>> {
    static constexpr int size = N;
    using elem = T;
    static SHADER_MATH_INLINE const fvec<N, T>& get(const fvec<N, T>& v) { return v; }
};
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
struct vec_traits<swz<T, N, Is...>> {
    static constexpr int size = static_cast<int>(sizeof...(Is));
    using elem = T;
    static SHADER_MATH_INLINE fvec<size, T> get(const swz<T, N, Is...>& s) {
        return fvec<size, T>(s);
    }
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
struct vec_traits<nswz<T, N, Is...>> {
    static constexpr int size = static_cast<int>(sizeof...(Is));
    using elem = T;
    static SHADER_MATH_INLINE fvec<size, T> get(const nswz<T, N, Is...>& s) {
        return fvec<size, T>(s);
    }
};
#endif

template <class V>
inline constexpr int vec_size_v = vec_traits<std::remove_cvref_t<V>>::size;
template <class V>
using vec_elem_t = typename vec_traits<std::remove_cvref_t<V>>::elem;

template <class V>
concept VecLike = (vec_size_v<V> >= 2);
// Same width AND same element type: half and float vectors never mix
// implicitly (convert explicitly instead).
template <class A, class B>
concept SameVec = VecLike<A> && VecLike<B> && (vec_size_v<A> == vec_size_v<B>) &&
                  std::is_same_v<vec_elem_t<A>, vec_elem_t<B>>;

// Scalar operand rules for vector-scalar operators:
//   float vectors: any arithmetic scalar (int / double literals keep working)
//   half vectors:  exactly half (no implicit float mixing: half3 * 2.0f fails)
//   both:          a single-component swizzle of the same element type
template <class S>
struct swz_elem {
    using type = void;
};
template <class T, int N, int I>
struct swz_elem<swz<T, N, I>> {
    using type = T;
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, int J>
struct swz_elem<nswz<T, N, J>> {
    using type = T;
};
#endif
template <class S, class T>
concept ScalarFor =
    std::is_same_v<std::remove_cvref_t<S>, T> ||
    (std::is_same_v<T, float> && std::is_arithmetic_v<std::remove_cvref_t<S>>) ||
    std::is_same_v<typename swz_elem<std::remove_cvref_t<S>>::type, T>;

template <class V>
SHADER_MATH_INLINE decltype(auto) to_vec(const V& t) {
    return vec_traits<std::remove_cvref_t<V>>::get(t);
}

// ---- component-wise vector operators (identical in HLSL and GLSL) ---------
#define SM_VEC_BINOP(sym, fn)                                                          \
    template <class A, class B>                                                        \
        requires SameVec<A, B>                                                         \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                     \
        constexpr int n = vec_size_v<A>;                                               \
        using T = vec_elem_t<A>;                                                       \
        return fvec<n, T>(vops<T>::fn(to_vec(a).q_, to_vec(b).q_));                    \
    }                                                                                  \
    template <class A, class S>                                                        \
        requires(VecLike<A> && ScalarFor<S, vec_elem_t<A>>)                            \
    SHADER_MATH_INLINE auto operator sym(const A& a, const S& s) {                     \
        constexpr int n = vec_size_v<A>;                                               \
        using T = vec_elem_t<A>;                                                       \
        return fvec<n, T>(vops<T>::fn(to_vec(a).q_, vops<T>::splat(static_cast<T>(s)))); \
    }                                                                                  \
    template <class B, class S>                                                        \
        requires(VecLike<B> && ScalarFor<S, vec_elem_t<B>>)                            \
    SHADER_MATH_INLINE auto operator sym(const S& s, const B& b) {                     \
        constexpr int n = vec_size_v<B>;                                               \
        using T = vec_elem_t<B>;                                                       \
        return fvec<n, T>(vops<T>::fn(vops<T>::splat(static_cast<T>(s)), to_vec(b).q_)); \
    }

SM_VEC_BINOP(+, add)
SM_VEC_BINOP(-, sub)
SM_VEC_BINOP(*, mul)
SM_VEC_BINOP(/, div)
#undef SM_VEC_BINOP

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto operator-(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::neg(to_vec(a).q_));
}

#define SM_VEC_COMPOUND(sym)                                                       \
    template <int N, class T, class B>                                             \
        requires(SameVec<fvec<N, T>, B>)                                           \
    SHADER_MATH_INLINE fvec<N, T>& operator sym##=(fvec<N, T>& a, const B& b) {    \
        a = a sym b;                                                               \
        return a;                                                                  \
    }                                                                              \
    template <int N, class T, class S>                                             \
        requires ScalarFor<S, T>                                                   \
    SHADER_MATH_INLINE fvec<N, T>& operator sym##=(fvec<N, T>& a, const S& s) {    \
        a = a sym s;                                                               \
        return a;                                                                  \
    }

SM_VEC_COMPOUND(+)
SM_VEC_COMPOUND(-)
SM_VEC_COMPOUND(*)
SM_VEC_COMPOUND(/)
#undef SM_VEC_COMPOUND

// ---- shared vector functions (same meaning in both languages) --------------
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE vec_elem_t<A> dot(const A& a0, const B& b0) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> a = to_vec(a0);
    const fvec<n, T> b = to_vec(b0);
    T t[4];
    vops<T>::store4(vops<T>::mul(a.q_, b.q_), t);
    T s = t[0] + t[1];
    if constexpr (n >= 3) s = s + t[2];
    if constexpr (n == 4) s = s + t[3];
    return s;
}

template <class A, class B>
    requires(SameVec<A, B> && vec_size_v<A> == 3)
SHADER_MATH_INLINE auto cross(const A& a0, const B& b0) {
    using T = vec_elem_t<A>;
    const fvec<3, T> a = to_vec(a0);
    const fvec<3, T> b = to_vec(b0);
    return fvec<3, T>(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                      a[0] * b[1] - a[1] * b[0]);
}

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE vec_elem_t<A> length(const A& a) {
    using T = vec_elem_t<A>;
    return static_cast<T>(std::sqrt(static_cast<float>(dot(a, a))));
}

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto normalize(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> v = to_vec(a);
    return v / length(v);
}

// ---- component-wise functions (same meaning in both languages) --------------
// Language-specific spellings (hlsl lerp/frac/rcp/rsqrt/saturate, glsl
// mix/fract/inversesqrt/roundEven) forward to the *_impl cores below from
// their own namespaces so they never leak across languages via ADL.

#define SM_VEC_UNARY_FN(name, fn)                                    \
    template <class A>                                               \
        requires VecLike<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        constexpr int n = vec_size_v<A>;                             \
        using T = vec_elem_t<A>;                                     \
        return fvec<n, T>(vops<T>::fn(to_vec(a).q_));                \
    }

SM_VEC_UNARY_FN(abs, abs)
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
        constexpr int n = vec_size_v<A>;                                       \
        using T = vec_elem_t<A>;                                               \
        return fvec<n, T>(vops<T>::name(to_vec(a).q_, to_vec(b).q_));          \
    }                                                                          \
    template <class A, class S>                                                \
        requires(VecLike<A> && ScalarFor<S, vec_elem_t<A>>)                    \
    SHADER_MATH_INLINE auto name(const A& a, const S& s) {                     \
        constexpr int n = vec_size_v<A>;                                       \
        using T = vec_elem_t<A>;                                               \
        return fvec<n, T>(                                                     \
            vops<T>::name(to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));   \
    }

SM_VEC_MINMAX_FN(min)
SM_VEC_MINMAX_FN(max)
#undef SM_VEC_MINMAX_FN

// sign: -1 / 0 / +1 as the same vector type in both languages (HLSL's int
// return type is intentionally not replicated). sign(NaN) == 0.
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto sign(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const auto v = to_vec(a).q_;
    const auto zero = vops<T>::zero();
    const auto one = vops<T>::splat(T(1.0f));
    return fvec<n, T>(vops<T>::sub(vops<T>::sel(vops<T>::cmp_gt(v, zero), one, zero),
                                   vops<T>::sel(vops<T>::cmp_lt(v, zero), one, zero)));
}

template <class A, class B, class C>
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto clamp(const A& x, const B& lo, const C& hi) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(
        vops<T>::min(vops<T>::max(to_vec(x).q_, to_vec(lo).q_), to_vec(hi).q_));
}
template <class A, class SL, class SH>
    requires(VecLike<A> && ScalarFor<SL, vec_elem_t<A>> && ScalarFor<SH, vec_elem_t<A>>)
SHADER_MATH_INLINE auto clamp(const A& x, const SL& lo, const SH& hi) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::min(
        vops<T>::max(to_vec(x).q_, vops<T>::splat(static_cast<T>(lo))),
        vops<T>::splat(static_cast<T>(hi))));
}

// hlsl::saturate (GLSL has no saturate; use clamp(x, 0.0, 1.0))
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto saturate_impl(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::min(vops<T>::max(to_vec(a).q_, vops<T>::zero()),
                                   vops<T>::splat(T(1.0f))));
}

// hlsl::lerp: a + t*(b - a), the form DXC emits for DXIL; fmadd path (rounding
// differs with FMA)
template <class A, class B, class C>
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto lerp_impl(const A& a, const B& b, const C& t) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const auto av = to_vec(a).q_;
    return fvec<n, T>(vops<T>::fmadd(to_vec(t).q_, vops<T>::sub(to_vec(b).q_, av), av));
}
template <class A, class B, class S>
    requires(SameVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto lerp_impl(const A& a, const B& b, const S& t) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const auto av = to_vec(a).q_;
    return fvec<n, T>(vops<T>::fmadd(vops<T>::splat(static_cast<T>(t)),
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
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto mix_impl(const A& x, const B& y, const C& a) {
    using T = vec_elem_t<A>;
    return fvec<vec_size_v<A>, T>(mix_core<T>(to_vec(x).q_, to_vec(y).q_, to_vec(a).q_));
}
template <class A, class B, class S>
    requires(SameVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto mix_impl(const A& x, const B& y, const S& a) {
    using T = vec_elem_t<A>;
    return fvec<vec_size_v<A>, T>(
        mix_core<T>(to_vec(x).q_, to_vec(y).q_, vops<T>::splat(static_cast<T>(a))));
}

// step(edge, x): x >= edge ? 1 : 0 (note the edge-first argument order)
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE auto step(const A& edge, const B& x) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::sel(vops<T>::cmp_ge(to_vec(x).q_, to_vec(edge).q_),
                                   vops<T>::splat(T(1.0f)), vops<T>::zero()));
}
template <class S, class B>
    requires(VecLike<B> && ScalarFor<S, vec_elem_t<B>>)
SHADER_MATH_INLINE auto step(const S& edge, const B& x) {
    constexpr int n = vec_size_v<B>;
    using T = vec_elem_t<B>;
    return fvec<n, T>(
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
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto smoothstep(const A& lo, const B& hi, const C& x) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(smoothstep_core<T>(to_vec(lo).q_, to_vec(hi).q_, to_vec(x).q_));
}
template <class SL, class SH, class C>
    requires(VecLike<C> && ScalarFor<SL, vec_elem_t<C>> && ScalarFor<SH, vec_elem_t<C>>)
SHADER_MATH_INLINE auto smoothstep(const SL& lo, const SH& hi, const C& x) {
    constexpr int n = vec_size_v<C>;
    using T = vec_elem_t<C>;
    return fvec<n, T>(smoothstep_core<T>(vops<T>::splat(static_cast<T>(lo)),
                                         vops<T>::splat(static_cast<T>(hi)), to_vec(x).q_));
}

// hlsl::frac / glsl::fract: x - floor(x)
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto fract_impl(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const auto v = to_vec(a).q_;
    return fvec<n, T>(vops<T>::sub(v, vops<T>::rnd_floor(v)));
}

// hlsl::rcp: exact 1/x (the approximate rcpps is NOT used: its precision is
// implementation-defined even across GPUs, which would make results
// impossible to cross-check)
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto rcp_impl(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::div(vops<T>::splat(T(1.0f)), to_vec(a).q_));
}

// hlsl::rsqrt / glsl::inversesqrt: exact 1/sqrt(x) (rsqrtps not used, as above)
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto rsqrt_impl(const A& a) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::div(vops<T>::splat(T(1.0f)), vops<T>::sqrt(to_vec(a).q_)));
}

// ---- transcendental functions: per-lane libm --------------------------------
// No SIMD polynomial approximations: correctness and cross-checkability first.
// half computes in f32 and narrows.
template <int N, class T, class F>
SHADER_MATH_INLINE fvec<N, T> map_lanes(const fvec<N, T>& v, F&& f) {
    T in[4];
    vops<T>::store4(v.q_, in);
    T out[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) out[i] = static_cast<T>(f(static_cast<float>(in[i])));
    return fvec<N, T>(vops<T>::load4(out));
}
template <int N, class T, class F>
SHADER_MATH_INLINE fvec<N, T> map_lanes2(const fvec<N, T>& a, const fvec<N, T>& b, F&& f) {
    T ta[4];
    T tb[4];
    vops<T>::store4(a.q_, ta);
    vops<T>::store4(b.q_, tb);
    T out[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i)
        out[i] = static_cast<T>(f(static_cast<float>(ta[i]), static_cast<float>(tb[i])));
    return fvec<N, T>(vops<T>::load4(out));
}

#define SM_VEC_LIBM_FN(name, call)                                   \
    template <class A>                                               \
        requires VecLike<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        constexpr int n = vec_size_v<A>;                             \
        using T = vec_elem_t<A>;                                     \
        const fvec<n, T> v = to_vec(a);                              \
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
    requires SameVec<A, B>
SHADER_MATH_INLINE auto pow(const A& a, const B& b) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> av = to_vec(a);
    const fvec<n, T> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float y) { return std::pow(x, y); });
}
template <class A, class S>
    requires(VecLike<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto pow(const A& a, const S& s) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> av = to_vec(a);
    const float y = static_cast<float>(static_cast<T>(s));
    return map_lanes(av, [y](float x) { return std::pow(x, y); });
}

// language-specific unary libm cores (exposed under their names by hlsl.hpp /
// glsl.hpp only)
#define SM_VEC_LIBM_IMPL(name, call)                                 \
    template <class A>                                               \
        requires VecLike<A>                                          \
    SHADER_MATH_INLINE auto name(const A& a) {                       \
        constexpr int n = vec_size_v<A>;                             \
        using T = vec_elem_t<A>;                                     \
        const fvec<n, T> v = to_vec(a);                              \
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
    requires SameVec<A, B>
SHADER_MATH_INLINE auto ldexp_impl(const A& a, const B& b) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> av = to_vec(a);
    const fvec<n, T> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float e) { return x * std::exp2(e); });
}

// hlsl::frexp(x, out e): mantissa in [0.5, 1) with the sign of x, e as float
template <class A, int N, class T>
    requires(VecLike<A> && vec_size_v<A> == N && std::is_same_v<vec_elem_t<A>, T>)
SHADER_MATH_INLINE fvec<N, T> frexp_impl(const A& a, fvec<N, T>& e) {
    T in[4];
    vops<T>::store4(to_vec(a).q_, in);
    T m[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    T ex[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) {
        int k = 0;
        m[i] = static_cast<T>(std::frexp(static_cast<float>(in[i]), &k));
        ex[i] = static_cast<T>(static_cast<float>(k));
    }
    e = fvec<N, T>(vops<T>::load4(ex));
    return fvec<N, T>(vops<T>::load4(m));
}

// hlsl::atan2(y, x) / glsl::atan(y, x): note the y-first argument order
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE auto atan2_impl(const A& y, const B& x) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> yv = to_vec(y);
    const fvec<n, T> xv = to_vec(x);
    return map_lanes2(yv, xv, [](float a, float b) { return std::atan2(a, b); });
}

// hlsl::fmod: x - y * trunc(x / y), sign of x (std::fmod, exact)
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE auto fmod_impl(const A& a, const B& b) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> av = to_vec(a);
    const fvec<n, T> bv = to_vec(b);
    return map_lanes2(av, bv, [](float x, float y) { return std::fmod(x, y); });
}
template <class A, class S>
    requires(VecLike<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto fmod_impl(const A& a, const S& s) {
    using T = vec_elem_t<A>;
    return fmod_impl(a, fvec<vec_size_v<A>, T>(static_cast<T>(s)));
}

// glsl::mod: x - y * floor(x / y), sign of y (the GLSL definition, evaluated
// as written; differs from hlsl::fmod for operands of opposite sign)
template <class T>
SHADER_MATH_INLINE vreg<T> mod_core(vreg<T> x, vreg<T> y) {
    return vops<T>::sub(x, vops<T>::mul(y, vops<T>::rnd_floor(vops<T>::div(x, y))));
}
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE auto mod_impl(const A& a, const B& b) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(mod_core<T>(to_vec(a).q_, to_vec(b).q_));
}
template <class A, class S>
    requires(VecLike<A> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto mod_impl(const A& a, const S& s) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(mod_core<T>(to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));
}

// hlsl::mad / glsl::fma: a * b + c through ops::fmadd (fused where the
// backend has FMA; neither language guarantees a single rounding unless the
// result is `precise`)
template <class A, class B, class C>
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto mad_impl(const A& a, const B& b, const C& c) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    return fvec<n, T>(vops<T>::fmadd(to_vec(a).q_, to_vec(b).q_, to_vec(c).q_));
}

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto radians(const A& a) {
    using T = vec_elem_t<A>;
    return to_vec(a) * T(0.017453292519943295f);
}
template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto degrees(const A& a) {
    using T = vec_elem_t<A>;
    return to_vec(a) * T(57.29577951308232f);
}

// modf(x, out ip): fractional part with the sign of x; ip = integer part.
// std::modf per lane, so +-inf gives a +-0 fraction rather than NaN.
template <class A, int N, class T>
    requires(VecLike<A> && vec_size_v<A> == N && std::is_same_v<vec_elem_t<A>, T>)
SHADER_MATH_INLINE fvec<N, T> modf(const A& a, fvec<N, T>& ip) {
    T in[4];
    vops<T>::store4(to_vec(a).q_, in);
    T fr[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    T ir[4] = {T(0.0f), T(0.0f), T(0.0f), T(0.0f)};
    for (int i = 0; i < N; ++i) {
        float whole = 0.0f;
        fr[i] = static_cast<T>(std::modf(static_cast<float>(in[i]), &whole));
        ir[i] = static_cast<T>(whole);
    }
    ip = fvec<N, T>(vops<T>::load4(ir));
    return fvec<N, T>(vops<T>::load4(fr));
}

// hlsl::sincos(x, out s, out c)
template <class A, int N, class T>
    requires(VecLike<A> && vec_size_v<A> == N && std::is_same_v<vec_elem_t<A>, T>)
SHADER_MATH_INLINE void sincos_impl(const A& a, fvec<N, T>& s, fvec<N, T>& c) {
    s = sin(a);
    c = cos(a);
}

// ---- geometric functions (same meaning in both languages) ------------------
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE vec_elem_t<A> distance(const A& a, const B& b) {
    return length(to_vec(a) - to_vec(b));
}

// reflect(i, n) = i - 2 * dot(n, i) * n   (n is expected to be normalized)
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE auto reflect(const A& i0, const B& n0) {
    using T = vec_elem_t<A>;
    const auto i = to_vec(i0);
    const auto nv = to_vec(n0);
    return i - nv * (T(2.0f) * dot(nv, i));
}

// refract(i, n, eta): zero vector on total internal reflection (k < 0)
template <class A, class B, class S>
    requires(SameVec<A, B> && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto refract(const A& i0, const B& n0, const S& eta0) {
    constexpr int n = vec_size_v<A>;
    using T = vec_elem_t<A>;
    const fvec<n, T> i = to_vec(i0);
    const fvec<n, T> nv = to_vec(n0);
    const T eta = static_cast<T>(eta0);
    const T d = dot(nv, i);
    const T k = T(1.0f) - eta * eta * (T(1.0f) - d * d);
    if (k < T(0.0f)) return fvec<n, T>();
    return i * eta - nv * (eta * d + static_cast<T>(std::sqrt(static_cast<float>(k))));
}

// faceforward(n, i, nref) = dot(nref, i) < 0 ? n : -n
template <class A, class B, class C>
    requires(SameVec<A, B> && SameVec<A, C>)
SHADER_MATH_INLINE auto faceforward(const A& n0, const B& i, const C& nref) {
    using T = vec_elem_t<A>;
    const auto nv = to_vec(n0);
    return dot(nref, i) < T(0.0f) ? nv : -nv;
}

// ---- bvec: comparison result type ------------------------------------------
// No arithmetic and no implicit bool conversion; reduce with all()/any().
// Shared by float and half comparisons (half comparisons widen to f32, which
// is exact for every pair of half values).
// Read-only single-component accessor (b.x, b.a) overlaying the mask
// register, like the float swizzle proxies; out-of-range lanes fail at use.
template <int N, int I>
struct SHADER_MATH_MAY_ALIAS bswz {
    mreg d;
    SHADER_MATH_INLINE operator bool() const {
        static_assert(I < N, "shader-math: bool vector component out of range");
        return ops::lane_b(d, I);
    }
};

template <int N>
struct bvec {
    static_assert(N >= 2 && N <= 4);
    static constexpr int size_v = N;
    union {
        mreg m_;
        bswz<N, 0> x, r;
        bswz<N, 1> y, g;
        bswz<N, 2> z, b;
        bswz<N, 3> w, a;
#if defined(SHADER_MATH_GLSL_STPQ)
        bswz<N, 0> s;
        bswz<N, 1> t;
        bswz<N, 2> p;
        bswz<N, 3> q;
#endif
    };

    SHADER_MATH_INLINE bvec() : m_() {}
    SHADER_MATH_INLINE explicit bvec(mreg m) : m_(m) {}
    // bool3(true, false, true) / bool3(true): exactly bool arguments, so an
    // int or a float does not silently become a mask
    template <class... Bs>
        requires(sizeof...(Bs) == N && (std::is_same_v<Bs, bool> && ...))
    SHADER_MATH_INLINE bvec(Bs... bs) : m_(from_bools({bs...})) {}
    template <class B>
        requires std::is_same_v<B, bool>
    SHADER_MATH_INLINE explicit bvec(B s) : m_(from_bools({s, s, s, s})) {}
    // explicit conversion from a numeric vector or swizzle: true where the
    // component is not zero (NaN included), like HLSL bool3(v) / GLSL bvec3(v)
    template <class V>
        requires(VecLike<V> && vec_size_v<V> == N)
    SHADER_MATH_INLINE explicit bvec(const V& v)
        : m_(vops<vec_elem_t<V>>::cmp_ne(to_vec(v).q_, vops<vec_elem_t<V>>::zero())) {}
    SHADER_MATH_INLINE bvec(const bvec& o) : m_(o.m_) {}
    SHADER_MATH_INLINE bvec& operator=(const bvec& o) {
        m_ = o.m_;
        return *this;
    }

    SHADER_MATH_INLINE bool operator[](int i) const { return ops::lane_b(m_, i); }

private:
    static SHADER_MATH_INLINE mreg from_bools(std::initializer_list<bool> bs) {
        float t[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        int i = 0;
        for (bool v : bs) t[i++] = v ? 1.0f : 0.0f;
        return ops::cmp_ne(ops::load4(t), ops::zero());
    }
};

template <int N>
SHADER_MATH_INLINE bool all(const bvec<N>& b) {
    return ops::all_n(b.m_, N);
}
template <int N>
SHADER_MATH_INLINE bool any(const bvec<N>& b) {
    return ops::any_n(b.m_, N);
}

// ---- bmat: matrix comparison result (HLSL) ---------------------------------
template <int R, int C>
struct bmat {
    bvec<C> row_[R];
    SHADER_MATH_INLINE bvec<C>& operator[](int r) { return row_[r]; }
    SHADER_MATH_INLINE const bvec<C>& operator[](int r) const { return row_[r]; }
};

template <int R, int C>
SHADER_MATH_INLINE bool all(const bmat<R, C>& b) {
    for (int r = 0; r < R; ++r)
        if (!all(b.row_[r])) return false;
    return true;
}
template <int R, int C>
SHADER_MATH_INLINE bool any(const bmat<R, C>& b) {
    for (int r = 0; r < R; ++r)
        if (any(b.row_[r])) return true;
    return false;
}

// isnan / isinf (both languages), hlsl::isfinite.
// The parameters name fvec / swz explicitly instead of a generic VecLike: the
// MSVC UCRT declares unconstrained global templates isnan(_Ty) / isinf(_Ty) /
// isfinite(_Ty), and only a more specialized parameter wins overload
// resolution against them (a constrained generic `const A&` is ambiguous).
template <int N, class T>
SHADER_MATH_INLINE bvec<N> isnan(const fvec<N, T>& v) {
    return bvec<N>(vops<T>::cmp_ne(v.q_, v.q_));
}
template <int N, class T>
SHADER_MATH_INLINE bvec<N> isinf(const fvec<N, T>& v) {
    const auto inf = vops<T>::splat(static_cast<T>(std::numeric_limits<float>::infinity()));
    return bvec<N>(vops<T>::cmp_eq(vops<T>::abs(v.q_), inf));
}
template <int N, class T>
SHADER_MATH_INLINE bvec<N> isfinite_impl(const fvec<N, T>& v) {
    const auto inf = vops<T>::splat(static_cast<T>(std::numeric_limits<float>::infinity()));
    return bvec<N>(vops<T>::cmp_lt(vops<T>::abs(v.q_), inf));
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
SHADER_MATH_INLINE auto isnan(const swz<T, N, Is...>& s) {
    return isnan(to_vec(s));
}
template <class T, int N, int... Is>
    requires(sizeof...(Is) >= 2)
SHADER_MATH_INLINE auto isinf(const swz<T, N, Is...>& s) {
    return isinf(to_vec(s));
}

// hlsl::select(cond, a, b) / glsl::mix(x, y, bvec): per-lane choice by a
// comparison mask (note the opposite operand order: select picks a where
// true, mix picks y where true)
template <int N, class A, class B>
    requires(SameVec<A, B> && vec_size_v<A> == N)
SHADER_MATH_INLINE auto select_impl(const bvec<N>& c, const A& a, const B& b) {
    using T = vec_elem_t<A>;
    return fvec<N, T>(vops<T>::sel(c.m_, to_vec(a).q_, to_vec(b).q_));
}
template <int N, class A, class S>
    requires(VecLike<A> && vec_size_v<A> == N && ScalarFor<S, vec_elem_t<A>>)
SHADER_MATH_INLINE auto select_impl(const bvec<N>& c, const A& a, const S& s) {
    using T = vec_elem_t<A>;
    return fvec<N, T>(vops<T>::sel(c.m_, to_vec(a).q_, vops<T>::splat(static_cast<T>(s))));
}
template <int N, class S, class B>
    requires(VecLike<B> && vec_size_v<B> == N && ScalarFor<S, vec_elem_t<B>>)
SHADER_MATH_INLINE auto select_impl(const bvec<N>& c, const S& s, const B& b) {
    using T = vec_elem_t<B>;
    return fvec<N, T>(vops<T>::sel(c.m_, vops<T>::splat(static_cast<T>(s)), to_vec(b).q_));
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
template <class T, int N, int I>
struct scalar_elem<swz<T, N, I>> {
    using type = T;
};
#if defined(SHADER_MATH_NESTED_SWIZZLE)
template <class T, int N, int J>
struct scalar_elem<nswz<T, N, J>> {
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
    (std::is_same_v<T, float> && std::is_arithmetic_v<std::remove_cvref_t<S>>);
template <class... S>
concept ScalarCall = !std::is_void_v<scalar_call_t<S...>> &&
                     (ScalarArgFor<S, scalar_call_t<S...>> && ...);

template <class T, class S>
SHADER_MATH_INLINE fvec<2, T> lane1(const S& s) {
    return fvec<2, T>(static_cast<T>(s));
}

}  // namespace detail
}  // namespace shader_math

// Scalar overloads of a component-wise function `name` that has vector
// overloads in the enclosing namespace.
#define SHADER_MATH_SCALAR_FN1(name)                                                  \
    template <class A>                                                                \
        requires ::shader_math::detail::ScalarCall<A>                                 \
    SHADER_MATH_INLINE auto name(const A& a) {                                        \
        using T_ = ::shader_math::detail::scalar_call_t<A>;                           \
        return name(::shader_math::detail::lane1<T_>(a))[0];                          \
    }
#define SHADER_MATH_SCALAR_FN2(name)                                                  \
    template <class A, class B>                                                       \
        requires ::shader_math::detail::ScalarCall<A, B>                              \
    SHADER_MATH_INLINE auto name(const A& a, const B& b) {                            \
        using T_ = ::shader_math::detail::scalar_call_t<A, B>;                        \
        return name(::shader_math::detail::lane1<T_>(a), ::shader_math::detail::lane1<T_>(b))[0]; \
    }
#define SHADER_MATH_SCALAR_FN3(name)                                                  \
    template <class A, class B, class C>                                              \
        requires ::shader_math::detail::ScalarCall<A, B, C>                           \
    SHADER_MATH_INLINE auto name(const A& a, const B& b, const C& c) {                \
        using T_ = ::shader_math::detail::scalar_call_t<A, B, C>;                     \
        return name(::shader_math::detail::lane1<T_>(a), ::shader_math::detail::lane1<T_>(b), \
                    ::shader_math::detail::lane1<T_>(c))[0];                          \
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

// round on scalars is not a template: libc++ and MSVC declare ::round(float)
// in the global namespace, and a template would quietly lose to it (halves
// away from zero). As a plain function an unqualified call is ambiguous
// there instead; hlsl::round(x) / glsl::round(x) always resolve here.
SHADER_MATH_INLINE float round(float a) {
    return round(fvec<2, float>(a))[0];
}
#if defined(SHADER_MATH_HAS_HALF)
SHADER_MATH_INLINE half_t round(half_t a) {
    return round(fvec<2, half_t>(a))[0];
}
#endif

// isnan / isinf on scalars: by value, so that against the MSVC UCRT's
// unconstrained isnan(_Ty) the constraint decides (see isnan above); where
// the C library has a plain isnan(float) it wins, with the same result
template <class A>
    requires ScalarCall<A>
SHADER_MATH_INLINE bool isnan(A a) {
    return isnan(lane1<scalar_call_t<A>>(a))[0];
}
template <class A>
    requires ScalarCall<A>
SHADER_MATH_INLINE bool isinf(A a) {
    return isinf(lane1<scalar_call_t<A>>(a))[0];
}

// geometric functions on scalars (one-component vectors)
template <class A, class B>
    requires ScalarCall<A, B>
SHADER_MATH_INLINE auto dot(const A& a, const B& b) {
    using T = scalar_call_t<A, B>;
    return static_cast<T>(static_cast<T>(a) * static_cast<T>(b));
}
template <class A>
    requires ScalarCall<A>
SHADER_MATH_INLINE auto length(const A& a) {
    return abs(a);
}
template <class A, class B>
    requires ScalarCall<A, B>
SHADER_MATH_INLINE auto distance(const A& a, const B& b) {
    using T = scalar_call_t<A, B>;
    return abs(static_cast<T>(static_cast<T>(a) - static_cast<T>(b)));
}
// modf(x, out ip)
template <class A, class T>
    requires(ScalarCall<A> && std::is_same_v<T, scalar_call_t<A>>)
SHADER_MATH_INLINE T modf(const A& a, T& ip) {
    fvec<2, T> whole;
    const T fr = modf(lane1<T>(a), whole)[0];
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
