// One-lane integer operations whose shader results are undefined, with a
// result defined for every input (no trap, no C++ undefined behaviour), the
// same on every backend: docs/design/int_vectors.md, "Undefined results".
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/int_lane.hpp"
#endif

#include <cstdint>

namespace shader_math {
namespace detail {

struct int_lane {
    // x / 0 and x % 0 give all bits set (D3D's udiv)
    static SHADER_MATH_INLINE std::uint32_t udiv(std::uint32_t a, std::uint32_t b) {
        return b != 0u ? a / b : 0xFFFFFFFFu;
    }
    static SHADER_MATH_INLINE std::uint32_t urem(std::uint32_t a, std::uint32_t b) {
        return b != 0u ? a % b : 0xFFFFFFFFu;
    }
    // x / 0 and x % 0 give -1 (all bits set, as for uint); INT_MIN / -1 wraps
    // to INT_MIN with remainder 0
    static SHADER_MATH_INLINE std::int32_t sdiv(std::int32_t a, std::int32_t b) {
        if (b == 0) return -1;
        if (b == -1) return static_cast<std::int32_t>(0u - static_cast<std::uint32_t>(a));
        return a / b;
    }
    static SHADER_MATH_INLINE std::int32_t srem(std::int32_t a, std::int32_t b) {
        if (b == 0) return -1;
        if (b == -1) return 0;
        return a % b;
    }

    // the shift count is taken modulo 32 (HLSL's rule)
    static SHADER_MATH_INLINE std::uint32_t shl(std::uint32_t a, std::uint32_t n) {
        return a << (n & 31u);
    }
    static SHADER_MATH_INLINE std::uint32_t shr_u(std::uint32_t a, std::uint32_t n) {
        return a >> (n & 31u);
    }
    static SHADER_MATH_INLINE std::int32_t shr_s(std::int32_t a, std::uint32_t n) {
        return a >> (n & 31u);
    }

    // float -> int truncates toward zero; NaN gives 0 and out-of-range values
    // saturate (D3D's ftoi / ftou, AArch64's FCVTZS / FCVTZU)
    static SHADER_MATH_INLINE std::int32_t f2i(float f) {
        if (!(f == f)) return 0;
        if (f >= 2147483648.0f) return INT32_MAX;
        if (f <= -2147483648.0f) return INT32_MIN;
        return static_cast<std::int32_t>(f);
    }
    static SHADER_MATH_INLINE std::uint32_t f2u(float f) {
        if (!(f > 0.0f)) return 0u;
        if (f >= 4294967296.0f) return UINT32_MAX;
        return static_cast<std::uint32_t>(f);
    }
};

}  // namespace detail
}  // namespace shader_math
