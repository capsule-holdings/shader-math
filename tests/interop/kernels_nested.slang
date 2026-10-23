// Slang side of the nested-swizzle cross-check (Slang CPU target, like
// kernels.slang); the C++ twin is built with SHADER_MATH_NESTED_SWIZZLE.
#include "shared/math_cases.hlsli"
#include "shared/nested_cases.hlsli"
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
    SM_INTEROP_NESTED_CASES(X)
#undef X
}
