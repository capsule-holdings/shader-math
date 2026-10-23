// Shared GLSL .stpq swizzle cases: compiled as GLSL by kernels_stpq.comp and
// as C++ by cpp_harness.cpp built with SM_GLSL_STPQ (which also defines
// SHADER_MATH_GLSL_STPQ: the swizzles are opt-in in shader-math). Same rules
// as math_cases.glsl. All cases are exact (swizzles only move lanes).

vec4 case_exact_stpq_read(vec4 a, vec4 b)
{
    vec2 st = a.st;
    vec3 qps = b.qps;
    vec4 tq = vec4(a.tq, b.ps);
    return vec4(st, qps.x, qps.z) + tq * 2.0f + vec4(a.stpq).qpts;
}

vec4 case_exact_stpq_write(vec4 a, vec4 b)
{
    vec4 r = a;
    r.ts = b.st;            // permuted write
    r.p = b.q;
    vec4 w = b;
    w.qs = a.pt * 0.5f;
    bvec4 c = lessThan(a, b);
    float flags = (c.s ? 1.0f : 0.0f) + (c.t ? 2.0f : 0.0f) + (c.p ? 4.0f : 0.0f) + (c.q ? 8.0f : 0.0f);
    return r + w + vec4(flags);
}
