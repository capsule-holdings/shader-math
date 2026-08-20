// 4-wide SIMD abstraction: NEON / SSE / scalar.
// All higher-level algorithms are written once against `ops`; only this file
// knows about intrinsics.
#pragma once

#include "config.hpp"

#if defined(SHADER_MATH_ISA_NEON)
#  include <arm_neon.h>
#elif defined(SHADER_MATH_ISA_SSE)
#  include <emmintrin.h>  // SSE2
#endif

namespace shader_math {
namespace detail {

#if defined(SHADER_MATH_ISA_NEON)

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

    static SHADER_MATH_INLINE freg add(freg a, freg b) { return vaddq_f32(a, b); }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) { return vsubq_f32(a, b); }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) { return vmulq_f32(a, b); }
    static SHADER_MATH_INLINE freg div(freg a, freg b) { return vdivq_f32(a, b); }
    static SHADER_MATH_INLINE freg neg(freg a) { return vnegq_f32(a); }
    // a*b + c
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) { return vfmaq_f32(c, a, b); }

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

#elif defined(SHADER_MATH_ISA_SSE)

using freg = __m128;
using mreg = __m128;  // comparison result masks live in float registers

struct ops {
    static SHADER_MATH_INLINE freg zero() { return _mm_setzero_ps(); }
    static SHADER_MATH_INLINE freg splat(float s) { return _mm_set1_ps(s); }
    static SHADER_MATH_INLINE freg set(float x, float y, float z, float w) {
        return _mm_set_ps(w, z, y, x);
    }
    static SHADER_MATH_INLINE freg load4(const float* p) { return _mm_loadu_ps(p); }
    static SHADER_MATH_INLINE void store4(freg v, float* p) { _mm_storeu_ps(p, v); }
    static SHADER_MATH_INLINE float lane(freg v, int i) {
        float t[4];
        _mm_storeu_ps(t, v);
        return t[i];
    }
    template <int I>
    static SHADER_MATH_INLINE float lane_c(freg v) {
        return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(I, I, I, I)));
    }
    template <int I>
    static SHADER_MATH_INLINE freg bcast(freg v) {
        return _mm_shuffle_ps(v, v, _MM_SHUFFLE(I, I, I, I));
    }

    static SHADER_MATH_INLINE freg add(freg a, freg b) { return _mm_add_ps(a, b); }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) { return _mm_sub_ps(a, b); }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) { return _mm_mul_ps(a, b); }
    static SHADER_MATH_INLINE freg div(freg a, freg b) { return _mm_div_ps(a, b); }
    static SHADER_MATH_INLINE freg neg(freg a) { return _mm_sub_ps(_mm_setzero_ps(), a); }
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) {
        return _mm_add_ps(_mm_mul_ps(a, b), c);
    }

    static SHADER_MATH_INLINE mreg cmp_eq(freg a, freg b) { return _mm_cmpeq_ps(a, b); }
    static SHADER_MATH_INLINE mreg cmp_ne(freg a, freg b) { return _mm_cmpneq_ps(a, b); }
    static SHADER_MATH_INLINE mreg cmp_lt(freg a, freg b) { return _mm_cmplt_ps(a, b); }
    static SHADER_MATH_INLINE mreg cmp_le(freg a, freg b) { return _mm_cmple_ps(a, b); }
    static SHADER_MATH_INLINE mreg cmp_gt(freg a, freg b) { return _mm_cmpgt_ps(a, b); }
    static SHADER_MATH_INLINE mreg cmp_ge(freg a, freg b) { return _mm_cmpge_ps(a, b); }

    static SHADER_MATH_INLINE mreg m_and(mreg a, mreg b) { return _mm_and_ps(a, b); }
    static SHADER_MATH_INLINE mreg m_or(mreg a, mreg b) { return _mm_or_ps(a, b); }
    static SHADER_MATH_INLINE mreg m_xor(mreg a, mreg b) { return _mm_xor_ps(a, b); }
    static SHADER_MATH_INLINE mreg m_not(mreg a) {
        return _mm_xor_ps(a, _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()));
    }

    static SHADER_MATH_INLINE bool lane_b(mreg m, int i) {
        return ((_mm_movemask_ps(m) >> i) & 1) != 0;
    }
    static SHADER_MATH_INLINE bool all_n(mreg m, int n) {
        const int bits = (1 << n) - 1;
        return (_mm_movemask_ps(m) & bits) == bits;
    }
    static SHADER_MATH_INLINE bool any_n(mreg m, int n) {
        const int bits = (1 << n) - 1;
        return (_mm_movemask_ps(m) & bits) != 0;
    }
};

#else  // scalar reference backend (RISC-V and anything without NEON/SSE)

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

    static SHADER_MATH_INLINE freg add(freg a, freg b) {
        return {{a.f[0] + b.f[0], a.f[1] + b.f[1], a.f[2] + b.f[2], a.f[3] + b.f[3]}};
    }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) {
        return {{a.f[0] - b.f[0], a.f[1] - b.f[1], a.f[2] - b.f[2], a.f[3] - b.f[3]}};
    }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) {
        return {{a.f[0] * b.f[0], a.f[1] * b.f[1], a.f[2] * b.f[2], a.f[3] * b.f[3]}};
    }
    static SHADER_MATH_INLINE freg div(freg a, freg b) {
        return {{a.f[0] / b.f[0], a.f[1] / b.f[1], a.f[2] / b.f[2], a.f[3] / b.f[3]}};
    }
    static SHADER_MATH_INLINE freg neg(freg a) { return {{-a.f[0], -a.f[1], -a.f[2], -a.f[3]}}; }
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) {
        return {{a.f[0] * b.f[0] + c.f[0], a.f[1] * b.f[1] + c.f[1], a.f[2] * b.f[2] + c.f[2],
                 a.f[3] * b.f[3] + c.f[3]}};
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

#endif

}  // namespace detail
}  // namespace shader_math
