// 64-bit IEEE double lanes (4 lanes in 32 bytes), on top of the selected
// backend's shared 32-bit comparison mask (mreg).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_double.hpp"
#endif

#include <cmath>

namespace shader_math {
namespace detail {

struct alignas(16) dreg {
    double v[4];
};

struct dops {
    static SHADER_MATH_INLINE dreg zero() { return {{0.0, 0.0, 0.0, 0.0}}; }
    static SHADER_MATH_INLINE dreg splat(double s) { return {{s, s, s, s}}; }
    static SHADER_MATH_INLINE dreg set(double x, double y, double z, double w) {
        return {{x, y, z, w}};
    }
    static SHADER_MATH_INLINE dreg load4(const double* p) { return {{p[0], p[1], p[2], p[3]}}; }
    static SHADER_MATH_INLINE void store4(dreg r, double* p) {
        p[0] = r.v[0];
        p[1] = r.v[1];
        p[2] = r.v[2];
        p[3] = r.v[3];
    }
    static SHADER_MATH_INLINE double lane(dreg r, int i) { return r.v[i]; }
    template <int I>
    static SHADER_MATH_INLINE double lane_c(dreg r) {
        return r.v[I];
    }
    template <int I>
    static SHADER_MATH_INLINE dreg bcast(dreg r) {
        return splat(r.v[I]);
    }

    static SHADER_MATH_INLINE dreg add(dreg a, dreg b) {
        return {{a.v[0] + b.v[0], a.v[1] + b.v[1], a.v[2] + b.v[2], a.v[3] + b.v[3]}};
    }
    static SHADER_MATH_INLINE dreg sub(dreg a, dreg b) {
        return {{a.v[0] - b.v[0], a.v[1] - b.v[1], a.v[2] - b.v[2], a.v[3] - b.v[3]}};
    }
    static SHADER_MATH_INLINE dreg mul(dreg a, dreg b) {
        return {{a.v[0] * b.v[0], a.v[1] * b.v[1], a.v[2] * b.v[2], a.v[3] * b.v[3]}};
    }
    // Unused lanes of an N < 4 vector divide 0 / 0 by design (their value is
    // unspecified); MSVC's optimizer reports the folded constant as C4723.
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4723)
#endif
    static SHADER_MATH_INLINE dreg div(dreg a, dreg b) {
        return {{a.v[0] / b.v[0], a.v[1] / b.v[1], a.v[2] / b.v[2], a.v[3] / b.v[3]}};
    }
#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
    static SHADER_MATH_INLINE dreg neg(dreg a) { return {{-a.v[0], -a.v[1], -a.v[2], -a.v[3]}}; }
    static SHADER_MATH_INLINE dreg fmadd(dreg a, dreg b, dreg c) {
        return {{std::fma(a.v[0], b.v[0], c.v[0]), std::fma(a.v[1], b.v[1], c.v[1]),
                 std::fma(a.v[2], b.v[2], c.v[2]), std::fma(a.v[3], b.v[3], c.v[3])}};
    }

#define SM_DOUBLE_MAP1(name, expr)                                                   \
    static SHADER_MATH_INLINE dreg name(dreg a) {                                    \
        auto f = [](double x) { return expr; };                                      \
        return {{f(a.v[0]), f(a.v[1]), f(a.v[2]), f(a.v[3])}};                       \
    }
    SM_DOUBLE_MAP1(abs, std::fabs(x))
    SM_DOUBLE_MAP1(sqrt, std::sqrt(x))
    SM_DOUBLE_MAP1(rnd_floor, std::floor(x))
    SM_DOUBLE_MAP1(rnd_ceil, std::ceil(x))
    SM_DOUBLE_MAP1(rnd_trunc, std::trunc(x))
    SM_DOUBLE_MAP1(rnd_even, std::nearbyint(x))  // assumes default FE_TONEAREST
#undef SM_DOUBLE_MAP1

    // min / max ignore a NaN operand (IEEE minNum / maxNum), as D3D defines them
    static SHADER_MATH_INLINE double min1(double a, double b) { return b != b ? a : (a < b ? a : b); }
    static SHADER_MATH_INLINE double max1(double a, double b) { return b != b ? a : (a > b ? a : b); }
    static SHADER_MATH_INLINE dreg min(dreg a, dreg b) {
        return {{min1(a.v[0], b.v[0]), min1(a.v[1], b.v[1]), min1(a.v[2], b.v[2]),
                 min1(a.v[3], b.v[3])}};
    }
    static SHADER_MATH_INLINE dreg max(dreg a, dreg b) {
        return {{max1(a.v[0], b.v[0]), max1(a.v[1], b.v[1]), max1(a.v[2], b.v[2]),
                 max1(a.v[3], b.v[3])}};
    }

    // m ? a : b, per lane
    static SHADER_MATH_INLINE dreg sel(mreg m, dreg a, dreg b) {
        return {{ops::lane_b(m, 0) ? a.v[0] : b.v[0], ops::lane_b(m, 1) ? a.v[1] : b.v[1],
                 ops::lane_b(m, 2) ? a.v[2] : b.v[2], ops::lane_b(m, 3) ? a.v[3] : b.v[3]}};
    }

    template <class F>
    static SHADER_MATH_INLINE mreg mask(dreg a, dreg b, F f) {
        float m[4];
        for (int i = 0; i < 4; ++i) m[i] = f(a.v[i], b.v[i]) ? 1.0f : 0.0f;
        return ops::cmp_ne(ops::load4(m), ops::zero());
    }
    static SHADER_MATH_INLINE mreg cmp_eq(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x == y; });
    }
    static SHADER_MATH_INLINE mreg cmp_ne(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x != y; });
    }
    static SHADER_MATH_INLINE mreg cmp_lt(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x < y; });
    }
    static SHADER_MATH_INLINE mreg cmp_le(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x <= y; });
    }
    static SHADER_MATH_INLINE mreg cmp_gt(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x > y; });
    }
    static SHADER_MATH_INLINE mreg cmp_ge(dreg a, dreg b) {
        return mask(a, b, [](double x, double y) { return x >= y; });
    }
};

}  // namespace detail
}  // namespace shader_math
