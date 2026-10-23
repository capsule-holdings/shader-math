// C++ side of the cross-check: evaluates the SAME shared cases through
// shader-math's hlsl surface and dumps the same "name[i] 0xBITS" lines.
// The dual compilation of math_cases.hlsli is itself the subset lint: any
// construct outside the shared subset fails one of the two compilers.
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <shader_math/shader_math.hpp>

#if !defined(SHADER_MATH_HAS_HALF)
#  error "the interop harness requires a compiler with half support (gcc 12+ / clang 15+)"
#endif

namespace interop {
using namespace shader_math::hlsl;

#include "shared/math_cases.hlsli"

#include "shared/case_list.h"

// SM_INTEROP_ENABLE_HALF (set by CMake for the half twin) selects the half
// list, which is checked against the SPIR-V kernel instead of Slang-CPU;
// SM_INTEROP_NESTED (the twin also built with SHADER_MATH_NESTED_SWIZZLE)
// selects the nested-swizzle list; SM_INTEROP_MATRIX_SWIZZLE (built with
// SHADER_MATH_MATRIX_SWIZZLE3) the multi-element matrix accessor list;
// SM_INTEROP_DXC the shared list plus the DXC-only cases; SM_INTEROP_DXIL
// those plus lit
#if defined(SM_INTEROP_ENABLE_HALF)
#  define SM_HARNESS_CASES SM_INTEROP_HALF_CASES
#elif defined(SM_INTEROP_DXIL)
#  include "dxc/dxc_cases.hlsli"
#  include "dxc/dxc_case_list.h"
#  define SM_HARNESS_CASES SM_DXIL_CASES
#elif defined(SM_INTEROP_DXC)
#  include "dxc/dxc_cases.hlsli"
#  include "dxc/dxc_case_list.h"
#  define SM_HARNESS_CASES SM_DXC_CASES
#elif defined(SM_INTEROP_MATRIX_SWIZZLE)
#  include "shared/matrix_swizzle_cases.hlsli"
#  define SM_HARNESS_CASES SM_INTEROP_MATRIX_SWIZZLE_CASES
#elif defined(SM_INTEROP_NESTED)
#  include "shared/nested_cases.hlsli"
#  define SM_HARNESS_CASES SM_INTEROP_NESTED_CASES
#else
#  define SM_HARNESS_CASES SM_INTEROP_CASES
#endif

inline int run_cases(float* out) {
    int slot = 0;
#define X(name, expr)                    \
    {                                    \
        const float4 r = float4(expr);   \
        out[slot++] = r[0];              \
        out[slot++] = r[1];              \
        out[slot++] = r[2];              \
        out[slot++] = r[3];              \
    }
    SM_HARNESS_CASES(X)
#undef X
    return slot;
}

inline const char* const* case_names(int* count) {
    static const char* names[] = {
#define X(name, expr) #name,
        SM_HARNESS_CASES(X)
#undef X
    };
    *count = static_cast<int>(sizeof(names) / sizeof(names[0]));
    return names;
}

}  // namespace interop

int main() {
    int cases = 0;
    const char* const* names = interop::case_names(&cases);
    float out[256] = {};
    const int slots = interop::run_cases(out);
    if (slots != cases * 4) {
        std::fprintf(stderr, "slot/case mismatch\n");
        return 1;
    }
    for (int c = 0; c < cases; ++c) {
        for (int i = 0; i < 4; ++i) {
            std::uint32_t bits;
            std::memcpy(&bits, &out[c * 4 + i], sizeof(bits));
            std::printf("%s[%d] 0x%08x\n", names[c], i, bits);
        }
    }
    return 0;
}
