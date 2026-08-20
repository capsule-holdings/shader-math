// Fallback half backend: hreg/hops for every host without the ARM FEAT_FP16
// vector ALU (x86 SSE2, scalar, pre-8.2 ARM). Each operation widens the four
// half lanes to f32, runs the float ops, and narrows back. This exists for
// expression compatibility, not speed; rounding matches neither a GPU half
// ALU nor ARM native fp16 (one extra f32 rounding per operation).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_fp16_promote.hpp"
#endif

namespace shader_math {
namespace detail {

struct alignas(8) hreg {
    half_t h[4];
};

struct hops {
    static SHADER_MATH_INLINE freg widen(hreg v) {
        return ops::set(static_cast<float>(v.h[0]), static_cast<float>(v.h[1]),
                        static_cast<float>(v.h[2]), static_cast<float>(v.h[3]));
    }
    static SHADER_MATH_INLINE hreg narrow(freg f) {
        float t[4];
        ops::store4(f, t);
        return {{static_cast<half_t>(t[0]), static_cast<half_t>(t[1]),
                 static_cast<half_t>(t[2]), static_cast<half_t>(t[3])}};
    }

    static SHADER_MATH_INLINE hreg zero() {
        const half_t z = static_cast<half_t>(0.0f);
        return {{z, z, z, z}};
    }
    static SHADER_MATH_INLINE hreg splat(half_t s) { return {{s, s, s, s}}; }
    static SHADER_MATH_INLINE hreg set(half_t x, half_t y, half_t z, half_t w) {
        return {{x, y, z, w}};
    }
    static SHADER_MATH_INLINE hreg load4(const half_t* p) { return {{p[0], p[1], p[2], p[3]}}; }
    static SHADER_MATH_INLINE void store4(hreg v, half_t* p) {
        p[0] = v.h[0];
        p[1] = v.h[1];
        p[2] = v.h[2];
        p[3] = v.h[3];
    }
    static SHADER_MATH_INLINE half_t lane(hreg v, int i) { return v.h[i]; }
    template <int I>
    static SHADER_MATH_INLINE half_t lane_c(hreg v) { return v.h[I]; }
    template <int I>
    static SHADER_MATH_INLINE hreg bcast(hreg v) { return splat(v.h[I]); }

    static SHADER_MATH_INLINE hreg add(hreg a, hreg b) {
        return narrow(ops::add(widen(a), widen(b)));
    }
    static SHADER_MATH_INLINE hreg sub(hreg a, hreg b) {
        return narrow(ops::sub(widen(a), widen(b)));
    }
    static SHADER_MATH_INLINE hreg mul(hreg a, hreg b) {
        return narrow(ops::mul(widen(a), widen(b)));
    }
    static SHADER_MATH_INLINE hreg div(hreg a, hreg b) {
        return narrow(ops::div(widen(a), widen(b)));
    }
    static SHADER_MATH_INLINE hreg neg(hreg a) { return narrow(ops::neg(widen(a))); }
    static SHADER_MATH_INLINE hreg fmadd(hreg a, hreg b, hreg c) {
        return narrow(ops::fmadd(widen(a), widen(b), widen(c)));
    }

    // Comparisons via f32 are exact for every pair of half values.
    static SHADER_MATH_INLINE mreg cmp_eq(hreg a, hreg b) { return ops::cmp_eq(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ne(hreg a, hreg b) { return ops::cmp_ne(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_lt(hreg a, hreg b) { return ops::cmp_lt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_le(hreg a, hreg b) { return ops::cmp_le(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_gt(hreg a, hreg b) { return ops::cmp_gt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ge(hreg a, hreg b) { return ops::cmp_ge(widen(a), widen(b)); }
};

}  // namespace detail
}  // namespace shader_math
