// X-macro lists for the DXC cross-checks: the shared float cases plus the
// DXC-only ones. Includes shared/case_list.h itself so vk_runner and
// d3d12_runner can use it as their only case header.
#include "shared/case_list.h"

#define SM_DXC_CASES_COMMON(X)                                 \
    SM_INTEROP_CASES_BASE(X)                                   \
    X(exact_dst, case_exact_dst(IN_A, IN_B))                   \
    X(exact_round_ties, case_exact_round_ties(IN_A))           \
    X(exact_ctor_ops, case_exact_ctor_ops(IN_A, IN_B))         \
    X(exact_lane_writes, case_exact_lane_writes(IN_A, IN_B))   \
    X(exact_index_matrix_args, case_exact_index_matrix_args(IN_A, IN_B)) \
    X(bits_int_sign, case_bits_int_sign(IN_A, IN_B))           \
    X(bits_int_compound_float, case_bits_int_compound_float(IN_A, IN_B)) \
    X(bits_bitfn_float_args, case_bits_bitfn_float_args(IN_A, IN_B)) \
    X(exact_matrix_dim1, case_exact_matrix_dim1(IN_A, IN_B))

// SPIR-V leaves lit out: DXC's SPIR-V backend computes its specular term as
// (n.h) * m instead of the documented (n.h)^m.
#define SM_DXC_CASES(X)                                        \
    SM_DXC_CASES_COMMON(X)                                     \
    X(exact_frexp, case_exact_frexp(IN_A))                     \
    X(exact_nan_rules, case_exact_nan_rules(IN_A, IN_B))       \
    X(exact_inf_edges, case_exact_inf_edges(IN_A))

// DXIL leaves frexp out: DXC's DXIL lowering builds the mantissa as
// (bits & 0x007fffff) | 0x3f000000, which drops the sign of a negative input.
// It adds the ++ / -- forms that DXC's SPIR-V backend cannot compile.
#define SM_DXIL_CASES(X)                                       \
    SM_DXC_CASES_COMMON(X)                                     \
    X(near_lit, case_near_lit())                               \
    X(exact_step_writes, case_exact_step_writes(IN_A))         \
    X(exact_matrix_dim1_dxil, case_exact_matrix_dim1_dxil(IN_A, IN_B))

// DXIL's half list adds the 16- and 64-bit bit functions that DXC's SPIR-V
// backend does not compile
#define SM_DXIL_HALF_CASES(X)                                  \
    SM_INTEROP_HALF_CASES(X)                                   \
    X(bits_int_width_firstbit, case_bits_int_width_firstbit(IN_A, IN_B))
