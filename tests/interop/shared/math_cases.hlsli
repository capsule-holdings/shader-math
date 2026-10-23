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

// ---- classification and matrix intrinsics (exact) ---------------------------
// bool results are folded into floats with all()/any() and scalar ?: (no
// bool-vector -> float-vector conversion in the shared subset)
float4 case_exact_classify(float4 a)
{
    float4 inf = rcp(float4(0.0f, 0.0f, 0.0f, 0.0f));
    float4 v = float4(a.x, inf.y, inf.z - inf.w, -inf.x);  // finite, +inf, NaN, -inf
    float4 r;
    // bool vectors are indexed, not swizzled (shader-math's boolN has no .xyzw)
    r.x = (isnan(v)[2] && !isnan(v)[0]) ? 1.0f : 0.0f;
    r.y = (isinf(v)[1] && isinf(v)[3] && !isinf(v)[2]) ? 1.0f : 0.0f;
    r.z = (isfinite(v)[0] && !isfinite(v)[1] && !isfinite(v)[2]) ? 1.0f : 0.0f;
    r.w = any(isnan(a)) ? 0.0f : 1.0f;
    return r;
}

float4 case_exact_matrix_intrinsics(float4 a, float4 b)
{
    float2x4 m = float2x4(a.x, a.y, a.z, a.w,
                          b.x, b.y, b.z, b.w);
    float2x4 r = abs(m) + floor(m) - saturate(m);
    r = r + min(m, 1.0f) + max(m, abs(m) * 0.5f);
    r = r + clamp(m, -1.0f, 2.0f) + step(0.0f, m) + sign(m) + frac(m);
    return r[0] + r[1] * 0.5f;
}

// ---- geometry / inverse trig / fmod (near) ----------------------------------
float4 case_near_geometry(float3 n, float3 l, float3 c)
{
    float3 nn = normalize(n);
    float3 i = normalize(l - c);
    float3 refl = reflect(i, nn);
    float3 refr = refract(i, nn, 0.75f);
    float3 tir = refract(normalize(float3(1.0f, -0.05f, 0.0f)), float3(0.0f, 1.0f, 0.0f), 1.5f);
    float3 ff = faceforward(nn, i, c);
    return float4(distance(n, l) + tir.x + tir.y, refl.x + refr.y, refl.z * refr.x + ff.y,
                  ff.x + ff.z + refr.z);
}

float4 case_near_inverse_trig(float4 a, float4 b)
{
    float4 s = clamp(a * 0.25f, -1.0f, 1.0f);
    float4 r = asin(s) + acos(s * 0.5f) + atan(b);
    r = r + atan2(a, b) + atan2(-a, b) + atan2(a, -b);
    r = r + sinh(s) + cosh(s) * 0.5f + tanh(b);
    return r + radians(a * 10.0f) + degrees(s) * 0.01f;
}

float4 case_near_fmod_mad(float4 a, float4 b)
{
    // fmod keeps the sign of x (trunc-based): opposite-sign operands included
    float4 r = fmod(a, b) + fmod(-a, b) * 0.5f + fmod(a * 3.0f, 1.5f);
    r = r + mad(a, b, float4(1.0f, -1.0f, 0.5f, 2.0f));
    float4 ip;
    float4 fr = modf(a * 1.5f, ip);
    float4 sn;
    float4 cs;
    sincos(b, sn, cs);
    return r + fr + ip * 0.25f + sn + cs * 0.5f;
}

float4 case_near_matrix_intrinsics(float4 a, float4 b)
{
    float3x2 m = float3x2(a.x, a.y, a.z, a.w, b.x, b.y);
    float3x2 t = float3x2(b.z, b.w, a.x, b.x, a.w, b.y);
    float3x2 r = lerp(m, t, 0.25f) + lerp(m, t, saturate(t));
    r = r + sqrt(abs(m)) + sin(m) * cos(t) + pow(abs(t) + 0.5f, 1.5f);
    r = r + smoothstep(-1.0f, 2.0f, m) + mad(m, t, m) + atan2(m, t) + fmod(m, 1.25f);
    return float4(r[0].x + r[1].y, r[0].y - r[2].x, r[1].x + r[2].y, r[2].x * 0.5f);
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
