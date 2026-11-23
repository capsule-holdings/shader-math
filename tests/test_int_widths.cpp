// 16- and 64-bit integer vectors: wrapping at their width, the defined
// results of division, shifts and float conversion there, and each
// language's implicit conversions and mixed operands across widths (the
// result types DXC and glslang give).
#include <cmath>
#include <cstdint>
#include <limits>
#include <sstream>
#include <type_traits>

#include <shader_math/shader_math.hpp>
#include <shader_math/std.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;

static_assert(sizeof(h::int16_t3) == 8 && sizeof(h::uint16_t4) == 8);
static_assert(sizeof(h::int64_t3) == 32 && sizeof(g::u64vec2) == 32);
static_assert(std::is_same_v<h::vector<std::int16_t, 3>, h::int16_t3>);

template <class A, class B>
using sum_t = decltype(std::declval<const A&>() + std::declval<const B&>());
template <class A, class B>
concept Addable = requires(const A& a, const B& b) { a + b; };

// HLSL: float when either is float, else the wider, unsigned when either is
static_assert(std::is_same_v<sum_t<h::int16_t3, h::int3>, h::int3>);
static_assert(std::is_same_v<sum_t<h::int16_t3, h::uint16_t3>, h::uint16_t3>);
static_assert(std::is_same_v<sum_t<h::uint16_t3, h::int3>, h::uint3>);
static_assert(std::is_same_v<sum_t<h::int3, h::int64_t3>, h::int64_t3>);
static_assert(std::is_same_v<sum_t<h::uint3, h::int64_t3>, h::uint64_t3>);
static_assert(std::is_same_v<sum_t<h::int64_t3, h::uint64_t3>, h::uint64_t3>);
static_assert(std::is_same_v<sum_t<h::int64_t3, h::float3>, h::float3>);
static_assert(std::is_same_v<sum_t<h::int16_t3, int>, h::int16_t3>);  // the literal adapts
static_assert(std::is_convertible_v<h::int3, h::int16_t3> && std::is_convertible_v<h::float3, h::int64_t3>);
// GLSL: one converts to the other implicitly
static_assert(std::is_same_v<sum_t<g::i16vec3, g::ivec3>, g::ivec3>);
static_assert(std::is_same_v<sum_t<g::i16vec3, g::u16vec3>, g::u16vec3>);
static_assert(std::is_same_v<sum_t<g::u16vec3, g::ivec3>, g::ivec3>);
static_assert(std::is_same_v<sum_t<g::uvec3, g::i64vec3>, g::i64vec3>);
static_assert(std::is_same_v<sum_t<g::i64vec3, g::u64vec3>, g::u64vec3>);
static_assert(!Addable<g::i64vec3, g::vec3> && !Addable<g::i64vec3, float>);
static_assert(std::is_convertible_v<g::i16vec3, g::ivec3> && std::is_convertible_v<g::uvec3, g::i64vec3>);
static_assert(!std::is_convertible_v<g::ivec3, g::i16vec3> && !std::is_convertible_v<g::u16vec3, g::i16vec3>);
static_assert(!std::is_convertible_v<g::i64vec3, g::vec3> && std::is_convertible_v<g::i16vec3, g::vec3>);
// The bit functions at every width: HLSL counts give uint and reversebits
// keeps the type; GLSL counts give a signed integer of the argument's width,
// and the bitfield functions (int and uint overloads only) take 16-bit lanes
// as int and no 64-bit ones. The 32-bit casts stay 32-bit.
template <class A>
concept HasBitfieldExtract = requires(const A& a) { g::bitfieldExtract(a, 0, 1); };
template <class A>
concept HasAsfloat = requires(const A& a) { h::asfloat(a); };
template <class A>
concept HasAsint16 = requires(const A& a) { h::asint16(a); };
static_assert(std::is_same_v<decltype(h::countbits(h::int16_t3())), h::uint3>);
static_assert(std::is_same_v<decltype(h::firstbithigh(h::uint64_t2())), h::uint2>);
static_assert(std::is_same_v<decltype(h::reversebits(h::int64_t3())), h::int64_t3>);
static_assert(std::is_same_v<decltype(h::countbits(std::uint64_t(1))), h::uint>);
static_assert(std::is_same_v<decltype(g::bitCount(g::u16vec3())), g::i16vec3>);
static_assert(std::is_same_v<decltype(g::findMSB(g::u64vec2())), g::i64vec2>);
static_assert(std::is_same_v<decltype(g::findLSB(std::uint16_t(1))), std::int16_t>);
static_assert(std::is_same_v<decltype(g::bitfieldExtract(g::u16vec3(), 0, 1)), g::ivec3>);
static_assert(std::is_same_v<decltype(g::bitfieldInsert(g::ivec3(), g::uvec3(), 0, 1)), g::uvec3>);
static_assert(HasBitfieldExtract<g::i16vec3> && !HasBitfieldExtract<g::i64vec3>);
static_assert(HasAsfloat<h::int3> && !HasAsfloat<h::int64_t3> && !HasAsfloat<h::uint16_t3>);
static_assert(HasAsint16<h::uint16_t3> && !HasAsint16<h::int3> && !HasAsint16<h::float3>);
// GLSL's casts and packing functions convert their argument as GLSL does
template <class A>
concept HasIntBitsToFloat = requires(const A& a) { g::intBitsToFloat(a); };
template <class A>
concept HasUnpackInt2x32 = requires(const A& a) { g::unpackInt2x32(a); };
template <class A>
concept HasPackInt2x16 = requires(const A& a) { g::packInt2x16(a); };
static_assert(HasIntBitsToFloat<g::i16vec3> && HasIntBitsToFloat<g::u16vec3> && !HasIntBitsToFloat<g::uvec3>);
static_assert(HasUnpackInt2x32<std::int64_t> && HasUnpackInt2x32<unsigned> && !HasUnpackInt2x32<std::uint64_t>);
static_assert(HasPackInt2x16<g::i16vec2> && !HasPackInt2x16<g::u16vec2> && !HasPackInt2x16<g::i16vec3>);

static void test_int16() {
    using namespace shader_math::hlsl;
    const int16_t2 w = int16_t2(32767, -32768) + int16_t2(1, -1);
    CHECK(w.x == -32768 && w.y == 32767);
    CHECK((uint16_t2(0, 5) - uint16_t2(1, 1)).x == 65535);
    CHECK((int16_t2(300, 2) * int16_t2(300, 2)).x == 24464);  // 90000 mod 2^16
    const int16_t3 d = int16_t3(7, -7, -32768) / int16_t3(0, 2, -1);
    CHECK(d.x == -1 && d.y == -3 && d.z == -32768);
    CHECK((uint16_t2(7, 7) / uint16_t2(0, 2)).x == 65535);
    CHECK((int16_t2(7, -7) % int16_t2(3, 0)).y == -1);
    // shift counts modulo 16; >> arithmetic on int16, logical on uint16
    CHECK((int16_t2(1, 1) << int16_t2(17, 3)).x == 2);
    CHECK((int16_t2(-16, 1) >> 2).x == -4);
    CHECK((uint16_t2(0x8000, 1) >> uint16_t2(15, 0)).x == 1);
    // float -> int16 saturates, NaN -> 0
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const int16_t3 s(float3(1e9f, -1e9f, nan));
    CHECK(s.x == 32767 && s.y == -32768 && s.z == 0);
    const uint16_t3 us(float3(-5.0f, 1e9f, 300.9f));
    CHECK(us.x == 0 && us.y == 65535 && us.z == 300);
    CHECK_EQF(float3(int16_t3(-3, 0, 32767)).z, 32767.0f);
    // comparisons, functions
    const bool3 lt = int16_t3(-1, 2, 3) < int16_t3(0, 2, 4);
    CHECK(lt.x && !lt.y && lt.z);
    CHECK(all(uint16_t2(40000, 1) > uint16_t2(1, 0)));
    CHECK(abs(int16_t2(-32768, -5)).x == -32768 && abs(int16_t2(-32768, -5)).y == 5);
    CHECK(clamp(int16_t3(-9, 5, 9), -1, 6).x == -1 && max(int16_t2(3, -3), int16_t2(1, 1)).y == 1);
    CHECK(sign(int16_t2(-7, 7)).x == -1);
    CHECK(dot(int16_t3(1, 2, 3), int16_t3(4, 5, 6)) == 32);
    // across widths (HLSL): int16 + int is an int, int + int16 assigns back implicitly
    const int3 wide = int16_t3(100, 200, 300) + int3(1, 2, 3);
    CHECK(wide.z == 303);
    const int16_t3 back = int3(70000, -70000, 5);
    CHECK(back.x == 4464 && back.y == -4464 && back.z == 5);  // modulo 2^16
    CHECK_EQF(sqrt(int16_t2(9, 16)).y, 4.0f);
    int16_t3 v(1, 2, 3);
    v.yz = int16_t2(-1, -2);
    v.x += 5;
    CHECK(v.x == 6 && v.z == -2);
}

static void test_int64() {
    using namespace shader_math::hlsl;
    constexpr std::int64_t mx = std::numeric_limits<std::int64_t>::max();
    constexpr std::int64_t mn = std::numeric_limits<std::int64_t>::min();
    const int64_t2 w = int64_t2(mx, 5) + int64_t2(1, 1);
    CHECK(w.x == mn && w.y == 6);
    CHECK((uint64_t2(0u, 5u) - uint64_t2(1u, 1u)).x == std::numeric_limits<std::uint64_t>::max());
    CHECK((int64_t2(1LL << 40, 3) * int64_t2(1LL << 30, 3)).x == 0);  // 2^70 wraps to 0
    const int64_t3 d = int64_t3(7, -7, mn) / int64_t3(0, 2, -1);
    CHECK(d.x == -1 && d.y == -3 && d.z == mn);
    CHECK((uint64_t2(7u, 7u) / uint64_t2(0u, 2u)).x == std::numeric_limits<std::uint64_t>::max());
    CHECK((int64_t2(mn, 9) % int64_t2(-1, 4)).x == 0);
    CHECK((int64_t2(1, 1) << int64_t2(65, 40)).x == 2 && (int64_t2(1, 1) << int64_t2(65, 40)).y == (1LL << 40));
    CHECK((int64_t2(-16, 1) >> 2).x == -4);
    CHECK((uint64_t2(1ULL << 63, 1u) >> uint64_t2(63u, 0u)).x == 1u);
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const int64_t3 s(float3(1e30f, -1e30f, nan));
    CHECK(s.x == mx && s.y == mn && s.z == 0);
    const uint64_t3 us(float3(-1.0f, 1e30f, 4294967296.0f));
    CHECK(us.x == 0u && us.y == std::numeric_limits<std::uint64_t>::max() && us.z == 4294967296u);
    CHECK_EQF(float3(int64_t3(1LL << 60, -3, 0)).x, 1152921504606846976.0f);
    const bool3 lt = int64_t3(-1, 2, mn) < int64_t3(0, 2, mx);
    CHECK(lt.x && !lt.y && lt.z);
    CHECK(all(uint64_t2(1ULL << 63, 1u) > uint64_t2(1u, 0u)));
    CHECK(abs(int64_t2(mn, -5)).x == mn && abs(int64_t2(mn, -5)).y == 5);
    CHECK(min(int64_t2(3, -3), int64_t2(1, 1)).y == -3 && sign(int64_t2(-7, 0)).x == -1);
    CHECK(dot(int64_t3(1LL << 32, 2, 3), int64_t3(4, 5, 6)) == (4LL << 32) + 28);
    // across widths (HLSL): uint + int64 is a uint64, int64 + float a float
    const uint64_t3 u = uint3(1u, 2u, 3u) + int64_t3(-1, 0, 1);
    CHECK(u.x == 0u && u.z == 4u);
    const float3 f = int64_t3(1, 2, 3) + float3(0.5f, 0.5f, 0.5f);
    CHECK_EQF(f.y, 2.5f);
    std::ostringstream os;
    os << int64_t2(mn, 7) << ' ' << int16_t2(-5, 6);
    CHECK(os.str() == "(-9223372036854775808, 7) (-5, 6)");
}

static void test_widths_glsl() {
    using namespace shader_math::glsl;
    const ivec3 a = i16vec3(int16_t(1), int16_t(2), int16_t(3)) + ivec3(10, 20, 30);
    CHECK(a.z == 33);
    const i64vec3 b = uvec3(0xFFFFFFFFu, 1u, 2u) + i64vec3(1, 1, 1);
    CHECK(b.x == 0x100000000LL);
    const ivec3 c = i16vec3(int16_t(-1), int16_t(0), int16_t(1));  // widens implicitly
    CHECK(c.x == -1);
    const vec3 f = u16vec3(uint16_t(1), uint16_t(2), uint16_t(65535));
    CHECK_EQF(f.z, 65535.0f);
    CHECK(i64vec2(5, 6) == i64vec2(5, 6));
    CHECK(all(lessThan(u16vec2(uint16_t(1), uint16_t(2)), u16vec2(uint16_t(2), uint16_t(2)))) == false);
}

static void test_width_bit_functions() {
    {
        using namespace shader_math::hlsl;
        // counted at the lane's width
        const uint3 c = countbits(int16_t3(int16_t(-1), int16_t(1), int16_t(-2)));
        CHECK(c.x == 16u && c.y == 1u && c.z == 15u);
        CHECK(countbits(int64_t(-1)) == 64u && countbits(0x100000000) == 1u);
        const uint3 hi = firstbithigh(int16_t3(int16_t(-1), int16_t(1), int16_t(-32768)));
        CHECK(hi.x == 0xFFFFFFFFu && hi.y == 0u && hi.z == 14u);
        const uint2 hl = firstbithigh(uint64_t2(std::uint64_t(1) << 40, 0u));
        CHECK(hl.x == 40u && hl.y == 0xFFFFFFFFu);
        CHECK(firstbitlow(int64_t(std::int64_t(1) << 50)) == 50u);
        const int16_t2 r = reversebits(int16_t2(int16_t(1), int16_t(0x00FF)));
        CHECK(r.x == -32768 && r.y == static_cast<std::int16_t>(0xFF00));
        CHECK(reversebits(std::uint64_t(1)) == std::uint64_t(1) << 63);
#if defined(SHADER_MATH_HAS_HALF)
        const int16_t3 hb = asint16(half3(half(1.0f), half(-2.0f), half(0.5f)));
        CHECK(hb.x == 0x3C00 && hb.y == static_cast<std::int16_t>(0xC000) && hb.z == 0x3800);
        CHECK(asuint16(half(1.0f)) == 0x3C00u);
        const half2 hf = asfloat16(uint16_t2(uint16_t(0x3C00), uint16_t(0xC000)));
        CHECK_EQF(static_cast<float>(hf[0]), 1.0f);
        CHECK_EQF(static_cast<float>(hf[1]), -2.0f);
#endif
    }
    {
        using namespace shader_math::glsl;
        const i16vec3 c = bitCount(i16vec3(int16_t(-1), int16_t(1), int16_t(-2)));
        CHECK(c.x == 16 && c.y == 1 && c.z == 15);
        const i64vec2 m = findMSB(i64vec2(-1, std::int64_t(1) << 40));
        CHECK(m.x == -1 && m.y == 40);
        CHECK(findLSB(std::uint64_t(0)) == -1);
        // a 16-bit argument converts to int: 0xFFFF is 65535, so its top
        // nibble sign-extends
        const ivec3 e = bitfieldExtract(u16vec3(uint16_t(0xFFFF), uint16_t(0x00F0), uint16_t(1)), 4, 4);
        CHECK(e.x == -1 && e.y == -1 && e.z == 0);
        CHECK(bitfieldReverse(uint16_t(1)) == static_cast<int>(0x80000000u));
        const ivec2 ins = bitfieldInsert(i16vec2(int16_t(0), int16_t(-1)), u16vec2(uint16_t(3), uint16_t(0)), 4, 4);
        CHECK(ins.x == 0x30 && ins.y == -241);
        CHECK(intBitsToFloat(int16_t(0)) == 0.0f && uintBitsToFloat(0x3F800000) == 1.0f);

        CHECK(packInt2x16(i16vec2(int16_t(-2), int16_t(3))) == 0x0003FFFE);
        CHECK(packUint2x16(i16vec2(int16_t(-1), int16_t(0))) == 0x0000FFFFu);
        const i16vec2 ui = unpackInt2x16(0x0003FFFE);
        CHECK(ui.x == -2 && ui.y == 3);
        CHECK(packUint4x16(u16vec4(uint16_t(1), uint16_t(2), uint16_t(3), uint16_t(0xFFFF))) ==
              0xFFFF000300020001ull);
        const i16vec4 u4 = unpackInt4x16(std::int64_t(-1));
        CHECK(u4.x == -1 && u4.w == -1);
        CHECK(packInt2x32(ivec2(-1, 0)) == 0x00000000FFFFFFFFLL);
        const uvec2 u2 = unpackUint2x32(std::uint64_t(0x1234567800000009ull));
        CHECK(u2.x == 9u && u2.y == 0x12345678u);
#if defined(SHADER_MATH_HAS_HALF)
        const f16vec2 h(glsl::float16_t(1.0f), glsl::float16_t(-2.0f));
        CHECK(packFloat2x16(h) == 0xC0003C00u);
        const f16vec2 uh = unpackFloat2x16(0xC0003C00u);
        CHECK_EQF(static_cast<float>(uh[1]), -2.0f);
        const i16vec2 hb = float16BitsToInt16(h);
        CHECK(hb.x == 0x3C00 && hb.y == static_cast<std::int16_t>(0xC000));
        const f16vec2 bh = uint16BitsToFloat16(i16vec2(int16_t(0x3C00), int16_t(0x3800)));
        CHECK_EQF(static_cast<float>(bh[1]), 0.5f);
#endif
    }
}

void test_int_widths() {
    test_int16();
    test_int64();
    test_widths_glsl();
    test_width_bit_functions();
}
