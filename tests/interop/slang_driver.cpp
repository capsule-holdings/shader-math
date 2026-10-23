// Host driver for the Slang-CPU kernel: runs the generated compute entry and
// dumps "name[i] 0xBITS" lines. The case names come from the shared X-macro;
// the case expressions are never compiled here.
#include "slang-cpp-prelude.h"  // from the Slang release include/ directory

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "shared/case_list.h"

// SM_DRIVER_CASES: the list the linked kernel was generated from
#if !defined(SM_DRIVER_CASES)
#  define SM_DRIVER_CASES SM_INTEROP_CASES
#endif

extern "C" void runCases(ComputeVaryingInput*, void*, void*);

// Mirrors the generated GlobalParams struct (one RWStructuredBuffer global).
struct GlobalParams {
    RWStructuredBuffer<float> outputBuf;
};

static const char* kCaseNames[] = {
#define X(name, expr) #name,
    SM_DRIVER_CASES(X)
#undef X
};

int main() {
    constexpr int kCases = static_cast<int>(sizeof(kCaseNames) / sizeof(kCaseNames[0]));
    float out[kCases * 4] = {};

    GlobalParams gp;
    gp.outputBuf.data = out;
    gp.outputBuf.count = kCases * 4;

    ComputeVaryingInput varying{};
    varying.startGroupID = {0, 0, 0};
    varying.endGroupID = {1, 1, 1};
    runCases(&varying, nullptr, &gp);

    for (int c = 0; c < kCases; ++c) {
        for (int i = 0; i < 4; ++i) {
            std::uint32_t bits;
            std::memcpy(&bits, &out[c * 4 + i], sizeof(bits));
            std::printf("%s[%d] 0x%08x\n", kCaseNames[c], i, bits);
        }
    }
    return 0;
}
