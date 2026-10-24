// Randomized differential tests: every backend against a plain-float (or
// double) model on seeded inputs, complementing the hand-picked values in
// the other files.
//
// Single-rounding operations (+ - * /, sqrt, rounding, min / max,
// comparisons, swizzles, transpose) must match the model bit for bit.
// Inner products (dot, mul, glsl *) are checked against a double model with
// the standard error bound n * eps * sum|a_i * b_i| (fmadd vs mul + add only
// moves the result within it). On failure the seed, iteration and inputs
// are printed so the case can be replayed.
//
// The generator is splitmix64 with an explicit float mapping: the standard
// <random> distributions are implementation-defined, which would give each
// standard library different inputs.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <shader_math/shader_math.hpp>

#include "framework.hpp"

namespace {

constexpr std::uint64_t kSeed = 0x5eed5ade12345678ull;
constexpr int kIters = 2000;

struct Rng {
    std::uint64_t s;
    std::uint64_t next() {
        std::uint64_t z = (s += 0x9e3779b97f4a7c15ull);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
        return z ^ (z >> 31);
    }
    // uniform in [0, 1) with 24 random bits
    float unit() { return static_cast<float>(next() >> 40) * 0x1.0p-24f; }
    // mostly wide-range magnitudes, with values that stress rounding and
    // signed zero mixed in
    float value() {
        const std::uint64_t pick = next() % 16;
        const float sign = (next() & 1) ? -1.0f : 1.0f;
        switch (pick) {
            case 0: return sign * 0.0f;
            case 1: return sign * (static_cast<float>(next() % 64) + 0.5f);  // exact halves
            case 2: return sign * (8388608.0f + static_cast<float>(next() % 16));  // 2^23 + k
            case 3: return sign * 0.49999997f;
            default: {
                const float e = unit() * 16.0f - 8.0f;  // 2^-8 .. 2^8
                return sign * std::exp2(e) * (1.0f + unit());
            }
        }
    }
    // nonzero divisor
    float divisor() {
        float v = value();
        while (v == 0.0f) v = value();
        return v;
    }
};

bool same_bits(float a, float b) {
    if (std::isnan(a) || std::isnan(b)) return std::isnan(a) && std::isnan(b);
    std::uint32_t ua;
    std::uint32_t ub;
    std::memcpy(&ua, &a, sizeof(ua));
    std::memcpy(&ub, &b, sizeof(ub));
    return ua == ub;
}

// |got - want| <= n * eps * sum|terms| (+ a tiny absolute floor)
bool within_dot_bound(float got, double want, double abs_terms, int n) {
    const double bound = n * 0x1.0p-23 * abs_terms + 1e-30;
    return std::fabs(static_cast<double>(got) - want) <= bound;
}

void report(const char* what, int iter, const float* in, int n_in) {
    std::printf("FAIL random %s: seed=0x%llx iter=%d inputs:", what,
                static_cast<unsigned long long>(kSeed), iter);
    for (int i = 0; i < n_in; ++i) std::printf(" %a", static_cast<double>(in[i]));
    std::printf("\n");
}

void test_vector_exact() {
    using namespace shader_math::hlsl;
    Rng rng{kSeed};
    bool ok = true;
    for (int it = 0; it < kIters && ok; ++it) {
        float a[4];
        float b[4];
        for (int i = 0; i < 4; ++i) {
            a[i] = rng.value();
            b[i] = rng.divisor();
        }
        const float4 va(a[0], a[1], a[2], a[3]);
        const float4 vb(b[0], b[1], b[2], b[3]);
        const float4 add = va + vb;
        const float4 sub = va - vb;
        const float4 mul = va * vb;
        const float4 div = va / vb;
        const float4 sq = sqrt(abs(va));
        const float4 fl = floor(va);
        const float4 ce = ceil(va);
        const float4 tr = trunc(va);
        const float4 ro = round(va);
        const float4 mn = min(va, vb);
        const float4 mx = max(va, vb);
        const bool4 lt = va < vb;
        const bool4 eq = va == va;
        const float4 sw = va.wzxy;
        for (int i = 0; i < 4 && ok; ++i) {
            ok = same_bits(add[i], a[i] + b[i]) && same_bits(sub[i], a[i] - b[i]) &&
                 same_bits(mul[i], a[i] * b[i]) && same_bits(div[i], a[i] / b[i]) &&
                 same_bits(sq[i], std::sqrt(std::fabs(a[i]))) &&
                 same_bits(fl[i], std::floor(a[i])) && same_bits(ce[i], std::ceil(a[i])) &&
                 same_bits(tr[i], std::trunc(a[i])) && same_bits(ro[i], std::nearbyint(a[i])) &&
                 mn[i] == (b[i] < a[i] ? b[i] : a[i]) && mx[i] == (a[i] < b[i] ? b[i] : a[i]) &&
                 lt[i] == (a[i] < b[i]) && eq[i];
        }
        const int perm[4] = {3, 2, 0, 1};
        for (int i = 0; i < 4 && ok; ++i) ok = same_bits(sw[i], a[perm[i]]);
        if (!ok) {
            const float in[8] = {a[0], a[1], a[2], a[3], b[0], b[1], b[2], b[3]};
            report("vector exact ops", it, in, 8);
        }
    }
    CHECK(ok);

    // glsl::roundEven is the only GLSL-only rounding spelling
    Rng rng2{kSeed ^ 1};
    bool even_ok = true;
    for (int it = 0; it < kIters && even_ok; ++it) {
        const float x = rng2.value();
        const float r = shader_math::glsl::roundEven(shader_math::glsl::vec2(x, x))[0];
        even_ok = same_bits(r, std::nearbyint(x));
        if (!even_ok) report("roundEven", it, &x, 1);
    }
    CHECK(even_ok);
}

void test_dot_and_products() {
    using namespace shader_math::hlsl;
    Rng rng{kSeed ^ 2};
    bool ok = true;
    for (int it = 0; it < kIters && ok; ++it) {
        float m[16];
        float v[4];
        for (float& x : m) x = rng.value();
        for (float& x : v) x = rng.value();
        const float4x4 hm(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10],
                          m[11], m[12], m[13], m[14], m[15]);  // row-major
        const float4 hv(v[0], v[1], v[2], v[3]);

        // dot
        double want = 0.0;
        double terms = 0.0;
        for (int i = 0; i < 4; ++i) {
            want += static_cast<double>(m[i]) * v[i];
            terms += std::fabs(static_cast<double>(m[i]) * v[i]);
        }
        ok = within_dot_bound(dot(hm[0], hv), want, terms, 4);

        // mul(m, v) / mul(v, m) and the GLSL spelling of the same products
        shader_math::glsl::mat4 gm;
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c) gm[c][r] = m[r * 4 + c];
        const shader_math::glsl::vec4 gv(v[0], v[1], v[2], v[3]);
        const float4 mv = mul(hm, hv);
        const float4 vm = mul(hv, hm);
        const shader_math::glsl::vec4 gmv = gm * gv;
        const shader_math::glsl::vec4 gvm = gv * gm;
        for (int r = 0; r < 4 && ok; ++r) {
            double w_mv = 0.0;
            double t_mv = 0.0;
            double w_vm = 0.0;
            double t_vm = 0.0;
            for (int k = 0; k < 4; ++k) {
                w_mv += static_cast<double>(m[r * 4 + k]) * v[k];
                t_mv += std::fabs(static_cast<double>(m[r * 4 + k]) * v[k]);
                w_vm += static_cast<double>(v[k]) * m[k * 4 + r];
                t_vm += std::fabs(static_cast<double>(v[k]) * m[k * 4 + r]);
            }
            ok = within_dot_bound(mv[r], w_mv, t_mv, 4) && within_dot_bound(vm[r], w_vm, t_vm, 4) &&
                 within_dot_bound(gmv[r], w_mv, t_mv, 4) &&
                 within_dot_bound(gvm[r], w_vm, t_vm, 4);
        }

        // mul(m, m), transpose (exact)
        const float4x4 mm = mul(hm, hm);
        const float4x4 tp = transpose(hm);
        for (int r = 0; r < 4 && ok; ++r)
            for (int c = 0; c < 4 && ok; ++c) {
                double w = 0.0;
                double t = 0.0;
                for (int k = 0; k < 4; ++k) {
                    w += static_cast<double>(m[r * 4 + k]) * m[k * 4 + c];
                    t += std::fabs(static_cast<double>(m[r * 4 + k]) * m[k * 4 + c]);
                }
                ok = within_dot_bound(mm[r][c], w, t, 4) && same_bits(tp[r][c], m[c * 4 + r]);
            }

        if (!ok) {
            float in[20];
            std::memcpy(in, m, sizeof(m));
            std::memcpy(in + 16, v, sizeof(v));
            report("dot / mul / glsl *", it, in, 20);
        }
    }
    CHECK(ok);
}

// inverse on diagonally dominant matrices (well conditioned): A * A^-1 ~ I
void test_inverse() {
    using namespace shader_math::hlsl;
    Rng rng{kSeed ^ 3};
    bool ok = true;
    for (int it = 0; it < kIters && ok; ++it) {
        float m[16];
        for (int r = 0; r < 4; ++r)
            for (int c = 0; c < 4; ++c)
                m[r * 4 + c] = (r == c) ? 8.0f + 4.0f * rng.unit() : rng.unit() * 2.0f - 1.0f;
        const float4x4 a(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11],
                         m[12], m[13], m[14], m[15]);
        const float4x4 p = mul(a, inverse(a));
        for (int r = 0; r < 4 && ok; ++r)
            for (int c = 0; c < 4 && ok; ++c) ok = std::fabs(p[r][c] - (r == c ? 1.0f : 0.0f)) < 1e-5f;
        if (!ok) report("inverse", it, m, 16);
    }
    CHECK(ok);
}

}  // namespace

void test_random() {
    test_vector_exact();
    test_dot_and_products();
    test_inverse();
}
