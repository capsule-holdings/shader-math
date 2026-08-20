#include <cstdio>

#include <shader_math/config.hpp>

#include "framework.hpp"

void test_vector();
void test_matrix_hlsl();
void test_matrix_glsl();
void test_cross_lang();
void test_restrict();
void test_simd();

int main() {
#if defined(SHADER_MATH_ISA_NEON)
    std::printf("[shader-math] backend: NEON\n");
#elif defined(SHADER_MATH_ISA_SSE2)
    std::printf("[shader-math] backend: SSE2\n");
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

    std::printf("[shader-math] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
