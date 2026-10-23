// Compile-only: vector isnan / isinf / isfinite must win against the
// unconstrained global templates the MSVC UCRT declares
// (template <class _Ty> bool isnan(_Ty), ...). MSVC sees the real ones; the
// other compilers get look-alike declarations so every CI job checks it.
#if !defined(_MSC_VER)
template <class Ty>
bool isnan(Ty) noexcept;
template <class Ty>
bool isinf(Ty) noexcept;
template <class Ty>
bool isfinite(Ty) noexcept;
#endif

#include <type_traits>

#include <shader_math/shader_math.hpp>

namespace hlsl_shadow {
using namespace shader_math::hlsl;
static_assert(std::is_same_v<decltype(isnan(float4())), bool4>);
static_assert(std::is_same_v<decltype(isinf(float4().xy)), bool2>);
static_assert(std::is_same_v<decltype(isfinite(float4())), bool4>);
static_assert(std::is_same_v<decltype(isfinite(float4().xyz)), bool3>);
static_assert(std::is_same_v<decltype(isnan(float3x3())), bool3x3>);
static_assert(std::is_same_v<decltype(isfinite(float2x4())), bool2x4>);
}  // namespace hlsl_shadow

namespace glsl_shadow {
using namespace shader_math::glsl;
static_assert(std::is_same_v<decltype(isnan(vec3())), bvec3>);
static_assert(std::is_same_v<decltype(isinf(vec3().xy)), bvec2>);
}  // namespace glsl_shadow
