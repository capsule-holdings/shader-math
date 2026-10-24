#include <cstdio>

#include <shader_math/config.hpp>

#include "framework.hpp"

void test_vector();
void test_matrix_hlsl();
void test_matrix_glsl();
void test_cross_lang();
void test_restrict();
void test_simd();
void test_half();
void test_funcs();
void test_pad_lanes();
void test_random();
void test_proxy_alias();
void test_std();
void test_stpq();

int main() {
#if defined(SHADER_MATH_ISA_NEON)
#  if defined(SHADER_MATH_HAS_FP16)
    std::printf("[shader-math] backend: NEON+FP16\n");
#  else
    std::printf("[shader-math] backend: NEON\n");
#  endif
#elif defined(SHADER_MATH_ISA_SSE2)
#  if defined(SHADER_MATH_HAS_FMA)
    std::printf("[shader-math] backend: SSE2+FMA\n");
#  else
    std::printf("[shader-math] backend: SSE2\n");
#  endif
#elif defined(SHADER_MATH_FORCE_SCALAR)
    std::printf("[shader-math] backend: scalar (forced)\n");
#else
    std::printf("[shader-math] backend: scalar (auto)\n");
#endif

    test_vector();
    test_matrix_hlsl();
    test_matrix_glsl();
    test_cross_lang();
    test_restrict();
    test_simd();
    test_half();
    test_funcs();
    test_pad_lanes();
    test_random();
    test_proxy_alias();
    test_std();
    test_stpq();

    std::printf("[shader-math] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
