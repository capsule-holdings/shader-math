// X-macro case list for the GLSL cross-check: the single source of truth for
// case order, names and invocations. Expanded by kernels.comp (GLSL),
// cpp_harness.cpp (C++ / shader-math) and vk_runner.cpp (names only).
// Every case returns a vec4 = 4 output slots.
#define SM_GLSL_CASES(X)                                                  \
    X(exact_ctor_layout, case_exact_ctor_layout(IN_A))                    \
    X(exact_ctor_special, case_exact_ctor_special(IN_A, IN_B))            \
    X(exact_componentwise, case_exact_componentwise(IN_A, IN_B))          \
    X(exact_rounding, case_exact_rounding(IN_A, IN_B))                    \
    X(exact_compare, case_exact_compare(IN_A, IN_B))                      \
    X(exact_misc, case_exact_misc(IN_A, IN_B))                            \
    X(exact_classify, case_exact_classify(IN_A))                          \
    X(exact_mix_bool, case_exact_mix_bool(IN_A, IN_B))                    \
    X(exact_mix_ends, case_exact_mix_ends(IN_A, IN_B))                    \
    X(near_matrix_product, case_near_matrix_product(IN_A))                \
    X(near_inverse, case_near_inverse(IN_A))                              \
    X(near_mod, case_near_mod(IN_A, IN_B))                                \
    X(near_geometry, case_near_geometry(IN_A, IN_B))                      \
    X(near_edges, case_near_edges(IN_A, IN_B))                            \
    X(exact_bool_numeric, case_exact_bool_numeric(IN_A, IN_B))            \
    X(exact_ctor_ops, case_exact_ctor_ops(IN_A, IN_B))                    \
    X(approx_transcend, case_approx_transcend(IN_A, IN_B))                \
    X(approx_scalar, case_approx_scalar(IN_A, IN_B))
