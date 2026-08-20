// Proof that the library compiles as a plain .cpp translation unit, plus a
// set of explicit instantiations covering every public type.
#include <shader_math/shader_math.hpp>

namespace shader_math {
namespace detail {
template struct fvec<2>;
template struct fvec<3>;
template struct fvec<4>;
template struct bvec<2>;
template struct bvec<3>;
template struct bvec<4>;
}  // namespace detail

namespace hlsl {
template struct matrix<2, 2>;
template struct matrix<2, 3>;
template struct matrix<2, 4>;
template struct matrix<3, 2>;
template struct matrix<3, 3>;
template struct matrix<3, 4>;
template struct matrix<4, 2>;
template struct matrix<4, 3>;
template struct matrix<4, 4>;
}  // namespace hlsl

namespace glsl {
template struct matrix<2, 2>;
template struct matrix<2, 3>;
template struct matrix<2, 4>;
template struct matrix<3, 2>;
template struct matrix<3, 3>;
template struct matrix<3, 4>;
template struct matrix<4, 2>;
template struct matrix<4, 3>;
template struct matrix<4, 4>;
}  // namespace glsl
}  // namespace shader_math

namespace {
// Exercise both language surfaces once so the TU links real code.
[[maybe_unused]] float smoke_test() {
    using namespace ::shader_math;
    hlsl::float4x4 a = hlsl::float4x4::identity();
    hlsl::float4 v(1.0f, 2.0f, 3.0f, 4.0f);
    const hlsl::float4 r = hlsl::mul(a, v);

    glsl::mat4 g(1.0f);
    glsl::vec4 u(1.0f, 2.0f, 3.0f, 4.0f);
    const glsl::vec4 s = g * u;
    return r[0] + s[0];
}
}  // namespace
