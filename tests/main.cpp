#include <cstdio>

#include "framework.hpp"

void test_vector();
void test_matrix_hlsl();
void test_matrix_glsl();
void test_cross_lang();
void test_restrict();

int main() {
#if defined(SHADER_MATH_FORCE_SCALAR)
    std::printf("[shader-math] backend: scalar (forced)\n");
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    std::printf("[shader-math] backend: NEON\n");
#elif defined(__SSE2__) || defined(_M_X64)
    std::printf("[shader-math] backend: SSE\n");
#else
    std::printf("[shader-math] backend: scalar (auto)\n");
#endif

    test_vector();
    test_matrix_hlsl();
    test_matrix_glsl();
    test_cross_lang();
    test_restrict();

    std::printf("[shader-math] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
