// 16-bit and 64-bit integer lanes, on top of the selected backend.
//   16-bit (int16_t / uint16_t): four lanes in 8 bytes, like half; every
//     operation widens to the 32-bit int_ops and wraps back to 16 bits.
//   64-bit (int64_t / uint64_t): four lanes in 32 bytes, one lane at a time
//     (correctness first; a SIMD form can replace it later).
// Results the shading languages leave undefined follow detail/int_lane.hpp at
// the lane's width: x / 0 and x % 0 give all bits set, MIN / -1 wraps, shift
// counts are taken modulo the width, float -> int saturates with NaN -> 0.
// Comparisons produce the shared 32-bit mask (mreg).
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_int_widths.hpp"
#endif

#include <cstdint>
#include <limits>
#include <type_traits>

namespace shader_math {
namespace detail {

// ---- 16-bit lanes -------------------------------------------------------------
template <class T>
struct alignas(8) nreg {
    T v[4];
};

template <class T>
struct narrow_ops {
    static constexpr bool is_signed = std::is_signed_v<T>;
    using W = std::conditional_t<is_signed, std::int32_t, std::uint32_t>;
    using WO = int_ops<is_signed>;
    using reg = nreg<T>;

    static SHADER_MATH_INLINE ireg widen(reg r) {
        return WO::set(static_cast<W>(r.v[0]), static_cast<W>(r.v[1]), static_cast<W>(r.v[2]),
                       static_cast<W>(r.v[3]));
    }
    // modulo 2^16
    static SHADER_MATH_INLINE reg narrow(ireg w) {
        W t[4];
        WO::store4(w, t);
        return {{static_cast<T>(t[0]), static_cast<T>(t[1]), static_cast<T>(t[2]),
                 static_cast<T>(t[3])}};
    }

    static SHADER_MATH_INLINE reg zero() { return {{T(0), T(0), T(0), T(0)}}; }
    static SHADER_MATH_INLINE reg splat(T s) { return {{s, s, s, s}}; }
    static SHADER_MATH_INLINE reg set(T x, T y, T z, T w) { return {{x, y, z, w}}; }
    static SHADER_MATH_INLINE reg load4(const T* p) { return {{p[0], p[1], p[2], p[3]}}; }
    static SHADER_MATH_INLINE void store4(reg r, T* p) {
        for (int i = 0; i < 4; ++i) p[i] = r.v[i];
    }
    static SHADER_MATH_INLINE T lane(reg r, int i) { return r.v[i]; }
    template <int I>
    static SHADER_MATH_INLINE T lane_c(reg r) {
        return r.v[I];
    }
    template <int I>
    static SHADER_MATH_INLINE reg bcast(reg r) {
        return splat(r.v[I]);
    }

#define SM_NARROW_BIN(name)                                          \
    static SHADER_MATH_INLINE reg name(reg a, reg b) {               \
        return narrow(WO::name(widen(a), widen(b)));                 \
    }
    SM_NARROW_BIN(add)
    SM_NARROW_BIN(sub)
    SM_NARROW_BIN(mul)
    SM_NARROW_BIN(div)
    SM_NARROW_BIN(rem)
    SM_NARROW_BIN(band)
    SM_NARROW_BIN(bor)
    SM_NARROW_BIN(bxor)
    SM_NARROW_BIN(min)
    SM_NARROW_BIN(max)
#undef SM_NARROW_BIN
    static SHADER_MATH_INLINE reg neg(reg a) { return narrow(WO::neg(widen(a))); }
    static SHADER_MATH_INLINE reg bnot(reg a) { return narrow(WO::bnot(widen(a))); }
    static SHADER_MATH_INLINE reg abs(reg a) { return narrow(WO::abs(widen(a))); }
    // the count modulo 16
    static SHADER_MATH_INLINE reg shl(reg a, reg n) {
        return narrow(WO::shl(widen(a), WO::band(widen(n), WO::splat(W(15)))));
    }
    static SHADER_MATH_INLINE reg shr(reg a, reg n) {
        return narrow(WO::shr(widen(a), WO::band(widen(n), WO::splat(W(15)))));
    }
    static SHADER_MATH_INLINE reg sel(mreg m, reg a, reg b) {
        return narrow(WO::sel(m, widen(a), widen(b)));
    }

    static SHADER_MATH_INLINE mreg cmp_eq(reg a, reg b) { return WO::cmp_eq(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ne(reg a, reg b) { return WO::cmp_ne(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_lt(reg a, reg b) { return WO::cmp_lt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_le(reg a, reg b) { return WO::cmp_le(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_gt(reg a, reg b) { return WO::cmp_gt(widen(a), widen(b)); }
    static SHADER_MATH_INLINE mreg cmp_ge(reg a, reg b) { return WO::cmp_ge(widen(a), widen(b)); }

    static SHADER_MATH_INLINE freg to_f32(reg a) { return WO::to_f32(widen(a)); }
    // saturates to the 16-bit range (the 32-bit conversion saturates to its own)
    static SHADER_MATH_INLINE reg from_f32(freg f) {
        const ireg w = WO::from_f32(f);
        return narrow(WO::min(WO::max(w, WO::splat(static_cast<W>(std::numeric_limits<T>::min()))),
                              WO::splat(static_cast<W>(std::numeric_limits<T>::max()))));
    }
    static SHADER_MATH_INLINE reg from_mask(mreg m) { return narrow(WO::from_mask(m)); }
};

// ---- 64-bit lanes -------------------------------------------------------------
template <class T>
struct alignas(16) wreg {
    T v[4];
};

template <class T>
struct wide_ops {
    static constexpr bool is_signed = std::is_signed_v<T>;
    using U = std::uint64_t;
    using reg = wreg<T>;

    static SHADER_MATH_INLINE U u(T x) { return static_cast<U>(x); }
    static SHADER_MATH_INLINE T t(U x) { return static_cast<T>(x); }
    template <class F>
    static SHADER_MATH_INLINE reg map(reg a, reg b, F f) {
        return {{f(a.v[0], b.v[0]), f(a.v[1], b.v[1]), f(a.v[2], b.v[2]), f(a.v[3], b.v[3])}};
    }
    template <class F>
    static SHADER_MATH_INLINE reg map(reg a, F f) {
        return {{f(a.v[0]), f(a.v[1]), f(a.v[2]), f(a.v[3])}};
    }
    template <class F>
    static SHADER_MATH_INLINE mreg mask(reg a, reg b, F f) {
        float m[4];
        for (int i = 0; i < 4; ++i) m[i] = f(a.v[i], b.v[i]) ? 1.0f : 0.0f;
        return ops::cmp_ne(ops::load4(m), ops::zero());
    }

    static SHADER_MATH_INLINE reg zero() { return {{T(0), T(0), T(0), T(0)}}; }
    static SHADER_MATH_INLINE reg splat(T s) { return {{s, s, s, s}}; }
    static SHADER_MATH_INLINE reg set(T x, T y, T z, T w) { return {{x, y, z, w}}; }
    static SHADER_MATH_INLINE reg load4(const T* p) { return {{p[0], p[1], p[2], p[3]}}; }
    static SHADER_MATH_INLINE void store4(reg r, T* p) {
        for (int i = 0; i < 4; ++i) p[i] = r.v[i];
    }
    static SHADER_MATH_INLINE T lane(reg r, int i) { return r.v[i]; }
    template <int I>
    static SHADER_MATH_INLINE T lane_c(reg r) {
        return r.v[I];
    }
    template <int I>
    static SHADER_MATH_INLINE reg bcast(reg r) {
        return splat(r.v[I]);
    }

    // wrapping arithmetic in unsigned 64 bits
    static SHADER_MATH_INLINE reg add(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) + u(y)); }); }
    static SHADER_MATH_INLINE reg sub(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) - u(y)); }); }
    static SHADER_MATH_INLINE reg mul(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) * u(y)); }); }
    static SHADER_MATH_INLINE reg neg(reg a) { return map(a, [](T x) { return t(U(0) - u(x)); }); }
    static SHADER_MATH_INLINE reg div(reg a, reg b) {
        return map(a, b, [](T x, T y) -> T {
            if (y == T(0)) return t(~U(0));
            if constexpr (is_signed)
                if (y == T(-1)) return t(U(0) - u(x));
            return static_cast<T>(x / y);
        });
    }
    static SHADER_MATH_INLINE reg rem(reg a, reg b) {
        return map(a, b, [](T x, T y) -> T {
            if (y == T(0)) return t(~U(0));
            if constexpr (is_signed)
                if (y == T(-1)) return T(0);
            return static_cast<T>(x % y);
        });
    }
    static SHADER_MATH_INLINE reg band(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) & u(y)); }); }
    static SHADER_MATH_INLINE reg bor(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) | u(y)); }); }
    static SHADER_MATH_INLINE reg bxor(reg a, reg b) { return map(a, b, [](T x, T y) { return t(u(x) ^ u(y)); }); }
    static SHADER_MATH_INLINE reg bnot(reg a) { return map(a, [](T x) { return t(~u(x)); }); }
    // the count modulo 64; >> is arithmetic on signed lanes (C++20)
    static SHADER_MATH_INLINE reg shl(reg a, reg n) {
        return map(a, n, [](T x, T c) { return t(u(x) << (u(c) & 63u)); });
    }
    static SHADER_MATH_INLINE reg shr(reg a, reg n) {
        return map(a, n, [](T x, T c) { return static_cast<T>(x >> (u(c) & 63u)); });
    }
    static SHADER_MATH_INLINE reg abs(reg a) {
        return map(a, [](T x) {
            if constexpr (is_signed)
                return x < T(0) ? t(U(0) - u(x)) : x;
            else
                return x;
        });
    }
    static SHADER_MATH_INLINE reg min(reg a, reg b) { return map(a, b, [](T x, T y) { return x < y ? x : y; }); }
    static SHADER_MATH_INLINE reg max(reg a, reg b) { return map(a, b, [](T x, T y) { return x > y ? x : y; }); }
    static SHADER_MATH_INLINE reg sel(mreg m, reg a, reg b) {
        return {{ops::lane_b(m, 0) ? a.v[0] : b.v[0], ops::lane_b(m, 1) ? a.v[1] : b.v[1],
                 ops::lane_b(m, 2) ? a.v[2] : b.v[2], ops::lane_b(m, 3) ? a.v[3] : b.v[3]}};
    }

    static SHADER_MATH_INLINE mreg cmp_eq(reg a, reg b) { return mask(a, b, [](T x, T y) { return x == y; }); }
    static SHADER_MATH_INLINE mreg cmp_ne(reg a, reg b) { return mask(a, b, [](T x, T y) { return x != y; }); }
    static SHADER_MATH_INLINE mreg cmp_lt(reg a, reg b) { return mask(a, b, [](T x, T y) { return x < y; }); }
    static SHADER_MATH_INLINE mreg cmp_le(reg a, reg b) { return mask(a, b, [](T x, T y) { return x <= y; }); }
    static SHADER_MATH_INLINE mreg cmp_gt(reg a, reg b) { return mask(a, b, [](T x, T y) { return x > y; }); }
    static SHADER_MATH_INLINE mreg cmp_ge(reg a, reg b) { return mask(a, b, [](T x, T y) { return x >= y; }); }

    static SHADER_MATH_INLINE freg to_f32(reg a) {
        return ops::set(static_cast<float>(a.v[0]), static_cast<float>(a.v[1]),
                        static_cast<float>(a.v[2]), static_cast<float>(a.v[3]));
    }
    static SHADER_MATH_INLINE reg from_f32(freg f) {
        float in[4];
        ops::store4(f, in);
        return {{int_lane::f2int<T>(in[0]), int_lane::f2int<T>(in[1]), int_lane::f2int<T>(in[2]),
                 int_lane::f2int<T>(in[3])}};
    }
    static SHADER_MATH_INLINE reg from_mask(mreg m) {
        return {{ops::lane_b(m, 0) ? t(~U(0)) : T(0), ops::lane_b(m, 1) ? t(~U(0)) : T(0),
                 ops::lane_b(m, 2) ? t(~U(0)) : T(0), ops::lane_b(m, 3) ? t(~U(0)) : T(0)}};
    }
};

}  // namespace detail
}  // namespace shader_math
