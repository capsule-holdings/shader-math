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

// ---- exact: mix with a bool selector, bool literals --------------------------
vec4 case_exact_mix_bool(vec4 a, vec4 b)
{
    bvec4 sel = bvec4(true, false, false, true);
    vec4 r = mix(a, b, sel) + mix(a, b, greaterThan(a, b));
    float flags = (sel.x ? 1.0f : 0.0f) + (sel.y ? 2.0f : 0.0f) + (lessThan(a, b).w ? 4.0f : 0.0f);
    return r + vec4(flags);
}

// ---- approx: transcendentals -------------------------------------------------
vec4 case_approx_transcend(vec4 a, vec4 b)
{
    vec4 s = clamp(a * 0.25f, -1.0f, 1.0f);
    vec4 r = atan(a, b) + atan(-a, b) + atan(b);  // atan(y, x): y first
    r = r + asin(s) + acos(s * 0.5f) + sin(a) * cos(b) + tan(s);
    r = r + exp2(clamp(a, -2.0f, 2.0f)) + log2(abs(b) + 1.0f) + sqrt(abs(a));
    r = r + sinh(s) + cosh(s) * 0.5f + tanh(b) + exp(s) + log(abs(b) + 1.0f);
    r = r + asinh(a) + acosh(abs(a) + 1.0f) + atanh(s * 0.5f);
    return r + pow(abs(b) + 0.5f, vec4(1.5f)) + radians(a * 10.0f) + degrees(s) * 0.01f;
}

// ---- approx: built-in functions on scalars -----------------------------------
// floats and one-component swizzles, f-suffixed literals next to them
vec4 case_approx_scalar(vec4 a, vec4 b)
{
    float d = max(dot(a.xyz, b.xyz), 0.0f);
    float s = clamp(a.w, 0.0f, 1.0f) + mix(a.x, b.x, 0.25f);
    float p = pow(abs(a.y), 1.5f) + min(b.z, 0.2f);
    float t = smoothstep(0.0f, 2.0f, a.x) + fract(b.w) + inversesqrt(abs(a.z) + 1.0f);
    return vec4(d + s, p, t, sign(b.x) * step(0.5f, a.w));
}

// ---- exact: mix at its ends -------------------------------------------------
// mix is x * (1 - a) + y * a: exact at a = 0 and a = 1 however far apart x
// and y are (x + a * (y - x) would lose y next to a large x)
vec4 case_exact_mix_ends(vec4 a, vec4 b)
{
    vec4 big = a * 1.0e8f;
    return mix(big, b, 1.0f) + mix(b, big, vec4(0.0f));
}

// ---- near: edge values ----------------------------------------------------------
// negative operands of mod, equal step bounds, fract of a signed zero
vec4 case_near_edges(vec4 a, vec4 b)
{
    vec4 m = mod(a * 2.0f, -b) + mod(-a, b);
    vec4 s = step(a, a) + step(b, a);
    vec4 z = fract(b * 0.0f);
    return m + s * 4.0f + z * 16.0f;
}

// ---- exact: bool <-> float vectors, isnan on a scalar ---------------------------
vec4 case_exact_bool_numeric(vec4 a, vec4 b)
{
    vec3 m = vec3(greaterThan(a.xyz, b.xyz));
    bvec3 k = bvec3(b.xyz);
    float n = isnan(a.w) ? 8.0f : 0.0f;
    return vec4(m + vec3(k) * 4.0f, n);
}

// component lists longer than the target (the last argument cut short),
// vector *= matrix, unary +, ++
vec4 case_exact_ctor_ops(vec4 a, vec4 b)
{
    vec3 c = vec3(a.xy, b.zw);
    mat2 m = mat2(a.xyz, b.x);
    mat2 n = mat2(b);
    vec2 r = vec2(1.0, 2.0);
    r *= m;
    vec4 v = +a;
    v++;
    return vec4(c, 0.0) + vec4(r, n * vec2(1.0, 0.0)) + v;
}

// one bool lane written, ++ / -- on components, vector from a matrix
// (column by column, the first N components)
vec4 case_exact_lane_writes(vec4 a, vec4 b)
{
    bvec3 k = bvec3(false);
    k.y = greaterThan(a.xyz, b.xyz).x;
    vec3 v = a.xyz;
    v.x++;
    v.yz--;
    vec4 c = vec4(mat2(a));
    vec3 d = vec3(mat3(2.0));
    return c + vec4(v, 0.0) + vec4(vec3(k), 0.0) + vec4(d, 0.0);
}

// a bool lane written through b[i]; a matrix inside a vector constructor
// (column by column, cut short at the end)
vec4 case_exact_index_matrix_args(vec4 a, vec4 b)
{
    bvec3 k = bvec3(false);
    k[2] = greaterThan(a.xyz, b.xyz)[1];
    vec4 c = vec4(b.w, mat2(a));
    vec4 d = vec4(b.xy, mat2(b));
    return c + d + vec4(vec3(k), 0.0);
}

// ---- integer tier (bits_: the 32-bit patterns must be identical) ------------
// int / uint results travel as intBitsToFloat / uintBitsToFloat. No case
// divides by zero, overflows a signed value, shifts by 32 or more, or takes %
// of a negative operand: GLSL leaves those undefined. uint arithmetic wraps.
vec4 case_bits_int_arith(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 4.0f);  // 5, -10, 15, 2
    ivec4 j = ivec4(b * 4.0f);  // -3, 16, 1, -6
    ivec4 r = i * j + i - j / 2 + abs(i) % (abs(j) + 1) + j / i;
    r = r + abs(j) + min(i, j) * 3 - max(i, j) + clamp(i, -4, 4) + sign(j);
    r += i.wzyx * 2;
    r.y -= 7;
    return intBitsToFloat(r);
}

vec4 case_bits_uint_wrap(vec4 a, vec4 b)
{
    uvec4 u = uvec4(ivec4(a * 4.0f));  // negative lanes keep their bits
    uvec4 v = u * 2654435761u + uvec4(b * b * 16.0f);
    v = v - uvec4(7u, 0xFFFFFFFFu, 3u, 0x80000000u);
    v = v / uvec4(3u, 7u, 0x10000u, 5u) + v % uvec4(10u, 1000u, 3u, 0x7FFFFFFFu);
    return uintBitsToFloat(v);
}

vec4 case_bits_int_bitwise(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 4.0f);
    ivec4 j = ivec4(b * 4.0f);
    uvec4 u = uvec4(i) * 2654435761u;
    uvec4 v = ((u >> 7) ^ (u << 3)) | (uvec4(j) & 0xFFFFu);
    v = ~v + (u >> uvec4(1u, 5u, 13u, 31u)) + (u << 4u);
    ivec4 s = (i << 3) + (j >> 1) + (j >> ivec4(0, 2, 4, 1)) + (~i & j) + (i ^ j) + (i | 3);
    return uintBitsToFloat(v + uvec4(s));
}

vec4 case_bits_int_compare(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 4.0f);
    ivec4 j = ivec4(b * 4.0f);
    uvec4 u = uvec4(i);
    ivec4 r = mix(j, i, lessThan(i, j)) * 4 + ivec4(greaterThan(u, uvec4(j)));
    r.x += i == j ? 1 : 0;
    r.y += all(notEqual(i, ivec4(0))) ? 16 : 0;
    r.z += any(equal(i - i, ivec4(1))) ? 32 : 0;
    r.w += i.x * j.z + i.y * j.w;
    return intBitsToFloat(r + ivec4(equal(i, j.yxwz)) * 128 + ivec4(greaterThanEqual(i, ivec4(2))) * 256);
}

vec4 case_bits_int_convert(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 3.3f);  // truncates toward zero
    uvec4 u = uvec4(abs(b) * 1000.5f);
    vec4 f = vec4(i) * 0.5f + vec4(u);
    uvec4 big = uvec4(0xFFFFFFFFu, 0x80000001u, 16777217u, 3u) - uvec4(ivec4(b));
    vec4 g = vec4(big);  // rounds to nearest
    uvec4 bits = floatBitsToUint(a) ^ floatBitsToUint(b);
    ivec4 e;
    vec4 m = frexp(a, e);
    vec4 l = ldexp(m, e + ivec4(1, -2, 3, 0));
    return uintBitsToFloat(floatBitsToUint(f + g) + bits + uvec4(i) + u +
                           floatBitsToUint(vec4(ivec4(-7, 0, 9, -100))) + uvec4(e) +
                           floatBitsToUint(l));
}

// mixed operands and implicit conversions (GLSL widens only: int -> uint,
// int / uint -> float)
vec4 case_bits_int_mixed(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 4.0f);
    uvec4 u = uvec4(abs(b) * 4.0f);
    vec4 f = i + a;
    f += i * 0.5f;
    uvec4 s = i + u;
    vec4 g = u;
    vec4 r = f + g * 0.25f;
    return uintBitsToFloat(floatBitsToUint(r) + s);
}

// integer functions (GLSL 8.8); fields stay inside the 32 bits, where GLSL
// defines them
vec4 case_bits_int_bitfns(vec4 a, vec4 b)
{
    ivec4 i = ivec4(a * 4.0f);                  // 5, -10, 15, 2
    uvec4 u = uvec4(i) * 2654435761u;
    ivec4 r = bitCount(u) + bitCount(i) * 64;
    r += (findMSB(i) << 12) + (findMSB(u) << 18) + (findLSB(u) << 24);
    r ^= ivec4(bitfieldReverse(u)) ^ bitfieldReverse(i);
    r += findMSB(ivec4(0, -1, 1, -2147483647 - 1)) + findLSB(uvec4(0u, 8u, 1u, 0u));
    r += findMSB(ivec4(b * 4.0f));              // -3, 16, 1, -6
    r += bitfieldExtract(i, 1, 3) + ivec4(bitfieldExtract(u, 28, 4));
    uvec4 ins = bitfieldInsert(u, uvec4(i), 8, 12);
    uvec4 carry, borrow, hi, lo;
    uvec4 sum = uaddCarry(u, ins, carry);
    uvec4 diff = usubBorrow(u, ins, borrow);
    umulExtended(u, ins, hi, lo);
    ivec4 shi, slo;
    imulExtended(i * 300000, ivec4(u), shi, slo);
    uvec4 w = sum + diff * 3u + carry * 5u + borrow * 7u + hi + (lo >> 1) + uvec4(shi) + uvec4(slo);
    return uintBitsToFloat(uvec4(r) ^ w);
}

// packing: inputs avoid what GLSL leaves to the implementation (ties, NaN,
// half overflow and subnormals). packSnorm4x8 has a negative value in its
// last component only: lavapipe (Mesa 25.2) does not mask the others to 8
// bits, so a negative one sets every byte above it (docs/glsl_interop.md)
vec4 case_bits_pack(vec4 a, vec4 b)
{
    vec4 n = a * 0.3f;  // 0.375, -0.75, 1.125, 0.15
    vec4 m = abs(n.yxzw) * vec4(1.0f, 1.0f, 1.0f, -1.0f);
    uvec4 r = uvec4(packUnorm2x16(n.xy), packSnorm2x16(n.yw), packUnorm4x8(n), packSnorm4x8(m));
    r ^= uvec4(packHalf2x16(a.xy), packHalf2x16(b.zw * 1000.1f), packHalf2x16(vec2(0.001f, 60000.0f)), 0u);
    return uintBitsToFloat(r);
}

vec4 case_near_unpack(vec4 a, vec4 b)
{
    vec2 h = unpackHalf2x16(floatBitsToUint(a.x) ^ floatBitsToUint(b.y));
    vec4 u = unpackUnorm4x8(0x12345678u) + unpackSnorm4x8(0x80FF7F01u);
    vec2 s = unpackUnorm2x16(0xFFFF1234u) + unpackSnorm2x16(0x80007FFFu);
    return u + vec4(h, s);
}
