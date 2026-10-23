// Cases only DXC can check: Slang (v2026.16) returns frexp's exponent as an
// int vector and its CPU prelude lacks dst. Compiled as HLSL by DXC and as
// C++ by cpp_harness.cpp (SM_INTEROP_DXC).
float4 case_exact_frexp_dst(float4 a, float4 b)
{
    float4 e;
    float4 m = frexp(a * 3.0f, e);
    float4 d = dst(a, b);
    return m + e * 0.25f + d;
}
