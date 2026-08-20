// Backend interface contracts.
//
// Every per-ISA implementation file (detail/simd_{neon,sse2,scalar}.hpp and
// detail/simd_fp16_{neon,promote}.hpp) must satisfy these concepts; the
// contract lives HERE, not implicitly copied across the implementations.
// simd.hpp static_asserts the selected backend against them right after
// inclusion, so a signature drift in any impl fails to compile even if no
// test happens to call the drifted function.
// Included exclusively via <shader_math/simd.hpp>; do not include directly.
#pragma once

#if !defined(SHADER_MATH_SIMD_DISPATCH)
#  error "shader-math: include <shader_math/simd.hpp> instead of detail/simd_concepts.hpp"
#endif

#include <concepts>

namespace shader_math {
namespace detail {

// Full 4-wide float backend: arithmetic, comparisons producing Mask, mask
// logic, and mask reductions.
template <class Ops, class Reg, class Mask, class T>
concept SimdFloatOps = requires(Reg r, Mask m, const T* cp, T* p, T s, int i, bool b) {
    { Ops::zero() } -> std::same_as<Reg>;
    { Ops::splat(s) } -> std::same_as<Reg>;
    { Ops::set(s, s, s, s) } -> std::same_as<Reg>;
    { Ops::load4(cp) } -> std::same_as<Reg>;
    { Ops::store4(r, p) } -> std::same_as<void>;
    { Ops::lane(r, i) } -> std::same_as<T>;
    { Ops::template lane_c<0>(r) } -> std::same_as<T>;
    { Ops::template bcast<0>(r) } -> std::same_as<Reg>;

    { Ops::add(r, r) } -> std::same_as<Reg>;
    { Ops::sub(r, r) } -> std::same_as<Reg>;
    { Ops::mul(r, r) } -> std::same_as<Reg>;
    { Ops::div(r, r) } -> std::same_as<Reg>;
    { Ops::neg(r) } -> std::same_as<Reg>;
    { Ops::fmadd(r, r, r) } -> std::same_as<Reg>;

    { Ops::abs(r) } -> std::same_as<Reg>;
    { Ops::min(r, r) } -> std::same_as<Reg>;
    { Ops::max(r, r) } -> std::same_as<Reg>;
    { Ops::sqrt(r) } -> std::same_as<Reg>;
    { Ops::rnd_floor(r) } -> std::same_as<Reg>;
    { Ops::rnd_ceil(r) } -> std::same_as<Reg>;
    { Ops::rnd_trunc(r) } -> std::same_as<Reg>;
    { Ops::rnd_even(r) } -> std::same_as<Reg>;
    { Ops::rnd_away(r) } -> std::same_as<Reg>;
    { Ops::sel(m, r, r) } -> std::same_as<Reg>;

    { Ops::cmp_eq(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_ne(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_lt(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_le(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_gt(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_ge(r, r) } -> std::same_as<Mask>;

    { Ops::m_and(m, m) } -> std::same_as<Mask>;
    { Ops::m_or(m, m) } -> std::same_as<Mask>;
    { Ops::m_xor(m, m) } -> std::same_as<Mask>;
    { Ops::m_not(m) } -> std::same_as<Mask>;

    { Ops::lane_b(m, i) } -> std::same_as<bool>;
    { Ops::all_n(m, i) } -> std::same_as<bool>;
    { Ops::any_n(m, i) } -> std::same_as<bool>;
};

// Half backend: same arithmetic surface; comparisons produce the SHARED
// 32-bit float mask (Mask = mreg, exact after widening to f32), and sel
// consumes that shared mask. No mask logic or reductions of its own --
// bvec keeps using the float ops.
template <class Ops, class Reg, class Mask, class T>
concept SimdHalfOps = requires(Reg r, Mask m, const T* cp, T* p, T s, int i) {
    { Ops::zero() } -> std::same_as<Reg>;
    { Ops::splat(s) } -> std::same_as<Reg>;
    { Ops::set(s, s, s, s) } -> std::same_as<Reg>;
    { Ops::load4(cp) } -> std::same_as<Reg>;
    { Ops::store4(r, p) } -> std::same_as<void>;
    { Ops::lane(r, i) } -> std::same_as<T>;
    { Ops::template lane_c<0>(r) } -> std::same_as<T>;
    { Ops::template bcast<0>(r) } -> std::same_as<Reg>;

    { Ops::add(r, r) } -> std::same_as<Reg>;
    { Ops::sub(r, r) } -> std::same_as<Reg>;
    { Ops::mul(r, r) } -> std::same_as<Reg>;
    { Ops::div(r, r) } -> std::same_as<Reg>;
    { Ops::neg(r) } -> std::same_as<Reg>;
    { Ops::fmadd(r, r, r) } -> std::same_as<Reg>;

    { Ops::abs(r) } -> std::same_as<Reg>;
    { Ops::min(r, r) } -> std::same_as<Reg>;
    { Ops::max(r, r) } -> std::same_as<Reg>;
    { Ops::sqrt(r) } -> std::same_as<Reg>;
    { Ops::rnd_floor(r) } -> std::same_as<Reg>;
    { Ops::rnd_ceil(r) } -> std::same_as<Reg>;
    { Ops::rnd_trunc(r) } -> std::same_as<Reg>;
    { Ops::rnd_even(r) } -> std::same_as<Reg>;
    { Ops::rnd_away(r) } -> std::same_as<Reg>;
    { Ops::sel(m, r, r) } -> std::same_as<Reg>;

    { Ops::cmp_eq(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_ne(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_lt(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_le(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_gt(r, r) } -> std::same_as<Mask>;
    { Ops::cmp_ge(r, r) } -> std::same_as<Mask>;
};

}  // namespace detail
}  // namespace shader_math
