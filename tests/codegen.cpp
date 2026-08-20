// Codegen probe kernels for tests/check_codegen.sh.
//
// Runtime tests cannot observe whether unroll<M> really unrolled or whether
// ops::fmadd emitted a fused instruction; this TU exports the fmadd-based
// kernels as extern "C" noinline symbols so the check script can disassemble
// them by name (no mangling) and inspect the instructions.
//
// Running the binary prints the backend tag the script keys its expectations
// on; it performs no numerical checks (test_simd.cpp does that).
#include <cstdio>

#include <shader_math/shader_math.hpp>

#if defined(_MSC_VER)
#  define SM_CG_NOINLINE __declspec(noinline)
#else
#  define SM_CG_NOINLINE __attribute__((noinline))
#endif

namespace hlsl = shader_math::hlsl;
namespace glsl = shader_math::glsl;

extern "C" {

// fmadd-based kernels (bcast + fmadd accumulate over the storage direction).
SM_CG_NOINLINE void sm_cg_hlsl_mul_m4m4(const hlsl::float4x4& a, const hlsl::float4x4& b,
                                        hlsl::float4x4& o) {
    o = hlsl::mul(a, b);
}

SM_CG_NOINLINE void sm_cg_hlsl_mul_v4m4(const hlsl::float4& v, const hlsl::float4x4& m,
                                        hlsl::float4& o) {
    o = hlsl::mul(v, m);
}

SM_CG_NOINLINE void sm_cg_glsl_mul_m4m4(const glsl::mat4& a, const glsl::mat4& b, glsl::mat4& o) {
    o = a * b;
}

SM_CG_NOINLINE void sm_cg_glsl_mul_m4v4(const glsl::mat4& m, const glsl::vec4& v, glsl::vec4& o) {
    o = m * v;
}

#if defined(SHADER_MATH_HAS_HALF)
// half multiply-add: on ARM FEAT_FP16 this must run on 16-bit lanes (.4h).
SM_CG_NOINLINE void sm_cg_half4_mad(const hlsl::half4& a, const hlsl::half4& b,
                                    const hlsl::half4& c, hlsl::half4& o) {
    o = a * b + c;
}

// half matrix product: hops::fmadd path, 16-bit fmla on FEAT_FP16.
SM_CG_NOINLINE void sm_cg_hlsl_mul_h4h4(const hlsl::half4x4& a, const hlsl::half4x4& b,
                                        hlsl::half4x4& o) {
    o = hlsl::mul(a, b);
}
#endif

}  // extern "C"

int main() {
    // Keep the kernels reachable even under aggressive IPO / section GC.
    volatile auto k0 = &sm_cg_hlsl_mul_m4m4;
    volatile auto k1 = &sm_cg_hlsl_mul_v4m4;
    volatile auto k2 = &sm_cg_glsl_mul_m4m4;
    volatile auto k3 = &sm_cg_glsl_mul_m4v4;
    (void)k0;
    (void)k1;
    (void)k2;
    (void)k3;

#if defined(SHADER_MATH_ISA_NEON)
#  if defined(SHADER_MATH_HAS_FP16)
    std::printf("NEON+FP16\n");
#  else
    std::printf("NEON\n");
#  endif
#elif defined(SHADER_MATH_ISA_SSE2)
#  if defined(SHADER_MATH_HAS_FMA)
    std::printf("SSE2+FMA\n");
#  else
    std::printf("SSE2\n");
#  endif
#else
    std::printf("SCALAR\n");
#endif
    return 0;
}
