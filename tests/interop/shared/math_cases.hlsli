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

// ---- constructors, element access, compound ops (exact) ---------------------
float4 case_exact_ctor_access(float4 a, float4 b)
{
    float3x3 rows = float3x3(a.xyz, b.xyz, float3(1.0f, 2.0f, 3.0f));  // row vectors
    float3x3 splat = float3x3(0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);
    float3x3 s1 = (float3x3)0.25f;  // scalar -> every element (DXC rejects the float3x3(s) spelling)
    float4x4 big = float4x4(a, b, a * 2.0f, b * 0.5f);
    float2x2 small = (float2x2)big;                                      // truncating cast
    float4 r = float4(rows._m01 + rows._12, rows._m20 * 10.0f, small._m11 + small._21, s1._33);
    r += splat[1].xyzx;
    r *= 2.0f;
    r -= float4(b.w, a.w, b.z, a.z);
    bool4 nb = !(a > b);
    r.w = r.w + (nb[0] ? 1.0f : 0.0f) + (nb[1] ? 2.0f : 0.0f);
    float3x2 t = transpose(float2x3(a.xyz, b.xyz));
    return r + float4(t[2].x, t[0].y, t[1].x, t[1].y);
}

// ---- remaining transcendentals / geometry (near) ---------------------------
float4 case_near_misc(float4 a, float4 b)
{
    float4 s = clamp(a * 0.25f, -1.0f, 1.0f);
    float4 r = exp(s) + log(abs(b) + 1.0f) + tan(s * 0.5f);
    float l = length(a.xyz) + length(b);
    return r + float4(l, l * 0.5f, length(a.xy), 0.0f);
}

// ---- additions: log10 ldexp select lit %, bool literals --------------------
// Left out: frexp (Slang returns the exponent as an int vector, HLSL and
// shader-math as float) and dst (the Slang CPU prelude does not implement it).
// ldexp gets integral exponents, which both read alike.
float4 case_exact_select_bool(float4 a, float4 b)
{
    bool4 lit4 = bool4(true, false, true, false);
    float4 s = select(lit4, a, b) + select(a > b, a, 0.5f);
    float flags = (lit4.x ? 1.0f : 0.0f) + (lit4.y ? 2.0f : 0.0f) + ((a > b).z ? 4.0f : 0.0f);
    return s + float4(flags, 0.0f, 0.0f, 0.0f) + (a * 3.0f) % b;
}

float4 case_near_log10_ldexp(float4 a, float4 b)
{
    return log10(abs(a) + 1.0f) + ldexp(b, float4(1.0f, -2.0f, 3.0f, 0.0f));
}

// intrinsics on scalars: floats and one-component swizzles, f-suffixed
// literals next to them
float4 case_near_scalar(float4 a, float4 b)
{
    float d = max(dot(a.xyz, b.xyz), 0.0f);
    float s = saturate(a.w) + lerp(a.x, b.x, 0.25f);
    float p = pow(abs(a.y), 1.5f) + clamp(b.z, 0.0f, 0.2f);
    float t = smoothstep(0.0f, 2.0f, a.x) + frac(b.w) + rsqrt(abs(a.z) + 1.0f);
    return float4(d + s, p, t, sign(b.x) * step(0.5f, a.w));
}

// D3D's NaN rules: min / max ignore a NaN operand, saturate(NaN) = 0 and
// clamp(NaN, lo, hi) = lo. Not in the DXIL check: DXC compiles DXIL with
// fast-math flags, which assume no NaN
float4 case_exact_nan_rules(float4 a, float4 b)
{
    float4 z = a - a;
    float4 q = z / z;
    float4 m = min(q, b) + max(a, q) + min(b, q);
    float4 s = saturate(q) + clamp(q, -1.0f, 1.0f);
    return m + s;
}

// edge values: negative operands, equal and reversed bounds, signed zero
float4 case_near_edges(float4 a, float4 b)
{
    float4 f = fmod(a * 2.0f, -b) + fmod(-a, b);
    float4 s = step(a, a) + step(b, a);
    float4 c = clamp(a, 1.0f, 0.0f);
    float4 z = frac(b * 0.0f) + smoothstep(1.0f, 0.0f, a * 0.25f);
    return f + s * 4.0f + c * 8.0f + z * 16.0f;
}

// rcp / rsqrt of a signed zero are infinities. Not in the DXIL check: DXC
// compiles DXIL with fast-math flags, which assume no infinities
float4 case_exact_inf_edges(float4 a)
{
    float4 z = a - a;
    return float4(rcp(z).x, rsqrt(z).y, rcp(-z).z, rcp(a).w);
}

// lit's specular term is (n.h)^m (Microsoft's lit documentation, Slang,
// DXC's DXIL backend, shader-math); DXC's SPIR-V backend emits (n.h) * m,
// so only its SPIR-V check leaves this case out (dxc/dxc_case_list.h)
float4 case_near_lit()
{
    return lit(0.5f, 0.4f, 4.0f) + lit(-0.25f, 0.5f, 2.0f) + lit(0.75f, 0.5f, 3.0f);
}

// ---- half tier --------------------------------------------------------------
// The Slang CPU target's C++ prelude (v2026.20) does not implement half vector
// arithmetic, so these cases run through kernels_half.slang compiled to SPIR-V
// on Vulkan (lavapipe) instead (SM_INTEROP_HALF_CASES); guarded so the CPU
// build never sees them.
#if defined(SM_INTEROP_ENABLE_HALF)
float4 case_half_chain(float4 af)
{
    half4 h = half4(af);
    half4 r = h * half(2.0f) + half4(af.wzyx);
    r = min(r, half(8.0f));
    r = abs(r) + half4(saturate(float4(h)));
    return float4(r);
}

float4 case_half_products(float4 af, float4 bf)
{
    half4 a = half4(af);
    half4 b = half4(bf);
    half d = dot(a.xyz, b.xyz);
    half4 l = lerp(a, b, half(0.25f));
    half4 m = max(a, b) - clamp(b, half(-1.0f), half(1.0f));
    half2x2 hm = half2x2(a.x, a.y, a.z, a.w);
    half2 v = mul(b.xy, hm);
    return float4(float(d), float(l.y + m.z), float(half(v.x)), float(half(v.y)));
}
#endif
