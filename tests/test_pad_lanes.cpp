// Lanes >= N of an N-wide vector (and of matrix rows / columns) are
// unspecified: splats, scalar operands and division leave arbitrary values
// there. Every result must depend on lanes [0, N) only, so these checks
// poison the unused lanes with NaN and compare against clean inputs.
#include <limits>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace d = shader_math::detail;

namespace {

const float kNaN = std::numeric_limits<float>::quiet_NaN();

// Lg: the language namespace's lang_tag, named unqualified at the call site
template <class Lg>
d::fvec<3, float, Lg> poison3(float x, float y, float z) {
    return d::fvec<3, float, Lg>(d::ops::set(x, y, z, kNaN));
}
template <class Lg>
d::fvec<2, float, Lg> poison2(float x, float y) {
    return d::fvec<2, float, Lg>(d::ops::set(x, y, kNaN, kNaN));
}

void test_hlsl_vectors() {
    using namespace shader_math::hlsl;
    const float3 a = poison3<lang_tag>(1.0f, 2.0f, 2.0f);
    const float3 b = poison3<lang_tag>(3.0f, -1.0f, 0.5f);
    const float3 ca(1.0f, 2.0f, 2.0f);
    const float3 cb(3.0f, -1.0f, 0.5f);

    CHECK_EQF(dot(a, b), dot(ca, cb));
    CHECK_EQF(length(a), 3.0f);
    CHECK(all(normalize(a) == normalize(ca)));
    CHECK(all(cross(a, b) == cross(ca, cb)));

    // comparisons and their reductions look at N lanes only
    CHECK(all(a == ca));
    CHECK(!any(a != ca));
    CHECK(all(a < a + 1.0f));
    CHECK(all(poison2<lang_tag>(1.0f, 2.0f) == float2(1.0f, 2.0f)));

    // division by a vector leaves 0/0 in the unused lane; results unaffected
    const float3 q = ca / cb;
    CHECK_NEAR(dot(q, float3(1.0f)), 1.0f / 3.0f - 2.0f + 4.0f);
}

void test_glsl_vectors() {
    using namespace shader_math::glsl;
    const vec3 a = poison3<lang_tag>(1.0f, 2.0f, 2.0f);
    const vec3 ca(1.0f, 2.0f, 2.0f);
    CHECK(a == ca);  // scalar bool over N lanes
    CHECK(!(a != ca));
    CHECK(all(equal(a, ca)));
    CHECK(all(lessThan(a, a + 1.0f)));
}

void test_hlsl_matrices() {
    using namespace shader_math::hlsl;
    float3x3 m(2.0f, 0.0f, 1.0f,
                  1.0f, 3.0f, 0.0f,
                  0.0f, 1.0f, 4.0f);
    const float3x3 clean = m;
    for (int r = 0; r < 3; ++r)
        m.row_[r] = poison3<lang_tag>(m.row_[r][0], m.row_[r][1], m.row_[r][2]);
    const float3 v = poison3<lang_tag>(1.0f, -2.0f, 0.5f);
    const float3 cv(1.0f, -2.0f, 0.5f);

    CHECK(all(mul(v, m) == mul(cv, clean)));
    CHECK(all(mul(m, v) == mul(clean, cv)));
    CHECK(all(mul(m, m) == mul(clean, clean)));
    CHECK(all(m == clean));
    CHECK_EQF(determinant(m), determinant(clean));
    CHECK(all(inverse(m) == inverse(clean)));
    CHECK(all(transpose(m) == transpose(clean)));
}

void test_glsl_matrices() {
    using namespace shader_math::glsl;
    mat3 m(2.0f, 1.0f, 0.0f,
              0.0f, 3.0f, 1.0f,
              1.0f, 0.0f, 4.0f);
    const mat3 clean = m;
    for (int c = 0; c < 3; ++c)
        m.col_[c] = poison3<lang_tag>(m.col_[c][0], m.col_[c][1], m.col_[c][2]);
    const vec3 v = poison3<lang_tag>(1.0f, -2.0f, 0.5f);
    const vec3 cv(1.0f, -2.0f, 0.5f);

    CHECK(m * v == clean * cv);
    CHECK(v * m == cv * clean);
    CHECK(m * m == clean * clean);
    CHECK(m == clean);
    CHECK_EQF(determinant(m), determinant(clean));
    CHECK(inverse(m) == inverse(clean));
    CHECK(transpose(m) == transpose(clean));
}

}  // namespace

void test_pad_lanes() {
    test_hlsl_vectors();
    test_glsl_vectors();
    test_hlsl_matrices();
    test_glsl_matrices();
}
