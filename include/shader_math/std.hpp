// shader-math: C++ standard library integration (opt-in: include this header).
//
//   std::cout << v                 (1, 2, 3)        vectors, swizzles, bool vectors
//   std::cout << m                 ((1, 2), (3, 4)) hlsl: rows, glsl: columns
//   std::format("{:.2f}", v)       the spec applies to every component
//   auto [x, y, z] = v;            tuple protocol; auto& binds the components
//   auto [r0, r1] = m;             hlsl: rows, glsl: columns (m[i])
//   std::unordered_set<float3>     std::hash + std::equal_to (value equality)
//   load<float3>(p) / store(v, p)  N floats; matrices in the constructor order
//
// Kept out of shader_math.hpp so that the library itself pulls in neither
// <ostream> nor <format>. Include it in every translation unit that uses
// the tuple protocol or std::hash on these types: those are specializations
// of std templates and must be visible wherever they are used.
#pragma once

#include "shader_math.hpp"

#include <cstddef>
#include <functional>
#include <ostream>
#include <tuple>
#include <type_traits>

#if __has_include(<format>)
#  include <format>
#endif

namespace shader_math {
namespace detail {

// Element type as written: half prints, formats and hashes through float.
template <class T>
SHADER_MATH_INLINE float to_float(T v) {
    return static_cast<float>(v);
}

template <class M>
struct is_matrix : std::false_type {};
template <int R, int C, class T>
struct is_matrix<hlsl::matrix<R, C, T>> : std::true_type {
    static constexpr int count = R;  // m[i] is a row
    static constexpr int lanes = C;
};
template <int Cn, int Rn, class T>
struct is_matrix<glsl::matrix<Cn, Rn, T>> : std::true_type {
    static constexpr int count = Cn;  // m[i] is a column
    static constexpr int lanes = Rn;
};

// ---- ostream ----------------------------------------------------------------
// Vectors and swizzles of them (std::cout << v.zyx)
template <class Ch, class Tr, class V>
    requires VecLike<V>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const V& v0) {
    const auto v = to_vec(v0);
    os << '(';
    for (int i = 0; i < vec_size_v<V>; ++i) {
        if (i) os << ", ";
        os << to_float(v[i]);
    }
    return os << ')';
}

template <class Ch, class Tr, int N>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const bvec<N>& b) {
    os << '(';
    for (int i = 0; i < N; ++i) os << (i ? ", " : "") << (b[i] ? "true" : "false");
    return os << ')';
}

// ---- tuple protocol (structured bindings) -----------------------------------
template <std::size_t I, int N, class T>
SHADER_MATH_INLINE T& get(fvec<N, T>& v) {
    static_assert(I < static_cast<std::size_t>(N), "shader-math: component out of range");
    return v[static_cast<int>(I)];
}
template <std::size_t I, int N, class T>
SHADER_MATH_INLINE T get(const fvec<N, T>& v) {
    static_assert(I < static_cast<std::size_t>(N), "shader-math: component out of range");
    return v[static_cast<int>(I)];
}
template <std::size_t I, int N, class T>
SHADER_MATH_INLINE T get(fvec<N, T>&& v) {
    static_assert(I < static_cast<std::size_t>(N), "shader-math: component out of range");
    return v[static_cast<int>(I)];
}

// ---- load / store -------------------------------------------------------------
// Vectors read / write N elements. Matrices use the constructor order of
// their language: hlsl row-major (R * C), glsl column-major (Cn * Rn).
template <class V, class T, int... Is>
SHADER_MATH_INLINE V load_lanes(const T* p, std::integer_sequence<int, Is...>) {
    return V(p[Is]...);
}

}  // namespace detail

template <class V, class T>
    requires(detail::is_matrix<V>::value || detail::VecLike<V>)
SHADER_MATH_INLINE V load(const T* p) {
    if constexpr (detail::is_matrix<V>::value) {
        constexpr int n = detail::is_matrix<V>::count * detail::is_matrix<V>::lanes;
        return detail::load_lanes<V>(p, std::make_integer_sequence<int, n>());
    } else {
        return detail::load_lanes<V>(p, std::make_integer_sequence<int, detail::vec_size_v<V>>());
    }
}

template <int N, class E, class T>
SHADER_MATH_INLINE void store(const detail::fvec<N, E>& v, T* p) {
    for (int i = 0; i < N; ++i) p[i] = static_cast<T>(v[i]);
}
template <class M, class T>
    requires detail::is_matrix<M>::value
SHADER_MATH_INLINE void store(const M& m, T* p) {
    constexpr int count = detail::is_matrix<M>::count;
    constexpr int lanes = detail::is_matrix<M>::lanes;
    for (int i = 0; i < count; ++i)
        for (int j = 0; j < lanes; ++j) p[i * lanes + j] = static_cast<T>(m[i][j]);
}

namespace hlsl {

template <class Ch, class Tr, int R, int C, class T>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const matrix<R, C, T>& m) {
    os << '(';
    for (int r = 0; r < R; ++r) os << (r ? ", " : "") << m[r];
    return os << ')';
}

template <std::size_t I, int R, int C, class T>
SHADER_MATH_INLINE detail::fvec<C, T>& get(matrix<R, C, T>& m) {
    static_assert(I < static_cast<std::size_t>(R), "shader-math: row out of range");
    return m[static_cast<int>(I)];
}
template <std::size_t I, int R, int C, class T>
SHADER_MATH_INLINE detail::fvec<C, T> get(const matrix<R, C, T>& m) {
    static_assert(I < static_cast<std::size_t>(R), "shader-math: row out of range");
    return m[static_cast<int>(I)];
}
template <std::size_t I, int R, int C, class T>
SHADER_MATH_INLINE detail::fvec<C, T> get(matrix<R, C, T>&& m) {
    static_assert(I < static_cast<std::size_t>(R), "shader-math: row out of range");
    return m[static_cast<int>(I)];
}

}  // namespace hlsl

namespace glsl {

template <class Ch, class Tr, int Cn, int Rn, class T>
std::basic_ostream<Ch, Tr>& operator<<(std::basic_ostream<Ch, Tr>& os, const matrix<Cn, Rn, T>& m) {
    os << '(';
    for (int c = 0; c < Cn; ++c) os << (c ? ", " : "") << m[c];
    return os << ')';
}

template <std::size_t I, int Cn, int Rn, class T>
SHADER_MATH_INLINE detail::fvec<Rn, T>& get(matrix<Cn, Rn, T>& m) {
    static_assert(I < static_cast<std::size_t>(Cn), "shader-math: column out of range");
    return m[static_cast<int>(I)];
}
template <std::size_t I, int Cn, int Rn, class T>
SHADER_MATH_INLINE detail::fvec<Rn, T> get(const matrix<Cn, Rn, T>& m) {
    static_assert(I < static_cast<std::size_t>(Cn), "shader-math: column out of range");
    return m[static_cast<int>(I)];
}
template <std::size_t I, int Cn, int Rn, class T>
SHADER_MATH_INLINE detail::fvec<Rn, T> get(matrix<Cn, Rn, T>&& m) {
    static_assert(I < static_cast<std::size_t>(Cn), "shader-math: column out of range");
    return m[static_cast<int>(I)];
}

}  // namespace glsl
}  // namespace shader_math

// ---- std specializations ------------------------------------------------------
template <int N, class T>
struct std::tuple_size<shader_math::detail::fvec<N, T>> : std::integral_constant<std::size_t, N> {};
template <std::size_t I, int N, class T>
struct std::tuple_element<I, shader_math::detail::fvec<N, T>> {
    using type = T;
};
template <int R, int C, class T>
struct std::tuple_size<shader_math::hlsl::matrix<R, C, T>>
    : std::integral_constant<std::size_t, R> {};
template <std::size_t I, int R, int C, class T>
struct std::tuple_element<I, shader_math::hlsl::matrix<R, C, T>> {
    using type = shader_math::detail::fvec<C, T>;
};
template <int Cn, int Rn, class T>
struct std::tuple_size<shader_math::glsl::matrix<Cn, Rn, T>>
    : std::integral_constant<std::size_t, Cn> {};
template <std::size_t I, int Cn, int Rn, class T>
struct std::tuple_element<I, shader_math::glsl::matrix<Cn, Rn, T>> {
    using type = shader_math::detail::fvec<Rn, T>;
};

// Value equality over the N components (what all(a == b) means in hlsl and
// a == b in glsl), so unordered containers work with either namespace in
// scope. -0 and +0 hash alike because they compare equal; a NaN key never
// matches, as with float keys.
template <int N, class T>
struct std::equal_to<shader_math::detail::fvec<N, T>> {
    bool operator()(const shader_math::detail::fvec<N, T>& a,
                    const shader_math::detail::fvec<N, T>& b) const {
        for (int i = 0; i < N; ++i)
            if (!(shader_math::detail::to_float(a[i]) == shader_math::detail::to_float(b[i])))
                return false;
        return true;
    }
};
template <int N, class T>
struct std::hash<shader_math::detail::fvec<N, T>> {
    std::size_t operator()(const shader_math::detail::fvec<N, T>& v) const {
        std::size_t h = 0;
        for (int i = 0; i < N; ++i) {
            const float f = shader_math::detail::to_float(v[i]);
            const std::size_t e = std::hash<float>{}(f == 0.0f ? 0.0f : f);
            h ^= e + 0x9e3779b9u + (h << 6) + (h >> 2);
        }
        return h;
    }
};

#if defined(__cpp_lib_format)
// The format spec (fill, width, precision, type) applies to each component.
template <int N, class T, class Ch>
struct std::formatter<shader_math::detail::fvec<N, T>, Ch> : std::formatter<float, Ch> {
    template <class Ctx>
    auto format(const shader_math::detail::fvec<N, T>& v, Ctx& ctx) const {
        auto out = ctx.out();
        *out++ = Ch('(');
        for (int i = 0; i < N; ++i) {
            if (i) {
                *out++ = Ch(',');
                *out++ = Ch(' ');
            }
            ctx.advance_to(out);
            out = std::formatter<float, Ch>::format(shader_math::detail::to_float(v[i]), ctx);
        }
        *out++ = Ch(')');
        return out;
    }
};

template <class M, class Ch>
    requires shader_math::detail::is_matrix<M>::value
struct std::formatter<M, Ch> {
    std::formatter<shader_math::detail::fvec<shader_math::detail::is_matrix<M>::lanes,
                                             typename M::elem_t>,
                   Ch>
        vec_;
    constexpr auto parse(std::basic_format_parse_context<Ch>& ctx) { return vec_.parse(ctx); }
    template <class Ctx>
    auto format(const M& m, Ctx& ctx) const {
        auto out = ctx.out();
        *out++ = Ch('(');
        for (int i = 0; i < shader_math::detail::is_matrix<M>::count; ++i) {
            if (i) {
                *out++ = Ch(',');
                *out++ = Ch(' ');
            }
            ctx.advance_to(out);
            out = vec_.format(m[i], ctx);
        }
        *out++ = Ch(')');
        return out;
    }
};
#endif
