// C++ side of the GLSL cross-check: evaluates the SAME shared cases through
// shader-math's glsl surface and dumps "name[i] 0xBITS" lines.
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <shader_math/shader_math.hpp>

namespace interop_glsl {
using namespace shader_math::glsl;

#define SM_NOT not_
#include "math_cases.glsl"

#include "case_list.h"

// SM_GLSL_HALF (set by CMake for the half twin) selects the f16 cases
#if defined(SM_GLSL_HALF)
#  include "half_cases.glsl"
#  include "half_case_list.h"
#  define SM_HARNESS_CASES SM_GLSL_HALF_CASES
#else
#  define SM_HARNESS_CASES SM_GLSL_CASES
#endif

static const char* const kNames[] = {
#define X(name, expr) #name,
    SM_HARNESS_CASES(X)
#undef X
};

inline int run_cases(float* out) {
    int slot = 0;
#define X(name, expr)               \
    {                               \
        const vec4 r = vec4(expr);  \
        out[slot++] = r[0];         \
        out[slot++] = r[1];         \
        out[slot++] = r[2];         \
        out[slot++] = r[3];         \
    }
    SM_HARNESS_CASES(X)
#undef X
    return slot;
}

}  // namespace interop_glsl

int main() {
    constexpr int kCases = static_cast<int>(sizeof(interop_glsl::kNames) / sizeof(interop_glsl::kNames[0]));
    float out[kCases * 4] = {};
    if (interop_glsl::run_cases(out) != kCases * 4) return 1;
    for (int c = 0; c < kCases; ++c) {
        for (int i = 0; i < 4; ++i) {
            std::uint32_t bits;
            std::memcpy(&bits, &out[c * 4 + i], sizeof(bits));
            std::printf("%s[%d] 0x%08x\n", interop_glsl::kNames[c], i, bits);
        }
    }
    return 0;
}
