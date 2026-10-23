// Cases only DXC can check: Slang (v2026.16) returns frexp's exponent as an
// int vector and its CPU prelude lacks dst. Compiled as HLSL by DXC and as
// C++ by cpp_harness.cpp (SM_INTEROP_DXC / SM_INTEROP_DXIL).
float4 case_exact_frexp(float4 a)
{
    float4 e;
    float4 m = frexp(a * 3.0f, e);
    return m + e * 0.25f;
}

float4 case_exact_dst(float4 a, float4 b)
{
    return dst(a, b);
}
