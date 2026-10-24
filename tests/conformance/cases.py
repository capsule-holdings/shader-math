# Conformance corpus: (id, category, body). The body is pasted verbatim into
# a real shader function and into a C++ function with the same parameters.
# Truth (accept / reject) comes from the real compiler (expected.tsv), not
# from this file. Category "out_of_scope" holds what shader-math does not
# cover (int / double types, bit casts, derivatives).

H_PARAMS = ("float s, float t, float2 a2, float2 b2, float3 a, float3 b, float3 c, "
            "float4 a4, float4 b4, float3x3 m, float3x3 n, float4x4 m4, float2x3 m23, "
            "float3x2 m32, bool3 bb, half3 h, half hs")
G_PARAMS = ("float s, float t, vec2 a2, vec2 b2, vec3 a, vec3 b, vec3 c, vec4 a4, vec4 b4, "
            "mat3 m, mat3 n, mat4 m4, mat2x3 m23, mat3x2 m32, bvec3 bb, f16vec3 h, float16_t hs")
# On ARM, arm_neon.h declares a global float16_t that makes the unqualified
# name ambiguous in C++ (a platform matter, not a language one), so the C++
# side names glsl::float16_t explicitly
G_CPP_PARAMS = G_PARAMS.replace("float16_t hs", "shader_math::glsl::float16_t hs")
G_CPP_PRELUDE = "using shader_math::glsl::float16_t;"

HLSL = []
GLSL = []


def H(cat, cid, body):
    HLSL.append((cid, cat, body))


def G(cat, cid, body):
    GLSL.append((cid, cat, body))


# ============================ HLSL ===========================================
# constructors / conversions
for cid, body in [
    ("ctor_scalars", "float3 r = float3(1.0, 2.0, 3.0);"),
    ("ctor_int_args", "float3 r = float3(1, 2, 3);"),
    ("ctor_vec_scalar", "float3 r = float3(a2, 1.0);"),
    ("ctor_scalar_vec", "float4 r = float4(1.0, a);"),
    ("ctor_vec_vec", "float4 r = float4(a2, b2);"),
    ("ctor_scalar_vec_scalar", "float4 r = float4(s, a2, t);"),
    ("ctor_single_scalar", "float3 r = float3(s);"),
    ("ctor_too_many", "float3 r = float3(a2, b2);"),
    ("ctor_too_few", "float3 r = float3(a2);"),
    ("ctor_from_wider", "float3 r = float3(a4);"),
    ("implicit_splat_var", "float3 r = s;"),
    ("implicit_splat_lit", "float3 r = 1.0;"),
    ("assign_splat", "float3 r = a; r = 0.0;"),
    ("brace_init", "float3 r = {1.0, 2.0, 3.0};"),
    ("brace_init_flatten", "float4 r = {a, 1.0};"),
    ("implicit_trunc_init", "float3 r = a4;"),
    ("implicit_widen", "float4 r = a;"),
    ("cast_trunc", "float3 r = (float3)a4;"),
    ("cast_splat", "float3 r = (float3)s;"),
    ("vector_template", "vector<float, 3> r = a;"),
    ("vector_bare", "vector r = a4;"),
    ("matrix_template", "matrix<float, 3, 3> r = m;"),
    ("float1", "float1 r = s;"),
    ("half_explicit", "half3 r = half3(a);"),
    ("half_implicit_from_float", "half3 r = a;"),
    ("half_times_float", "half3 r = h * a;"),
    ("half_times_lit", "half3 r = h * 2.0;"),
    ("half_cast_lit", "half3 r = h * (half)2.0;"),
    ("float_from_half_explicit", "float3 r = float3(h);"),
    ("float_from_half_implicit", "float3 r = h;"),
    ("bool_from_float_explicit", "bool3 r = bool3(a);"),
    ("bool_from_float_implicit", "bool3 r = a;"),
    ("float_from_bool_explicit", "float3 r = float3(bb);"),
    ("float_from_bool_implicit", "float3 r = bb;"),
    ("bool_ctor", "bool3 r = bool3(true, false, true);"),
    ("bool_splat_init", "bool3 r = true;"),
    ("mat_ctor_scalars", "float2x2 r = float2x2(1.0, 2.0, 3.0, 4.0);"),
    ("mat_ctor_rows", "float3x3 r = float3x3(a, b, c);"),
    ("mat_ctor_mixed", "float2x2 r = float2x2(a2, 3.0, 4.0);"),
    ("mat_ctor_single_scalar", "float3x3 r = float3x3(s);"),
    ("mat_ctor_two_scalars", "float2x2 r = float2x2(1.0, 2.0);"),
    ("mat_implicit_splat", "float3x3 r = s;"),
    ("mat_cast_trunc", "float2x2 r = (float2x2)m;"),
    ("mat_ctor_trunc", "float2x2 r = float2x2(m);"),
    ("mat_implicit_trunc", "float2x2 r = m;"),
    ("mat_cast_widen", "float4x4 r = (float4x4)m;"),
    ("mat_brace", "float2x2 r = {1.0, 2.0, 3.0, 4.0};"),
    ("half_mat", "half3x3 r = (half3x3)m;"),
]:
    H("ctor", cid, body)

# swizzles / indexing
for cid, body in [
    ("swz_xy", "float2 r = a.xy;"),
    ("swz_zyx", "float3 r = a.zyx;"),
    ("swz_rgb", "float3 r = a4.rgb;"),
    ("swz_dup", "float3 r = a.xxx;"),
    ("swz_mixed_sets", "float2 r = a.xg;"),
    ("swz_stpq", "float2 r = a.st;"),
    ("swz_out_of_range", "float r = a2.z;"),
    ("swz_write", "float3 r = a; r.xy = b2;"),
    ("swz_write_dup", "float3 r = a; r.xx = b2;"),
    ("swz_write_scalar", "float3 r = a; r.x = 1.0;"),
    ("swz_compound", "float3 r = a; r.yz += b2;"),
    ("swz_write_vec", "float4 r = a4; r.xyz = a;"),
    ("swz_on_scalar", "float3 r = s.xxx;"),
    ("swz_nested", "float2 r = a.xy.yx;"),
    ("swz_of_expr", "float2 r = (a + b).xy;"),
    ("swz_component_arg", "float r = max(a.x, b.y);"),
    ("index_read", "float r = a[1];"),
    ("index_write", "float3 r = a; r[1] = 2.0;"),
    ("mat_row", "float3 r = m[0];"),
    ("mat_elem", "float r = m[0][1];"),
    ("mat_row_swz", "float2 r = m[0].xy;"),
    ("mat_m00", "float r = m._m00;"),
    ("mat_11", "float r = m._11;"),
    ("mat_multi_elem", "float2 r = m._m00_m11;"),
    ("mat_write_elem", "float3x3 r = m; r._m01 = 1.0;"),
    ("mat_write_row", "float3x3 r = m; r[1] = a;"),
    ("bool_component", "bool r = bb.x;"),
    ("bool_index", "bool r = bb[1];"),
]:
    H("swizzle", cid, body)

# operators
for cid, body in [
    ("add", "float3 r = a + b;"),
    ("sub", "float3 r = a - b;"),
    ("mul", "float3 r = a * b;"),
    ("div", "float3 r = a / b;"),
    ("vec_times_scalar", "float3 r = a * s;"),
    ("scalar_times_vec", "float3 r = s * a;"),
    ("vec_times_int", "float3 r = a * 2;"),
    ("vec_plus_lit", "float3 r = a + 1.0;"),
    ("neg", "float3 r = -a;"),
    ("unary_plus", "float3 r = +a;"),
    ("mod_vec", "float3 r = a % b;"),
    ("mod_scalar", "float3 r = a % s;"),
    ("size_mismatch_trunc", "float3 r = a + a4;"),
    ("size_mismatch_2_3", "float2 r = a2 + a;"),
    ("cmp_eq", "bool3 r = a == b;"),
    ("cmp_lt", "bool3 r = a < b;"),
    ("cmp_scalar", "bool3 r = a < 1.0;"),
    ("if_vector_cond", "float3 r = a; if (a == b) { r = b; }"),
    ("if_all", "float3 r = a; if (all(a == b)) { r = b; }"),
    ("logical_and_vec", "bool3 r = bb && bb;"),
    ("logical_not_vec", "bool3 r = !bb;"),
    ("bool_eq", "bool3 r = bb == bb;"),
    ("ternary_scalar_cond", "float3 r = s > 0.0 ? a : b;"),
    ("ternary_vector_cond", "float3 r = bb ? a : b;"),
    ("compound_add", "float3 r = a; r += b;"),
    ("compound_mul_scalar", "float3 r = a; r *= s;"),
    ("compound_div", "float3 r = a; r /= b;"),
    ("compound_mod", "float3 r = a; r %= b;"),
    ("increment", "float3 r = a; r++;"),
    ("mat_hadamard", "float3x3 r = m * n;"),
    ("mat_add", "float3x3 r = m + n;"),
    ("mat_scalar", "float3x3 r = m * s;"),
    ("scalar_mat", "float3x3 r = s * m;"),
    ("mat_neg", "float3x3 r = -m;"),
    ("mat_div", "float3x3 r = m / n;"),
    ("mat_cmp", "bool3x3 r = m == n;"),
    ("mat_mod", "float3x3 r = m % n;"),
    ("mat_times_vec_star", "float3 r = m * a;"),
    ("mat_shape_mismatch_star", "float2x3 r = m23 * m32;"),
    ("mul_mm", "float3x3 r = mul(m, n);"),
    ("mul_vm", "float3 r = mul(a, m);"),
    ("mul_mv", "float3 r = mul(m, a);"),
    ("mul_sm", "float3x3 r = mul(s, m);"),
    ("mul_ms", "float3x3 r = mul(m, s);"),
    ("mul_vv", "float r = mul(a, b);"),
    ("mul_sv", "float3 r = mul(s, a);"),
    ("mul_ss", "float r = mul(s, t);"),
    ("mul_nonsquare", "float2x2 r = mul(m23, m32);"),
    ("mul_shape_mismatch", "float3x3 r = mul(m, m23);"),
    ("mul_vec_size_mismatch", "float3 r = mul(a4, m);"),
    ("mat_compound", "float3x3 r = m; r *= n;"),
]:
    H("operator", cid, body)

# intrinsics: vector forms
UN_V = ["abs", "acos", "asin", "atan", "ceil", "cos", "cosh", "degrees", "exp", "exp2", "floor",
        "frac", "log", "log10", "log2", "normalize", "radians", "rcp", "round", "rsqrt",
        "saturate", "sign", "sin", "sinh", "sqrt", "tan", "tanh", "trunc"]
for f in UN_V:
    H("intrinsic", f"v_{f}", f"float3 r = {f}(a);")
for cid, body in [
    ("v_atan2", "float3 r = atan2(a, b);"),
    ("v_clamp", "float3 r = clamp(a, b, c);"),
    ("v_clamp_scalar", "float3 r = clamp(a, 0.0, 1.0);"),
    ("v_cross", "float3 r = cross(a, b);"),
    ("v_distance", "float r = distance(a, b);"),
    ("v_dot", "float r = dot(a, b);"),
    ("v_dst", "float4 r = dst(a4, b4);"),
    ("v_faceforward", "float3 r = faceforward(a, b, c);"),
    ("v_fmod", "float3 r = fmod(a, b);"),
    ("v_frexp", "float3 e; float3 r = frexp(a, e);"),
    ("v_isfinite", "bool3 r = isfinite(a);"),
    ("v_isinf", "bool3 r = isinf(a);"),
    ("v_isnan", "bool3 r = isnan(a);"),
    ("v_ldexp", "float3 r = ldexp(a, b);"),
    ("v_length", "float r = length(a);"),
    ("v_lerp", "float3 r = lerp(a, b, c);"),
    ("v_lerp_scalar_t", "float3 r = lerp(a, b, 0.5);"),
    ("v_lit", "float4 r = lit(s, t, 2.0);"),
    ("v_mad", "float3 r = mad(a, b, c);"),
    ("v_max", "float3 r = max(a, b);"),
    ("v_max_scalar", "float3 r = max(a, 0.0);"),
    ("v_min", "float3 r = min(a, b);"),
    ("v_modf", "float3 ip; float3 r = modf(a, ip);"),
    ("v_pow", "float3 r = pow(a, b);"),
    ("v_pow_scalar", "float3 r = pow(a, 2.0);"),
    ("v_reflect", "float3 r = reflect(a, b);"),
    ("v_refract", "float3 r = refract(a, b, 0.5);"),
    ("v_select", "float3 r = select(bb, a, b);"),
    ("v_select_scalar_cond", "float3 r = select(s > 0.0, a, b);"),
    ("v_sincos", "float3 sn; float3 cs; sincos(a, sn, cs);"),
    ("v_smoothstep", "float3 r = smoothstep(a, b, c);"),
    ("v_smoothstep_scalar", "float3 r = smoothstep(0.0, 1.0, a);"),
    ("v_step", "float3 r = step(a, b);"),
    ("v_step_scalar", "float3 r = step(0.5, a);"),
    ("v_all_numeric", "bool r = all(a);"),
    ("v_any_numeric", "bool r = any(a);"),
    ("v_all_bool", "bool r = all(bb);"),
    ("v_any_bool", "bool r = any(bb);"),
    ("v_fma_float", "float3 r = fma(a, b, c);"),
    ("v_noise", "float r = noise(a);"),
    ("g_mix", "float3 r = mix(a, b, c);"),
    ("g_fract", "float3 r = fract(a);"),
    ("g_mod", "float3 r = mod(a, b);"),
    ("g_inversesqrt", "float3 r = inversesqrt(a);"),
    ("g_roundEven", "float3 r = roundEven(a);"),
    ("g_lessThan", "bool3 r = lessThan(a, b);"),
    ("g_atan_yx", "float3 r = atan(a, b);"),
    ("half_lerp", "half3 r = lerp(h, h, (half)0.5);"),
    ("half_sqrt", "half3 r = sqrt(h);"),
    ("half_dot", "half r = dot(h, h);"),
]:
    H("intrinsic", cid, body)

# intrinsics: scalar forms
for cid, body in [
    ("s_abs", "float r = abs(s);"),
    ("s_sqrt", "float r = sqrt(s);"),
    ("s_max", "float r = max(s, t);"),
    ("s_max_lit", "float r = max(s, 0.0);"),
    ("s_clamp", "float r = clamp(s, 0.0, 1.0);"),
    ("s_lerp", "float r = lerp(s, t, 0.5);"),
    ("s_saturate", "float r = saturate(s);"),
    ("s_frac", "float r = frac(s);"),
    ("s_pow", "float r = pow(s, t);"),
    ("s_sign", "float r = sign(s);"),
    ("s_step", "float r = step(s, t);"),
    ("s_smoothstep", "float r = smoothstep(0.0, 1.0, s);"),
    ("s_round", "float r = round(s);"),
    ("s_isnan", "bool r = isnan(s);"),
    ("s_isfinite", "bool r = isfinite(s);"),
    ("s_dot", "float r = dot(s, t);"),
    ("s_length", "float r = length(s);"),
    ("s_atan2", "float r = atan2(s, t);"),
    ("s_fmod", "float r = fmod(s, t);"),
    ("s_rsqrt", "float r = rsqrt(s);"),
    ("s_select", "float r = select(s > 0.0, s, t);"),
    ("s_all", "bool r = all(s);"),
    ("s_sincos", "float sn; float cs; sincos(s, sn, cs);"),
    ("s_modf", "float ip; float r = modf(s, ip);"),
    ("s_component", "float r = sqrt(a.x) + saturate(a.y);"),
    ("s_mat_elem", "float r = abs(m._m00);"),
    ("s_double_lit_max", "float r = max(1.0, 2.0);"),
    ("s_normalize", "float r = normalize(s);"),
]:
    H("scalar", cid, body)

# intrinsics: matrix forms
for cid, body in [
    ("m_transpose", "float3x3 r = transpose(m);"),
    ("m_transpose_nonsquare", "float3x2 r = transpose(m23);"),
    ("m_determinant", "float r = determinant(m);"),
    ("m_inverse", "float3x3 r = inverse(m);"),
    ("m_abs", "float3x3 r = abs(m);"),
    ("m_sin", "float3x3 r = sin(m);"),
    ("m_lerp", "float3x3 r = lerp(m, n, 0.5);"),
    ("m_max", "float3x3 r = max(m, n);"),
    ("m_clamp_scalar", "float3x3 r = clamp(m, 0.0, 1.0);"),
    ("m_saturate", "float3x3 r = saturate(m);"),
    ("m_isnan", "bool3x3 r = isnan(m);"),
    ("m_all", "bool r = all(m);"),
    ("m_any_cmp", "bool r = any(m == n);"),
    ("m_dot_err", "float r = dot(m, n);"),
    ("m_length_err", "float r = length(m);"),
]:
    H("matrix_fn", cid, body)

# out of scope (int / bit casts / derivatives / textures)
for cid, body in [
    ("oos_int3", "int3 r = int3(1, 2, 3);"),
    ("oos_uint", "uint r = 1u;"),
    ("oos_asuint", "uint3 r = asuint(a);"),
    ("oos_asfloat", "float3 r = asfloat(uint3(1, 2, 3));"),
    ("oos_f32tof16", "uint r = f32tof16(s);"),
    ("oos_double", "double r = 1.0L;"),
    ("oos_countbits", "uint r = countbits(1u);"),
    ("oos_sign_int", "int3 r = sign(a);"),
    ("oos_ddx", "float3 r = ddx(a);"),
]:
    H("out_of_scope", cid, body)

# boundaries found while checking the constructor and operator additions
for cid, body in [
    ("mat_ctor_from_vec4", "float2x2 r = float2x2(a4);"),
    ("mat_implicit_from_vec4", "float2x2 r = a4;"),
    ("mat_ctor_v3_s", "float2x2 r = float2x2(a, s);"),
    ("mat_ctor_v3_v3", "float2x2 r = float2x2(a, a);"),
    ("mat_ctor_rows_v2", "float2x2 r = float2x2(a2, b2);"),
    ("mat_ctor_v2_v4", "float2x3 r = float2x3(a2, a4);"),
    ("mat_brace_mixed", "float2x2 r = {a2, 3.0, 4.0};"),
    ("mat_ctor_half_vec", "float2x2 r = float2x2(a2, h.xy);"),
    ("ctor_half_vec", "float4 r = float4(a2, h.xy);"),
    ("vec_from_mat_implicit", "float2x2 k = a4; float4 r = k;"),
    ("bool_assign_scalar", "bool3 r = bb; r = false;"),
]:
    H("ctor", cid, body)
for cid, body in [
    ("bool_component_write", "bool3 r = bb; r.x = true;"),
]:
    H("swizzle", cid, body)
for cid, body in [
    ("s_all_bool", "bool r = all(s > 0.0);"),
]:
    H("scalar", cid, body)
for cid, body in [
    ("swz_increment", "float3 r = a; r.x++;"),
]:
    H("swizzle", cid, body)
for cid, body in [
    ("bool_unary_plus", "bool3 r = +bb;"),
    ("decrement_prefix", "float3 r = a; --r;"),
    ("mat_increment", "float3x3 r = m; r++;"),
    ("mat_unary_plus", "float3x3 r = +m;"),
    ("vec_compound_mat", "float3 r = a; r *= m;"),
    ("mat_compound_vec", "float3x3 r = m; r *= a;"),
]:
    H("operator", cid, body)
for cid, body in [
    ("s_reflect", "float r = reflect(s, t);"),
    ("s_refract", "float r = refract(s, t, 0.5);"),
    ("s_faceforward", "float r = faceforward(s, t, s);"),
]:
    H("scalar", cid, body)

# ============================ GLSL ===========================================
for cid, body in [
    ("ctor_scalars", "vec3 r = vec3(1.0, 2.0, 3.0);"),
    ("ctor_int_args", "vec3 r = vec3(1, 2, 3);"),
    ("ctor_vec_scalar", "vec3 r = vec3(a2, 1.0);"),
    ("ctor_scalar_vec", "vec4 r = vec4(1.0, a);"),
    ("ctor_vec_vec", "vec4 r = vec4(a2, b2);"),
    ("ctor_scalar_vec_scalar", "vec4 r = vec4(s, a2, t);"),
    ("ctor_single_scalar", "vec3 r = vec3(s);"),
    ("ctor_too_many", "vec3 r = vec3(a2, b2);"),
    ("ctor_too_few", "vec3 r = vec3(a2);"),
    ("ctor_from_wider", "vec3 r = vec3(a4);"),
    ("implicit_splat_var", "vec3 r = s;"),
    ("implicit_splat_lit", "vec3 r = 1.0;"),
    ("assign_splat", "vec3 r = a; r = 0.0;"),
    ("brace_init", "vec3 r = {1.0, 2.0, 3.0};"),
    ("implicit_trunc_init", "vec3 r = a4;"),
    ("implicit_widen", "vec4 r = a;"),
    ("int_lit_suffix_f", "vec3 r = a * 2.0f;"),
    ("half_explicit", "f16vec3 r = f16vec3(a);"),
    ("half_implicit_from_float", "f16vec3 r = a;"),
    ("float_from_half_implicit", "vec3 r = h;"),
    ("half_times_float", "vec3 r = h * a;"),
    ("bvec_from_vec", "bvec3 r = bvec3(a);"),
    ("vec_from_bvec", "vec3 r = vec3(bb);"),
    ("vec_from_bvec_implicit", "vec3 r = bb;"),
    ("bvec_ctor", "bvec3 r = bvec3(true, false, true);"),
    ("mat_ctor_scalars", "mat2 r = mat2(1.0, 2.0, 3.0, 4.0);"),
    ("mat_ctor_cols", "mat3 r = mat3(a, b, c);"),
    ("mat_ctor_mixed", "mat2 r = mat2(a2, 3.0, 4.0);"),
    ("mat_ctor_single_scalar", "mat3 r = mat3(s);"),
    ("mat_ctor_two_scalars", "mat2 r = mat2(1.0, 2.0);"),
    ("mat_implicit_splat", "mat3 r = s;"),
    ("mat_from_bigger", "mat3 r = mat3(m4);"),
    ("mat_from_smaller", "mat4 r = mat4(m);"),
    ("mat_implicit_trunc", "mat2 r = m;"),
    ("mat_from_vec4", "mat2 r = mat2(a4);"),
    ("mat_nonsquare_ctor", "mat2x3 r = mat2x3(a, b);"),
    ("half_mat", "f16mat3 r = f16mat3(m);"),
    ("cast_c_style", "vec3 r = (vec3)a4;"),
]:
    G("ctor", cid, body)

for cid, body in [
    ("swz_xy", "vec2 r = a.xy;"),
    ("swz_zyx", "vec3 r = a.zyx;"),
    ("swz_rgb", "vec3 r = a4.rgb;"),
    ("swz_stp", "vec3 r = a4.stp;"),
    ("swz_dup", "vec3 r = a.xxx;"),
    ("swz_mixed_sets", "vec2 r = a.xg;"),
    ("swz_out_of_range", "float r = a2.z;"),
    ("swz_write", "vec3 r = a; r.xy = b2;"),
    ("swz_write_dup", "vec3 r = a; r.xx = b2;"),
    ("swz_write_stpq", "vec3 r = a; r.st = b2;"),
    ("swz_compound", "vec3 r = a; r.yz += b2;"),
    ("swz_on_scalar", "vec3 r = s.xxx;"),
    ("swz_nested", "vec2 r = a.xy.yx;"),
    ("swz_of_expr", "vec2 r = (a + b).xy;"),
    ("swz_component_arg", "float r = max(a.x, b.y);"),
    ("index_read", "float r = a[1];"),
    ("index_write", "vec3 r = a; r[1] = 2.0;"),
    ("length_method", "int r = a.length();"),
    ("mat_length_method", "int r = m.length();"),
    ("mat_col", "vec3 r = m[0];"),
    ("mat_elem", "float r = m[0][1];"),
    ("mat_col_swz", "vec2 r = m[0].xy;"),
    ("mat_write_col", "mat3 r = m; r[1] = a;"),
    ("mat_hlsl_elem", "float r = m._m00;"),
    ("bvec_component", "bool r = bb.x;"),
    ("bvec_index", "bool r = bb[1];"),
]:
    G("swizzle", cid, body)

for cid, body in [
    ("add", "vec3 r = a + b;"),
    ("sub", "vec3 r = a - b;"),
    ("mul", "vec3 r = a * b;"),
    ("div", "vec3 r = a / b;"),
    ("vec_times_scalar", "vec3 r = a * s;"),
    ("scalar_times_vec", "vec3 r = s * a;"),
    ("vec_times_int", "vec3 r = a * 2;"),
    ("vec_plus_lit", "vec3 r = a + 1.0;"),
    ("neg", "vec3 r = -a;"),
    ("unary_plus", "vec3 r = +a;"),
    ("mod_op", "vec3 r = a % b;"),
    ("size_mismatch", "vec3 r = a + a4;"),
    ("cmp_eq", "bool r = a == b;"),
    ("cmp_ne", "bool r = a != b;"),
    ("cmp_lt", "bool r = a < b;"),
    ("if_vector_eq", "vec3 r = a; if (a == b) { r = b; }"),
    ("bvec_eq", "bool r = bb == bb;"),
    ("logical_not_bvec", "bvec3 r = !bb;"),
    ("logical_and_bvec", "bool r = bb && bb;"),
    ("ternary_scalar_cond", "vec3 r = s > 0.0 ? a : b;"),
    ("ternary_bvec_cond", "vec3 r = bb ? a : b;"),
    ("compound_add", "vec3 r = a; r += b;"),
    ("compound_mul_scalar", "vec3 r = a; r *= s;"),
    ("increment", "vec3 r = a; r++;"),
    ("mat_mul", "mat3 r = m * n;"),
    ("mat_vec", "vec3 r = m * a;"),
    ("vec_mat", "vec3 r = a * m;"),
    ("mat_add", "mat3 r = m + n;"),
    ("mat_scalar", "mat3 r = m * s;"),
    ("scalar_mat", "mat3 r = s * m;"),
    ("mat_neg", "mat3 r = -m;"),
    ("mat_div", "mat3 r = m / n;"),
    ("mat_eq", "bool r = m == n;"),
    ("mat_lt", "bool r = m[0][0] < n[0][0];"),
    ("mat_nonsquare_mul", "mat3 r = m23 * m32;"),
    ("mat_shape_mismatch", "mat3 r = m * m23;"),
    ("mat_compound_mul", "mat3 r = m; r *= n;"),
    ("vec_compound_mat", "vec3 r = a; r *= m;"),
    ("mat_vec_size_mismatch", "vec3 r = m * a4;"),
]:
    G("operator", cid, body)

UN_G = ["abs", "acos", "acosh", "asin", "asinh", "atan", "atanh", "ceil", "cos", "cosh", "degrees",
        "exp", "exp2", "floor", "fract", "inversesqrt", "log", "log2", "normalize", "radians",
        "round", "roundEven", "sign", "sin", "sinh", "sqrt", "tan", "tanh", "trunc"]
for f in UN_G:
    G("intrinsic", f"v_{f}", f"vec3 r = {f}(a);")
for cid, body in [
    ("v_atan_yx", "vec3 r = atan(a, b);"),
    ("v_clamp", "vec3 r = clamp(a, b, c);"),
    ("v_clamp_scalar", "vec3 r = clamp(a, 0.0, 1.0);"),
    ("v_cross", "vec3 r = cross(a, b);"),
    ("v_distance", "float r = distance(a, b);"),
    ("v_dot", "float r = dot(a, b);"),
    ("v_faceforward", "vec3 r = faceforward(a, b, c);"),
    ("v_fma", "vec3 r = fma(a, b, c);"),
    ("v_isinf", "bvec3 r = isinf(a);"),
    ("v_isnan", "bvec3 r = isnan(a);"),
    ("v_length", "float r = length(a);"),
    ("v_max", "vec3 r = max(a, b);"),
    ("v_max_scalar", "vec3 r = max(a, 0.0);"),
    ("v_min", "vec3 r = min(a, b);"),
    ("v_mix", "vec3 r = mix(a, b, c);"),
    ("v_mix_scalar", "vec3 r = mix(a, b, 0.5);"),
    ("v_mix_bvec", "vec3 r = mix(a, b, bb);"),
    ("v_mod", "vec3 r = mod(a, b);"),
    ("v_mod_scalar", "vec3 r = mod(a, 2.0);"),
    ("v_modf", "vec3 ip; vec3 r = modf(a, ip);"),
    ("v_pow", "vec3 r = pow(a, b);"),
    ("v_reflect", "vec3 r = reflect(a, b);"),
    ("v_refract", "vec3 r = refract(a, b, 0.5);"),
    ("v_smoothstep", "vec3 r = smoothstep(a, b, c);"),
    ("v_smoothstep_scalar", "vec3 r = smoothstep(0.0, 1.0, a);"),
    ("v_step", "vec3 r = step(a, b);"),
    ("v_step_scalar", "vec3 r = step(0.5, a);"),
    ("v_lessThan", "bvec3 r = lessThan(a, b);"),
    ("v_lessThanEqual", "bvec3 r = lessThanEqual(a, b);"),
    ("v_greaterThan", "bvec3 r = greaterThan(a, b);"),
    ("v_greaterThanEqual", "bvec3 r = greaterThanEqual(a, b);"),
    ("v_equal", "bvec3 r = equal(a, b);"),
    ("v_notEqual", "bvec3 r = notEqual(a, b);"),
    ("v_any", "bool r = any(bb);"),
    ("v_all", "bool r = all(bb);"),
    ("v_not", "bvec3 r = not(bb);"),
    ("v_all_numeric", "bool r = all(a);"),
    ("v_frexp", "ivec3 e; vec3 r = frexp(a, e);"),
    ("v_ldexp", "vec3 r = ldexp(a, ivec3(1));"),
    ("h_lerp", "vec3 r = lerp(a, b, c);"),
    ("h_frac", "vec3 r = frac(a);"),
    ("h_saturate", "vec3 r = saturate(a);"),
    ("h_rsqrt", "vec3 r = rsqrt(a);"),
    ("h_fmod", "vec3 r = fmod(a, b);"),
    ("h_atan2", "vec3 r = atan2(a, b);"),
    ("h_mul", "mat3 r = mul(m, n);"),
    ("h_isfinite", "bvec3 r = isfinite(a);"),
    ("half_mix", "f16vec3 r = mix(h, h, float16_t(0.5));"),
    ("half_sqrt", "f16vec3 r = sqrt(h);"),
    ("half_dot", "float16_t r = dot(h, h);"),
]:
    G("intrinsic", cid, body)

for cid, body in [
    ("s_abs", "float r = abs(s);"),
    ("s_sqrt", "float r = sqrt(s);"),
    ("s_max", "float r = max(s, t);"),
    ("s_max_lit", "float r = max(s, 0.0);"),
    ("s_clamp", "float r = clamp(s, 0.0, 1.0);"),
    ("s_mix", "float r = mix(s, t, 0.5);"),
    ("s_mix_bool", "float r = mix(s, t, true);"),
    ("s_fract", "float r = fract(s);"),
    ("s_pow", "float r = pow(s, t);"),
    ("s_sign", "float r = sign(s);"),
    ("s_step", "float r = step(s, t);"),
    ("s_smoothstep", "float r = smoothstep(0.0, 1.0, s);"),
    ("s_round", "float r = round(s);"),
    ("s_isnan", "bool r = isnan(s);"),
    ("s_dot", "float r = dot(s, t);"),
    ("s_length", "float r = length(s);"),
    ("s_atan_yx", "float r = atan(s, t);"),
    ("s_mod", "float r = mod(s, t);"),
    ("s_inversesqrt", "float r = inversesqrt(s);"),
    ("s_modf", "float ip; float r = modf(s, ip);"),
    ("s_normalize", "float r = normalize(s);"),
    ("s_component", "float r = sqrt(a.x) + fract(a.y);"),
    ("s_double_lit_max", "float r = max(1.0, 2.0);"),
]:
    G("scalar", cid, body)

for cid, body in [
    ("m_transpose", "mat3 r = transpose(m);"),
    ("m_transpose_nonsquare", "mat3x2 r = transpose(m23);"),
    ("m_determinant", "float r = determinant(m);"),
    ("m_inverse", "mat3 r = inverse(m);"),
    ("m_compmult", "mat3 r = matrixCompMult(m, n);"),
    ("m_outer", "mat3 r = outerProduct(a, b);"),
    ("m_outer_nonsquare", "mat2x3 r = outerProduct(a, b2);"),
    ("m_abs_err", "mat3 r = abs(m);"),
    ("m_sin_err", "mat3 r = sin(m);"),
    ("m_max_err", "mat3 r = max(m, n);"),
]:
    G("matrix_fn", cid, body)

for cid, body in [
    ("oos_ivec3", "ivec3 r = ivec3(1, 2, 3);"),
    ("oos_uint", "uint r = 1u;"),
    ("oos_floatBitsToInt", "ivec3 r = floatBitsToInt(a);"),
    ("oos_packHalf2x16", "uint r = packHalf2x16(a2);"),
    ("oos_double", "double r = 1.0lf;"),
    ("oos_dvec3", "dvec3 r = dvec3(a);"),
    ("oos_bitCount", "int r = bitCount(1);"),
]:
    G("out_of_scope", cid, body)

# boundaries found while checking the constructor and operator additions
for cid, body in [
    ("ctor_vec2_unused_arg", "vec2 r = vec2(a2, s);"),
    ("ctor_s_v2", "vec2 r = vec2(s, a2);"),
    ("ctor_v3_v3", "vec4 r = vec4(a, a);"),
    ("ctor_s_v4", "vec4 r = vec4(s, a4);"),
    ("ctor_v2_v2_s", "vec3 r = vec3(a2, a2, s);"),
    ("ctor_four_scalars", "vec3 r = vec3(s, s, s, s);"),
    ("ctor_bool_arg", "vec3 r = vec3(a2, true);"),
    ("ctor_half_vec", "vec4 r = vec4(a2, h.xy);"),
    ("mat_ctor_v3_v3", "mat2 r = mat2(a, a);"),
    ("mat_ctor_v4_s", "mat2 r = mat2(a4, s);"),
    ("mat_ctor_v3_s", "mat2 r = mat2(a, s);"),
    ("mat_ctor_v4_v4_s", "mat3 r = mat3(a4, a4, s);"),
    ("mat_implicit_from_vec4", "mat2 r = a4;"),
    ("mat_brace_mixed", "mat2 r = {a2, 3.0, 4.0};"),
    ("vec_from_mat", "vec4 r = vec4(mat2(a4));"),
    ("bvec_assign_scalar", "bvec3 r = bb; r = false;"),
]:
    G("ctor", cid, body)
for cid, body in [
    ("bvec_component_write", "bvec3 r = bb; r.x = true;"),
]:
    G("swizzle", cid, body)
for cid, body in [
    ("swz_increment", "vec3 r = a; r.x++;"),
]:
    G("swizzle", cid, body)
for cid, body in [
    ("bvec_unary_plus", "bvec3 r = +bb;"),
    ("decrement_prefix", "vec3 r = a; --r;"),
    ("mat_increment", "mat3 r = m; r++;"),
    ("mat_unary_plus", "mat3 r = +m;"),
    ("mat_compound_vec", "mat3 r = m; r *= a;"),
    ("vec_mod_assign", "vec3 r = a; r %= 2.0;"),
]:
    G("operator", cid, body)
for cid, body in [
    ("s_reflect", "float r = reflect(s, t);"),
    ("s_refract", "float r = refract(s, t, 0.5);"),
    ("s_faceforward", "float r = faceforward(s, t, s);"),
]:
    G("scalar", cid, body)
