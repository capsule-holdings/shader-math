// Swizzles of swizzles, shared like math_cases.glsl. In shader-math they are
// opt-in, so the C++ side is a twin built with SM_GLSL_NESTED, which also
// defines SHADER_MATH_NESTED_SWIZZLE. Only 2- and 3-component swizzles are
// nested there. All
// cases are exact (lanes only move).

vec4 case_exact_nested_read(vec4 a, vec4 b)
{
    vec2 p = a.zw.yx;
    vec3 q = b.wzy.xzy;
    vec4 r = a.xy.xyyx;
    vec4 s = b.bgr.rrgb;
    return vec4(p, q.yz) + r * 2.0f + s * 4.0f + vec4(a.yz.y, b.zx.yx, a.xyz.z);
}

vec4 case_exact_nested_mixed(vec4 a, vec4 b)
{
    vec3 c = a.xyz;
    vec2 d = c.zx.yx - b.xy.yx;
    vec4 r = a.xyz.zyxx * b.yx.xyxy;
    r.xy = b.zw.yx;  // nested read on the right of a swizzle write
    return r + vec4(d, b.stp.ps);
}
