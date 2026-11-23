// int / uint vectors (hlsl intN / uintN, glsl ivecN / uvecN): construction and
// conversion, swizzles, the integer operators with their defined results,
// comparisons, functions, bit casts, std.hpp, and the forms the languages
// reject (docs/design/int_vectors.md, phase 1: no implicit mixing).
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

namespace hlsl_scope {
using namespace shader_math::hlsl;
static_assert(Addable<int3, int3> && Addable<int3, int> && Addable<uint3, unsigned>);
static_assert(!Addable<int3, float>);      // int3 * 2.5f: phase 2 (promotion)
static_assert(!Addable<int3, uint3>);      // int3 + uint3: phase 2
static_assert(!Addable<float3, int3>);     // float3 + int3: phase 2
static_assert(!Addable<int3, bool>);
static_assert(!Addable<int3, int4>);
static_assert(Shiftable<int3, uint3> && Shiftable<uint3, int> && !Shiftable<int3, float>);
static_assert(!Shiftable<float3, int3>);
static_assert(Ordered<int3, int3>);
static_assert(!HasSqrt<int3>);  // HLSL converts to float: phase 2
static_assert(HasNot<int3> && HasDot<int3, int3>);
static_assert(!std::is_convertible_v<float3, int3> && !std::is_convertible_v<int3, float3>);
static_assert(std::is_convertible_v<int, int3>);  // int3 v = 1;
static_assert(!std::is_convertible_v<bool3, int3> && !std::is_convertible_v<int3, bool3>);
static_assert(!HasHlslIsnan<int3> && HasHlslIsnan<float3>);
static_assert(!HasHlslFrac<int3> && !HasHlslLength<int3>);
static_assert(!HasHlslSign<uint3> && HasHlslSign<int3>);
static_assert(Shiftable<int, int3> && !Shiftable<float, int3>);  // k << v
static_assert(CompAssignable<int3, float> && CompAssignable<int3, bool>);
}  // namespace hlsl_scope

namespace glsl_scope {
using namespace shader_math::glsl;
static_assert(Addable<ivec3, ivec3> && !Addable<ivec3, uvec3> && !Addable<ivec3, float>);
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
    test_int_swizzles();
    test_int_operators();
    test_int_comparisons();
    test_int_functions();
    test_int_std();
}
