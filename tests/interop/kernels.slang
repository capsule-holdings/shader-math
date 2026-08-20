// Slang side of the cross-check: evaluates the shared cases on the Slang CPU
// target (slangc -target cpp) and writes 4 floats per case to outputBuf.
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
    SM_INTEROP_CASES(X)
#undef X
}
