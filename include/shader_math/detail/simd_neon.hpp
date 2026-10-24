// NEON backend: freg/mreg/ops for AArch64 / ARM with NEON.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_neon.hpp"
#endif

#include <arm_neon.h>

namespace shader_math {
namespace detail {

using freg = float32x4_t;
using mreg = uint32x4_t;

struct ops {
    static SHADER_MATH_INLINE freg zero() { return vdupq_n_f32(0.0f); }
    static SHADER_MATH_INLINE freg splat(float s) { return vdupq_n_f32(s); }
    static SHADER_MATH_INLINE freg set(float x, float y, float z, float w) {
        const float t[4] = {x, y, z, w};
        return vld1q_f32(t);
    }
    static SHADER_MATH_INLINE freg load4(const float* p) { return vld1q_f32(p); }
    static SHADER_MATH_INLINE void store4(freg v, float* p) { vst1q_f32(p, v); }
    static SHADER_MATH_INLINE float lane(freg v, int i) {
        float t[4];
        vst1q_f32(t, v);
        return t[i];
    }
    template <int I>
    static SHADER_MATH_INLINE float lane_c(freg v) { return vgetq_lane_f32(v, I); }
    template <int I>
    static SHADER_MATH_INLINE freg bcast(freg v) { return vdupq_laneq_f32(v, I); }

    static SHADER_MATH_INLINE freg cat_lo(freg a, freg b) {
        return vcombine_f32(vget_low_f32(a), vget_low_f32(b));
    }
    static SHADER_MATH_INLINE freg cat_hi(freg a, freg b) {
        return vcombine_f32(vget_high_f32(a), vget_high_f32(b));
    }
    static SHADER_MATH_INLINE freg even(freg a, freg b) { return vuzp1q_f32(a, b); }
    static SHADER_MATH_INLINE freg odd(freg a, freg b) { return vuzp2q_f32(a, b); }
    static SHADER_MATH_INLINE freg swap_pairs(freg a) { return vrev64q_f32(a); }
    static SHADER_MATH_INLINE freg swap_halves(freg a) { return vextq_f32(a, a, 2); }

    static SHADER_MATH_INLINE freg add(freg a, freg b) { return vaddq_f32(a, b); }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) { return vsubq_f32(a, b); }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) { return vmulq_f32(a, b); }
    static SHADER_MATH_INLINE freg div(freg a, freg b) { return vdivq_f32(a, b); }
    static SHADER_MATH_INLINE freg neg(freg a) { return vnegq_f32(a); }
    // a*b + c
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) { return vfmaq_f32(c, a, b); }
    // c - a*b
    static SHADER_MATH_INLINE freg fnmadd(freg a, freg b, freg c) { return vfmsq_f32(c, a, b); }

    static SHADER_MATH_INLINE freg abs(freg a) { return vabsq_f32(a); }
    // min / max ignore a NaN operand (IEEE minNum / maxNum), as D3D defines them
    static SHADER_MATH_INLINE freg min(freg a, freg b) { return vminnmq_f32(a, b); }
    static SHADER_MATH_INLINE freg max(freg a, freg b) { return vmaxnmq_f32(a, b); }
    static SHADER_MATH_INLINE freg sqrt(freg a) { return vsqrtq_f32(a); }

    static SHADER_MATH_INLINE freg rnd_floor(freg v) { return vrndmq_f32(v); }
    static SHADER_MATH_INLINE freg rnd_ceil(freg v) { return vrndpq_f32(v); }
    static SHADER_MATH_INLINE freg rnd_trunc(freg v) { return vrndq_f32(v); }
    static SHADER_MATH_INLINE freg rnd_even(freg v) { return vrndnq_f32(v); }

    // m ? a : b, per lane
    static SHADER_MATH_INLINE freg sel(mreg m, freg a, freg b) { return vbslq_f32(m, a, b); }

    static SHADER_MATH_INLINE mreg cmp_eq(freg a, freg b) { return vceqq_f32(a, b); }
    static SHADER_MATH_INLINE mreg cmp_ne(freg a, freg b) { return vmvnq_u32(vceqq_f32(a, b)); }
    static SHADER_MATH_INLINE mreg cmp_lt(freg a, freg b) { return vcltq_f32(a, b); }
    static SHADER_MATH_INLINE mreg cmp_le(freg a, freg b) { return vcleq_f32(a, b); }
    static SHADER_MATH_INLINE mreg cmp_gt(freg a, freg b) { return vcgtq_f32(a, b); }
    static SHADER_MATH_INLINE mreg cmp_ge(freg a, freg b) { return vcgeq_f32(a, b); }

    static SHADER_MATH_INLINE mreg m_and(mreg a, mreg b) { return vandq_u32(a, b); }
    static SHADER_MATH_INLINE mreg m_or(mreg a, mreg b) { return vorrq_u32(a, b); }
    static SHADER_MATH_INLINE mreg m_xor(mreg a, mreg b) { return veorq_u32(a, b); }
    static SHADER_MATH_INLINE mreg m_not(mreg a) { return vmvnq_u32(a); }

    static SHADER_MATH_INLINE bool lane_b(mreg m, int i) {
        unsigned t[4];
        vst1q_u32(t, m);
        return t[i] != 0u;
    }
    static SHADER_MATH_INLINE bool all_n(mreg m, int n) {
        unsigned t[4];
        vst1q_u32(t, m);
        for (int i = 0; i < n; ++i)
            if (t[i] == 0u) return false;
        return true;
    }
    static SHADER_MATH_INLINE bool any_n(mreg m, int n) {
        unsigned t[4];
        vst1q_u32(t, m);
        for (int i = 0; i < n; ++i)
            if (t[i] != 0u) return true;
        return false;
    }
};

}  // namespace detail
}  // namespace shader_math
