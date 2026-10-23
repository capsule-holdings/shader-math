// Swizzles of swizzles, shared like math_cases.hlsli (included after it). In
// shader-math they are opt-in, so the C++ side is a twin built with
// SHADER_MATH_NESTED_SWIZZLE (kernels_nested.slang on the Slang side). Only
// 2- and 3-component swizzles are nested there. All cases are exact (lanes
// only move).

float4 case_exact_nested_read(float4 a, float4 b)
{
    float2 p = a.zw.yx;
    float3 q = b.wzy.xzy;
    float4 r = a.xy.xyyx;
    float4 s = b.bgr.rrgb;
    return float4(p, q.yz) + r * 2.0f + s * 4.0f + float4(a.yz.y, b.zx.yx, a.xyz.z);
}

float4 case_exact_nested_mixed(float4 a, float4 b)
{
    float3 c = a.xyz;
    float2 d = c.zx.yx - b.xy.yx;
    float4 r = a.xyz.zyxx * b.yx.xyxy;
    r.xy = b.zw.yx;  // nested read on the right of a swizzle write
    return r + float4(d, b.xyz.zx);
}
