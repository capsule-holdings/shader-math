// Cases only DXC can check: Slang (v2026.20) returns frexp's exponent as an
// int vector, its CPU prelude lacks dst, and it has no matrix constructor
// from a mixed list (float2x2(float2, float, float)). Compiled as HLSL by DXC and as
// C++ by cpp_harness.cpp (SM_INTEROP_DXC / SM_INTEROP_DXIL).
float4 case_exact_frexp(float4 a)
{
    float4 e;
    float4 m = frexp(a * 3.0f, e);
    return m + e * 0.25f;
}

// HLSL round sends halves to even. Slang's CPU target uses ::roundf (halves
// away from zero), so exact halves are checked against DXC only.
float4 case_exact_round_ties(float4 a)
{
    return float4(round(a).yw, round(a * 2.0f).xz);  // -2.5, 0.5, 2.5, 7.5
}

float4 case_exact_dst(float4 a, float4 b)
{
    return dst(a, b);
}

// component-list matrix constructors, unary +, ++, %=, bool splat
float4 case_exact_ctor_ops(float4 a, float4 b)
{
    float2x2 m = float2x2(a.xy, b.z, b.w);
    float2x2 n = float2x2(b);
    float4 v = +a;
    v++;
    float4 w = b;
    w %= 3.0f;
    bool3 t = true;
    float2 r = mul(float2(1.0f, 2.0f), m) + n[1];
    return float4(r, 0.0f, 0.0f) + v + w + float4(float3(t), 0.0f);
}

// one bool lane written, ++ on a component, a float2x2 read as a float4 (row
// by row), all / any on bool
float4 case_exact_lane_writes(float4 a, float4 b)
{
    bool3 k = false;
    k.y = (a.xyz > b.xyz).x;
    float3 v = a.xyz;
    v.x++;
    float2x2 m = float2x2(a);
    float4 c = m;
    bool t = any(a.x > b.x) || all(false);
    return c + float4(v, 0.0f) + float4(float3(k), t ? 1.0f : 0.0f);
}

// ++ / -- on a two-component swizzle and on a matrix element. DXIL only:
// DXC's SPIR-V backend (v1.9.2609) emits invalid SPIR-V for both ("OpStore
// Pointer ... is not a logical pointer").
float4 case_exact_step_writes(float4 a)
{
    float3 v = a.xyz;
    v.yz--;
    float3x3 n = float3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    n._12++;
    n._m22--;
    return float4(v, n._12 + n._m22);
}
