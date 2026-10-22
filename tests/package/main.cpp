#include <shader_math/shader_math.hpp>

int main() {
    using namespace hlsl;
    const float4 v = mul(float4(1.0f, 2.0f, 3.0f, 4.0f), float4x4::identity());
    return all(v == float4(1.0f, 2.0f, 3.0f, 4.0f)) ? 0 : 1;
}
