// Cases only DXC can check: Slang (v2026.20) returns frexp's exponent as an
// int vector and its CPU prelude lacks dst. Compiled as HLSL by DXC and as
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
