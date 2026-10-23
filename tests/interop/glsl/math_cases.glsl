// Shared GLSL math cases: this file compiles BOTH as GLSL 4.50 (included from
// kernels.comp, run on Vulkan) and as C++ (included from cpp_harness.cpp
// inside a namespace with `using namespace shader_math::glsl`).
//
// Rules for code in this file:
//   - shared subset only: plain functions over vec / mat, no resources
//   - float literals carry the f suffix (valid in both languages)
//   - no function parameters declared `out` / `inout` (C++ has no such
//     keyword); calling built-ins with out parameters (modf) is fine
//   - inputs avoid denormals (the Vulkan implementation may flush them)
//   - built-in functions take vector arguments only (shader-math leaves pure
//     scalars to std::), so apply them to a vector and pick a component
//
// Case naming drives the comparison tolerance (compare.py):
//   exact_  : bit-identical. Single-rounding operations only: a GLSL
//             compiler may fuse a * b + c and evaluate division and
//             transcendentals with relaxed precision
//   near_   : relative 1e-5 (products, sums of products, division)
//   approx_ : relative 1e-4 (transcendentals: GLSL allows relaxed precision)

// GLSL's not() is not_() in shader-math (`not` is a C++ alternative token);
// kernels.comp defines SM_NOT as not, cpp_harness.cpp as not_
const vec4 IN_A = vec4(1.25f, -2.5f, 3.75f, 0.5f);
const vec4 IN_B = vec4(-0.75f, 4.0f, 0.25f, -1.5f);

// ---- exact: constructor layouts --------------------------------------------
// GLSL matrices fill column by column; matCxR is C columns of R rows.
vec4 case_exact_ctor_layout(vec4 a)
{
    mat3 m = mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    mat2x3 n = mat2x3(a.x, a.y, a.z, a.w, 10.0f, 11.0f);  // 2 columns, 3 rows
    mat3x2 p = mat3x2(a.w, a.z, a.y, a.x, 12.0f, 13.0f);  // 3 columns, 2 rows
    return vec4(m[0][1] + m[2][0] * 10.0f, m[1][2], n[1][2] + n[0][1] * 100.0f,
                p[2][1] + p[1][0] * 100.0f);
}

// scalar ctor = diagonal; matrix ctor grows from the identity / shrinks to
// the upper-left block; vector composites
vec4 case_exact_ctor_special(vec4 a, vec4 b)
{
    mat3 d = mat3(2.5f);
    mat4 grown = mat4(mat2(a.x, a.y, a.z, a.w));
    mat2 shrunk = mat2(mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f));
    vec4 comp = vec4(a.xy, b.zw) + vec4(vec3(b.x), a.w) + vec4(a.z, vec2(b.y, a.x), 1.0f);
    return vec4(d[0][0] + d[0][1] + d[1][1] * 10.0f,
                grown[1][1] + grown[2][2] * 100.0f + grown[3][0] * 1000.0f + grown[2][3],
                shrunk[1][0] + shrunk[1][1] * 10.0f,
                comp.x + comp.y * 2.0f + comp.z * 4.0f + comp.w * 8.0f);
}

// ---- exact: component-wise operations ---------------------------------------
vec4 case_exact_componentwise(vec4 a, vec4 b)
{
    mat2x3 m = mat2x3(a.x, a.y, a.z, b.x, b.y, b.z);
    mat2x3 n = mat2x3(b.w, a.w, b.y, a.z, a.x, b.x);
    mat2x3 h = matrixCompMult(m, n);
    mat3x2 t = transpose(m);
    mat3 o = outerProduct(a.xyz, b.xyz);
    vec4 r = vec4(h[0][1], h[1][2], t[2][0] + t[0][1], o[1][2] + o[2][0]);
    return r + abs(a) - floor(b) + fract(a) + roundEven(a + 0.25f) + sign(b);
}

vec4 case_exact_rounding(vec4 a, vec4 b)
{
    vec4 halves = vec4(0.5f, 1.5f, 2.5f, -2.5f);
    vec4 r = roundEven(halves) + trunc(b) + ceil(a);
    r = r + min(a, b) + max(a, 1.0f) + clamp(b, -1.0f, 2.0f);
    return r + step(0.0f, a) + step(b, a);
}

// ---- exact: comparisons -----------------------------------------------------
// vector ==, != and matrix == are scalar bool in GLSL; relations on vectors
// are the lessThan / equal family
vec4 case_exact_compare(vec4 a, vec4 b)
{
    mat2 m = mat2(a.x, a.y, a.z, a.w);
    mat2 n = mat2(a.x, a.y, a.z, a.w);
    vec4 r;
    r.x = (a == a && a != b) ? 1.0f : 0.0f;
    r.y = (m == n && !(m != n)) ? 1.0f : 0.0f;
    r.z = all(lessThan(a, a + 1.0f)) ? 1.0f : 0.0f;
    r.w = (any(equal(a, b)) ? 10.0f : 0.0f) + (all(greaterThanEqual(abs(a), vec4(0.5f))) ? 1.0f : 0.0f);
    return r;
}

// ---- near: products ---------------------------------------------------------
// * is the matrix product; v * m is the row-vector form (= transpose(m) * v)
vec4 case_near_matrix_product(vec4 v)
{
    mat4 m = mat4(2.0f, 0.5f, -1.0f, 3.25f,
                  -0.25f, 1.75f, 2.5f, -3.5f,
                  4.25f, -2.0f, 0.75f, 1.25f,
                  -1.5f, 3.0f, -0.5f, 2.25f);
    mat3x2 a = mat3x2(v.x, v.y, v.z, v.w, 1.5f, -0.5f);  // 3 columns, 2 rows
    mat2x3 b = mat2x3(v.w, v.z, v.y, v.x, 0.25f, 2.0f);  // 2 columns, 3 rows
    mat2 ab = a * b;                                     // (2x3 math) * (3x2 math)
    vec4 r = m * v + (v * m) * 0.5f;
    return r + vec4(ab[0][0], ab[0][1], ab[1][0], ab[1][1]) + (m * m)[2];
}

vec4 case_near_inverse(vec4 v)
{
    mat3 m = mat3(v.x, v.y, v.z,
                  v.w, 2.0f, -1.0f,
                  0.5f, -0.25f, 3.0f);
    mat3 i = inverse(m);
    mat4 q = mat4(v.x, v.y, v.z, v.w,
                  1.0f, 2.0f, 0.5f, -1.0f,
                  -0.5f, 1.5f, 3.0f, 0.25f,
                  2.0f, -1.0f, 0.75f, 1.0f);
    return vec4(determinant(m), determinant(q), i[0][1] + i[2][2], (i * m)[1][1]);
}

// ---- near: mod (floor-based, sign of y) and friends --------------------------
vec4 case_near_mod(vec4 a, vec4 b)
{
    vec4 r = mod(a, b) + mod(-a, b) * 0.5f + mod(a * 3.0f, 1.5f);
    r = r + mix(a, b, 0.25f) + mix(a, b, vec4(0.0f, 0.5f, 1.0f, 0.75f));
    vec4 ip;
    vec4 fr = modf(a * 1.5f, ip);
    return r + fr + ip * 0.25f + fma(a, b, vec4(1.0f));
}

vec4 case_near_geometry(vec4 a, vec4 b)
{
    vec3 n = normalize(vec3(0.25f, 1.0f, -0.5f));
    vec3 i = normalize(a.xyz - b.xyz);
    vec3 refl = reflect(i, n);
    vec3 refr = refract(i, n, 0.75f);
    vec3 ff = faceforward(n, i, b.xyz);
    return vec4(distance(a, b) + length(refl), refl.x + refr.y, dot(refr, ff) + cross(i, n).z,
                inversesqrt(abs(a) + 1.0f).x + smoothstep(0.0f, 2.0f, a).w);
}

// ---- exact: remaining constructors, operators, swizzles, relations ----------
vec4 case_exact_misc(vec4 a, vec4 b)
{
    mat3 cols = mat3(a.xyz, b.xyz, vec3(1.0f, 2.0f, 3.0f));  // column vectors
    mat3 sum = cols + mat3(2.0f) - cols * 0.5f;              // component-wise + -, scalar *
    mat2 q = mat2(a.x, a.y, a.z, a.w) / 2.0f;                // component-wise / by a scalar
    vec4 r = vec4(cols[1][2], sum[0][0], sum[2][1], q[1][0]);
    r += b.rgba * 0.5f;                                      // .rgba swizzle, compound ops
    r *= 2.0f;
    vec4 w = vec4(1.4f, -1.4f, 2.6f, -0.2f);                 // round() away from .5 ties
    r = r + round(w) + vec4(a.bgr, b.a);
    float flags = (all(lessThanEqual(a, a)) ? 1.0f : 0.0f) + (any(greaterThan(a, b)) ? 2.0f : 0.0f) +
                  (any(notEqual(a, b)) ? 4.0f : 0.0f) + (all(SM_NOT(equal(a, b))) ? 8.0f : 0.0f);
    return r + vec4(flags);
}

vec4 case_exact_classify(vec4 a)
{
    vec4 big = vec4(3.0e38f) * 10.0f;                        // +inf without a division
    vec4 v = vec4(a.x, big.y, big.z - big.w, -big.x);        // finite, +inf, NaN, -inf
    vec4 r;
    r.x = (isnan(v)[2] && !isnan(v)[0]) ? 1.0f : 0.0f;
    r.y = (isinf(v)[1] && isinf(v)[3] && !isinf(v)[2]) ? 1.0f : 0.0f;
    r.z = any(isnan(a)) ? 0.0f : 1.0f;
    r.w = any(isinf(a)) ? 0.0f : 1.0f;
    return r;
}

// ---- approx: transcendentals -------------------------------------------------
vec4 case_approx_transcend(vec4 a, vec4 b)
{
    vec4 s = clamp(a * 0.25f, -1.0f, 1.0f);
    vec4 r = atan(a, b) + atan(-a, b) + atan(b);  // atan(y, x): y first
    r = r + asin(s) + acos(s * 0.5f) + sin(a) * cos(b) + tan(s);
    r = r + exp2(clamp(a, -2.0f, 2.0f)) + log2(abs(b) + 1.0f) + sqrt(abs(a));
    r = r + sinh(s) + cosh(s) * 0.5f + tanh(b) + exp(s) + log(abs(b) + 1.0f);
    return r + pow(abs(b) + 0.5f, vec4(1.5f)) + radians(a * 10.0f) + degrees(s) * 0.01f;
}
