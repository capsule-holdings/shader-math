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
// the bit functions and casts stay 32-bit
template <class A>
concept HasBitCount = requires(const A& a) { g::bitCount(a); };
template <class A>
concept HasAsfloat = requires(const A& a) { h::asfloat(a); };
static_assert(HasBitCount<g::ivec3> && !HasBitCount<g::i16vec3> && !HasBitCount<g::i64vec3>);
static_assert(HasAsfloat<h::int3> && !HasAsfloat<h::int64_t3>);

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

void test_int_widths() {
    test_int16();
    test_int64();
    test_widths_glsl();
}
