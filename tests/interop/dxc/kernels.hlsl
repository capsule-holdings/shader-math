// DXC side of the HLSL cross-check: the shared cases (plus the DXC-only
// ones) compiled by the official HLSL compiler to SPIR-V and run once on
// Vulkan by the shared vk_runner. SM_INTEROP_ENABLE_HALF (with
// -enable-16bit-types) selects the half cases instead, and
// SM_INTEROP_MATRIX_SWIZZLE the multi-element matrix accessor cases.
#include "shared/math_cases.hlsli"
#include "dxc/dxc_cases.hlsli"
#include "dxc/dxc_case_list.h"
#if defined(SM_INTEROP_MATRIX_SWIZZLE)
#  include "shared/matrix_swizzle_cases.hlsli"
#endif

RWStructuredBuffer<float> outputBuf : register(u0);

[numthreads(1, 1, 1)]
void main()
{
    int slot = 0;
#define X(name, expr)          \
    {                          \
        float4 r = (expr);     \
        outputBuf[slot] = r.x; \
        outputBuf[slot + 1] = r.y; \
        outputBuf[slot + 2] = r.z; \
        outputBuf[slot + 3] = r.w; \
        slot += 4;             \
    }
#if defined(SM_INTEROP_ENABLE_HALF)
    SM_INTEROP_HALF_CASES(X)
#elif defined(SM_INTEROP_MATRIX_SWIZZLE)
    SM_INTEROP_MATRIX_SWIZZLE_CASES(X)
#else
    SM_DXC_CASES(X)
#endif
#undef X
}
