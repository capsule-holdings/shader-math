// Header-cost probe for bench/compile_time.py: completes every vector and
// matrix type (which instantiates their swizzle members) and nothing else.
#include <shader_math/shader_math.hpp>

namespace probe {
using namespace shader_math::hlsl;

float2 f2;
float3 f3;
float4 f4;
bool2 b2;
bool3 b3;
bool4 b4;
float2x2 m22;
float2x3 m23;
float2x4 m24;
float3x2 m32;
float3x3 m33;
float3x4 m34;
float4x2 m42;
float4x3 m43;
float4x4 m44;
#if defined(SHADER_MATH_HAS_HALF)
half2 h2;
half3 h3;
half4 h4;
half4x4 hm44;
#endif
shader_math::glsl::mat4 gm4;
}  // namespace probe
