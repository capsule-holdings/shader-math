// SSE2 backend: freg/mreg/ops for x86/x64 (SSE2 is the x86-64 baseline).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_sse2.hpp"
#endif

#include <emmintrin.h>  // SSE2

namespace shader_math {
namespace detail {

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

}  // namespace detail
}  // namespace shader_math
