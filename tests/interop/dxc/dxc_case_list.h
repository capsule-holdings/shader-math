// X-macro lists for the DXC cross-checks: the shared float cases plus the
// DXC-only ones. Includes shared/case_list.h itself so vk_runner and
// d3d12_runner can use it as their only case header.
#include "shared/case_list.h"

// SPIR-V (SM_DXC_CASES) leaves lit out: DXC's SPIR-V backend computes its
// specular term as (n.h) * m instead of the documented (n.h)^m. The DXIL
// backend emits the power, so SM_DXIL_CASES checks it.
#define SM_DXC_CASES(X)                                        \
    SM_INTEROP_CASES_BASE(X)                                   \
    X(exact_frexp_dst, case_exact_frexp_dst(IN_A, IN_B))

#define SM_DXIL_CASES(X)                                       \
    SM_DXC_CASES(X)                                            \
    X(near_lit, case_near_lit())
