// Shared GLSL half cases (f16vec / f16mat, GL_EXT_shader_explicit_arithmetic_
// types_float16): compiled as GLSL by kernels_half.comp and as C++ by
// cpp_harness.cpp (SM_GLSL_HALF). Same rules as math_cases.glsl, plus: no
// float16_t scalar spelling (on ARM, arm_neon.h's global float16_t makes the
// unqualified name ambiguous in C++), so constants are f16vec splats.
// All cases use the half_ tolerance (relative 2e-3).

vec4 case_half_arith(vec4 af, vec4 bf)
{
    f16vec4 a = f16vec4(af);
    f16vec4 b = f16vec4(bf);
    f16vec4 r = a * f16vec4(2.0f) + b;
    r = min(r, f16vec4(8.0f)) + abs(a) - clamp(b, f16vec4(-1.0f), f16vec4(1.0f));
    return vec4(r);
}

vec4 case_half_products(vec4 af, vec4 bf)
{
    f16vec4 a = f16vec4(af);
    f16vec4 b = f16vec4(bf);
    f16mat2 m = f16mat2(a.x, a.y, a.z, a.w);
    f16vec2 v = m * b.xy;
    f16vec4 l = mix(a, b, f16vec4(0.25f));
    return vec4(float(dot(a.xyz, b.xyz)), float(l.y), float(v.x), float(v.y));
}

// 16- and 64-bit integers (the half kernel carries them: they need the
// explicit arithmetic types and the shaderInt16 / shaderInt64 features)
vec4 case_bits_int_widths(vec4 af, vec4 bf)
{
    i16vec4 s = i16vec4(af * 4000.0f);  // 5000, -10000, 15000, 2000
    s = s * int16_t(7) + int16_t(30000);
    u16vec4 u = u16vec4(s) >> uint16_t(3);
    i64vec4 l = i64vec4(af * 1000000000.0f) * int64_t(123457) - i64vec4(bf * 4.0f);
    l = l / int64_t(1000) + (l >> int64_t(3));
    uint64_t k = (uint64_t(0x9E3779B9u) << uint64_t(32)) | uint64_t(0x7F4A7C15u);
    u64vec4 ul = u64vec4(l) * k;
    ivec4 m = ivec4(s) + ivec4(u);
    uvec4 lo = uvec4(ul & uint64_t(0xFFFFFFFFu));
    uvec4 hi = uvec4(ul >> uint64_t(32));
    return uintBitsToFloat(uvec4(m) ^ lo ^ (hi << 1u));
}
