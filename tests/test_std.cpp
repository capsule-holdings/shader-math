// <shader_math/std.hpp>: ostream, std::format, the tuple protocol,
// std::hash / std::equal_to, and load / store.
#include <shader_math/std.hpp>

#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "framework.hpp"

namespace {

template <class X>
std::string str(const X& x) {
    std::ostringstream os;
    os << x;
    return os.str();
}

void test_ostream() {
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;
    const h::float3 v(1.0f, -2.5f, 3.0f);
    CHECK(str(v) == "(1, -2.5, 3)");
    CHECK(str(v.zx) == "(3, 1)");
    CHECK(str(h::bool2(true, false)) == "(true, false)");
    const h::float2x3 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    CHECK(str(m) == "((1, 2, 3), (4, 5, 6))");  // rows
    const g::mat2 gm(1.0f, 2.0f, 3.0f, 4.0f);
    CHECK(str(gm) == "((1, 2), (3, 4))");  // columns
#if defined(SHADER_MATH_HAS_HALF)
    CHECK(str(h::half2(0.5f, 2.0f)) == "(0.5, 2)");
#endif
}

void test_format() {
#if defined(__cpp_lib_format)
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;
    CHECK(std::format("{}", h::float2(1.0f, 0.5f)) == "(1, 0.5)");
    CHECK(std::format("{:.2f}", h::float3(1.0f, 2.5f, -3.0f)) == "(1.00, 2.50, -3.00)");
    CHECK(std::format("{:>4}", h::float2(1.0f, 2.0f)) == "(   1,    2)");
    CHECK(std::format("{:.1f}", h::float2x2(1.0f, 2.0f, 3.0f, 4.0f)) ==
          "((1.0, 2.0), (3.0, 4.0))");
    CHECK(std::format("{}", g::mat2(1.0f, 2.0f, 3.0f, 4.0f)) == "((1, 2), (3, 4))");
#  if defined(SHADER_MATH_HAS_HALF)
    CHECK(std::format("{}", h::half2(0.5f, 2.0f)) == "(0.5, 2)");
#  endif
#endif
}

void test_bindings() {
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;
    h::float3 v(1.0f, 2.0f, 3.0f);
    auto [x, y, z] = v;
    CHECK_EQF(x, 1.0f);
    CHECK_EQF(y, 2.0f);
    CHECK_EQF(z, 3.0f);
    auto& [rx, ry, rz] = v;
    ry = 20.0f;
    CHECK_EQF(v[1], 20.0f);
    (void)rx;
    (void)rz;
    const auto [cx, cy] = h::float2(4.0f, 5.0f);
    CHECK_EQF(cx + cy, 9.0f);

    h::float2x3 m(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f);
    auto& [row0, row1] = m;
    row1 = h::float3(7.0f, 8.0f, 9.0f);
    CHECK_EQF(m[1][2], 9.0f);
    CHECK_EQF(row0[0], 1.0f);
    g::mat2 gm(1.0f, 2.0f, 3.0f, 4.0f);
    auto [col0, col1] = gm;
    CHECK_EQF(col0[1], 2.0f);
    CHECK_EQF(col1[0], 3.0f);
    static_assert(std::tuple_size_v<h::float4> == 4);
    static_assert(std::is_same_v<std::tuple_element_t<1, h::float3x2>, h::float2>);
    static_assert(std::is_same_v<std::tuple_element_t<0, g::mat3x2>, g::vec2>);
}

void test_hash() {
    namespace h = shader_math::hlsl;
    std::unordered_map<h::float3, int> ids;
    ids[h::float3(1.0f, 2.0f, 3.0f)] = 1;
    ids[h::float3(0.0f, 0.0f, 1.0f)] = 2;
    CHECK(ids.size() == 2);
    CHECK(ids.at(h::float3(1.0f, 2.0f, 3.0f)) == 1);
    CHECK(ids.count(h::float3(-0.0f, 0.0f, 1.0f)) == 1);  // -0 == +0
    CHECK(ids.count(h::float3(1.0f, 2.0f, 4.0f)) == 0);
    std::unordered_set<shader_math::glsl::vec2> s;
    s.insert(shader_math::glsl::vec2(1.0f, 2.0f));
    s.insert(shader_math::glsl::vec2(1.0f, 2.0f));
    CHECK(s.size() == 1);
    // pad lanes past N never take part
    h::float4 wide(1.0f, 2.0f, 3.0f, 99.0f);
    CHECK(ids.count(h::float3(wide)) == 1);
}

void test_load_store() {
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;
    const float data[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    const auto v = shader_math::load<h::float3>(data + 1);
    CHECK_EQF(v[0], 2.0f);
    CHECK_EQF(v[2], 4.0f);
    const auto hm = shader_math::load<h::float2x3>(data);  // row-major
    CHECK_EQF(hm[1][0], 4.0f);
    const auto gm = shader_math::load<g::mat2x3>(data);  // column-major: 2 columns of 3
    CHECK_EQF(gm[1][0], 4.0f);
    CHECK(all(h::float2x3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f) == hm));

    float out[16] = {};
    shader_math::store(h::float2(7.0f, 8.0f), out);
    CHECK_EQF(out[1], 8.0f);
    CHECK_EQF(out[2], 0.0f);  // only N elements are written
    shader_math::store(hm, out);
    for (int i = 0; i < 6; ++i) CHECK_EQF(out[i], data[i]);
    shader_math::store(gm, out);
    for (int i = 0; i < 6; ++i) CHECK_EQF(out[i], data[i]);
    double d[3] = {};
    shader_math::store(v, d);
    CHECK(d[2] == 4.0);
}

}  // namespace

void test_std() {
    test_ostream();
    test_format();
    test_bindings();
    test_hash();
    test_load_store();
}
