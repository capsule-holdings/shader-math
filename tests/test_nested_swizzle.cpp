// Opt-in swizzles of swizzles (SHADER_MATH_NESTED_SWIZZLE). Its own
// executable: the macro changes the vector class definition, so it must not be
// mixed with translation units built without it.
#if !defined(SHADER_MATH_NESTED_SWIZZLE)
#  error "build this test with SHADER_MATH_NESTED_SWIZZLE"
#endif

#include <cstdio>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

template <class V2>
concept can_write_nested = requires(shader_math::hlsl::float4 v, const V2& u) { v.xy.yx = u; };
template <class V>
concept can_copy_nested = requires(V a, V b) { a.xy.yx = b.zw.yx; };
template <class V>
concept can_nest_4wide = requires(V v) { v.xyzw.xy; };

int main() {
    using namespace shader_math::hlsl;

    static_assert(!can_write_nested<float2>);  // read-only
    static_assert(!can_copy_nested<float4>);
    static_assert(!can_nest_4wide<float4>);    // float4(v.wzyx).xy instead

    const float4 v(1.0f, 2.0f, 3.0f, 4.0f);
    const float2 a = v.zw.yx;
    CHECK_EQF(a[0], 4.0f);
    CHECK_EQF(a[1], 3.0f);
    const float3 b = v.wzy.xzy;  // (4,3,2) -> (4,2,3)
    CHECK_EQF(b[0], 4.0f);
    CHECK_EQF(b[1], 2.0f);
    CHECK_EQF(b[2], 3.0f);
    CHECK_EQF(float(v.yz.y), 3.0f);
    const float4 c = v.xy.xyyx;
    CHECK_EQF(c[1], 2.0f);
    CHECK_EQF(c[3], 1.0f);
    const float4 c3 = v.zyx.zzxy;  // (3,2,1) -> (1,1,3,2)
    CHECK_EQF(c3[0], 1.0f);
    CHECK_EQF(c3[2], 3.0f);
    CHECK_EQF(c3[3], 2.0f);
    const float3 d = v.bgr.rrg;  // rgba letters on both levels
    CHECK_EQF(d[0], 3.0f);
    CHECK_EQF(d[2], 2.0f);
    const float2 mixed = v.xyz.rb;  // letter sets are per level
    CHECK_EQF(mixed[1], 3.0f);
    CHECK_NEAR(dot(v.xy.yx, v.zw), 2.0f * 3.0f + 1.0f * 4.0f);
    const float2 e = v.yzw.zy * 2.0f;
    CHECK_EQF(e[0], 8.0f);
    CHECK_EQF(e[1], 6.0f);
    const float3 p(5.0f, 6.0f, 7.0f);  // 3-wide parent
    const float2 f = p.zx.yx;
    CHECK_EQF(f[0], 5.0f);
    CHECK_EQF(f[1], 7.0f);

    float4 w(1.0f, 2.0f, 3.0f, 4.0f);
    w.xy = w.zw.yx;  // nested read on the right of a swizzle write
    CHECK_EQF(w[0], 4.0f);
    CHECK_EQF(w[1], 3.0f);

    {
        using namespace shader_math::glsl;
        const vec4 g(1.0f, 2.0f, 3.0f, 4.0f);
        const vec2 st = g.stp.ts;
        CHECK_EQF(st[0], 2.0f);
        CHECK_EQF(st[1], 1.0f);
        CHECK_EQF(float(g.qp.s), 4.0f);
    }

#if defined(SHADER_MATH_HAS_HALF)
    {
        const half4 hv(half(1.0f), half(2.0f), half(3.0f), half(4.0f));
        const half2 hn = hv.wz.yx;
        CHECK_EQF(float(hn[0]), 3.0f);
        CHECK_EQF(float(hn[1]), 4.0f);
    }
#endif

    std::printf("[shader-math nested swizzle] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
