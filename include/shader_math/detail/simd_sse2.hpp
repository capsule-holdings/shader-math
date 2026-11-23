// SSE2 backend: freg/mreg/ops for x86/x64 (SSE2 is the x86-64 baseline).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_sse2.hpp"
#endif

#if defined(SHADER_MATH_HAS_FMA)
#  include <immintrin.h>  // SSE2 + FMA3 (VEX)
#elif defined(SHADER_MATH_HAS_SSE41)
#  include <smmintrin.h>  // SSE2 + SSE4.1
#else
#  include <emmintrin.h>  // SSE2
#endif

#if !defined(SHADER_MATH_HAS_SSE41)
#  include <cmath>  // per-lane rounding: SSE2 has no roundps (that is SSE4.1)
#endif
#include <cstdint>
#include <type_traits>

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

    static SHADER_MATH_INLINE freg cat_lo(freg a, freg b) { return _mm_movelh_ps(a, b); }
    static SHADER_MATH_INLINE freg cat_hi(freg a, freg b) { return _mm_movehl_ps(b, a); }
    static SHADER_MATH_INLINE freg even(freg a, freg b) {
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(2, 0, 2, 0));
    }
    static SHADER_MATH_INLINE freg odd(freg a, freg b) {
        return _mm_shuffle_ps(a, b, _MM_SHUFFLE(3, 1, 3, 1));
    }
    static SHADER_MATH_INLINE freg swap_pairs(freg a) {
        return _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 3, 0, 1));
    }
    static SHADER_MATH_INLINE freg swap_halves(freg a) {
        return _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 3, 2));
    }

    static SHADER_MATH_INLINE freg add(freg a, freg b) { return _mm_add_ps(a, b); }
    static SHADER_MATH_INLINE freg sub(freg a, freg b) { return _mm_sub_ps(a, b); }
    static SHADER_MATH_INLINE freg mul(freg a, freg b) { return _mm_mul_ps(a, b); }
    static SHADER_MATH_INLINE freg div(freg a, freg b) { return _mm_div_ps(a, b); }
    // IEEE negate (sign-bit flip): -(+0) = -0
    static SHADER_MATH_INLINE freg neg(freg a) { return _mm_xor_ps(a, _mm_set1_ps(-0.0f)); }
    // a*b + c: single rounding with FMA3, two roundings on plain SSE2
    static SHADER_MATH_INLINE freg fmadd(freg a, freg b, freg c) {
#if defined(SHADER_MATH_HAS_FMA)
        return _mm_fmadd_ps(a, b, c);
#else
        return _mm_add_ps(_mm_mul_ps(a, b), c);
#endif
    }
    // c - a*b, rounded like fmadd
    static SHADER_MATH_INLINE freg fnmadd(freg a, freg b, freg c) {
#if defined(SHADER_MATH_HAS_FMA)
        return _mm_fnmadd_ps(a, b, c);
#else
        return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif
    }

    static SHADER_MATH_INLINE freg abs(freg a) {
        return _mm_andnot_ps(_mm_set1_ps(-0.0f), a);
    }
    // min / max ignore a NaN operand (IEEE minNum / maxNum), as D3D defines
    // them: minps / maxps already return b when a is NaN, and b is replaced
    // by a where b is NaN.
    static SHADER_MATH_INLINE freg min(freg a, freg b) {
        const freg b_nan = _mm_cmpunord_ps(b, b);
        return _mm_or_ps(_mm_and_ps(b_nan, a), _mm_andnot_ps(b_nan, _mm_min_ps(a, b)));
    }
    static SHADER_MATH_INLINE freg max(freg a, freg b) {
        const freg b_nan = _mm_cmpunord_ps(b, b);
        return _mm_or_ps(_mm_and_ps(b_nan, a), _mm_andnot_ps(b_nan, _mm_max_ps(a, b)));
    }
    static SHADER_MATH_INLINE freg sqrt(freg a) { return _mm_sqrt_ps(a); }

#if defined(SHADER_MATH_HAS_SSE41)
    // Rounding with SSE4.1 roundps. _MM_FROUND_NO_EXC: no inexact
    // exception, matching std::floor / ceil / trunc.
    static SHADER_MATH_INLINE freg rnd_floor(freg v) {
        return _mm_round_ps(v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
    }
    static SHADER_MATH_INLINE freg rnd_ceil(freg v) {
        return _mm_round_ps(v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC);
    }
    static SHADER_MATH_INLINE freg rnd_trunc(freg v) {
        return _mm_round_ps(v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC);
    }
    // Ties to even regardless of MXCSR (the libm path uses nearbyint under
    // the default FE_TONEAREST, which gives the same results).
    static SHADER_MATH_INLINE freg rnd_even(freg v) {
        return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    }
#else
    // Rounding: the SSE2 baseline has no packed rounding instruction
    // (roundps is SSE4.1), so these are per-lane libm. Correctness over
    // speed; the baseline is deliberately not raised.
#define SHADER_MATH_SSE2_RND(name, fn)                                  \
    static SHADER_MATH_INLINE freg name(freg v) {                       \
        float t[4];                                                     \
        _mm_storeu_ps(t, v);                                            \
        return _mm_setr_ps(fn(t[0]), fn(t[1]), fn(t[2]), fn(t[3]));     \
    }
    SHADER_MATH_SSE2_RND(rnd_floor, std::floor)
    SHADER_MATH_SSE2_RND(rnd_ceil, std::ceil)
    SHADER_MATH_SSE2_RND(rnd_trunc, std::trunc)
    SHADER_MATH_SSE2_RND(rnd_even, std::nearbyint)  // assumes default FE_TONEAREST
#undef SHADER_MATH_SSE2_RND
#endif

    // m ? a : b, per lane
    static SHADER_MATH_INLINE freg sel(mreg m, freg a, freg b) {
        return _mm_or_ps(_mm_and_ps(m, a), _mm_andnot_ps(m, b));
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

// 32-bit integer lanes, signed (iops) or unsigned (uops), in one register type
using ireg = __m128i;

template <bool Signed>
struct int_ops {
    using T = std::conditional_t<Signed, std::int32_t, std::uint32_t>;

    static SHADER_MATH_INLINE ireg zero() { return _mm_setzero_si128(); }
    static SHADER_MATH_INLINE ireg splat(T s) { return _mm_set1_epi32(static_cast<int>(s)); }
    static SHADER_MATH_INLINE ireg set(T x, T y, T z, T w) {
        return _mm_setr_epi32(static_cast<int>(x), static_cast<int>(y), static_cast<int>(z),
                              static_cast<int>(w));
    }
    static SHADER_MATH_INLINE ireg load4(const T* p) {
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
    }
    static SHADER_MATH_INLINE void store4(ireg v, T* p) {
        _mm_storeu_si128(reinterpret_cast<__m128i*>(p), v);
    }
    static SHADER_MATH_INLINE T lane(ireg v, int i) {
        T t[4];
        store4(v, t);
        return t[i];
    }
    template <int I>
    static SHADER_MATH_INLINE T lane_c(ireg v) {
        return static_cast<T>(_mm_cvtsi128_si32(_mm_shuffle_epi32(v, _MM_SHUFFLE(I, I, I, I))));
    }
    template <int I>
    static SHADER_MATH_INLINE ireg bcast(ireg v) {
        return _mm_shuffle_epi32(v, _MM_SHUFFLE(I, I, I, I));
    }

    static SHADER_MATH_INLINE ireg add(ireg a, ireg b) { return _mm_add_epi32(a, b); }
    static SHADER_MATH_INLINE ireg sub(ireg a, ireg b) { return _mm_sub_epi32(a, b); }
    static SHADER_MATH_INLINE ireg neg(ireg a) { return _mm_sub_epi32(_mm_setzero_si128(), a); }
    static SHADER_MATH_INLINE ireg mul(ireg a, ireg b) {
#if defined(SHADER_MATH_HAS_SSE41)
        return _mm_mullo_epi32(a, b);
#else
        // SSE2 multiplies lanes 0 / 2 into 64-bit products; the low halves
        // of the even and odd products interleave back into place
        const __m128i even = _mm_mul_epu32(a, b);
        const __m128i odd = _mm_mul_epu32(_mm_srli_epi64(a, 32), _mm_srli_epi64(b, 32));
        return _mm_unpacklo_epi32(_mm_shuffle_epi32(even, _MM_SHUFFLE(0, 0, 2, 0)),
                                  _mm_shuffle_epi32(odd, _MM_SHUFFLE(0, 0, 2, 0)));
#endif
    }
    // no integer division in SSE: one lane at a time
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

    static SHADER_MATH_INLINE ireg band(ireg a, ireg b) { return _mm_and_si128(a, b); }
    static SHADER_MATH_INLINE ireg bor(ireg a, ireg b) { return _mm_or_si128(a, b); }
    static SHADER_MATH_INLINE ireg bxor(ireg a, ireg b) { return _mm_xor_si128(a, b); }
    static SHADER_MATH_INLINE ireg bnot(ireg a) {
        return _mm_xor_si128(a, _mm_cmpeq_epi32(a, a));
    }
    // per-lane shift counts (SSE2 shifts every lane by one count): one lane
    // at a time
    static SHADER_MATH_INLINE ireg shl(ireg a, ireg n) {
        std::uint32_t x[4], c[4];
        _mm_storeu_si128(reinterpret_cast<__m128i*>(x), a);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(c), n);
        return _mm_setr_epi32(static_cast<int>(int_lane::shl(x[0], c[0])),
                              static_cast<int>(int_lane::shl(x[1], c[1])),
                              static_cast<int>(int_lane::shl(x[2], c[2])),
                              static_cast<int>(int_lane::shl(x[3], c[3])));
    }
    static SHADER_MATH_INLINE ireg shr(ireg a, ireg n) {
        T x[4];
        std::uint32_t c[4];
        store4(a, x);
        _mm_storeu_si128(reinterpret_cast<__m128i*>(c), n);
        if constexpr (Signed)
            return set(int_lane::shr_s(x[0], c[0]), int_lane::shr_s(x[1], c[1]),
                       int_lane::shr_s(x[2], c[2]), int_lane::shr_s(x[3], c[3]));
        else
            return set(int_lane::shr_u(x[0], c[0]), int_lane::shr_u(x[1], c[1]),
                       int_lane::shr_u(x[2], c[2]), int_lane::shr_u(x[3], c[3]));
    }

    // m ? a : b, per lane
    static SHADER_MATH_INLINE ireg sel(mreg m, ireg a, ireg b) {
        const __m128i mi = _mm_castps_si128(m);
        return _mm_or_si128(_mm_and_si128(mi, a), _mm_andnot_si128(mi, b));
    }

    static SHADER_MATH_INLINE mreg cmp_eq(ireg a, ireg b) {
        return _mm_castsi128_ps(_mm_cmpeq_epi32(a, b));
    }
    static SHADER_MATH_INLINE mreg cmp_ne(ireg a, ireg b) {
        const __m128i eq = _mm_cmpeq_epi32(a, b);
        return _mm_castsi128_ps(_mm_xor_si128(eq, _mm_cmpeq_epi32(eq, eq)));
    }
    static SHADER_MATH_INLINE mreg cmp_gt(ireg a, ireg b) { return _mm_castsi128_ps(gt(a, b)); }
    static SHADER_MATH_INLINE mreg cmp_lt(ireg a, ireg b) { return _mm_castsi128_ps(gt(b, a)); }
    static SHADER_MATH_INLINE mreg cmp_ge(ireg a, ireg b) {
        const __m128i lt = gt(b, a);
        return _mm_castsi128_ps(_mm_xor_si128(lt, _mm_cmpeq_epi32(lt, lt)));
    }
    static SHADER_MATH_INLINE mreg cmp_le(ireg a, ireg b) {
        const __m128i g = gt(a, b);
        return _mm_castsi128_ps(_mm_xor_si128(g, _mm_cmpeq_epi32(g, g)));
    }

    static SHADER_MATH_INLINE ireg min(ireg a, ireg b) {
#if defined(SHADER_MATH_HAS_SSE41)
        if constexpr (Signed)
            return _mm_min_epi32(a, b);
        else
            return _mm_min_epu32(a, b);
#else
        const __m128i g = gt(a, b);
        return _mm_or_si128(_mm_and_si128(g, b), _mm_andnot_si128(g, a));
#endif
    }
    static SHADER_MATH_INLINE ireg max(ireg a, ireg b) {
#if defined(SHADER_MATH_HAS_SSE41)
        if constexpr (Signed)
            return _mm_max_epi32(a, b);
        else
            return _mm_max_epu32(a, b);
#else
        const __m128i g = gt(a, b);
        return _mm_or_si128(_mm_and_si128(g, a), _mm_andnot_si128(g, b));
#endif
    }
    // abs(INT_MIN) wraps to INT_MIN; unsigned lanes are returned unchanged
    static SHADER_MATH_INLINE ireg abs(ireg a) {
        if constexpr (Signed) {
#if defined(SHADER_MATH_HAS_SSE41)
            return _mm_abs_epi32(a);
#else
            const __m128i s = _mm_srai_epi32(a, 31);
            return _mm_sub_epi32(_mm_xor_si128(a, s), s);
#endif
        } else {
            return a;
        }
    }

    // value conversions (rounding to nearest; the reverse truncates, see
    // int_lane::f2i / f2u for NaN and out-of-range inputs)
    static SHADER_MATH_INLINE freg to_f32(ireg a) {
        if constexpr (Signed) {
            return _mm_cvtepi32_ps(a);
        } else {
            // both 16-bit halves convert exactly; the one add rounds once
            const __m128 hi = _mm_cvtepi32_ps(_mm_srli_epi32(a, 16));
            const __m128 lo = _mm_cvtepi32_ps(_mm_and_si128(a, _mm_set1_epi32(0xFFFF)));
            return _mm_add_ps(_mm_mul_ps(hi, _mm_set1_ps(65536.0f)), lo);
        }
    }
    static SHADER_MATH_INLINE ireg from_f32(freg f) {
        if constexpr (Signed) {
            // cvttps2dq gives INT_MIN for NaN and out-of-range lanes, which is
            // already right for large negative ones
            const __m128i t = _mm_cvttps_epi32(f);
            const __m128i big = _mm_castps_si128(_mm_cmpge_ps(f, _mm_set1_ps(2147483648.0f)));
            const __m128i nan = _mm_castps_si128(_mm_cmpunord_ps(f, f));
            const __m128i r = _mm_or_si128(_mm_andnot_si128(big, t),
                                           _mm_and_si128(big, _mm_set1_epi32(INT32_MAX)));
            return _mm_andnot_si128(nan, r);
        } else {
            // maxps returns its second operand for a NaN: NaN and negative
            // lanes become 0. Lanes at or above 2^31 convert after
            // subtracting 2^31, which the xor adds back.
            const __m128 c = _mm_max_ps(f, _mm_setzero_ps());
            const __m128 two31 = _mm_set1_ps(2147483648.0f);
            const __m128 high = _mm_cmpge_ps(c, two31);
            const __m128i t =
                _mm_cvttps_epi32(_mm_sub_ps(c, _mm_and_ps(high, two31)));
            const __m128i r = _mm_xor_si128(
                t, _mm_and_si128(_mm_castps_si128(high), _mm_set1_epi32(INT32_MIN)));
            const __m128i sat = _mm_castps_si128(_mm_cmpge_ps(c, _mm_set1_ps(4294967296.0f)));
            return _mm_or_si128(r, sat);
        }
    }
    // bit reinterpretation (asfloat / asint, floatBitsToInt ...)
    static SHADER_MATH_INLINE freg bits_to_f32(ireg a) { return _mm_castsi128_ps(a); }
    static SHADER_MATH_INLINE ireg bits_from_f32(freg f) { return _mm_castps_si128(f); }
    // a comparison mask as lanes of all bits set / zero
    static SHADER_MATH_INLINE ireg from_mask(mreg m) { return _mm_castps_si128(m); }

private:
    // a > b; unsigned lanes compare as signed after flipping the sign bit
    static SHADER_MATH_INLINE __m128i gt(ireg a, ireg b) {
        if constexpr (Signed) {
            return _mm_cmpgt_epi32(a, b);
        } else {
            const __m128i bias = _mm_set1_epi32(INT32_MIN);
            return _mm_cmpgt_epi32(_mm_xor_si128(a, bias), _mm_xor_si128(b, bias));
        }
    }
};
using iops = int_ops<true>;
using uops = int_ops<false>;

}  // namespace detail
}  // namespace shader_math
