// "Undefined operations do not compile" — verified with requires-expressions.
// NOTE: no using-directives in this file; visibility itself is under test.
#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace h = shader_math::hlsl;
namespace g = shader_math::glsl;
namespace d = shader_math::detail;

template <class A, class B>
concept can_mul_op = requires(const A& a, const B& b) { a* b; };
template <class A, class B>
concept can_add_op = requires(const A& a, const B& b) { a + b; };
template <class A, class B>
concept can_assign = requires(A a, const B& b) { a = b; };

// ---- HLSL '*' is same-shape only (matrix product needs mul) -------------------
static_assert(can_mul_op<h::float3x3, h::float3x3>);
static_assert(!can_mul_op<h::float3x4, h::float4x3>);  // use mul()
static_assert(!can_mul_op<h::float2x2, h::float3x3>);

// ---- glsl '*' enforces inner dimensions ---------------------------------------
static_assert(can_mul_op<g::mat3, g::mat3>);
static_assert(can_mul_op<g::mat3x2, g::mat2x3>);   // (2x3 math)*(3x2 math)
static_assert(!can_mul_op<g::mat3, g::mat4x2>);    // inner 3 vs 2
static_assert(!can_mul_op<g::mat2x3, g::mat2x3>);  // inner 2 vs 3... (3x2)*(3x2)

// ---- vector binary ops require equal width ------------------------------------
static_assert(can_add_op<h::float3, h::float3>);
static_assert(!can_add_op<h::float3, h::float4>);
static_assert(!can_mul_op<h::float2, h::float3>);

// ---- comparison results are not silently bool-convertible ----------------------
static_assert(!std::is_convertible_v<h::bool3, bool>);
static_assert(!std::is_convertible_v<h::bool4x4, bool>);

// ---- hlsl/glsl matrices never convert implicitly --------------------------------
static_assert(!std::is_convertible_v<g::mat3, h::float3x3>);
static_assert(!std::is_convertible_v<h::float3x3, g::mat3>);
static_assert(std::is_constructible_v<h::float3x3, g::mat3>);  // explicit only
static_assert(std::is_constructible_v<g::mat3, h::float3x3>);

// ---- swizzle writes: duplicates do not compile, permutations do ------------------
template <class V2>
concept can_write_xx = requires(h::float3 v, const V2& u) { v.xx = u; };
template <class V2>
concept can_write_xy = requires(h::float3 v, const V2& u) { v.xy = u; };
static_assert(!can_write_xx<h::float2>);
static_assert(can_write_xy<h::float2>);

// ---- swizzles of swizzles are opt-in (SHADER_MATH_NESTED_SWIZZLE) -------------
template <class V>
concept can_nest = requires(V v) { v.xy.yx; };
static_assert(!can_nest<h::float4>);

// ---- multi-element matrix accessors are opt-in (SHADER_MATH_MATRIX_SWIZZLE) -----
template <class M>
concept has_matrix_swizzle = requires(M m) { m._m00_m11; };
static_assert(!has_matrix_swizzle<h::float4x4>);

// ---- comparison operator return types differ per language -----------------------
static_assert(std::is_same_v<decltype(h::operator==(std::declval<h::float3>(),
                                                    std::declval<h::float3>())),
                             h::bool3>);
static_assert(std::is_same_v<decltype(g::operator==(std::declval<g::vec3>(),
                                                    std::declval<g::vec3>())),
                             bool>);

// ---- glsl has no relational operators on vectors (lessThan() instead) ------------
// Without a using-directive, ADL only sees shader_math::detail, which defines
// no relational operators -> the expression must not compile.
template <class V>
concept can_less = requires(const V& a, const V& b) { a < b; };
static_assert(!can_less<g::vec3>);

// ---- vector truncation must be explicit ------------------------------------------
static_assert(!std::is_convertible_v<h::float4, h::float3>);
static_assert(std::is_constructible_v<h::float3, h::float4>);

// ---- assignment across mismatched widths does not compile -------------------------
static_assert(!can_assign<h::float3, h::float4>);

// ---- bool vectors: built from bools only, components are read-only ----------
template <class B, class... As>
concept can_build = requires(As... as) { B(as...); };
static_assert(can_build<h::bool3, bool, bool, bool>);
static_assert(can_build<h::bool3, bool>);                // splat
static_assert(!can_build<h::bool3, int, int, int>);      // no int -> mask
static_assert(!can_build<h::bool3, float, float, float>);
static_assert(!can_build<h::bool3, bool, bool>);         // arity
static_assert(!can_assign<decltype(h::bool3().x)&, bool>);  // read-only
static_assert(std::is_convertible_v<decltype(h::bool3().x), bool>);

void test_restrict() {
    // Runtime side of the same guarantees: mul with correct dims still works.
    h::float3x4 a = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
    h::float4x3 b = {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
    h::float3x3 p = h::mul(a, b);
    CHECK(all(p == h::float3x3::identity()));

    using namespace shader_math::hlsl;
    // `if (v == u)` does not compile in hlsl (bool3 is not bool) — enforced by
    // the static_asserts above; here we show the correct HLSL idiom instead.
    float3 v(1.0f, 2.0f, 3.0f);
    float3 u(1.0f, 2.0f, 3.0f);
    if (all(v == u)) {
        CHECK(true);
    } else {
        CHECK(false);
    }
}
