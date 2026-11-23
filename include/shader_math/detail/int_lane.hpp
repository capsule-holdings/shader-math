// One-lane integer operations: those whose shader results are undefined, with
// a result defined for every input (no trap, no C++ undefined behaviour), the
// same on every backend (docs/design/int_vectors.md, "Undefined results"),
// and the bit-manipulation functions.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/int_lane.hpp"
#endif

#include <bit>
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

    // bit counts and positions; -1 where there is no such bit
    static SHADER_MATH_INLINE std::uint32_t popcount(std::uint32_t x) {
        return static_cast<std::uint32_t>(std::popcount(x));
    }
    static SHADER_MATH_INLINE std::int32_t msb(std::uint32_t x) {
        return x != 0u ? 31 - std::countl_zero(x) : -1;
    }
    // signed: the highest bit that differs from the sign bit (-1 for 0 and -1)
    static SHADER_MATH_INLINE std::int32_t msb_s(std::int32_t x) {
        const auto u = static_cast<std::uint32_t>(x);
        return msb(x < 0 ? ~u : u);
    }
    static SHADER_MATH_INLINE std::int32_t lsb(std::uint32_t x) {
        return x != 0u ? std::countr_zero(x) : -1;
    }
    static SHADER_MATH_INLINE std::uint32_t reverse(std::uint32_t x) {
        x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
        x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
        x = ((x >> 4) & 0x0F0F0F0Fu) | ((x & 0x0F0F0F0Fu) << 4);
        x = ((x >> 8) & 0x00FF00FFu) | ((x & 0x00FF00FFu) << 8);
        return (x >> 16) | (x << 16);
    }

    // bitfieldExtract / bitfieldInsert on the field [offset, offset + bits).
    // GLSL leaves a field outside the 32 bits undefined; here negative values
    // count as 0 and the field ends at bit 31.
    static SHADER_MATH_INLINE void clamp_field(int& offset, int& bits) {
        offset = offset < 0 ? 0 : (offset > 32 ? 32 : offset);
        const int room = 32 - offset;
        bits = bits < 0 ? 0 : (bits > room ? room : bits);
    }
    static SHADER_MATH_INLINE std::uint32_t low_mask(int bits) {
        return bits >= 32 ? 0xFFFFFFFFu : (1u << bits) - 1u;
    }
    static SHADER_MATH_INLINE std::uint32_t extract_u(std::uint32_t v, int offset, int bits) {
        clamp_field(offset, bits);
        if (bits == 0) return 0u;
        return (v >> offset) & low_mask(bits);
    }
    // the field sign-extended from its top bit
    static SHADER_MATH_INLINE std::int32_t extract_s(std::int32_t v, int offset, int bits) {
        clamp_field(offset, bits);
        if (bits == 0) return 0;
        const std::uint32_t f = (static_cast<std::uint32_t>(v) >> offset) & low_mask(bits);
        const std::uint32_t sign = 1u << (bits - 1);
        return static_cast<std::int32_t>((f ^ sign) - sign);
    }
    static SHADER_MATH_INLINE std::uint32_t insert(std::uint32_t base, std::uint32_t ins, int offset,
                                                   int bits) {
        clamp_field(offset, bits);
        if (bits == 0) return base;
        const std::uint32_t m = low_mask(bits) << offset;
        return (base & ~m) | ((ins << offset) & m);
    }
};

}  // namespace detail
}  // namespace shader_math
