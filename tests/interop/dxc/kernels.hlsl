// DXC side of the HLSL cross-checks: the shared cases (plus the DXC-only
// ones) compiled by the official HLSL compiler, either to SPIR-V and run on
// Vulkan by vk_runner, or to DXIL (SM_INTEROP_DXIL) and run on D3D12 WARP by
// d3d12_runner. SM_INTEROP_ENABLE_HALF (with -enable-16bit-types) selects
// the half cases instead.
#include "shared/math_cases.hlsli"
#include "dxc/dxc_cases.hlsli"
#include "dxc/dxc_case_list.h"

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
#elif defined(SM_INTEROP_DXIL)
    SM_DXIL_CASES(X)
#else
    SM_DXC_CASES(X)
#endif
#undef X
}
