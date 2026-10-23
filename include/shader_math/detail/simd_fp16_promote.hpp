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

#if defined(SHADER_MATH_HAS_F16C)
#  include <immintrin.h>
#endif

namespace shader_math {
namespace detail {

struct alignas(8) hreg {
    half_t h[4];
};

struct hops {
#if defined(SHADER_MATH_HAS_F16C)
    // One instruction each way. vcvtps2ph with an explicit round-to-nearest-
    // even immediate gives the same bits as static_cast<half_t>(float).
    static SHADER_MATH_INLINE freg widen(hreg v) {
        return _mm_cvtph_ps(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(v.h)));
    }
    static SHADER_MATH_INLINE hreg narrow(freg f) {
        hreg r;
        _mm_storel_epi64(reinterpret_cast<__m128i*>(r.h),
                         _mm_cvtps_ph(f, _MM_FROUND_TO_NEAREST_INT));
        return r;
    }
#elif defined(SHADER_MATH_ISA_SSE2)
    // SSE2 without F16C: compilers convert _Float16 one lane at a time through
    // runtime helpers, so convert all four lanes with integer bit operations.
    // Results match static_cast bit for bit except NaN payloads (a NaN stays
    // a NaN; narrow returns the canonical quiet NaN).
    static SHADER_MATH_INLINE freg widen(hreg v) {
        // exponent / mantissa shifted into place and rescaled by 2^112 as a
        // float multiply, which also normalizes half subnormals exactly
        const __m128i h = _mm_unpacklo_epi16(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(v.h)),
                                             _mm_setzero_si128());
        const __m128i expmant = _mm_and_si128(h, _mm_set1_epi32(0x7fff));
        const __m128i sign = _mm_slli_epi32(_mm_xor_si128(h, expmant), 16);
        const __m128 scaled = _mm_mul_ps(_mm_castsi128_ps(_mm_slli_epi32(expmant, 13)),
                                         _mm_castsi128_ps(_mm_set1_epi32((254 - 15) << 23)));
        const __m128i infnan = _mm_and_si128(_mm_cmpgt_epi32(expmant, _mm_set1_epi32(0x7bff)),
                                             _mm_set1_epi32(255 << 23));
        return _mm_or_ps(scaled, _mm_castsi128_ps(_mm_or_si128(sign, infnan)));
    }
    static SHADER_MATH_INLINE hreg narrow(freg f) {
        // round to nearest even; three cases blended by masks:
        //   |f| >= 2^16          -> inf, or the quiet NaN for a NaN input
        //   |f| <  2^-14         -> subnormal: an add of 0.5 * 2^-(14-10) lets
        //                            the FPU round the mantissa into place
        //   otherwise            -> rebias the exponent, add 0xfff + odd bit
        const __m128i bits = _mm_castps_si128(f);
        const __m128i sign = _mm_and_si128(bits, _mm_set1_epi32(static_cast<int>(0x80000000u)));
        const __m128i a = _mm_xor_si128(bits, sign);
        const __m128i is_big = _mm_cmpgt_epi32(a, _mm_set1_epi32(((127 + 16) << 23) - 1));
        const __m128i is_nan = _mm_cmpgt_epi32(a, _mm_set1_epi32(255 << 23));
        const __m128i is_small = _mm_cmplt_epi32(a, _mm_set1_epi32(113 << 23));
        const __m128i big = _mm_or_si128(_mm_set1_epi32(0x7c00),
                                         _mm_and_si128(is_nan, _mm_set1_epi32(0x0200)));
        const __m128i magic = _mm_set1_epi32(((127 - 15) + (23 - 10) + 1) << 23);
        const __m128i small = _mm_sub_epi32(
            _mm_castps_si128(_mm_add_ps(_mm_castsi128_ps(a), _mm_castsi128_ps(magic))), magic);
        const __m128i odd = _mm_and_si128(_mm_srli_epi32(a, 13), _mm_set1_epi32(1));
        const __m128i normal = _mm_srli_epi32(
            _mm_add_epi32(_mm_add_epi32(a, _mm_set1_epi32(static_cast<int>((15u - 127u) << 23) + 0xfff)),
                          odd),
            13);
        __m128i o = _mm_or_si128(_mm_and_si128(is_small, small), _mm_andnot_si128(is_small, normal));
        o = _mm_or_si128(_mm_and_si128(is_big, big), _mm_andnot_si128(is_big, o));
        o = _mm_or_si128(o, _mm_srli_epi32(sign, 16));
        // pack the four 16-bit results (the values fit: no saturation)
        o = _mm_shufflelo_epi16(o, _MM_SHUFFLE(3, 3, 2, 0));
        o = _mm_shufflehi_epi16(o, _MM_SHUFFLE(3, 3, 2, 0));
        o = _mm_castps_si128(_mm_shuffle_ps(_mm_castsi128_ps(o), _mm_castsi128_ps(o), _MM_SHUFFLE(3, 3, 2, 0)));
        hreg r;
        _mm_storel_epi64(reinterpret_cast<__m128i*>(r.h), o);
        return r;
    }
#else
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
#endif

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

#define SHADER_MATH_HP_UNARY(name, fn)                             \
    static SHADER_MATH_INLINE hreg name(hreg a) {                  \
        return narrow(ops::fn(widen(a)));                          \
    }
    SHADER_MATH_HP_UNARY(abs, abs)
    SHADER_MATH_HP_UNARY(sqrt, sqrt)
    SHADER_MATH_HP_UNARY(rnd_floor, rnd_floor)
    SHADER_MATH_HP_UNARY(rnd_ceil, rnd_ceil)
    SHADER_MATH_HP_UNARY(rnd_trunc, rnd_trunc)
    SHADER_MATH_HP_UNARY(rnd_even, rnd_even)
    SHADER_MATH_HP_UNARY(rnd_away, rnd_away)
#undef SHADER_MATH_HP_UNARY

    static SHADER_MATH_INLINE hreg min(hreg a, hreg b) {
        return narrow(ops::min(widen(a), widen(b)));
    }
    static SHADER_MATH_INLINE hreg max(hreg a, hreg b) {
        return narrow(ops::max(widen(a), widen(b)));
    }
    // m ? a : b with the shared 32-bit mask (selection is exact via f32)
    static SHADER_MATH_INLINE hreg sel(mreg m, hreg a, hreg b) {
        return narrow(ops::sel(m, widen(a), widen(b)));
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
