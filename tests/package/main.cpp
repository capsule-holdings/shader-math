#include <shader_math/shader_math.hpp>

#if defined(SM_EXPECT_SCALAR) && !defined(SHADER_MATH_ISA_SCALAR)
#  error "SHADER_MATH_FORCE_SCALAR did not reach the consumer through shader_math::shader_math"
#endif

int main() {
    using namespace hlsl;
    const float4 v = mul(float4(1.0f, 2.0f, 3.0f, 4.0f), float4x4::identity());
    return all(v == float4(1.0f, 2.0f, 3.0f, 4.0f)) ? 0 : 1;
}
