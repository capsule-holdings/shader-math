// Vectors shared by both language namespaces.
//   hlsl::floatN and glsl::vecN are aliases of detail::fvec<N> (same type).
//   bvec<N> is the comparison-result type (mask register, no arithmetic).
// Full swizzle support via proxy members (read: implicit conversion,
// write: constrained operator=; duplicate/out-of-range writes do not compile).
#pragma once

#include <cmath>
#include <type_traits>
#include <utility>

#include "simd.hpp"

namespace shader_math {
namespace detail {

template <int N>
struct fvec;

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
// N = parent vector width, Is... = selected component indices.
template <int N, int... Is>
struct swz {
    static constexpr int M = static_cast<int>(sizeof...(Is));
    static constexpr int idx[M] = {Is...};
    static constexpr bool in_range = ((Is >= 0 && Is < N) && ...);
    static constexpr bool writable = in_range && swz_unique<Is...>();

    float d[4];

    // ---- reads --------------------------------------------------------
    SHADER_MATH_INLINE operator float() const
        requires(M == 1)
    {
        static_assert(in_range,
                      "shader-math: swizzle component out of range for this vector width");
        return d[idx[0]];
    }
    SHADER_MATH_INLINE operator fvec<M>() const
        requires(M >= 2)
    {
        static_assert(in_range,
                      "shader-math: swizzle component out of range for this vector width");
        return fvec<M>(d[Is]...);
    }

    // ---- writes (constraint-based: misuse is a compile error) ----------
    SHADER_MATH_INLINE swz& operator=(float s)
        requires(M == 1 && writable)
    {
        d[idx[0]] = s;
        return *this;
    }
    SHADER_MATH_INLINE swz& operator=(const fvec<M>& v)
        requires(M >= 2 && writable)
    {
        float t[4];
        ops::store4(v.q_, t);
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
            *this = fvec<M>(o);  // gather first: handles aliasing (v.xy = v.yx)
        }
        return *this;
    }
    template <int N2, int... Js>
    SHADER_MATH_INLINE swz& operator=(const swz<N2, Js...>& o)
        requires(static_cast<int>(sizeof...(Js)) == M && M >= 2 && writable)
    {
        return *this = fvec<M>(o);
    }

    // ---- compound assignment -------------------------------------------
#define SM_SWZ_COMPOUND(sym)                                     \
    SHADER_MATH_INLINE swz& operator sym##=(float s)             \
        requires(M == 1 && writable)                             \
    {                                                            \
        d[idx[0]] sym## = s;                                     \
        return *this;                                            \
    }                                                            \
    template <class V>                                           \
    SHADER_MATH_INLINE swz& operator sym##=(const V& v)          \
        requires(M >= 2 && writable)                             \
    {                                                            \
        *this = fvec<M>(*this) sym v;                            \
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
#define SM_SW1(name, i0) ::shader_math::detail::swz<N, i0> name;
#define SM_SW2(name, i0, i1) ::shader_math::detail::swz<N, i0, i1> name;
#define SM_SW3(name, i0, i1, i2) ::shader_math::detail::swz<N, i0, i1, i2> name;
#define SM_SW4(name, i0, i1, i2, i3) ::shader_math::detail::swz<N, i0, i1, i2, i3> name;

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
template <int N>
struct fvec {
    static_assert(N >= 2 && N <= 4, "shader-math: vector width must be 2..4");
    static constexpr int size_v = N;

    union {
        freg q_;
        float comp_[4];
        SM_GEN_XYZW
        SM_GEN_RGBA
    };

    SHADER_MATH_INLINE fvec() : q_(ops::zero()) {}
    SHADER_MATH_INLINE fvec(const fvec& o) : q_(o.q_) {}
    SHADER_MATH_INLINE fvec& operator=(const fvec& o) {
        q_ = o.q_;
        return *this;
    }
    SHADER_MATH_INLINE explicit fvec(freg r) : q_(r) {}
    SHADER_MATH_INLINE fvec(float s) : q_(ops::splat(s)) {}

    SHADER_MATH_INLINE fvec(float x, float y)
        requires(N == 2)
        : q_(ops::set(x, y, 0.0f, 0.0f)) {}
    SHADER_MATH_INLINE fvec(float x, float y, float z)
        requires(N == 3)
        : q_(ops::set(x, y, z, 0.0f)) {}
    SHADER_MATH_INLINE fvec(float x, float y, float z, float w)
        requires(N == 4)
        : q_(ops::set(x, y, z, w)) {}

    // composite constructors (HLSL/GLSL: concatenate vectors and scalars)
    SHADER_MATH_INLINE fvec(const fvec<2>& v, float z)
        requires(N == 3)
        : q_(ops::set(v[0], v[1], z, 0.0f)) {}
    SHADER_MATH_INLINE fvec(float x, const fvec<2>& v)
        requires(N == 3)
        : q_(ops::set(x, v[0], v[1], 0.0f)) {}
    SHADER_MATH_INLINE fvec(const fvec<2>& v, float z, float w)
        requires(N == 4)
        : q_(ops::set(v[0], v[1], z, w)) {}
    SHADER_MATH_INLINE fvec(float x, const fvec<2>& v, float w)
        requires(N == 4)
        : q_(ops::set(x, v[0], v[1], w)) {}
    SHADER_MATH_INLINE fvec(float x, float y, const fvec<2>& v)
        requires(N == 4)
        : q_(ops::set(x, y, v[0], v[1])) {}
    SHADER_MATH_INLINE fvec(const fvec<2>& v, const fvec<2>& u)
        requires(N == 4)
        : q_(ops::set(v[0], v[1], u[0], u[1])) {}
    SHADER_MATH_INLINE fvec(const fvec<3>& v, float w)
        requires(N == 4)
        : q_(ops::set(v[0], v[1], v[2], w)) {}
    SHADER_MATH_INLINE fvec(float x, const fvec<3>& v)
        requires(N == 4)
        : q_(ops::set(x, v[0], v[1], v[2])) {}

    // explicit truncation (HLSL cast / GLSL constructor shortening)
    template <int N2>
    SHADER_MATH_INLINE explicit fvec(const fvec<N2>& o)
        requires(N2 > N)
        : q_(o.q_) {}

    SHADER_MATH_INLINE float& operator[](int i) { return comp_[i]; }
    SHADER_MATH_INLINE float operator[](int i) const { return comp_[i]; }

    SHADER_MATH_INLINE void store(float* p) const {
        float t[4];
        ops::store4(q_, t);
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
template <class T>
struct vec_traits {
    static constexpr int size = 0;
};
template <int N>
struct vec_traits<fvec<N>> {
    static constexpr int size = N;
    static SHADER_MATH_INLINE const fvec<N>& get(const fvec<N>& v) { return v; }
};
template <int N, int... Is>
    requires(sizeof...(Is) >= 2)
struct vec_traits<swz<N, Is...>> {
    static constexpr int size = static_cast<int>(sizeof...(Is));
    static SHADER_MATH_INLINE fvec<size> get(const swz<N, Is...>& s) { return fvec<size>(s); }
};

template <class T>
inline constexpr int vec_size_v = vec_traits<std::remove_cvref_t<T>>::size;

template <class T>
concept VecLike = (vec_size_v<T> >= 2);
template <class A, class B>
concept SameVec = VecLike<A> && VecLike<B> && (vec_size_v<A> == vec_size_v<B>);

template <class T>
SHADER_MATH_INLINE decltype(auto) to_vec(const T& t) {
    return vec_traits<std::remove_cvref_t<T>>::get(t);
}

// ---- component-wise vector operators (identical in HLSL and GLSL) ---------
#define SM_VEC_BINOP(sym, fn)                                                          \
    template <class A, class B>                                                        \
        requires SameVec<A, B>                                                         \
    SHADER_MATH_INLINE auto operator sym(const A& a, const B& b) {                     \
        constexpr int n = vec_size_v<A>;                                               \
        return fvec<n>(ops::fn(to_vec(a).q_, to_vec(b).q_));                           \
    }                                                                                  \
    template <class A>                                                                 \
        requires VecLike<A>                                                            \
    SHADER_MATH_INLINE auto operator sym(const A& a, float s) {                        \
        constexpr int n = vec_size_v<A>;                                               \
        return fvec<n>(ops::fn(to_vec(a).q_, ops::splat(s)));                          \
    }                                                                                  \
    template <class B>                                                                 \
        requires VecLike<B>                                                            \
    SHADER_MATH_INLINE auto operator sym(float s, const B& b) {                        \
        constexpr int n = vec_size_v<B>;                                               \
        return fvec<n>(ops::fn(ops::splat(s), to_vec(b).q_));                          \
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
    return fvec<n>(ops::neg(to_vec(a).q_));
}

#define SM_VEC_COMPOUND(sym)                                                    \
    template <int N, class B>                                                   \
        requires(VecLike<B> && vec_size_v<B> == N)                              \
    SHADER_MATH_INLINE fvec<N>& operator sym##=(fvec<N>& a, const B& b) {       \
        a = a sym b;                                                            \
        return a;                                                               \
    }                                                                           \
    template <int N>                                                            \
    SHADER_MATH_INLINE fvec<N>& operator sym##=(fvec<N>& a, float s) {          \
        a = a sym s;                                                            \
        return a;                                                               \
    }

SM_VEC_COMPOUND(+)
SM_VEC_COMPOUND(-)
SM_VEC_COMPOUND(*)
SM_VEC_COMPOUND(/)
#undef SM_VEC_COMPOUND

// ---- shared vector functions (same meaning in both languages) --------------
template <class A, class B>
    requires SameVec<A, B>
SHADER_MATH_INLINE float dot(const A& a0, const B& b0) {
    constexpr int n = vec_size_v<A>;
    const fvec<n> a = to_vec(a0);
    const fvec<n> b = to_vec(b0);
    float t[4];
    ops::store4(ops::mul(a.q_, b.q_), t);
    float s = t[0] + t[1];
    if constexpr (n >= 3) s += t[2];
    if constexpr (n == 4) s += t[3];
    return s;
}

template <class A, class B>
    requires(SameVec<A, B> && vec_size_v<A> == 3)
SHADER_MATH_INLINE fvec<3> cross(const A& a0, const B& b0) {
    const fvec<3> a = to_vec(a0);
    const fvec<3> b = to_vec(b0);
    return fvec<3>(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                   a[0] * b[1] - a[1] * b[0]);
}

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE float length(const A& a) {
    return std::sqrt(dot(a, a));
}

template <class A>
    requires VecLike<A>
SHADER_MATH_INLINE auto normalize(const A& a) {
    constexpr int n = vec_size_v<A>;
    const fvec<n> v = to_vec(a);
    return v / length(v);
}

// ---- bvec: comparison result type ------------------------------------------
// No arithmetic and no implicit bool conversion; reduce with all()/any().
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
