#include <shader_math/shader_math.hpp>

#include "framework.hpp"

#if defined(_MSC_VER)
#  define SM_TEST_NOINLINE __declspec(noinline)
#else
#  define SM_TEST_NOINLINE __attribute__((noinline))
#endif

// Proxy reads after register writes. A matrix result is zero-initialized
// (row_{}) and then written row by row through fvec pointers; m._22 then
// reads the same bytes through the mswz proxy type. Unless the proxy types
// are may_alias, GCC (x86 / AArch64 / scalar backend, -O2 and up) folds such
// a read to the zero-initialized value. Each function is noinline so the
// pattern is optimized in isolation, as it is in user code.
namespace {
using namespace shader_math::hlsl;

SM_TEST_NOINLINE float hadamard2_m22(float2x2 a, float2x2 b) {
    const float2x2 m = a * b;
    return m._22;
}
SM_TEST_NOINLINE float hadamard3_m21(float3x3 a) {
    const float3x3 m = a * a;
    return m._m21;
}
SM_TEST_NOINLINE float mul4_m44(float4x4 a) {
    const float4x4 m = mul(a, a);
    return m._44;
}
SM_TEST_NOINLINE float array_m10(float2x2 a, int n) {
    float2x2 m[2];
    for (int i = 0; i < n; ++i) m[i] = a * a;
    return m[1]._m10;
}
SM_TEST_NOINLINE float ctor_m11(float a, float b, float c, float d) {
    const float2x2 m = {a, b, c, d};
    return m._m11;
}
// Opposite direction: proxy write, then a register-based operation.
SM_TEST_NOINLINE float proxy_write_then_add(float2x2 a) {
    float2x2 m = a * a;
    m._m11 = 100.0f;
    const float2x2 n = m + m;
    return n[1][1];
}
SM_TEST_NOINLINE float swizzle_write_then_mul(float4 a) {
    float4 v[2] = {a, a};
    v[1].zw = float2(7.0f, 8.0f);
    const float4 w = v[1] * 2.0f;
    return w[3];
}
}  // namespace

void test_proxy_alias() {
    const float2x2 a2 = {1.0f, 2.0f, 3.0f, 4.0f};
    const float2x2 b2 = {5.0f, 6.0f, 7.0f, 8.0f};
    CHECK_EQF(hadamard2_m22(a2, b2), 32.0f);

    const float3x3 a3 = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    CHECK_EQF(hadamard3_m21(a3), 64.0f);

    const float4x4 a4 = {1.0f, 2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
                         9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f};
    CHECK_EQF(mul4_m44(a4), 600.0f);

    CHECK_EQF(array_m10(a2, 2), 9.0f);
    CHECK_EQF(ctor_m11(1.0f, 1.1f, 2.0f, 2.1f), 2.1f);
    CHECK_EQF(proxy_write_then_add(a2), 200.0f);
    CHECK_EQF(swizzle_write_then_mul(float4(1.0f, 2.0f, 3.0f, 4.0f)), 16.0f);
}
