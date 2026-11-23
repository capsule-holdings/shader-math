// NEON backend: freg/mreg/ops for AArch64 / ARM with NEON.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_neon.hpp"
#endif

#include <arm_neon.h>

#include <cstdint>
#include <type_traits>

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

// 32-bit integer lanes, signed (iops) or unsigned (uops), in one register type
using ireg = int32x4_t;

template <bool Signed>
struct int_ops {
    using T = std::conditional_t<Signed, std::int32_t, std::uint32_t>;

    static SHADER_MATH_INLINE ireg zero() { return vdupq_n_s32(0); }
    static SHADER_MATH_INLINE ireg splat(T s) { return vdupq_n_s32(static_cast<std::int32_t>(s)); }
    static SHADER_MATH_INLINE ireg set(T x, T y, T z, T w) {
        const std::int32_t t[4] = {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y),
                                   static_cast<std::int32_t>(z), static_cast<std::int32_t>(w)};
        return vld1q_s32(t);
    }
    static SHADER_MATH_INLINE ireg load4(const T* p) {
        return vld1q_s32(reinterpret_cast<const std::int32_t*>(p));
    }
    static SHADER_MATH_INLINE void store4(ireg v, T* p) {
        vst1q_s32(reinterpret_cast<std::int32_t*>(p), v);
    }
    static SHADER_MATH_INLINE T lane(ireg v, int i) {
        T t[4];
        store4(v, t);
        return t[i];
    }
    template <int I>
    static SHADER_MATH_INLINE T lane_c(ireg v) {
        return static_cast<T>(vgetq_lane_s32(v, I));
    }
    template <int I>
    static SHADER_MATH_INLINE ireg bcast(ireg v) { return vdupq_laneq_s32(v, I); }

    static SHADER_MATH_INLINE ireg add(ireg a, ireg b) { return vaddq_s32(a, b); }
    static SHADER_MATH_INLINE ireg sub(ireg a, ireg b) { return vsubq_s32(a, b); }
    static SHADER_MATH_INLINE ireg neg(ireg a) { return vnegq_s32(a); }
    static SHADER_MATH_INLINE ireg mul(ireg a, ireg b) { return vmulq_s32(a, b); }
    // no integer division in Advanced SIMD: one lane at a time
    static SHADER_MATH_INLINE ireg div(ireg a, ireg b) {
        T x[4], y[4];
        store4(a, x);
        store4(b, y);
        if constexpr (Signed)
            return set(int_lane::sdiv(x[0], y[0]), int_lane::sdiv(x[1], y[1]),
                       int_lane::sdiv(x[2], y[2]), int_lane::sdiv(x[3], y[3]));
        else
            return set(int_lane::udiv(x[0], y[0]), int_lane::udiv(x[1], y[1]),
                       int_lane::udiv(x[2], y[2]), int_lane::udiv(x[3], y[3]));
    }
    static SHADER_MATH_INLINE ireg rem(ireg a, ireg b) {
        T x[4], y[4];
        store4(a, x);
        store4(b, y);
        if constexpr (Signed)
            return set(int_lane::srem(x[0], y[0]), int_lane::srem(x[1], y[1]),
                       int_lane::srem(x[2], y[2]), int_lane::srem(x[3], y[3]));
        else
            return set(int_lane::urem(x[0], y[0]), int_lane::urem(x[1], y[1]),
                       int_lane::urem(x[2], y[2]), int_lane::urem(x[3], y[3]));
    }

    static SHADER_MATH_INLINE ireg band(ireg a, ireg b) { return vandq_s32(a, b); }
    static SHADER_MATH_INLINE ireg bor(ireg a, ireg b) { return vorrq_s32(a, b); }
    static SHADER_MATH_INLINE ireg bxor(ireg a, ireg b) { return veorq_s32(a, b); }
    static SHADER_MATH_INLINE ireg bnot(ireg a) { return vmvnq_s32(a); }
    // USHL / SSHL shift left by a signed per-lane count and right by its
    // negation; the count is taken modulo 32 first
    static SHADER_MATH_INLINE ireg shl(ireg a, ireg n) {
        return vshlq_s32(a, vandq_s32(n, vdupq_n_s32(31)));
    }
    static SHADER_MATH_INLINE ireg shr(ireg a, ireg n) {
        const int32x4_t c = vnegq_s32(vandq_s32(n, vdupq_n_s32(31)));
        if constexpr (Signed)
            return vshlq_s32(a, c);
        else
            return vreinterpretq_s32_u32(vshlq_u32(vreinterpretq_u32_s32(a), c));
    }

    // m ? a : b, per lane
    static SHADER_MATH_INLINE ireg sel(mreg m, ireg a, ireg b) { return vbslq_s32(m, a, b); }

    static SHADER_MATH_INLINE mreg cmp_eq(ireg a, ireg b) { return vceqq_s32(a, b); }
    static SHADER_MATH_INLINE mreg cmp_ne(ireg a, ireg b) { return vmvnq_u32(vceqq_s32(a, b)); }
#define SM_NEON_ICMP(name, fn)                                                          \
    static SHADER_MATH_INLINE mreg name(ireg a, ireg b) {                               \
        if constexpr (Signed)                                                           \
            return fn##_s32(a, b);                                                      \
        else                                                                            \
            return fn##_u32(vreinterpretq_u32_s32(a), vreinterpretq_u32_s32(b));        \
    }
    SM_NEON_ICMP(cmp_lt, vcltq)
    SM_NEON_ICMP(cmp_le, vcleq)
    SM_NEON_ICMP(cmp_gt, vcgtq)
    SM_NEON_ICMP(cmp_ge, vcgeq)
#undef SM_NEON_ICMP

    static SHADER_MATH_INLINE ireg min(ireg a, ireg b) {
        if constexpr (Signed)
            return vminq_s32(a, b);
        else
            return vreinterpretq_s32_u32(
                vminq_u32(vreinterpretq_u32_s32(a), vreinterpretq_u32_s32(b)));
    }
    static SHADER_MATH_INLINE ireg max(ireg a, ireg b) {
        if constexpr (Signed)
            return vmaxq_s32(a, b);
        else
            return vreinterpretq_s32_u32(
                vmaxq_u32(vreinterpretq_u32_s32(a), vreinterpretq_u32_s32(b)));
    }
    // abs(INT_MIN) wraps to INT_MIN; unsigned lanes are returned unchanged
    static SHADER_MATH_INLINE ireg abs(ireg a) {
        if constexpr (Signed)
            return vabsq_s32(a);
        else
            return a;
    }

    // value conversions: SCVTF / UCVTF round to nearest; FCVTZS / FCVTZU
    // truncate, saturate and map NaN to 0 (int_lane::f2i / f2u)
    static SHADER_MATH_INLINE freg to_f32(ireg a) {
        if constexpr (Signed)
            return vcvtq_f32_s32(a);
        else
            return vcvtq_f32_u32(vreinterpretq_u32_s32(a));
    }
    static SHADER_MATH_INLINE ireg from_f32(freg f) {
        if constexpr (Signed)
            return vcvtq_s32_f32(f);
        else
            return vreinterpretq_s32_u32(vcvtq_u32_f32(f));
    }
    // bit reinterpretation (asfloat / asint, floatBitsToInt ...)
    static SHADER_MATH_INLINE freg bits_to_f32(ireg a) { return vreinterpretq_f32_s32(a); }
    static SHADER_MATH_INLINE ireg bits_from_f32(freg f) { return vreinterpretq_s32_f32(f); }
    // a comparison mask as lanes of all bits set / zero
    static SHADER_MATH_INLINE ireg from_mask(mreg m) { return vreinterpretq_s32_u32(m); }
};
using iops = int_ops<true>;
using uops = int_ops<false>;

}  // namespace detail
}  // namespace shader_math
