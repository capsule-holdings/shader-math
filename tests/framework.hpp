// Minimal test framework: CHECK / CHECK_EQF / CHECK_NEAR with counters.
#pragma once

#include <cmath>
#include <cstdio>

inline int g_checks = 0;
inline int g_failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        ++g_checks;                                                          \
        if (!(cond)) {                                                       \
            ++g_failures;                                                    \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);      \
        }                                                                    \
    } while (0)

// Exact float equality: for operations with no rounding (copies, Hadamard of
// exact values, transpose, constructors).
#define CHECK_EQF(a, b)                                                              \
    do {                                                                             \
        ++g_checks;                                                                  \
        const float va_ = (a);                                                       \
        const float vb_ = (b);                                                       \
        if (!(va_ == vb_)) {                                                         \
            ++g_failures;                                                            \
            std::printf("FAIL %s:%d: %s == %s (%g vs %g)\n", __FILE__, __LINE__, #a, \
                        #b, static_cast<double>(va_), static_cast<double>(vb_));     \
        }                                                                            \
    } while (0)

// Tolerant comparison: fmadd vs mul+add and -ffp-contract change rounding
// between backends, so anything with an inner product uses this.
inline bool near_f(float a, float b, float eps = 1e-5f) {
    const float d = std::fabs(a - b);
    const float m = std::fmax(1.0f, std::fmax(std::fabs(a), std::fabs(b)));
    return d <= eps * m;
}

#define CHECK_NEAR(a, b)                                                             \
    do {                                                                             \
        ++g_checks;                                                                  \
        const float va_ = (a);                                                       \
        const float vb_ = (b);                                                       \
        if (!near_f(va_, vb_)) {                                                     \
            ++g_failures;                                                            \
            std::printf("FAIL %s:%d: %s ~= %s (%g vs %g)\n", __FILE__, __LINE__, #a, \
                        #b, static_cast<double>(va_), static_cast<double>(vb_));     \
        }                                                                            \
    } while (0)

// Half-precision tolerant comparison: binary16 has ~11 mantissa bits
// (ulp near 1.0 is about 9.8e-4), and the promoted fallback path adds one
// extra f32 rounding per operation, so 1e-5 can never pass.
#define CHECK_NEAR_H(a, b)                                                           \
    do {                                                                             \
        ++g_checks;                                                                  \
        const float va_ = static_cast<float>(a);                                     \
        const float vb_ = static_cast<float>(b);                                     \
        if (!near_f(va_, vb_, 2e-3f)) {                                              \
            ++g_failures;                                                            \
            std::printf("FAIL %s:%d: %s ~=h %s (%g vs %g)\n", __FILE__, __LINE__,    \
                        #a, #b, static_cast<double>(va_), static_cast<double>(vb_)); \
        }                                                                            \
    } while (0)
