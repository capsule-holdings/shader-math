// X-macro list of the GLSL half cases (see case_list.h for the float ones).
#define SM_GLSL_HALF_CASES(X)                                             \
    X(half_arith, case_half_arith(IN_A, IN_B))                            \
    X(half_products, case_half_products(IN_A, IN_B))
