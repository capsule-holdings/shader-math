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

namespace shader_math {
namespace detail {

#if defined(SHADER_MATH_HALF_IS_FLOAT16)

using half_t = _Float16;

#else

// Software binary16 for compilers without _Float16 (MSVC): the bits, with
// every operation through float (half_bits converts)
struct binary16 {
    unsigned short bits_;
    static SHADER_MATH_INLINE float to_f32(unsigned short h) { return half_bits::to_f32(h); }
    static SHADER_MATH_INLINE unsigned short from_f32(float f) { return half_bits::from_f32(f); }

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
