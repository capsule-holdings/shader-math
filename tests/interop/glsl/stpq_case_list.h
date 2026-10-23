// X-macro list of the GLSL .stpq cases (see case_list.h for the float ones).
#define SM_GLSL_STPQ_CASES(X)                                             \
    X(exact_stpq_read, case_exact_stpq_read(IN_A, IN_B))                  \
    X(exact_stpq_write, case_exact_stpq_write(IN_A, IN_B))
