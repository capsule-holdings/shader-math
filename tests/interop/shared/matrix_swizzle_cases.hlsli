// Multi-element matrix accessors, shared like math_cases.hlsli (included
// after it). In shader-math they are opt-in, so the C++ side is a twin built
// with SHADER_MATH_MATRIX_SWIZZLE3 (kernels_matrix_swizzle.slang on the Slang
// side). Only 2- and 3-element lists exist there. All cases are exact.

float4 case_exact_matrix_swizzle_read(float4 a, float4 b)
{
    float4x4 m = float4x4(a, b, a * 2.0f, b * 0.5f);
    float3 t = m._m30_m31_m32;
    float3 d = m._11_22_33;
    float2 p = m._m03_m21;
    float3x2 s = float3x2(a.xy, b.zw, a.zw);
    return float4(t + d * 4.0f, 0.0f) + float4(p, s._m21_m00) * 8.0f;
}

float4 case_exact_matrix_swizzle_write(float4 a, float4 b)
{
    float4x4 m = float4x4(a, b, a, b);
    m._m00_m11 = b.wz;
    m._m01_m10 = m._m10_m01;
    m._41_42_43 += a.xyz;
    float3x3 n = (float3x3)m;
    n._m02_m20 = n._m20_m02;
    return float4(m._m00_m01_m10, m._m33) + float4(n._m02_m20_m11, 0.0f) * 2.0f;
}
