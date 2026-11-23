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

// the bitfield functions on 16-bit integers (computed as int), the 16-bit
// casts and the 16- / 32-bit packing functions. bitCount / findMSB / findLSB
// at 16 and 64 bits are left out: glslang (15.1) emits OpBitCount and
// GLSL.std.450 FindSMsb / FindUMsb / FindILsb on them, which Vulkan's SPIR-V
// limits to 32 bits (spirv-val rejects them; lavapipe's 64-bit findMSB sees
// the low 32 bits only); see docs/glsl_interop.md.
vec4 case_bits_width_bitfns(vec4 af, vec4 bf)
{
    i16vec4 s = i16vec4(af * 4000.0f) - int16_t(7000);  // -2000, -17000, 8000, -5000
    u16vec4 u = u16vec4(s) ^ uint16_t(0x5A5A);
    ivec4 r = ivec4(s) * 77777;
    r ^= bitfieldExtract(s, 3, 9) + bitfieldExtract(u, 4, 8) * 3 + bitfieldReverse(u) +
         bitfieldInsert(s, u, 5, 6);
    i16vec4 hb = float16BitsToInt16(f16vec4(af)) ^ int16_t(0x1234);
    f16vec4 hf = uint16BitsToFloat16(float16BitsToUint16(f16vec4(bf)) + uint16_t(1));
    uint p = packFloat2x16(hf.xy) ^ packUint2x16(u.zw) ^ uint(packInt2x16(s.xy));
    i64vec2 q = i64vec2(packInt4x16(s), int64_t(packUint4x16(u))) ^
                i64vec2(packInt2x32(r.xy), int64_t(packUint2x32(uvec2(r.zw))));
    i16vec4 us = unpackInt4x16(q.x) + i16vec4(unpackUint4x16(uint64_t(q.y)));
    ivec2 uq = unpackInt2x32(q.y) + ivec2(unpackUint2x32(uint64_t(q.x)));
    f16vec2 uh = unpackFloat2x16(p);
    i16vec2 u2 = unpackInt2x16(int(p)) + i16vec2(unpackUint2x16(p ^ 0x80008000u));
    r ^= ivec4(us) + ivec4(uq, ivec2(u2)) + (ivec4(hb) << 9);
    return uintBitsToFloat(uvec4(r) ^ floatBitsToUint(vec4(hf)) ^ floatBitsToUint(vec4(vec2(uh), vec2(uh))));
}

// a 16-bit vector with an int literal computes as int (2 is an int): no
// wrap at 16 bits
vec4 case_bits_int16_literal(vec4 af, vec4 bf)
{
    i16vec4 s = i16vec4(af * 8000.0f);  // 10000, -20000, 30000, 4000
    u16vec4 u = u16vec4(abs(bf) * 5000.0f);  // 3750, 20000, 1250, 7500
    ivec4 w = s * 3 + (u * 4 - 7);
    return uintBitsToFloat(uvec4(w));
}

// 64-bit double vectors, exact fma, packing and 64-bit bitcasts
vec4 case_bits_double_ops(vec4 af, vec4 bf)
{
    dvec4 a = dvec4(af) + dvec4(double(1.0f) / double(1048576.0f));
    dvec4 b = dvec4(bf) - dvec4(double(1.0f) / double(524288.0f));
    dvec4 r = fma(a, b, dvec4(af) * double(2.0f));
    r = floor(r) + ceil(a) + trunc(b) + roundEven(dvec4(0.5f, 1.5f, 2.5f, -2.5f));
    dvec4 ip;
    dvec4 fr = modf(a * double(1.5f), ip);
    ivec4 ex;
    dvec4 mn = frexp(b, ex);
    dvec4 rebuilt = ldexp(mn, ex);
    r = r + fr + ip + rebuilt + mix(a, bf, lessThan(a, b));
    double pk = packDouble2x32(unpackDouble2x32(r.x) ^ uvec2(0u, 0x00010000u));
    u64vec4 ub = doubleBitsToUint64(dvec4(pk, r.y, r.z, r.w));
    i64vec4 ib = doubleBitsToInt64(uint64BitsToDouble(ub));
    dvec4 rt = int64BitsToDouble(ib);
    u64vec4 fin = doubleBitsToUint64(rt);
    uvec4 lo = uvec4(fin & uint64_t(0xFFFFFFFFu));
    uvec4 hi = uvec4(fin >> uint64_t(32));
    return uintBitsToFloat(lo ^ (hi << 1u));
}

vec4 case_near_double_matrix(vec4 af, vec4 bf)
{
    dmat3 dm = dmat3(double(af.x), double(af.y), double(af.z),
                     double(af.w), double(2.0f), double(-1.0f),
                     double(0.5f), double(-0.25f), double(3.0f));
    mat3 fm = mat3(1.0f, 0.25f, -0.5f,
                   0.5f, 1.5f, 0.25f,
                   -0.25f, 0.5f, 2.0f);
    dmat3 prod = dm * fm + transpose(dm);
    dmat3 inv = inverse(dm);
    dvec3 v = prod * bf.xyz + outerProduct(dvec3(af.xyz), bf.xyz)[1];
    return vec4(float(determinant(dm)), float((inv * dm)[1][1]),
                float(dot(v, dvec3(bf.xyz))), float(length(v)));
}
