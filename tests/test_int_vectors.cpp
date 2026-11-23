// int / uint vectors (hlsl intN / uintN, glsl ivecN / uvecN): construction and
// conversion, swizzles, the integer operators with their defined results,
// comparisons, functions, bit casts, std.hpp, each language's implicit
// conversions and mixed operands, and the forms the languages reject
// (docs/design/int_vectors.md).
#include <climits>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <type_traits>
#include <unordered_set>

#if __has_include(<sys/types.h>)
#  include <sys/types.h>  // glibc declares ::uint as well
#endif

#include <shader_math/shader_math.hpp>
#include <shader_math/std.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;

static_assert(sizeof(h::int3) == 16 && sizeof(h::uint4) == 16);
static_assert(std::is_same_v<h::vector<int, 3>, h::int3>);
static_assert(std::is_same_v<h::vector<h::uint, 2>, h::uint2>);
static_assert(std::is_same_v<h::dword, unsigned int>);
static_assert(!std::is_same_v<h::int3, g::ivec3>);
static_assert(g::ivec3::length() == 3);

// ---- forms the languages reject (or that phase 1 leaves explicit) -----------
template <class A, class B>
concept Addable = requires(const A& a, const B& b) { a + b; };
template <class A, class B>
concept Shiftable = requires(const A& a, const B& b) { a << b; };
template <class A, class B>
concept Ordered = requires(const A& a, const B& b) { a < b; };
template <class A>
concept HasSqrt = requires(const A& a) { sqrt(a); };
template <class A>
concept HasNot = requires(const A& a) { !a; };
template <class A, class B>
concept HasDot = requires(const A& a, const B& b) { dot(a, b); };
// qualified: the MSVC UCRT's unconstrained ::isnan(_Ty) would accept anything
template <class A>
concept HasHlslIsnan = requires(const A& a) { h::isnan(a); };
template <class A>
concept HasHlslFrac = requires(const A& a) { h::frac(a); };
template <class A>
concept HasHlslSign = requires(const A& a) { h::sign(a); };
template <class A>
concept HasHlslLength = requires(const A& a) { h::length(a); };
template <class A>
concept HasOuterProduct = requires(const A& a) { g::outerProduct(a, a); };
template <class V, class S>
concept CompAssignable = requires(V& v, const S& s) { v.x = s; };
template <class A>
concept HasBitCount = requires(const A& a) { g::bitCount(a); };
template <class A>
concept HasCountbits = requires(const A& a) { h::countbits(a); };
template <class A, class O>
concept HasExtract = requires(const A& a, const O& o) { g::bitfieldExtract(a, o, 2); };
template <class A, class B>
concept HasAddCarry = requires(const A& a, const B& b, A& c) { g::uaddCarry(a, b, c); };
template <class A, class B>
concept AddAssignable = requires(A& a, const B& b) { a += b; };
template <class A, class B>
using sum_t = decltype(std::declval<const A&>() + std::declval<const B&>());

namespace hlsl_scope {
using namespace shader_math::hlsl;
static_assert(Addable<int3, int3> && Addable<int3, int> && Addable<uint3, unsigned>);
// mixed operands: float when either side is float, else uint
static_assert(std::is_same_v<sum_t<int3, float3>, float3> && std::is_same_v<sum_t<float3, uint3>, float3>);
static_assert(std::is_same_v<sum_t<int3, uint3>, uint3> && std::is_same_v<sum_t<int3, double>, float3>);
static_assert(std::is_same_v<sum_t<float, int3>, float3>);
static_assert(AddAssignable<int3, float3> && AddAssignable<int3, uint3> && AddAssignable<float3, int3>);
static_assert(!Addable<int3, bool3> && !Addable<int3, bool>);
#if defined(SHADER_MATH_HAS_HALF)
static_assert(!Addable<int3, half3> && !std::is_convertible_v<half3, int3>);
#endif
static_assert(!Addable<int3, int4>);
static_assert(Shiftable<int3, uint3> && Shiftable<uint3, int> && !Shiftable<int3, float>);
static_assert(!Shiftable<float3, int3>);
static_assert(Ordered<int3, int3>);
static_assert(HasSqrt<int3>);  // converted to float, as HLSL does
static_assert(HasNot<int3> && HasDot<int3, int3>);
// implicit conversions both ways between int, uint and float (not half, bool or glsl)
static_assert(std::is_convertible_v<float3, int3> && std::is_convertible_v<int3, float3>);
static_assert(std::is_convertible_v<uint3, int3> && std::is_convertible_v<int3, uint3>);
static_assert(!std::is_convertible_v<g::ivec3, int3>);
static_assert(std::is_convertible_v<int, int3>);  // int3 v = 1;
static_assert(!std::is_convertible_v<bool3, int3> && !std::is_convertible_v<int3, bool3>);
static_assert(!HasHlslIsnan<int3> && HasHlslIsnan<float3>);
static_assert(HasHlslFrac<int3> && HasHlslLength<int3>);
static_assert(!HasHlslSign<uint3> && HasHlslSign<int3>);
static_assert(Shiftable<int, int3> && !Shiftable<float, int3>);  // k << v
static_assert(CompAssignable<int3, float> && CompAssignable<int3, bool>);
}  // namespace hlsl_scope

namespace glsl_scope {
using namespace shader_math::glsl;
static_assert(Addable<ivec3, ivec3>);
static_assert(std::is_same_v<sum_t<ivec3, uvec3>, uvec3> && std::is_same_v<sum_t<ivec3, vec3>, vec3>);
static_assert(std::is_same_v<sum_t<uvec3, float>, vec3>);
// GLSL converts implicitly only int -> uint and int / uint -> float
static_assert(std::is_convertible_v<ivec3, uvec3> && std::is_convertible_v<ivec3, vec3> &&
              std::is_convertible_v<uvec3, vec3>);
static_assert(!std::is_convertible_v<uvec3, ivec3> && !std::is_convertible_v<vec3, ivec3> &&
              !std::is_convertible_v<vec3, uvec3>);
static_assert(AddAssignable<vec3, ivec3> && AddAssignable<uvec3, ivec3>);
static_assert(!AddAssignable<ivec3, vec3> && !AddAssignable<ivec3, uvec3> && !AddAssignable<ivec3, float>);
static_assert(!HasSqrt<ivec3>);  // GLSL: ambiguous (float and double), an error
static_assert(!Ordered<ivec3, ivec3>);  // GLSL: lessThan(), not <
static_assert(!HasNot<ivec3>);           // GLSL's ! takes a bool only
static_assert(!HasDot<ivec3, ivec3>);    // GLSL's dot takes floats only
static_assert(!std::is_convertible_v<int, ivec3>);  // ivec3(1) only
static_assert(!HasOuterProduct<ivec3> && HasOuterProduct<vec3>);
static_assert(!Shiftable<int, ivec3>);  // GLSL: a vector on the left only
static_assert(CompAssignable<ivec3, int> && CompAssignable<ivec3, unsigned>);
static_assert(!CompAssignable<ivec3, float> && !CompAssignable<ivec3, bool>);
static_assert(CompAssignable<vec3, int>);  // int -> float converts in GLSL
}  // namespace glsl_scope

namespace bits_scope {
using namespace shader_math::glsl;
static_assert(HasBitCount<ivec3> && HasBitCount<uvec2> && HasBitCount<int>);
static_assert(!HasBitCount<vec3> && !HasBitCount<float>);       // GLSL: integers only
static_assert(!HasCountbits<ivec3>);                             // an HLSL name
static_assert(HasExtract<ivec3, int> && !HasExtract<ivec3, unsigned>);  // offset is int
static_assert(HasAddCarry<uvec3, uvec3> && !HasAddCarry<ivec3, ivec3> && !HasAddCarry<float, float>);
static_assert(HasAddCarry<unsigned, unsigned>);
}  // namespace bits_scope
static_assert(HasCountbits<h::int3> && HasCountbits<h::float3> && !HasBitCount<h::int3>);

static void test_int_bits() {
    {
        using namespace shader_math::hlsl;
        const uint3 c = countbits(uint3(0u, 0xFFu, 0xFFFFFFFFu));
        CHECK(c.x == 0u && c.y == 8u && c.z == 32u);
        CHECK(countbits(int2(-1, 5)).x == 32u && countbits(7u) == 3u);
        CHECK(countbits(float2(3.9f, 255.0f)).y == 8u);  // float converts to uint
        const uint3 hu = firstbithigh(uint3(0u, 1u, 0x80000000u));
        CHECK(hu.x == 0xFFFFFFFFu && hu.y == 0u && hu.z == 31u);
        // int: the highest bit that differs from the sign bit
        const uint3 hs = firstbithigh(int3(-1, -2, 0x7FFFFFFF));
        CHECK(hs.x == 0xFFFFFFFFu && hs.y == 0u && hs.z == 30u);
        CHECK(firstbithigh(-8) == 2u);
        const uint3 lo = firstbitlow(uint3(0u, 8u, 0x80000000u));
        CHECK(lo.x == 0xFFFFFFFFu && lo.y == 3u && lo.z == 31u);
        CHECK(reversebits(1u) == 0x80000000u);
        CHECK(reversebits(int2(1, 0x00F0000F)).y == 0xF0000F00u);
        const int3 v(12, -1, 0);
        CHECK(countbits(v.x) == 2u);  // one component
    }
    {
        using namespace shader_math::glsl;
        const ivec2 bc = bitCount(ivec2(-1, 5));
        CHECK(bc.x == 32 && bc.y == 2);
        const ivec3 m = findMSB(ivec3(-1, -2, 5));
        CHECK(m.x == -1 && m.y == 0 && m.z == 2);
        CHECK(findMSB(uvec2(0u, 0x80000000u)).y == 31 && findMSB(0u) == -1);
        CHECK(findLSB(ivec2(0, 12)).x == -1 && findLSB(12) == 2);
        const uvec2 rev = bitfieldReverse(uvec2(1u, 0x0000FFFFu));
        CHECK(rev.x == 0x80000000u && rev.y == 0xFFFF0000u);
        CHECK(bitfieldReverse(1) == INT_MIN);
        // extract: sign-extended on int, zero-extended on uint
        const ivec2 ex = bitfieldExtract(ivec2(0x000000F0, 0x00007000), 4, 4);
        CHECK(ex.x == -1 && ex.y == 0);
        CHECK(bitfieldExtract(uvec2(0xF0u, 0u), 4, 4).x == 15u);
        CHECK(bitfieldExtract(INT_MIN, 31, 1) == -1 && bitfieldExtract(5, 0, 0) == 0);
        // a field past bit 31 ends there; negative values count as 0
        CHECK(bitfieldExtract(0xC0000000u, 30, 8) == 3u);
        CHECK(bitfieldExtract(0xFFu, -4, 4) == 15u);
        const uvec2 ins = bitfieldInsert(uvec2(0xFFFFFFFFu, 0u), uvec2(0u, 0xFFu), 8, 4);
        CHECK(ins.x == 0xFFFFF0FFu && ins.y == 0x00000F00u);
        CHECK(bitfieldInsert(0, -1, 28, 8) == static_cast<int>(0xF0000000u));
        CHECK(bitfieldInsert(7u, 0u, 0, 0) == 7u);
        // carries, borrows and 64-bit products
        uvec2 carry;
        const uvec2 s = uaddCarry(uvec2(0xFFFFFFFFu, 1u), uvec2(1u, 2u), carry);
        CHECK(s.x == 0u && s.y == 3u && carry.x == 1u && carry.y == 0u);
        uvec2 borrow;
        const uvec2 d = usubBorrow(uvec2(0u, 5u), uvec2(1u, 2u), borrow);
        CHECK(d.x == 0xFFFFFFFFu && d.y == 3u && borrow.x == 1u && borrow.y == 0u);
        uvec2 hi, low;
        umulExtended(uvec2(0xFFFFFFFFu, 3u), uvec2(0xFFFFFFFFu, 5u), hi, low);
        CHECK(hi.x == 0xFFFFFFFEu && low.x == 1u && hi.y == 0u && low.y == 15u);
        ivec2 shi, slo;
        imulExtended(ivec2(-1, INT_MIN), ivec2(2, -1), shi, slo);
        CHECK(shi.x == -1 && slo.x == -2 && shi.y == 0 && slo.y == INT_MIN);
        uint c1 = 0u;
        CHECK(uaddCarry(0xFFFFFFFFu, 1u, c1) == 0u && c1 == 1u);
        uint b1 = 0u;
        CHECK(usubBorrow(1u, 2u, b1) == 0xFFFFFFFFu && b1 == 1u);
        uint mh = 0u, ml = 0u;
        umulExtended(0x10000u, 0x10000u, mh, ml);
        CHECK(mh == 1u && ml == 0u);
        int ih = 0, il = 0;
        imulExtended(-3, 5, ih, il);
        CHECK(ih == -1 && il == -15);
    }
}

template <class A>
concept HasPackHalf = requires(const A& a) { g::packHalf2x16(a); };
template <class A>
concept HasF32ToF16 = requires(const A& a) { h::f32tof16(a); };
static_assert(HasPackHalf<g::vec2> && !HasPackHalf<g::vec3> && !HasPackHalf<g::ivec2> &&
              !HasPackHalf<h::float2>);
static_assert(HasF32ToF16<h::float3> && HasF32ToF16<float> && !HasF32ToF16<g::vec3>);

static void test_int_pack() {
    const float nan = std::numeric_limits<float>::quiet_NaN();
    {
        using namespace shader_math::glsl;
        CHECK(packHalf2x16(vec2(1.0f, -2.0f)) == 0xC0003C00u);
        const vec2 h = unpackHalf2x16(0xC0003C00u);
        CHECK_EQF(h.x, 1.0f);
        CHECK_EQF(h.y, -2.0f);
        // nearest even; overflow to infinity; NaN stays NaN
        CHECK((packHalf2x16(vec2(1.0f + 1.0f / 2048.0f, 1.0f + 3.0f / 2048.0f)) & 0xFFFFu) == 0x3C00u);
        CHECK((packHalf2x16(vec2(1.0f + 1.0f / 2048.0f, 1.0f + 3.0f / 2048.0f)) >> 16) == 0x3C02u);
        CHECK((packHalf2x16(vec2(70000.0f, nan)) & 0xFFFFu) == 0x7C00u);
        CHECK(((packHalf2x16(vec2(70000.0f, nan)) >> 16) & 0x7C00u) == 0x7C00u);
        CHECK(((packHalf2x16(vec2(70000.0f, nan)) >> 16) & 0x03FFu) != 0u);
        CHECK_EQF(unpackHalf2x16(1u).x, 5.9604645e-8f);  // the smallest subnormal
        // unorm / snorm: clamp, round to nearest even, the first component low
        CHECK(packUnorm2x16(vec2(0.5f, 1.5f)) == 0xFFFF8000u);
        CHECK(packSnorm2x16(vec2(-1.5f, 0.25f)) == 0x20008001u);
        CHECK(packUnorm4x8(vec4(0.0f, 1.0f, 0.5f, -1.0f)) == 0x0080FF00u);
        CHECK(packSnorm4x8(vec4(1.0f, -1.0f, 0.5f, nan)) == 0x0040817Fu);
        const vec4 u8 = unpackUnorm4x8(0x0080FF00u);
        CHECK_EQF(u8.y, 1.0f);
        CHECK_EQF(u8.z, 128.0f / 255.0f);
        CHECK_EQF(unpackSnorm4x8(0x80u).x, -1.0f);  // -128 clamps to -1
        CHECK_EQF(unpackSnorm2x16(0x8000u).x, -1.0f);
        CHECK_EQF(unpackSnorm2x16(0x7FFFu).x, 1.0f);
        const vec2 u16 = unpackUnorm2x16(0xFFFF0000u);
        CHECK_EQF(u16.x, 0.0f);
        CHECK_EQF(u16.y, 1.0f);
        // round trips
        CHECK(packUnorm4x8(unpackUnorm4x8(0x12345678u)) == 0x12345678u);
        CHECK(packSnorm2x16(unpackSnorm2x16(0x7FFF8001u)) == 0x7FFF8001u);
    }
    {
        using namespace shader_math::hlsl;
        CHECK(f32tof16(1.0f) == 0x3C00u);
        const uint2 b = f32tof16(float2(65504.0f, -0.0f));
        CHECK(b.x == 0x7BFFu && b.y == 0x8000u);
        CHECK_EQF(f16tof32(0x3C00u), 1.0f);
        const float2 f = f16tof32(uint2(0x12343C00u, 0x7C00u));  // the low 16 bits only
        CHECK_EQF(f.x, 1.0f);
        CHECK(std::isinf(f.y));
        CHECK_EQF(f16tof32(f32tof16(float3(0.1f, 3.0f, -7.25f))).z, -7.25f);
        const float3 v(2.0f, 0.5f, 1.0f);
        CHECK(f32tof16(v.y) == 0x3800u);  // one component
    }
}

static void test_int_mixed() {
    {
        using namespace shader_math::hlsl;
        const int3 i(3, -4, 7);
        const uint3 u(1u, 2u, 0xFFFFFFFFu);
        const float3 f(0.5f, 0.25f, 2.0f);
        const float3 fi = i + f;
        CHECK_EQF(fi.x, 3.5f);
        CHECK_EQF(fi.y, -3.75f);
        const uint3 ui = i + u;  // i converts to uint
        CHECK(ui.x == 4u && ui.y == 0xFFFFFFFEu && ui.z == 6u);
        const float3 scaled = i * 2.5;
        CHECK_EQF(scaled.y, -10.0f);
        CHECK_EQF((1.5f - i).z, -5.5f);
        int3 t = f * 7.0f;  // implicit float3 -> int3 truncates
        CHECK(t.x == 3 && t.y == 1 && t.z == 14);
        t += f;             // computes in float, truncates back
        CHECK(t.x == 3 && t.z == 16);
        t *= 0.5;
        CHECK(t.x == 1 && t.z == 8);
        float3 g = i;  // implicit int3 -> float3
        CHECK_EQF(g.y, -4.0f);
        g = u;
        CHECK_EQF(g.z, 4294967296.0f);
        const uint3 iu = i;
        CHECK(iu.y == 0xFFFFFFFCu);
        // comparisons in the common type
        const bool3 lt = i < f;
        CHECK(!lt.x && lt.y && !lt.z);
        CHECK(all((int3(1, 2, 3) == float3(1.0f, 2.0f, 3.0f))));
        CHECK(any(i > 6.5));
        // functions convert int vectors to float
        const float3 r = sqrt(int3(4, 9, 16));
        CHECK_EQF(r.x, 2.0f);
        CHECK_EQF(r.z, 4.0f);
        CHECK_EQF(length(int2(3, 4)), 5.0f);
        CHECK_EQF(lerp(int2(0, 10), float2(10.0f, 20.0f), 0.5f).y, 15.0f);
        CHECK_EQF(frac(int2(3, -2)).x, 0.0f);
        CHECK_EQF((int2(7, -7) % float2(3.0f, 3.0f)).y, -1.0f);  // fmod
        CHECK(abs(int2(-3, 3)).x == 3);  // abs keeps int
        const int3 sg = sign(float3(-2.0f, 0.0f, 5.0f));  // float3 -> int3
        CHECK(sg.x == -1 && sg.y == 0 && sg.z == 1);
        CHECK(((int2(6, 7) & uint2(3u, 3u)).y) == 3u);
    }
    {
        using namespace shader_math::glsl;
        const ivec3 i(3, -4, 7);
        const vec3 v = i * 0.5;
        CHECK_EQF(v.y, -2.0f);
        const uvec3 w = i + uvec3(1u, 5u, 0u);
        CHECK(w.y == 1u);
        vec3 acc(0.5f);
        acc += i;
        CHECK_EQF(acc.z, 7.5f);
        const vec3 fromi = i;
        CHECK_EQF(fromi.x, 3.0f);
        const uvec3 fromi_u = i;
        CHECK(fromi_u.y == 0xFFFFFFFCu);
    }
}

static void test_int_construction() {
    using namespace shader_math::hlsl;
    const int3 a(1, -2, 3);
    CHECK(a.x == 1 && a.y == -2 && a.z == 3);
    const int3 s = 5;
    CHECK(s.x == 5 && s.z == 5);
    const uint3 u(1u, 2u, 0xFFFFFFFFu);
    CHECK(u.z == 0xFFFFFFFFu);
    // component lists mix element types and truncate floats toward zero
    const int4 m(int2(7, 8), 2.9f, -2.9f);
    CHECK(m.x == 7 && m.y == 8 && m.z == 2 && m.w == -2);
    const int3 c(1.9f, -1.9, u.x);
    CHECK(c.x == 1 && c.y == -1 && c.z == 1);
    // float -> int: truncation, NaN -> 0, saturation
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const int4 fi(float4(2.75f, -2.75f, 1e20f, nan));
    CHECK(fi.x == 2 && fi.y == -2 && fi.z == INT_MAX && fi.w == 0);
    const uint4 fu(float4(-1.0f, 3e9f, 1e20f, nan));
    CHECK(fu.x == 0u && fu.y == 3000000000u && fu.z == 0xFFFFFFFFu && fu.w == 0u);
    CHECK(int3(-1e20f).x == INT_MIN);
    // int / uint -> float rounds to nearest
    const float3 f(int3(-3, 16777217, 0));
    CHECK_EQF(f.x, -3.0f);
    CHECK_EQF(f.y, 16777216.0f);
    CHECK_EQF(float2(uint2(0xFFFFFFFFu, 3000000000u)).x, 4294967296.0f);
    CHECK_EQF(float2(uint2(0xFFFFFFFFu, 3000000000u)).y, 3000000000.0f);
    // int <-> uint keeps the bits
    CHECK(uint2(int2(-1, 5)).x == 0xFFFFFFFFu);
    CHECK(int2(uint2(0x80000000u, 5u)).x == INT_MIN);
    // bool <-> int
    const int3 bi(bool3(true, false, true));
    CHECK(bi.x == 1 && bi.y == 0 && bi.z == 1);
    const bool3 ib(int3(0, -4, 1));
    CHECK(!ib.x && ib.y && ib.z);
    // other language: explicit
    const shader_math::glsl::ivec3 gi(a);
    CHECK(gi.y == -2);
#if defined(SHADER_MATH_HAS_HALF)
    const int2 hi(half2(half(2.5f), half(-7.0f)));
    CHECK(hi.x == 2 && hi.y == -7);
#endif
}

static void test_int_swizzles() {
    using namespace shader_math::glsl;
    ivec4 v(1, 2, 3, 4);
    const ivec3 r = v.wzx;
    CHECK(r.x == 4 && r.y == 3 && r.z == 1);
    v.xy = ivec2(10, 20);
    CHECK(v.x == 10 && v.y == 20 && v.z == 3);
    v.zw += ivec2(1, 1);
    CHECK(v.z == 4 && v.w == 5);
    v.yz++;
    CHECK(v.y == 21 && v.z == 5);
    CHECK(v.s == 10 && v.q == 5);
    uvec2 u(1u, 2u);
    u.yx = uvec2(7u, 8u);
    CHECK(u.x == 8u && u.y == 7u);
}

static void test_int_operators() {
    using namespace shader_math::hlsl;
    const int3 a(7, -7, INT_MAX);
    const int3 b(2, 2, 1);
    const int3 sum = a + b;
    CHECK(sum.x == 9 && sum.z == INT_MIN);  // wraps
    const int3 q = a / b;
    CHECK(q.x == 3 && q.y == -3);  // truncates toward zero
    const int3 r = a % b;
    CHECK(r.x == 1 && r.y == -1);  // sign of the dividend
    const int3 z = a / int3(0, -1, 0);
    CHECK(z.x == -1 && z.y == 7 && z.z == -1);
    CHECK((int2(INT_MIN, 4) / int2(-1, 2)).x == INT_MIN);
    CHECK((int2(INT_MIN, 4) % int2(-1, 0)).x == 0);
    CHECK((int2(INT_MIN, 4) % int2(-1, 0)).y == -1);
    const uint2 u(5u, 0u);
    CHECK((u - 6u).x == 0xFFFFFFFFu);
    CHECK((u / uint2(0u, 0u)).x == 0xFFFFFFFFu);
    CHECK((u % uint2(0u, 3u)).x == 0xFFFFFFFFu);
    CHECK((-u).x == 0xFFFFFFFBu);
    CHECK((int2(3, 4) * int2(INT_MAX, -1)).x == INT_MAX - 2);  // 3 * INT_MAX wraps

    // bitwise
    const uint3 m(0xF0F0F0F0u, 0x0000FFFFu, 0u);
    CHECK((m & 0xFF00FF00u).x == 0xF000F000u);
    CHECK((m | uint3(1u, 1u, 1u)).y == 0x0000FFFFu);
    CHECK((m ^ m).x == 0u);
    CHECK((~m).z == 0xFFFFFFFFu);
    CHECK((~int2(0, -1)).x == -1);
    // shifts: count modulo 32, arithmetic >> on int, logical on uint
    const int3 s(-16, 1, 1);
    CHECK((s >> 2).x == -4);
    CHECK((uint3(0x80000000u, 1u, 1u) >> 31u).x == 1u);
    CHECK((s << uint3(1u, 31u, 33u)).y == INT_MIN);
    CHECK((s << uint3(1u, 31u, 33u)).z == 2);
    CHECK((s << -1).y == INT_MIN);  // -1 & 31 == 31
    CHECK((uint2(1u, 1u) << int2(4, 32)).x == 16u && (uint2(1u, 1u) << int2(4, 32)).y == 1u);
    // compound forms
    int3 c(6, 7, 8);
    c %= 4;
    CHECK(c.x == 2 && c.y == 3 && c.z == 0);
    c <<= int3(1, 2, 3);
    CHECK(c.x == 4 && c.y == 12 && c.z == 0);
    c |= 1;
    c &= int3(5, 5, 5);
    c ^= int3(0, 1, 0);
    CHECK(c.x == 5 && c.y == 4 && c.z == 1);
    c >>= 1;
    CHECK(c.x == 2);
    c *= 3;
    c -= int3(1, 1, 1);
    CHECK(c.x == 5);
    ++c;
    c--;
    CHECK(c.x == 5);
    // scalar << vector (HLSL)
    const int3 sv = 1 << int3(0, 4, 33);
    CHECK(sv.x == 1 && sv.y == 16 && sv.z == 2);
    CHECK((-64 >> int2(2, 3)).y == -8);
    // one component from a float truncates (HLSL)
    int3 w(0, 0, 0);
    w.x = 2.9f;
    w.y = -1e20f;
    w.z += 2.5f;
    CHECK(w.x == 2 && w.y == INT_MIN && w.z == 2);
    // ! (HLSL): true where zero
    const bool3 nz = !int3(0, 3, 0);
    CHECK(nz.x && !nz.y && nz.z);
}

static void test_int_comparisons() {
    {
        using namespace shader_math::hlsl;
        const bool3 lt = int3(-1, 2, 3) < int3(0, 2, 4);
        CHECK(lt.x && !lt.y && lt.z);
        // unsigned order: 0x80000000 is large
        const bool2 ug = uint2(0x80000000u, 1u) > 1u;
        CHECK(ug.x && !ug.y);
        CHECK(all(int3(1, -1, 2)) && !all(int3(1, 0, 2)) && any(uint2(0u, 4u)));
        CHECK(all(int3(5, 5, 5) == 5));
    }
    {
        using namespace shader_math::glsl;
        CHECK(ivec3(1, 2, 3) == ivec3(1, 2, 3));
        CHECK(ivec3(1, 2, 3) != ivec3(1, 2, 4));
        const bvec3 lt = lessThan(uvec3(1u, 0xFFFFFFFFu, 5u), uvec3(2u, 1u, 5u));
        CHECK(lt.x && !lt.y && !lt.z);
        CHECK(all(greaterThanEqual(ivec2(3, -3), ivec2(3, -4))));
    }
}

static void test_int_functions() {
    {
        using namespace shader_math::hlsl;
        const int3 v(-5, 0, INT_MIN);
        const int3 av = abs(v);
        CHECK(av.x == 5 && av.y == 0 && av.z == INT_MIN);  // abs(INT_MIN) wraps
        CHECK(abs(uint2(7u, 0xFFFFFFFFu)).y == 0xFFFFFFFFu);
        const int3 sg = sign(v);
        CHECK(sg.x == -1 && sg.y == 0 && sg.z == -1);
        CHECK(min(int2(3, -3), int2(1, 1)).y == -3);
        CHECK(max(uint2(0x80000000u, 1u), 2u).x == 0x80000000u);
        const int3 cl = clamp(int3(-9, 5, 9), -1, 6);
        CHECK(cl.x == -1 && cl.y == 5 && cl.z == 6);
        CHECK(clamp(int3(-9, 5, 9), int3(0, 0, 0), int3(1, 1, 1)).z == 1);
        const int3 sel = select(bool3(true, false, true), int3(1, 2, 3), int3(4, 5, 6));
        CHECK(sel.x == 1 && sel.y == 5 && sel.z == 3);
        CHECK(dot(int3(1, 2, 3), int3(4, 5, 6)) == 32);
        CHECK(dot(uint2(0x80000000u, 1u), uint2(2u, 1u)) == 1u);  // wraps
        CHECK(mul(int2(2, 3), int2(4, 5)) == 23);
        CHECK(mul(2, int2(4, 5)).y == 10);
        // scalar calls on components
        const int3 w(4, -9, 2);
        CHECK(max(w.x, 6) == 6);
        CHECK(abs(w.y) == 9);
        CHECK(clamp(w.z, 0, 1) == 1);
        static_assert(std::is_same_v<decltype(max(w.x, 6)), int>);
        // bit casts
        const int3 bits = asint(float3(1.0f, -2.0f, 0.0f));
        CHECK(bits.x == 0x3F800000 && bits.y == static_cast<int>(0xC0000000u) && bits.z == 0);
        CHECK(asuint(-1.0f) == 0xBF800000u);
        CHECK(asuint(int2(-1, 0)).x == 0xFFFFFFFFu);
        CHECK_EQF(asfloat(uint2(0x40400000u, 0u)).x, 3.0f);
        CHECK_EQF(asfloat(0x3F000000), 0.5f);
        CHECK(asint(w.x) == 4);
        const float3 rt = asfloat(asint(float3(0.1f, -7.5f, 1e30f)));
        CHECK_EQF(rt.z, 1e30f);
    }
    {
        using namespace shader_math::glsl;
        const ivec3 v(-5, 0, 7);
        CHECK(abs(v).x == 5 && sign(v).z == 1);
        const ivec3 mx = mix(ivec3(1, 2, 3), ivec3(4, 5, 6), bvec3(false, true, false));
        CHECK(mx.x == 1 && mx.y == 5 && mx.z == 3);
        CHECK(floatBitsToInt(vec2(1.0f, 0.0f)).x == 0x3F800000);
        CHECK(floatBitsToUint(-0.0f) == 0x80000000u);
        CHECK_EQF(intBitsToFloat(ivec2(0x3F800000, 0)).x, 1.0f);
        CHECK_EQF(uintBitsToFloat(0x40000000u), 2.0f);
        // frexp / ldexp with int exponents
        ivec3 e;
        const vec3 mant = frexp(vec3(8.0f, -0.75f, 0.0f), e);
        CHECK_EQF(mant.x, 0.5f);
        CHECK(e.x == 4 && e.y == 0 && e.z == 0);
        CHECK_EQF(mant.y, -0.75f);
        const vec3 back = ldexp(mant, e);
        CHECK_EQF(back.x, 8.0f);
        CHECK_EQF(back.y, -0.75f);
        int es = 0;
        CHECK_EQF(frexp(12.0f, es), 0.75f);
        CHECK(es == 4);
        CHECK_EQF(ldexp(0.75f, 4), 12.0f);
    }
}

static void test_int_std() {
    using namespace shader_math::hlsl;
    std::ostringstream os;
    os << int3(1, -2, INT_MAX) << ' ' << uint2(0xFFFFFFFFu, 0u);
    CHECK(os.str() == "(1, -2, 2147483647) (4294967295, 0)");
    // large values stay distinct (no rounding through float)
    std::unordered_set<int2> set{int2(16777216, 0), int2(16777217, 0)};
    CHECK(set.size() == 2);
#if defined(__cpp_lib_format)
    CHECK(std::format("{:x}", uint2(255u, 16u)) == "(ff, 10)");
#endif
    // the hlsl / glsl uint aliases coexist with a global ::uint
    uint x = 3u;
    CHECK(x == 3u);
}

void test_int_vectors() {
    test_int_construction();
    test_int_mixed();
    test_int_pack();
    test_int_bits();
    test_int_swizzles();
    test_int_operators();
    test_int_comparisons();
    test_int_functions();
    test_int_std();
}
