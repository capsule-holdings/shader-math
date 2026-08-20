// Shared math cases: this file compiles BOTH as Slang (included from
// kernels.slang) and as C++ (included from cpp_harness.cpp inside a namespace
// with `using namespace shader_math::hlsl`).
//
// Rules for code in this file (see docs/slang_compat.md):
//   - shared subset only: no resources, no Slang generics/interfaces
//   - float literals always carry the f suffix
//   - no implicit half/float mixing (explicit constructor conversions only;
//     the C++ compile enforces this mechanically)
//   - no `inverse` (shader-math extension, not an HLSL/Slang intrinsic)
//
// Case naming convention drives the comparison tolerance (compare.py):
//   case_exact_* : bit-identical between Slang-CPU and shader-math
//   case_near_*  : relative 1e-5 (inner products, divisions, transcendentals)
//   case_half_*  : relative 2e-3 (binary16 paths)

// ---- deterministic inputs (exact binary fractions) --------------------------
static const float4 IN_A = float4(1.25f, -2.5f, 3.75f, 0.5f);
static const float4 IN_B = float4(-0.75f, 4.0f, 0.25f, -1.5f);
static const float3 IN_N = float3(0.25f, 1.0f, -0.5f);
static const float3 IN_L = float3(0.6f, 0.8f, 0.0f);
static const float3 IN_C = float3(0.9f, 0.5f, 0.25f);

// ---- exact tier -------------------------------------------------------------
float4 case_exact_hadamard(float4 a, float4 b)
{
    return a * b + a - b * 0.5f;
}

float4 case_exact_swizzle(float4 a, float4 b)
{
    float4 r = a.wzyx + b.xxzz;
    r.xy = r.yx;
    return r - a.yyww;
}

float4 case_exact_intrinsics(float4 a, float4 b)
{
    float4 r = abs(a) + sign(b) * 0.5f;
    r = r + floor(a) - frac(b);
    r = r + clamp(b, -1.0f, 1.0f) + step(0.0f, a);
    r = min(r, 100.0f);
    r = max(r, -100.0f);
    return r + saturate(b) + trunc(a) + ceil(b) + round(a + 0.25f);
}

// ---- near tier --------------------------------------------------------------
float4 case_near_dot_cross(float4 a, float4 b)
{
    float3 c = normalize(cross(a.xyz, b.xyz));
    return float4(dot(a.xyz, b.xyz), c.x, c.y, c.z);
}

float4 case_near_matrix(float4 v)
{
    float4x4 m = float4x4(2.0f, 0.5f, -1.0f, 3.25f,
                          -0.25f, 1.75f, 2.5f, -3.5f,
                          4.25f, -2.0f, 0.75f, 1.25f,
                          -1.5f, 3.0f, -0.5f, 2.25f);
    float4 r1 = mul(v, m);   // row vector * matrix
    float4 r2 = mul(m, v);   // matrix * column vector
    return r1 + r2 * 0.5f;
}

float4 case_near_determinant(float4 v)
{
    float2x2 m2 = float2x2(v.x, v.y, v.z, v.w);
    float3x3 m3 = float3x3(v.x, v.y, v.z,
                           v.w, 2.0f, -1.0f,
                           0.5f, -0.25f, 3.0f);
    float4x4 m4 = float4x4(v.x, v.y, v.z, v.w,
                           1.0f, 2.0f, 0.5f, -1.0f,
                           -0.5f, 1.5f, 3.0f, 0.25f,
                           2.0f, -1.0f, 0.75f, 1.0f);
    return float4(determinant(m2), determinant(m3), determinant(m4), 0.0f);
}

float4 case_near_lighting(float3 n, float3 l, float3 c)
{
    // Subset rule: intrinsics take vector arguments only (shader-math leaves
    // pure scalars to std::), so keep n.l on a vector lane instead of writing
    // saturate(dot(...)) on a bare float.
    float d = dot(normalize(n), normalize(l));
    float3 ndl = saturate(float3(d, d, d));
    float3 lit = lerp(c * 0.1f, c, ndl);
    lit = lit * smoothstep(0.0f, 1.0f, ndl);
    return float4(lit.x, lit.y, lit.z, ndl.x);
}

float4 case_near_transcend(float4 a, float4 b)
{
    float4 r = exp2(clamp(a, -2.0f, 2.0f)) + log2(abs(b) + 1.0f);
    r = r + sin(a) * cos(b);
    r = r + sqrt(abs(a)) + pow(abs(b) + 0.5f, 2.5f);
    return r + rsqrt(abs(a) + 1.0f) + rcp(b);
}

// ---- half tier --------------------------------------------------------------
// Excluded from SM_INTEROP_CASES for now: the Slang CPU target's C++ prelude
// (v2026.16) does not implement half vector arithmetic, so generated code
// containing half math fails to compile. Kept here (and dual-compiled once the
// SPIR-V phase lands); guarded so slangc does not emit it while unreferenced.
#if defined(SM_INTEROP_ENABLE_HALF)
float4 case_half_chain(float4 af)
{
    half4 h = half4(af);
    half4 r = h * half(2.0f) + half4(af.wzyx);
    r = min(r, half(8.0f));
    r = abs(r) + half4(saturate(float4(h)));
    return float4(r);
}
#endif
