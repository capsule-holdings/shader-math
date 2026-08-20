#include <shader_math/shader_math.hpp>

#include "framework.hpp"

void test_vector() {
    using namespace shader_math::hlsl;  // vectors are shared; hlsl surface used here

    // ---- constructors ------------------------------------------------------
    {
        float3 v(1.0f, 2.0f, 3.0f);
        CHECK_EQF(v[0], 1.0f);
        CHECK_EQF(v[1], 2.0f);
        CHECK_EQF(v[2], 3.0f);

        float3 s(7.0f);  // splat in BOTH languages
        CHECK_EQF(s[0], 7.0f);
        CHECK_EQF(s[1], 7.0f);
        CHECK_EQF(s[2], 7.0f);

        float2 d;  // default zero
        CHECK_EQF(d[0], 0.0f);
        CHECK_EQF(d[1], 0.0f);
    }
    // composite constructors
    {
        float2 xy(1.0f, 2.0f);
        float3 a(xy, 3.0f);
        CHECK_EQF(a[2], 3.0f);
        float3 b(0.0f, xy);
        CHECK_EQF(b[1], 1.0f);
        float4 c(xy, xy);
        CHECK_EQF(c[3], 2.0f);
        float4 e(a, 4.0f);
        CHECK_EQF(e[3], 4.0f);
        float4 f(9.0f, a);
        CHECK_EQF(f[0], 9.0f);
        CHECK_EQF(f[3], 3.0f);
        float2 t(e);  // explicit truncation
        CHECK_EQF(t[0], 1.0f);
        CHECK_EQF(t[1], 2.0f);
    }

    // ---- component-wise arithmetic (identical in HLSL and GLSL) -------------
    {
        float4 a(1.0f, 2.0f, 3.0f, 4.0f);
        float4 b(10.0f, 20.0f, 30.0f, 40.0f);
        float4 m = a * b;  // component-wise, NOT dot
        CHECK_EQF(m[0], 10.0f);
        CHECK_EQF(m[1], 40.0f);
        CHECK_EQF(m[2], 90.0f);
        CHECK_EQF(m[3], 160.0f);

        float4 s = a + b;
        CHECK_EQF(s[2], 33.0f);
        float4 d = b - a;
        CHECK_EQF(d[3], 36.0f);
        float4 q = b / a;
        CHECK_EQF(q[1], 10.0f);
        float4 n = -a;
        CHECK_EQF(n[0], -1.0f);

        float4 sc = a * 2.0f;
        CHECK_EQF(sc[3], 8.0f);
        float4 sc2 = 2.0f * a;
        CHECK_EQF(sc2[3], 8.0f);
        float4 sd = a - 1.0f;
        CHECK_EQF(sd[0], 0.0f);

        float4 cp = a;
        cp += b;
        CHECK_EQF(cp[0], 11.0f);
        cp *= 0.5f;
        CHECK_EQF(cp[0], 5.5f);
    }

    // ---- dot / cross / length / normalize -----------------------------------
    {
        float3 a(1.0f, 2.0f, 3.0f);
        float3 b(4.0f, -5.0f, 6.0f);
        CHECK_NEAR(dot(a, b), 1.0f * 4.0f + 2.0f * -5.0f + 3.0f * 6.0f);

        float3 c = cross(a, b);
        CHECK_NEAR(c[0], 2.0f * 6.0f - 3.0f * -5.0f);
        CHECK_NEAR(c[1], 3.0f * 4.0f - 1.0f * 6.0f);
        CHECK_NEAR(c[2], 1.0f * -5.0f - 2.0f * 4.0f);

        float2 v(3.0f, 4.0f);
        CHECK_NEAR(length(v), 5.0f);
        float2 n = normalize(v);
        CHECK_NEAR(n[0], 0.6f);
        CHECK_NEAR(n[1], 0.8f);
        CHECK_NEAR(length(n), 1.0f);
    }

    // ---- swizzle reads -------------------------------------------------------
    {
        float4 v(1.0f, 2.0f, 3.0f, 4.0f);
        CHECK_EQF(float(v.x), 1.0f);
        CHECK_EQF(float(v.w), 4.0f);
        CHECK_EQF(float(v.r), 1.0f);
        CHECK_EQF(float(v.a), 4.0f);

        float2 xy = v.xy;
        CHECK_EQF(xy[0], 1.0f);
        CHECK_EQF(xy[1], 2.0f);
        float2 zx = v.zx;
        CHECK_EQF(zx[0], 3.0f);
        CHECK_EQF(zx[1], 1.0f);
        float2 xx = v.xx;  // duplicates read fine
        CHECK_EQF(xx[1], 1.0f);
        float3 zyx = v.zyx;
        CHECK_EQF(zyx[0], 3.0f);
        CHECK_EQF(zyx[2], 1.0f);
        float4 wzyx = v.wzyx;
        CHECK_EQF(wzyx[0], 4.0f);
        CHECK_EQF(wzyx[3], 1.0f);
        float3 rgb = v.rgb;
        CHECK_EQF(rgb[2], 3.0f);
        float4 bgra = v.bgra;
        CHECK_EQF(bgra[0], 3.0f);
        CHECK_EQF(bgra[3], 4.0f);
    }

    // ---- swizzle writes ------------------------------------------------------
    {
        float4 v(1.0f, 2.0f, 3.0f, 4.0f);
        v.x = 10.0f;
        CHECK_EQF(v[0], 10.0f);
        v.w += 1.0f;
        CHECK_EQF(v[3], 5.0f);

        v = float4(1.0f, 2.0f, 3.0f, 4.0f);
        v.xy = float2(8.0f, 9.0f);
        CHECK_EQF(v[0], 8.0f);
        CHECK_EQF(v[1], 9.0f);
        CHECK_EQF(v[2], 3.0f);

        // masked write in a different order
        v = float4(1.0f, 2.0f, 3.0f, 4.0f);
        v.wzyx = float4(1.0f, 2.0f, 3.0f, 4.0f);
        CHECK_EQF(v[0], 4.0f);
        CHECK_EQF(v[3], 1.0f);

        // aliasing: gather before scatter
        v = float4(1.0f, 2.0f, 3.0f, 4.0f);
        v.xy = v.yx;
        CHECK_EQF(v[0], 2.0f);
        CHECK_EQF(v[1], 1.0f);

        // swizzle-to-swizzle across objects
        float4 u(5.0f, 6.0f, 7.0f, 8.0f);
        v = float4(1.0f, 2.0f, 3.0f, 4.0f);
        v.zw = u.xy;
        CHECK_EQF(v[2], 5.0f);
        CHECK_EQF(v[3], 6.0f);

        // compound on multi-component swizzle
        v = float4(1.0f, 2.0f, 3.0f, 4.0f);
        v.xyz *= 2.0f;
        CHECK_EQF(v[0], 2.0f);
        CHECK_EQF(v[2], 6.0f);
        CHECK_EQF(v[3], 4.0f);
    }

    // ---- swizzles participate in arithmetic ---------------------------------
    {
        float4 v(1.0f, 2.0f, 3.0f, 4.0f);
        float3 s = v.xyz * v.www;  // (1,2,3) * (4,4,4)
        CHECK_EQF(s[0], 4.0f);
        CHECK_EQF(s[2], 12.0f);
        CHECK_NEAR(dot(v.xy, v.zw), 1.0f * 3.0f + 2.0f * 4.0f);
        float3 c = cross(v.xyz, v.yzx);
        CHECK_NEAR(c[0], 2.0f * 1.0f - 3.0f * 3.0f);
    }

    // ---- operator[] write ----------------------------------------------------
    {
        float3 v;
        v[0] = 1.0f;
        v[1] = 2.0f;
        v[2] = 3.0f;
        CHECK_EQF(float(v.z), 3.0f);
    }
}
