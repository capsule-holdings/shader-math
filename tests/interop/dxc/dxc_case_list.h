// X-macro list for the DXC cross-check: the shared float cases plus the
// DXC-only ones. Includes shared/case_list.h itself so vk_runner can use it
// as its only case header.
#include "shared/case_list.h"

// lit is left out: DXC's SPIR-V backend computes its specular term as
// (n.h) * m instead of the documented (n.h)^m.
#define SM_DXC_CASES(X)                                        \
    SM_INTEROP_CASES_BASE(X)                                   \
    X(exact_frexp_dst, case_exact_frexp_dst(IN_A, IN_B))
