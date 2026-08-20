// Vectors shared by both language namespaces.
//   hlsl::floatN and glsl::vecN are aliases of detail::fvec<N> (same type).
//   hlsl::halfN and glsl::f16vecN are aliases of detail::fvec<N, half_t>.
//   bvec<N> is the comparison-result type (mask register, no arithmetic);
//   it is shared by float and half vectors (half comparisons widen to f32).
// Full swizzle support via proxy members (read: implicit conversion,
// write: constrained operator=; duplicate/out-of-range writes do not compile).
#pragma once

#include <cmath>
#include <type_traits>
#include <utility>

#include "simd.hpp"

namespace shader_math {
namespace detail {

template <int N, class T = float>
struct fvec;

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

// Swizzle proxy. Lives inside fvec's union; d overlays the register storage.
// T = element type, N = parent vector width, Is... = selected component indices.
template <class T, int N, int... Is>
struct swz {
    static constexpr int M = static_cast<int>(sizeof...(Is));
    static constexpr int idx[M] = {Is...};
    static constexpr bool in_range = ((Is >= 0 && Is < N) && ...);
    static constexpr bool writable = in_range && swz_unique<Is...>();

    T d[4];

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

// ---- bvec: comparison result type ------------------------------------------
// No arithmetic and no implicit bool conversion; reduce with all()/any().
// Shared by float and half comparisons (half comparisons widen to f32, which
// is exact for every pair of half values).
template <int N>
struct bvec {
    static_assert(N >= 2 && N <= 4);
    static constexpr int size_v = N;
    mreg m_;

    SHADER_MATH_INLINE bvec() : m_() {}
    SHADER_MATH_INLINE explicit bvec(mreg m) : m_(m) {}
    SHADER_MATH_INLINE bool operator[](int i) const { return ops::lane_b(m_, i); }
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

// compile-time loop helper (used by matrix code)
template <int NN, class F>
SHADER_MATH_INLINE void unroll(F&& f) {
    [&]<int... I>(std::integer_sequence<int, I...>) {
        (f.template operator()<I>(), ...);
    }(std::make_integer_sequence<int, NN>{});
}

}  // namespace detail
}  // namespace shader_math
