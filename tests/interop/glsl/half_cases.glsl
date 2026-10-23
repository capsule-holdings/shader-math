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
