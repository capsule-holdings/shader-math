// DXC side of the HLSL cross-checks: the shared cases (plus the DXC-only
// ones) compiled by the official HLSL compiler, either to SPIR-V and run on
// Vulkan by vk_runner, or to DXIL (SM_INTEROP_DXIL) and run on D3D12 WARP by
// d3d12_runner. SM_INTEROP_ENABLE_HALF (with -enable-16bit-types) selects
// the half cases instead, and SM_INTEROP_MATRIX_SWIZZLE the multi-element
// matrix accessor cases.
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
#if defined(SM_INTEROP_OPAQUE_INPUTS)
    // DXC's DXIL path folds intrinsics on constant inputs at compile time
    // (and folds frac(-0.75) to 0.75), so the inputs are xor-ed with bits of
    // the zero-initialized output buffer: the device evaluates every case.
    const uint zero = asuint(outputBuf[0]);
    const float4 opaque_a = asfloat(asuint(IN_A) ^ zero);
    const float4 opaque_b = asfloat(asuint(IN_B) ^ zero);
    const float3 opaque_n = asfloat(asuint(IN_N) ^ zero);
    const float3 opaque_l = asfloat(asuint(IN_L) ^ zero);
    const float3 opaque_c = asfloat(asuint(IN_C) ^ zero);
#  define IN_A opaque_a
#  define IN_B opaque_b
#  define IN_N opaque_n
#  define IN_L opaque_l
#  define IN_C opaque_c
#endif
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
#elif defined(SM_INTEROP_DXIL)
    SM_DXIL_CASES(X)
#else
    SM_DXC_CASES(X)
#endif
#undef X
}
