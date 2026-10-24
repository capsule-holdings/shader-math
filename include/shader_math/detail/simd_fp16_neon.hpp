// Native half backend: hreg/hops on the ARM FEAT_FP16 vector ALU
// (ARMv8.2-a +fp16). Arithmetic runs on real 16-bit lanes (float16x4_t);
// comparisons widen to f32 so the shared 32-bit mask type (mreg / bvec)
// stays unchanged.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_fp16_neon.hpp"
#endif

namespace shader_math {
namespace detail {

using hreg = float16x4_t;

struct hops {
    static SHADER_MATH_INLINE float32x4_t widen(hreg v) { return vcvt_f32_f16(v); }
    static SHADER_MATH_INLINE hreg narrow(float32x4_t f) { return vcvt_f16_f32(f); }

    static SHADER_MATH_INLINE hreg zero() { return vdup_n_f16(static_cast<float16_t>(0.0f)); }
    static SHADER_MATH_INLINE hreg splat(half_t s) {
        return vdup_n_f16(static_cast<float16_t>(s));
    }
    static SHADER_MATH_INLINE hreg set(half_t x, half_t y, half_t z, half_t w) {
        const float16_t t[4] = {static_cast<float16_t>(x), static_cast<float16_t>(y),
                                static_cast<float16_t>(z), static_cast<float16_t>(w)};
        return vld1_f16(t);
    }
    static SHADER_MATH_INLINE hreg load4(const half_t* p) {
        return set(p[0], p[1], p[2], p[3]);
    }
    static SHADER_MATH_INLINE void store4(hreg v, half_t* p) {
        float16_t t[4];
        vst1_f16(t, v);
        p[0] = static_cast<half_t>(t[0]);
        p[1] = static_cast<half_t>(t[1]);
        p[2] = static_cast<half_t>(t[2]);
        p[3] = static_cast<half_t>(t[3]);
    }
    static SHADER_MATH_INLINE half_t lane(hreg v, int i) {
        half_t t[4];
        store4(v, t);
        return t[i];
    }
    template <int I>
    static SHADER_MATH_INLINE half_t lane_c(hreg v) {
        return static_cast<half_t>(vget_lane_f16(v, I));
    }
    template <int I>
    static SHADER_MATH_INLINE hreg bcast(hreg v) { return vdup_lane_f16(v, I); }

    static SHADER_MATH_INLINE hreg add(hreg a, hreg b) { return vadd_f16(a, b); }
    static SHADER_MATH_INLINE hreg sub(hreg a, hreg b) { return vsub_f16(a, b); }
    static SHADER_MATH_INLINE hreg mul(hreg a, hreg b) { return vmul_f16(a, b); }
    static SHADER_MATH_INLINE hreg div(hreg a, hreg b) { return vdiv_f16(a, b); }
    static SHADER_MATH_INLINE hreg neg(hreg a) { return vneg_f16(a); }
    // a*b + c, fused on the 16-bit ALU
    static SHADER_MATH_INLINE hreg fmadd(hreg a, hreg b, hreg c) { return vfma_f16(c, a, b); }

    static SHADER_MATH_INLINE hreg abs(hreg a) { return vabs_f16(a); }
    // minNum / maxNum, like the float backend
    static SHADER_MATH_INLINE hreg min(hreg a, hreg b) { return vminnm_f16(a, b); }
    static SHADER_MATH_INLINE hreg max(hreg a, hreg b) { return vmaxnm_f16(a, b); }
    static SHADER_MATH_INLINE hreg sqrt(hreg a) { return vsqrt_f16(a); }

    static SHADER_MATH_INLINE hreg rnd_floor(hreg v) { return vrndm_f16(v); }
    static SHADER_MATH_INLINE hreg rnd_ceil(hreg v) { return vrndp_f16(v); }
    static SHADER_MATH_INLINE hreg rnd_trunc(hreg v) { return vrnd_f16(v); }
    static SHADER_MATH_INLINE hreg rnd_even(hreg v) { return vrndn_f16(v); }

    // m ? a : b with the shared 32-bit mask: select in the f32 domain
    // (selection is exact, so widening costs no precision).
    static SHADER_MATH_INLINE hreg sel(mreg m, hreg a, hreg b) {
        return narrow(ops::sel(m, widen(a), widen(b)));
    }

    // Comparisons reuse the 32-bit mask path: widen, compare as f32. Exact for
    // every pair of half values (the widening conversion is exact).
    static SHADER_MATH_INLINE mreg cmp_eq(hreg a, hreg b) { return ops::cmp_eq(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ne(hreg a, hreg b) { return ops::cmp_ne(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_lt(hreg a, hreg b) { return ops::cmp_lt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_le(hreg a, hreg b) { return ops::cmp_le(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_gt(hreg a, hreg b) { return ops::cmp_gt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ge(hreg a, hreg b) { return ops::cmp_ge(widen(a), widen(b)); }
};

}  // namespace detail
}  // namespace shader_math
