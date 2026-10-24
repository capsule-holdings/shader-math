// "Undefined operations do not compile" — verified with requires-expressions.
// NOTE: no using-directives in this file; visibility itself is under test.
#include <shader_math/shader_math.hpp>

#include <ostream>

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

template <class V>
concept can_print = requires(std::ostream& os, const V& v) { os << v; };

// ---- bool <-> float vectors convert explicitly only (GLSL has no implicit form) --
static_assert(std::is_constructible_v<h::float3, h::bool3>);
static_assert(!std::is_convertible_v<h::bool3, h::float3>);
static_assert(std::is_constructible_v<h::bool3, h::float3>);
static_assert(!std::is_convertible_v<h::float3, h::bool3>);
static_assert(!std::is_constructible_v<h::float3, h::bool2>);  // widths must match

// ---- std integration is opt-in (<shader_math/std.hpp>) -------------------------
static_assert(!can_print<h::float3>);
static_assert(!can_print<h::float2x2>);

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
// found by ADL on each language's vectors, with no using-directive
static_assert(std::is_same_v<decltype(std::declval<h::float3>() == std::declval<h::float3>()),
                             h::bool3>);
static_assert(std::is_same_v<decltype(std::declval<g::vec3>() == std::declval<g::vec3>()), bool>);
static_assert(std::is_same_v<decltype(std::declval<h::bool3>() == std::declval<h::bool3>()),
                             h::bool3>);
static_assert(std::is_same_v<decltype(std::declval<g::bvec3>() == std::declval<g::bvec3>()), bool>);

// ---- glsl has no relational operators on vectors (lessThan() instead) ------------
// ADL on a glsl vector sees shader_math::glsl and shader_math::detail, neither
// of which defines relational operators -> the expression must not compile.
template <class V>
concept can_less = requires(const V& a, const V& b) { a < b; };
static_assert(!can_less<g::vec3>);

// ---- vector truncation must be explicit ------------------------------------------
static_assert(!std::is_convertible_v<h::float4, h::float3>);
static_assert(std::is_constructible_v<h::float3, h::float4>);

// ---- assignment across mismatched widths does not compile -------------------------
static_assert(!can_assign<h::float3, h::float4>);

// ---- a scalar becomes a vector implicitly in hlsl, by constructor only in glsl -----
static_assert(std::is_convertible_v<float, h::float3> && can_assign<h::float3, float>);
static_assert(!std::is_convertible_v<float, g::vec3> && !can_assign<g::vec3, float>);
static_assert(std::is_constructible_v<g::vec3, float>);
// ... so a matrix is not built from scalar "rows" / "columns" either
static_assert(!std::is_constructible_v<h::float2x2, float, float>);
static_assert(!std::is_constructible_v<g::mat2, float, float>);

// ---- component lists: HLSL needs the exact count, GLSL lets only the last argument
// be cut short; a single vector converts to a matrix implicitly in HLSL only ----------
static_assert(std::is_constructible_v<h::float2x2, h::float2, float, float>);
static_assert(!std::is_constructible_v<h::float2x2, h::float3, h::float3>);
static_assert(std::is_convertible_v<h::float4, h::float2x2>);
static_assert(std::is_constructible_v<g::vec3, g::vec2, g::vec2>);
static_assert(!std::is_constructible_v<g::vec2, g::vec2, float>);  // float unused
static_assert(!std::is_constructible_v<g::vec3, float, float, float, float>);
static_assert(std::is_constructible_v<g::mat2, g::vec3, g::vec3>);
static_assert(!std::is_constructible_v<g::mat2, g::vec4, float>);
static_assert(!std::is_convertible_v<g::vec4, g::mat2>);
static_assert(!std::is_constructible_v<g::vec3, h::float2, h::float2>);  // one language
static_assert(std::is_convertible_v<bool, h::bool3> && !std::is_convertible_v<bool, g::bvec3>);

// ---- a language namespace takes its own language's arguments only, qualified or not ----
template <class A>
concept hlsl_dot = requires(const A& a) { h::dot(a, a); };
template <class A>
concept glsl_dot = requires(const A& a) { g::dot(a, a); };
template <class A>
concept hlsl_abs = requires(const A& a) { h::abs(a); };
template <class A>
concept hlsl_isnan = requires(const A& a) { h::isnan(a); };
template <class A>
concept hlsl_round = requires(const A& a) { h::round(a); };
template <class A>
concept hlsl_all = requires(const A& a) { h::all(a); };
template <class A>
concept adl_dot = requires(const A& a) { dot(a, a); };
static_assert(hlsl_dot<h::float3> && !hlsl_dot<g::vec3>);
static_assert(glsl_dot<g::vec3> && !glsl_dot<h::float3>);
static_assert(hlsl_abs<h::float3> && !hlsl_abs<g::vec3> && hlsl_abs<float>);
static_assert(hlsl_isnan<h::float3> && !hlsl_isnan<g::vec3> && hlsl_isnan<float>);
static_assert(hlsl_round<h::float3> && !hlsl_round<g::vec3> && hlsl_round<float>);
static_assert(hlsl_all<h::bool3> && !hlsl_all<g::bvec3>);
static_assert(adl_dot<h::float3> && adl_dot<g::vec3>);
// one-component swizzles are scalars: either namespace takes them
static_assert(requires(h::float3 v) { h::round(v.x); h::isnan(v.y); h::abs(v.z); });
static_assert(requires(g::vec3 v) { g::round(v.x); g::isnan(v.y); g::abs(v.z); });

// ---- extensions are reachable qualified only (shader_math::ext) -------------------
template <class M>
concept adl_inverse = requires(const M& m) { inverse(m); };
template <class M>
concept ext_inverse = requires(const M& m) { shader_math::ext::inverse(m); };
static_assert(!adl_inverse<h::float3x3> && ext_inverse<h::float3x3>);
static_assert(adl_inverse<g::mat3>);  // GLSL's own inverse

// ---- glsl .length() is the component (column) count; hlsl has no such member --------
template <class V>
concept has_length_member = requires(const V& v) { v.length(); };
static_assert(g::vec3::length() == 3 && g::bvec2::length() == 2 && g::mat4x2::length() == 4);
static_assert(!has_length_member<h::float3> && !has_length_member<h::bool3>);
static_assert(!has_length_member<h::float3x3>);

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

    const g::vec3 w(1.0f);
    CHECK(w.length() == 3);

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
