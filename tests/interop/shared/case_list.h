// X-macro case list: the single source of truth for case order, names, and
// invocations. Expanded by kernels.slang (Slang), cpp_harness.cpp (C++ /
// shader-math), and slang_driver.cpp (names only; the expressions are never
// compiled there).
//
// Every case returns a float4 = 4 output slots. Tolerances are chosen from
// the name prefix by compare.py (exact_ / near_ / half_).

// NOTE: no half case yet. Measured on Slang v2026.16: the CPU target's C++
// prelude does not implement half vector arithmetic (no operator* for
// Vector<_Float16, 4>), so generated code with half math does not compile.
// Half cross-checking is deferred to the SPIR-V execution phase (lavapipe);
// see docs/slang_compat.md.
#define SM_INTEROP_CASES(X)                                    \
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
    X(near_misc, case_near_misc(IN_A, IN_B))
