// Slang side of the half cross-check: the half cases compiled to SPIR-V
// (slangc -target spirv) and run on Vulkan by the shared vk_runner, since the
// CPU target cannot compile half vector arithmetic.
#define SM_INTEROP_ENABLE_HALF 1
#include "shared/math_cases.hlsli"
#include "shared/case_list.h"

RWStructuredBuffer<float> outputBuf;

[shader("compute")]
[numthreads(1, 1, 1)]
void runCases(uint3 tid: SV_DispatchThreadID)
{
    int slot = 0;
#define X(name, expr)          \
    {                          \
        float4 r = (expr);     \
        outputBuf[slot] = r.x; \
        slot = slot + 1;       \
        outputBuf[slot] = r.y; \
        slot = slot + 1;       \
        outputBuf[slot] = r.z; \
        slot = slot + 1;       \
        outputBuf[slot] = r.w; \
        slot = slot + 1;       \
    }
    SM_INTEROP_HALF_CASES(X)
#undef X
}
