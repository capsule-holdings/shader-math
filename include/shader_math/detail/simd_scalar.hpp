// Scalar reference backend: 4-wide freg/mreg/ops without intrinsics
// (RISC-V and anything without NEON/SSE2, or SHADER_MATH_FORCE_SCALAR).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_scalar.hpp"
#endif

#include <cmath>

namespace shader_math {
namespace detail {

struct alignas(16) freg {
    float f[4];
};
struct alignas(16) mreg {
    unsigned m[4];
};

struct ops {
    static SHADER_MATH_INLINE freg zero() { return {{0.0f, 0.0f, 0.0f, 0.0f}}; }
    static SHADER_MATH_INLINE freg splat(float s) { return {{s, s, s, s}}; }
    static SHADER_MATH_INLINE freg set(float x, float y, float z, float w) {
        return {{x, y, z, w}};
    }
    static SHADER_MATH_INLINE freg load4(const float* p) { return {{p[0], p[1], p[2], p[3]}}; }
    static SHADER_MATH_INLINE void store4(freg v, float* p) {
        p[0] = v.f[0];
        p[1] = v.f[1];
        p[2] = v.f[2];
        p[3] = v.f[3];
    }
    static SHADER_MATH_INLINE float lane(freg v, int i) { return v.f[i]; }
    template <int I>
    static SHADER_MATH_INLINE float lane_c(freg v) { return v.f[I]; }
    template <int I>
    static SHADER_MATH_INLINE freg bcast(freg v) { return splat(v.f[I]); }

    static SHADER_MATH_INLINE freg cat_lo(freg a, freg b) {
        return {{a.f[0], a.f[1], b.f[0], b.f[1]}};
    }
    static SHADER_MATH_INLINE freg cat_hi(freg a, freg b) {
        return {{a.f[2], a.f[3], b.f[2], b.f[3]}};
    }
    static SHADER_MATH_INLINE freg even(freg a, freg b) {
        return {{a.f[0], a.f[2], b.f[0], b.f[2]}};
    }
    static SHADER_MATH_INLINE freg odd(freg a, freg b) {
        return {{a.f[1], a.f[3], b.f[1], b.f[3]}};
    }
    static SHADER_MATH_INLINE freg swap_pairs(freg a) {
        return {{a.f[1], a.f[0], a.f[3], a.f[2]}};
    }
    static SHADER_MATH_INLINE freg swap_halves(freg a) {
        return {{a.f[2], a.f[3], a.f[0], a.f[1]}};
    }

    static SHADER_MATH_INLINE freg add(freg a, freg b) {
        return {{a.f[0] + b.f[0], a.f[1] + b.f[1], a.f[2] + b.f[2], a.f[3] + b.f[3]}};
    }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) {
        return {{a.f[0] - b.f[0], a.f[1] - b.f[1], a.f[2] - b.f[2], a.f[3] - b.f[3]}};
    }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) {
        return {{a.f[0] * b.f[0], a.f[1] * b.f[1], a.f[2] * b.f[2], a.f[3] * b.f[3]}};
    }
    // Unused lanes of an N < 4 vector divide 0 / 0 by design (their value is
    // unspecified); MSVC's optimizer reports the folded constant as C4723.
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4723)
#endif
    static SHADER_MATH_INLINE freg div(freg a, freg b) {
        return {{a.f[0] / b.f[0], a.f[1] / b.f[1], a.f[2] / b.f[2], a.f[3] / b.f[3]}};
    }
#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
    static SHADER_MATH_INLINE freg neg(freg a) { return {{-a.f[0], -a.f[1], -a.f[2], -a.f[3]}}; }
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) {
        return {{a.f[0] * b.f[0] + c.f[0], a.f[1] * b.f[1] + c.f[1], a.f[2] * b.f[2] + c.f[2],
                 a.f[3] * b.f[3] + c.f[3]}};
    }
    // c - a*b
    static SHADER_MATH_INLINE freg fnmadd(freg a, freg b, freg c) {
        return {{c.f[0] - a.f[0] * b.f[0], c.f[1] - a.f[1] * b.f[1], c.f[2] - a.f[2] * b.f[2],
                 c.f[3] - a.f[3] * b.f[3]}};
    }

#define SHADER_MATH_SCALAR_MAP1(name, expr)                                          \
    static SHADER_MATH_INLINE freg name(freg a) {                                    \
        auto f = [](float x) { return expr; };                                       \
        return {{f(a.f[0]), f(a.f[1]), f(a.f[2]), f(a.f[3])}};                       \
    }
    SHADER_MATH_SCALAR_MAP1(abs, std::fabs(x))
    SHADER_MATH_SCALAR_MAP1(sqrt, std::sqrt(x))
    SHADER_MATH_SCALAR_MAP1(rnd_floor, std::floor(x))
    SHADER_MATH_SCALAR_MAP1(rnd_ceil, std::ceil(x))
    SHADER_MATH_SCALAR_MAP1(rnd_trunc, std::trunc(x))
    SHADER_MATH_SCALAR_MAP1(rnd_even, std::nearbyint(x))  // assumes default FE_TONEAREST
    SHADER_MATH_SCALAR_MAP1(rnd_away, std::round(x))
#undef SHADER_MATH_SCALAR_MAP1

    // NaN handling matches the SSE2 backend (returns b when the comparison
    // with a NaN operand is false); shader languages leave it undefined.
    static SHADER_MATH_INLINE freg min(freg a, freg b) {
        return {{a.f[0] < b.f[0] ? a.f[0] : b.f[0], a.f[1] < b.f[1] ? a.f[1] : b.f[1],
                 a.f[2] < b.f[2] ? a.f[2] : b.f[2], a.f[3] < b.f[3] ? a.f[3] : b.f[3]}};
    }
    static SHADER_MATH_INLINE freg max(freg a, freg b) {
        return {{a.f[0] > b.f[0] ? a.f[0] : b.f[0], a.f[1] > b.f[1] ? a.f[1] : b.f[1],
                 a.f[2] > b.f[2] ? a.f[2] : b.f[2], a.f[3] > b.f[3] ? a.f[3] : b.f[3]}};
    }

    // m ? a : b, per lane
    static SHADER_MATH_INLINE freg sel(mreg m, freg a, freg b) {
        return {{m.m[0] ? a.f[0] : b.f[0], m.m[1] ? a.f[1] : b.f[1],
                 m.m[2] ? a.f[2] : b.f[2], m.m[3] ? a.f[3] : b.f[3]}};
    }

#define SHADER_MATH_SCALAR_CMP(name, op)                                             \
    static SHADER_MATH_INLINE mreg name(freg a, freg b) {                            \
        return {{a.f[0] op b.f[0] ? 0xFFFFFFFFu : 0u, a.f[1] op b.f[1] ? 0xFFFFFFFFu : 0u, \
                 a.f[2] op b.f[2] ? 0xFFFFFFFFu : 0u, a.f[3] op b.f[3] ? 0xFFFFFFFFu : 0u}}; \
    }
    SHADER_MATH_SCALAR_CMP(cmp_eq, ==)
    SHADER_MATH_SCALAR_CMP(cmp_ne, !=)
    SHADER_MATH_SCALAR_CMP(cmp_lt, <)
    SHADER_MATH_SCALAR_CMP(cmp_le, <=)
    SHADER_MATH_SCALAR_CMP(cmp_gt, >)
    SHADER_MATH_SCALAR_CMP(cmp_ge, >=)
#undef SHADER_MATH_SCALAR_CMP

    static SHADER_MATH_INLINE mreg m_and(mreg a, mreg b) {
        return {{a.m[0] & b.m[0], a.m[1] & b.m[1], a.m[2] & b.m[2], a.m[3] & b.m[3]}};
    }
    static SHADER_MATH_INLINE mreg m_or(mreg a, mreg b) {
        return {{a.m[0] | b.m[0], a.m[1] | b.m[1], a.m[2] | b.m[2], a.m[3] | b.m[3]}};
    }
    static SHADER_MATH_INLINE mreg m_xor(mreg a, mreg b) {
        return {{a.m[0] ^ b.m[0], a.m[1] ^ b.m[1], a.m[2] ^ b.m[2], a.m[3] ^ b.m[3]}};
    }
    static SHADER_MATH_INLINE mreg m_not(mreg a) {
        return {{~a.m[0], ~a.m[1], ~a.m[2], ~a.m[3]}};
    }

    static SHADER_MATH_INLINE bool lane_b(mreg m, int i) { return m.m[i] != 0u; }
    static SHADER_MATH_INLINE bool all_n(mreg m, int n) {
        for (int i = 0; i < n; ++i)
            if (m.m[i] == 0u) return false;
        return true;
    }
    static SHADER_MATH_INLINE bool any_n(mreg m, int n) {
        for (int i = 0; i < n; ++i)
            if (m.m[i] != 0u) return true;
        return false;
    }
};

}  // namespace detail
}  // namespace shader_math
