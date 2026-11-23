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

// a bool lane written through b[i]; a matrix inside a matrix constructor
// (row by row)
float4 case_exact_index_matrix_args(float4 a, float4 b)
{
    bool3 k = false;
    k[2] = (a.xyz > b.xyz)[1];
    float2x2 m2 = float2x2(a);
    float3x3 m = float3x3(m2, b.x, b.y, b.z, b.w, 1.0f);
    return float4(m[0].x + m[1].y, m[1].x, m[2].z, 0.0f) + float4(float3(k), m[0].z);
}

// sign on int vectors: the Slang CPU target's C++ prelude (v2026.20) has no
// I32_sign, so only DXC checks it
float4 case_bits_int_sign(float4 a, float4 b)
{
    int4 i = int4(a * 4.0f);
    int4 j = int4(b * 4.0f);
    return asfloat(sign(i) * 3 + sign(j) + sign(i - i) * 5);
}

// int4 += float4 computes in float and truncates the sum (DXC, shader-math);
// Slang (v2026.20) converts the right operand to int first
float4 case_bits_int_compound_float(float4 a, float4 b)
{
    int4 t = int4(a * 3.0f);
    t += b;
    t *= 0.5f;
    uint4 u = uint4(abs(a) * 4.0f);
    u -= b * 2.0f;
    return asfloat(uint4(t) + u);
}

// the bit functions on float lanes: HLSL converts them to int (a negative
// lane keeps its bits; reversebits(float4) is an int4)
float4 case_bits_bitfn_float_args(float4 a, float4 b)
{
    float4 f = a * 4.0f - b * 9.0f;  // 11.75, -46, 12.75, 15.5
    uint4 r = countbits(f) + (firstbithigh(f) << 6u) + (firstbitlow(-f) << 12u);
    int4 v = reversebits(f) >> 3;
    return asfloat(r ^ uint4(v));
}

// 1-dimension matrices (1xN, Nx1, 1x1): vector conversions, mul, transpose.
// Slang's CPU target (v2026.20) rejects matrix.init for 1-dim matrices and
// implicit vector <-> 1-dim matrix conversions, so DXC checks it.
// DXC's DXIL lowering (v1.9.2609) crashes when float(float1x1) is used in a
// scalar binary op, so pass float(d) directly to a vector constructor.
float4 case_exact_matrix_dim1(float4 a, float4 b)
{
    float1x3 r13 = a.xyz;
    float3x1 c31 = b.xyz;
    float1x1 d = mul(r13, c31);
    float3x3 outer = mul(c31, r13);
    float1x3 scaled_r = mul(2.0f, r13);
    float3x1 scaled_c = mul(c31, 0.5f);
    float3x1 t = transpose(r13) + scaled_c * 2.0f;
    float1 s = mul(r13, b.xyz) + mul(a.xyz, c31);
    float3 sum = float3(scaled_r) + float3(t) + outer[0] + outer[1] + outer[2];
    return float4(sum, d._m00 + s.x + r13._m02 + c31._31) + float4(0.0f, 0.0f, 0.0f, float(d));
}

// determinant(float1x1) and mul with float1 against 1-dim matrices. DXIL only:
// DXC's SPIR-V backend (v1.9.2609) rejects determinant(float1x1) and crashes
// on mul(float1, float1x3) / mul(float3x1, float1).
float4 case_exact_matrix_dim1_dxil(float4 a, float4 b)
{
    float1x3 r13 = a.xyz;
    float3x1 c31 = b.xyz;
    float1x1 d = mul(r13, c31);
    float3 v = mul(float1(2.0f), r13) + mul(c31, float1(0.5f));
    return float4(v, determinant(d));
}

#if defined(SM_INTEROP_ENABLE_HALF)
// countbits / firstbithigh / firstbitlow on 16- and 64-bit integers count at
// the lane's width (DXIL's Countbits / FirstbitSHi on i16 and i64). DXC's
// SPIR-V backend does not compile firstbithigh / firstbitlow below or above
// 32 bits, and Slang (v2026.20) widens int16_t to 32 bits before counting
// (docs/slang_compat.md), so only DXIL checks them.
float4 case_bits_int_width_firstbit(float4 af, float4 bf)
{
    int16_t4 s = int16_t4(af * 4000.0f) - int16_t(7000);  // -2000, -17000, 8000, -5000
    uint16_t4 u = uint16_t4(s) ^ uint16_t(0x5A5A);
    int64_t4 l = int64_t4(af * 1000000000.0f) * int64_t(-123457);
    uint64_t4 ul = uint64_t4(l) ^ (uint64_t(0x0F0F0F0Fu) << uint64_t(32));
    uint4 r = countbits(s) + (firstbithigh(s) << 5u) + (firstbithigh(u) << 10u) + (firstbitlow(s) << 15u);
    r ^= (firstbithigh(l) << 3u) + (firstbithigh(ul) << 10u) + (firstbitlow(l) << 17u) + (firstbitlow(ul) << 24u);
    r += firstbithigh(int16_t4(0, -1, 1, -32768)) + firstbitlow(uint64_t4(0, 8, 1, 0));
    return asfloat(r + firstbithigh(int64_t4(bf * 4.0f)));
}

// 64-bit double vectors, exact fma, asdouble / 3-arg asuint, and double matrices
float4 case_bits_double_hlsl(float4 af, float4 bf)
{
    double4 a = double4(af) + double4(1.0 / 1048576.0, -1.0 / 524288.0, 1.0 / 262144.0, 0.0);
    double4 b = double4(bf);
    double4 r = fma(a, b, double4(1.0, -2.0, 0.5, 3.0));
    r = abs(r) + min(a, b) + max(a, bf) + saturate(a) + mad(a, b, double4(0.25, 0.25, 0.25, 0.25));
    double2x2 dm = double2x2(a.x, a.y, b.x, b.y);
    float2x2 fm = float2x2(1.0f, 0.5f, -0.5f, 2.0f);
    double2x2 pm = mul(dm, fm) + transpose(dm);
    double1x3 r13 = a.xyz;
    double3x1 c31 = b.xyz;
    double1x1 d11 = mul(r13, c31);
    r = r + double4(pm[0].x, pm[0].y, pm[1].x, pm[1].y) + double4(double(d11), a.x * b.x + a.y * b.y, 0.0, 0.0);
    uint4 lo, hi;
    asuint(r, lo, hi);
    double4 rt = asdouble(lo, hi);
    uint4 lo2, hi2;
    asuint(rt, lo2, hi2);
    return asfloat(lo2 ^ (hi2 << 1u));
}
#endif
