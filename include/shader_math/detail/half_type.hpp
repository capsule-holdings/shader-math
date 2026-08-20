// Half (binary16) scalar type.
//   GCC/Clang: half_t = _Float16 (native or promoted arithmetic, IEEE binary16).
//   MSVC:      half_t = binary16, a bit-stored software type; every operation
//              converts through float. Rounding therefore differs from a true
//              half ALU by one extra float rounding per operation.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/half_type.hpp"
#endif

#if !defined(SHADER_MATH_HALF_IS_FLOAT16)
#  include <cstring>
#endif

namespace shader_math {
namespace detail {

#if defined(SHADER_MATH_HALF_IS_FLOAT16)

using half_t = _Float16;

#else

// Software binary16 for compilers without _Float16 (MSVC).
struct binary16 {
    unsigned short bits_;

    static SHADER_MATH_INLINE float to_f32(unsigned short h) {
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

    static SHADER_MATH_INLINE unsigned short from_f32(float f) {
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

    SHADER_MATH_INLINE binary16() : bits_(0) {}
    SHADER_MATH_INLINE binary16(float f) : bits_(from_f32(f)) {}
    SHADER_MATH_INLINE binary16(double d) : bits_(from_f32(static_cast<float>(d))) {}
    SHADER_MATH_INLINE binary16(int i) : bits_(from_f32(static_cast<float>(i))) {}
    SHADER_MATH_INLINE operator float() const { return to_f32(bits_); }

    SHADER_MATH_INLINE binary16 operator-() const { return binary16(-to_f32(bits_)); }

#define SM_B16_ARITH(sym)                                                    \
    SHADER_MATH_INLINE friend binary16 operator sym(binary16 a, binary16 b) { \
        return binary16(static_cast<float>(a) sym static_cast<float>(b));    \
    }                                                                        \
    SHADER_MATH_INLINE binary16& operator sym##=(binary16 b) {               \
        *this = *this sym b;                                                 \
        return *this;                                                        \
    }
    SM_B16_ARITH(+)
    SM_B16_ARITH(-)
    SM_B16_ARITH(*)
    SM_B16_ARITH(/)
#undef SM_B16_ARITH

#define SM_B16_CMP(sym)                                                     \
    SHADER_MATH_INLINE friend bool operator sym(binary16 a, binary16 b) {   \
        return static_cast<float>(a) sym static_cast<float>(b);             \
    }
    SM_B16_CMP(==)
    SM_B16_CMP(!=)
    SM_B16_CMP(<)
    SM_B16_CMP(<=)
    SM_B16_CMP(>)
    SM_B16_CMP(>=)
#undef SM_B16_CMP
};

using half_t = binary16;

#endif  // SHADER_MATH_HALF_IS_FLOAT16

static_assert(sizeof(half_t) == 2, "shader-math: half must be 16 bits");

}  // namespace detail
}  // namespace shader_math
