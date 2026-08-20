// AVX1 + FMA3 helpers: pack two 128-bit rows/columns of a float 4x4 matrix
// product into one YMM register. Everything here is AVX1 (VEX 256-bit float
// ops + vpermilps-immediate); the per-128-lane broadcast needs no AVX2
// (vpermps), because _mm256_permute_ps applies the same immediate to both
// halves independently. Kernel-internal only: freg stays __m128, no public
// type changes width, 2x2 / 3x3 / non-square products keep the 128-bit path.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_avx.hpp"
#endif

#include <immintrin.h>

namespace shader_math {
namespace detail {

struct avx2row {
    using dreg = __m256;  // [row r | row r+1] (or two columns for GLSL)

    static SHADER_MATH_INLINE dreg zero() { return _mm256_setzero_ps(); }
    // pack two 128-bit registers into the low/high halves
    static SHADER_MATH_INLINE dreg pack(freg lo128, freg hi128) {
        return _mm256_insertf128_ps(_mm256_castps128_ps256(lo128), hi128, 1);
    }
    static SHADER_MATH_INLINE freg lo(dreg v) { return _mm256_castps256_ps128(v); }
    static SHADER_MATH_INLINE freg hi(dreg v) { return _mm256_extractf128_ps(v, 1); }
    // broadcast one 128-bit register to both halves
    static SHADER_MATH_INLINE dreg bcast128(freg r) {
        return _mm256_insertf128_ps(_mm256_castps128_ps256(r), r, 1);
    }
    // broadcast lane J within each 128-bit half independently (the scalar
    // differs per packed row, hence per-lane, not cross-lane)
    template <int J>
    static SHADER_MATH_INLINE dreg bcast_per_lane(dreg v) {
        return _mm256_permute_ps(v, _MM_SHUFFLE(J, J, J, J));
    }
    static SHADER_MATH_INLINE dreg fmadd(dreg a, dreg b, dreg c) {
        return _mm256_fmadd_ps(a, b, c);
    }

    // Shared float 4x4 product kernel, two output registers per YMM:
    //   out[j], out[j+1] = sum_K bcast_per_lane<K>(pack(pk[j], pk[j+1]))
    //                            * bcast128(bc[K])
    // HLSL: pk = rows of a, bc = rows of b. GLSL: pk = columns of b,
    // bc = columns of a (the transpose symmetry swaps the roles). Same
    // operation sequence and rounding as the 128-bit fmadd path, so results
    // are bit-identical.
    // V is fvec<4> passed as a template parameter: this header is included
    // (via simd.hpp) before vector.hpp defines fvec, so the register arrays
    // of both matrix types can be handed over directly without a copy at the
    // call sites.
    template <class V>
    static SHADER_MATH_INLINE void mul4x4(const V* pk, const V* bc, V* out) {
        for (int j = 0; j < 4; j += 2) {
            const dreg p2 = pack(pk[j].q_, pk[j + 1].q_);
            dreg acc = zero();
            acc = fmadd(bcast_per_lane<0>(p2), bcast128(bc[0].q_), acc);
            acc = fmadd(bcast_per_lane<1>(p2), bcast128(bc[1].q_), acc);
            acc = fmadd(bcast_per_lane<2>(p2), bcast128(bc[2].q_), acc);
            acc = fmadd(bcast_per_lane<3>(p2), bcast128(bc[3].q_), acc);
            out[j] = V(lo(acc));
            out[j + 1] = V(hi(acc));
        }
    }
};

}  // namespace detail
}  // namespace shader_math
