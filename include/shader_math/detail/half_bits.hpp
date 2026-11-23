// binary16 <-> float conversion on the bits, for every compiler: the software
// half type of MSVC and the GLSL / HLSL packing functions (packHalf2x16,
// f32tof16) use it. Rounds to nearest even; NaN stays NaN.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/half_bits.hpp"
#endif

#include <cstring>

namespace shader_math {
namespace detail {

struct half_bits {
    static float to_f32(unsigned short h) {
        const unsigned sign = static_cast<unsigned>(h & 0x8000u) << 16;
        const unsigned exp = (h >> 10) & 0x1Fu;
        unsigned man = h & 0x3FFu;
        unsigned out;
        if (exp == 0u) {
            if (man == 0u) {
                out = sign;
            } else {  // subnormal half -> normalized float
                int e = 0;
                while (!(man & 0x400u)) {
                    man <<= 1;
                    ++e;
                }
                man &= 0x3FFu;
                out = sign | ((127u - 15u + 1u - static_cast<unsigned>(e)) << 23) | (man << 13);
            }
        } else if (exp == 31u) {  // inf / NaN
            out = sign | 0x7F800000u | (man << 13);
        } else {
            out = sign | ((exp - 15u + 127u) << 23) | (man << 13);
        }
        float f;
        std::memcpy(&f, &out, sizeof(f));
        return f;
    }

    static unsigned short from_f32(float f) {
        unsigned in;
        std::memcpy(&in, &f, sizeof(in));
        const unsigned short sign = static_cast<unsigned short>((in >> 16) & 0x8000u);
        const unsigned fexp = (in >> 23) & 0xFFu;
        const unsigned man = in & 0x7FFFFFu;
        if (fexp == 0xFFu)  // inf / NaN (keep NaN payload non-zero)
            return static_cast<unsigned short>(
                sign | 0x7C00u | (man ? (0x200u | (man >> 13)) : 0u));
        const int exp = static_cast<int>(fexp) - 127 + 15;
        if (exp >= 31) return static_cast<unsigned short>(sign | 0x7C00u);  // overflow -> inf
        if (exp <= 0) {                                                     // subnormal / zero
            if (exp < -10) return sign;  // too small: round to zero
            const unsigned full = man | 0x800000u;
            const unsigned shift = static_cast<unsigned>(14 - exp);  // 14..24
            unsigned half_man = full >> shift;
            const unsigned rem = full & ((1u << shift) - 1u);
            const unsigned halfway = 1u << (shift - 1u);
            if (rem > halfway || (rem == halfway && (half_man & 1u))) ++half_man;
            return static_cast<unsigned short>(sign | half_man);
        }
        unsigned short out =
            static_cast<unsigned short>(sign | (static_cast<unsigned>(exp) << 10) | (man >> 13));
        const unsigned rem = man & 0x1FFFu;
        // Round to nearest even; a mantissa carry correctly bumps the exponent.
        if (rem > 0x1000u || (rem == 0x1000u && (out & 1u))) ++out;
        return out;
    }
};

}  // namespace detail
}  // namespace shader_math
