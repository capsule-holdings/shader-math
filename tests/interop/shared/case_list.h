// X-macro case list: the single source of truth for case order, names, and
// invocations. Expanded by kernels.slang (Slang), cpp_harness.cpp (C++ /
// shader-math), and slang_driver.cpp (names only; the expressions are never
// compiled there).
//
// Every case returns a float4 = 4 output slots. Tolerances are chosen from
// the name prefix by compare.py (exact_ / near_ / half_).

// Half cases are a separate list: the Slang CPU target's C++ prelude (v2026.20)
// does not implement half vector arithmetic, so SM_INTEROP_HALF_CASES runs
// through kernels_half.slang -> SPIR-V on Vulkan (lavapipe) instead; see
// docs/slang_compat.md.
#define SM_INTEROP_HALF_CASES(X)                               \
    X(half_chain, case_half_chain(IN_A))                       \
    X(half_products, case_half_products(IN_A, IN_B))

// Swizzles of swizzles are opt-in in shader-math (SHADER_MATH_NESTED_SWIZZLE),
// so they are a separate list run by kernels_nested.slang and a C++ twin
// built with the macro.
#define SM_INTEROP_NESTED_CASES(X)                             \
    X(exact_nested_read, case_exact_nested_read(IN_A, IN_B))   \
    X(exact_nested_mixed, case_exact_nested_mixed(IN_A, IN_B))

// SM_INTEROP_CASES_BASE is everything both Slang and DXC check;
// SM_INTEROP_CASES (Slang) adds lit, which DXC's SPIR-V backend computes
// differently (see case_near_lit).
#define SM_INTEROP_CASES(X)                                    \
    SM_INTEROP_CASES_BASE(X)                                   \
    X(near_lit, case_near_lit())

#define SM_INTEROP_CASES_BASE(X)                               \
    X(exact_hadamard, case_exact_hadamard(IN_A, IN_B))         \
    X(exact_swizzle, case_exact_swizzle(IN_A, IN_B))           \
    X(exact_intrinsics, case_exact_intrinsics(IN_A, IN_B))     \
    X(near_dot_cross, case_near_dot_cross(IN_A, IN_B))         \
    X(near_matrix, case_near_matrix(IN_A))                     \
    X(near_determinant, case_near_determinant(IN_A))           \
    X(near_lighting, case_near_lighting(IN_N, IN_L, IN_C))     \
    X(near_transcend, case_near_transcend(IN_A, IN_B))         \
    X(exact_classify, case_exact_classify(IN_A))               \
    X(exact_matrix_intrinsics, case_exact_matrix_intrinsics(IN_A, IN_B)) \
    X(near_geometry, case_near_geometry(IN_N, IN_L, IN_C))     \
    X(near_inverse_trig, case_near_inverse_trig(IN_A, IN_B))   \
    X(near_fmod_mad, case_near_fmod_mad(IN_A, IN_B))           \
    X(near_matrix_intrinsics, case_near_matrix_intrinsics(IN_A, IN_B)) \
    X(exact_ctor_access, case_exact_ctor_access(IN_A, IN_B))   \
    X(near_misc, case_near_misc(IN_A, IN_B))                   \
    X(exact_select_bool, case_exact_select_bool(IN_A, IN_B))   \
    X(near_log10_ldexp, case_near_log10_ldexp(IN_A, IN_B))
