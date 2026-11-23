// 32-bit integer backend (detail::iops / uops) against a 64-bit reference:
// wrapping arithmetic, division and shifts with their defined results for
// zero divisors, INT_MIN / -1 and large counts, comparisons, and the float
// conversions on NaN, infinities and the edges of the integer range.
#include <cstdint>
#include <cstring>
#include <limits>
#include <type_traits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace {

using shader_math::detail::freg;
using shader_math::detail::iops;
using shader_math::detail::ireg;
using shader_math::detail::mreg;
using shader_math::detail::ops;
using shader_math::detail::uops;

constexpr std::uint32_t kEdges[] = {
    0u,          1u,          2u,          3u,          7u,          31u,         32u,
    33u,         65535u,      65536u,      65537u,      0x00FFFFFFu, 0x01000001u, 0x12345678u,
    0x7FFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xDEADBEEFu, 0xFFFF0000u, 0xFFFFFFF9u,
    0xFFFFFFFEu, 0xFFFFFFFFu,
};
constexpr int kNumEdges = static_cast<int>(sizeof(kEdges) / sizeof(kEdges[0]));

std::int32_t as_s(std::uint32_t u) { return static_cast<std::int32_t>(u); }

// Reference results, computed in 64 bits and reduced modulo 2^32.
std::uint32_t ref_div(bool sgn, std::uint32_t a, std::uint32_t b) {
    if (b == 0u) return 0xFFFFFFFFu;
    if (!sgn) return a / b;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(as_s(a)) / as_s(b));
}
std::uint32_t ref_rem(bool sgn, std::uint32_t a, std::uint32_t b) {
    if (b == 0u) return 0xFFFFFFFFu;
    if (!sgn) return a % b;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(as_s(a)) % as_s(b));
}
std::uint32_t ref_shr(bool sgn, std::uint32_t a, std::uint32_t n) {
    n &= 31u;
    if (!sgn) return a >> n;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(as_s(a)) >> n);
}
bool ref_lt(bool sgn, std::uint32_t a, std::uint32_t b) { return sgn ? as_s(a) < as_s(b) : a < b; }

template <class Ops>
void store_u(ireg r, std::uint32_t* out) {
    typename Ops::T t[4];
    Ops::store4(r, t);
    for (int i = 0; i < 4; ++i) out[i] = static_cast<std::uint32_t>(t[i]);
}
template <class Ops>
ireg load_u(const std::uint32_t* in) {
    return Ops::set(static_cast<typename Ops::T>(in[0]), static_cast<typename Ops::T>(in[1]),
                    static_cast<typename Ops::T>(in[2]), static_cast<typename Ops::T>(in[3]));
}
std::uint32_t mask_lane(mreg m, int i) { return ops::lane_b(m, i) ? 0xFFFFFFFFu : 0u; }

// Every (a, b) pair of edge values, four pairs per register.
template <class Ops, class F>
void for_pairs(F f) {
    std::uint32_t a[4], b[4];
    int k = 0;
    for (int i = 0; i < kNumEdges; ++i) {
        for (int j = 0; j < kNumEdges; ++j) {
            a[k] = kEdges[i];
            b[k] = kEdges[j];
            if (++k == 4) {
                f(a, b);
                k = 0;
            }
        }
    }
    if (k != 0) {
        for (; k < 4; ++k) a[k] = b[k] = 1u;
        f(a, b);
    }
}

template <class Ops>
void test_binary_ops() {
    constexpr bool sgn = std::is_same_v<typename Ops::T, std::int32_t>;
    for_pairs<Ops>([](const std::uint32_t* a, const std::uint32_t* b) {
        const ireg ra = load_u<Ops>(a);
        const ireg rb = load_u<Ops>(b);
        std::uint32_t add[4], sub[4], mul[4], div[4], rem[4], shl[4], shr[4], mn[4], mx[4];
        std::uint32_t an[4], orr[4], xr[4];
        store_u<Ops>(Ops::add(ra, rb), add);
        store_u<Ops>(Ops::sub(ra, rb), sub);
        store_u<Ops>(Ops::mul(ra, rb), mul);
        store_u<Ops>(Ops::div(ra, rb), div);
        store_u<Ops>(Ops::rem(ra, rb), rem);
        store_u<Ops>(Ops::shl(ra, rb), shl);
        store_u<Ops>(Ops::shr(ra, rb), shr);
        store_u<Ops>(Ops::min(ra, rb), mn);
        store_u<Ops>(Ops::max(ra, rb), mx);
        store_u<Ops>(Ops::band(ra, rb), an);
        store_u<Ops>(Ops::bor(ra, rb), orr);
        store_u<Ops>(Ops::bxor(ra, rb), xr);
        const mreg eq = Ops::cmp_eq(ra, rb), ne = Ops::cmp_ne(ra, rb);
        const mreg lt = Ops::cmp_lt(ra, rb), le = Ops::cmp_le(ra, rb);
        const mreg gt = Ops::cmp_gt(ra, rb), ge = Ops::cmp_ge(ra, rb);
        for (int i = 0; i < 4; ++i) {
            const std::uint32_t x = a[i], y = b[i];
            CHECK(add[i] == x + y);
            CHECK(sub[i] == x - y);
            CHECK(mul[i] == x * y);
            CHECK(div[i] == ref_div(sgn, x, y));
            CHECK(rem[i] == ref_rem(sgn, x, y));
            CHECK(shl[i] == x << (y & 31u));
            CHECK(shr[i] == ref_shr(sgn, x, y));
            CHECK(mn[i] == (ref_lt(sgn, x, y) ? x : y));
            CHECK(mx[i] == (ref_lt(sgn, y, x) ? x : y));
            CHECK(an[i] == (x & y));
            CHECK(orr[i] == (x | y));
            CHECK(xr[i] == (x ^ y));
            CHECK(ops::lane_b(eq, i) == (x == y));
            CHECK(ops::lane_b(ne, i) == (x != y));
            CHECK(ops::lane_b(lt, i) == ref_lt(sgn, x, y));
            CHECK(ops::lane_b(le, i) == !ref_lt(sgn, y, x));
            CHECK(ops::lane_b(gt, i) == ref_lt(sgn, y, x));
            CHECK(ops::lane_b(ge, i) == !ref_lt(sgn, x, y));
            // a full lane mask, not just a set bit
            CHECK(mask_lane(eq, i) == (x == y ? 0xFFFFFFFFu : 0u));
        }
        std::uint32_t lt_bits[4];
        store_u<Ops>(Ops::from_mask(lt), lt_bits);
        for (int i = 0; i < 4; ++i)
            CHECK(lt_bits[i] == (ref_lt(sgn, a[i], b[i]) ? 0xFFFFFFFFu : 0u));
        std::uint32_t pick[4];
        store_u<Ops>(Ops::sel(lt, ra, rb), pick);
        for (int i = 0; i < 4; ++i) CHECK(pick[i] == (ref_lt(sgn, a[i], b[i]) ? a[i] : b[i]));
    });
}

template <class Ops>
void test_unary_ops() {
    constexpr bool sgn = std::is_same_v<typename Ops::T, std::int32_t>;
    for (int i = 0; i < kNumEdges; i += 4) {
        std::uint32_t a[4];
        for (int k = 0; k < 4; ++k) a[k] = kEdges[(i + k) % kNumEdges];
        const ireg r = load_u<Ops>(a);
        std::uint32_t neg[4], bnot[4], abs[4], lanes[4];
        store_u<Ops>(Ops::neg(r), neg);
        store_u<Ops>(Ops::bnot(r), bnot);
        store_u<Ops>(Ops::abs(r), abs);
        for (int k = 0; k < 4; ++k) {
            CHECK(neg[k] == 0u - a[k]);
            CHECK(bnot[k] == ~a[k]);
            CHECK(abs[k] == (sgn && (a[k] >> 31) ? 0u - a[k] : a[k]));
            lanes[k] = static_cast<std::uint32_t>(Ops::lane(r, k));
            CHECK(lanes[k] == a[k]);
        }
        CHECK(static_cast<std::uint32_t>(Ops::template lane_c<2>(r)) == a[2]);
        std::uint32_t b[4];
        store_u<Ops>(Ops::template bcast<3>(r), b);
        for (int k = 0; k < 4; ++k) CHECK(b[k] == a[3]);
        typename Ops::T mem[4];
        Ops::store4(r, mem);
        std::uint32_t back[4];
        store_u<Ops>(Ops::load4(mem), back);
        for (int k = 0; k < 4; ++k) CHECK(back[k] == a[k]);
    }
    std::uint32_t z[4], s[4];
    store_u<Ops>(Ops::zero(), z);
    store_u<Ops>(Ops::splat(static_cast<typename Ops::T>(0x80000001u)), s);
    for (int k = 0; k < 4; ++k) {
        CHECK(z[k] == 0u);
        CHECK(s[k] == 0x80000001u);
    }
}

std::uint32_t f_bits(float f) {
    std::uint32_t u;
    std::memcpy(&u, &f, sizeof(u));
    return u;
}

void test_conversions() {
    constexpr float inf = std::numeric_limits<float>::infinity();
    const float nan = std::numeric_limits<float>::quiet_NaN();
    // value, int32 result, uint32 result
    struct Case {
        float f;
        std::int32_t i;
        std::uint32_t u;
    };
    const Case cases[] = {
        {0.0f, 0, 0u},
        {-0.0f, 0, 0u},
        {0.75f, 0, 0u},
        {-0.75f, 0, 0u},
        {1.5f, 1, 1u},
        {-1.5f, -1, 0u},
        {-1.0f, -1, 0u},
        {16777217.0f, 16777216, 16777216u},
        {2147483520.0f, 2147483520, 2147483520u},  // largest float below 2^31
        {2147483648.0f, INT32_MAX, 2147483648u},
        {-2147483648.0f, INT32_MIN, 0u},
        {-2147483904.0f, INT32_MIN, 0u},
        {3000000000.0f, INT32_MAX, 3000000000u},
        {4294967040.0f, INT32_MAX, 4294967040u},  // largest float below 2^32
        {4294967296.0f, INT32_MAX, UINT32_MAX},
        {1e20f, INT32_MAX, UINT32_MAX},
        {-1e20f, INT32_MIN, 0u},
        {inf, INT32_MAX, UINT32_MAX},
        {-inf, INT32_MIN, 0u},
        {nan, 0, 0u},
        {-nan, 0, 0u},
    };
    constexpr int n = static_cast<int>(sizeof(cases) / sizeof(cases[0]));
    for (int base = 0; base < n; base += 4) {
        float in[4];
        for (int k = 0; k < 4; ++k) in[k] = cases[(base + k) % n].f;
        const freg f = ops::load4(in);
        std::int32_t si[4];
        std::uint32_t ui[4];
        iops::store4(iops::from_f32(f), si);
        uops::store4(uops::from_f32(f), ui);
        for (int k = 0; k < 4; ++k) {
            const Case& c = cases[(base + k) % n];
            CHECK(si[k] == c.i);
            CHECK(ui[k] == c.u);
        }
    }

    // int / uint -> float rounds to nearest even, like static_cast
    for_pairs<uops>([](const std::uint32_t* a, const std::uint32_t*) {
        float fs[4], fu[4];
        ops::store4(iops::to_f32(load_u<iops>(a)), fs);
        ops::store4(uops::to_f32(load_u<uops>(a)), fu);
        for (int k = 0; k < 4; ++k) {
            CHECK(f_bits(fs[k]) == f_bits(static_cast<float>(as_s(a[k]))));
            CHECK(f_bits(fu[k]) == f_bits(static_cast<float>(a[k])));
        }
    });

    // bit casts round-trip every pattern, NaN payloads included
    for_pairs<uops>([](const std::uint32_t* a, const std::uint32_t*) {
        const ireg r = load_u<uops>(a);
        float f[4];
        ops::store4(uops::bits_to_f32(r), f);
        std::uint32_t back[4];
        store_u<uops>(uops::bits_from_f32(uops::bits_to_f32(r)), back);
        for (int k = 0; k < 4; ++k) {
            CHECK(f_bits(f[k]) == a[k]);
            CHECK(back[k] == a[k]);
        }
    });
}

}  // namespace

void test_int_ops() {
    test_binary_ops<iops>();
    test_binary_ops<uops>();
    test_unary_ops<iops>();
    test_unary_ops<uops>();
    test_conversions();
}
