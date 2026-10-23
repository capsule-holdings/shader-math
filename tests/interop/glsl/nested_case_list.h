// X-macro list of the nested-swizzle cases (see case_list.h for the float ones).
#define SM_GLSL_NESTED_CASES(X)                                       \
    X(exact_nested_read, case_exact_nested_read(IN_A, IN_B))          \
    X(exact_nested_mixed, case_exact_nested_mixed(IN_A, IN_B))
