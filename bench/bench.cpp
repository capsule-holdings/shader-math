// Micro-benchmarks for the hot paths (informational; nothing is asserted).
//
// Each kernel runs over a working set of kCount elements; the reported
// figure is the best of kTrials runs in nanoseconds per element, which is
// the least noisy number on a shared CI runner. The same source is built
// twice (shader_math_bench / shader_math_bench_scalar) so the SIMD backend
// can be compared with the scalar reference.
//
// The array loops measure throughput, and in the scalar twin the compiler
// may vectorize such a loop across elements (gcc -O3 does for inverse and
// normalize), which the SIMD backend's 4-lane code cannot get. The "latency"
// rows feed each result into the next call, so neither build can batch.
//
//   shader_math_bench [--markdown | --json]
//
// --json prints the github-action-benchmark "customSmallerIsBetter" array;
// each name carries the backend so the SIMD and scalar runs can share a chart.
#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>

#include <shader_math/shader_math.hpp>

namespace {

constexpr int kCount = 4096;
constexpr int kTrials = 15;
constexpr int kReps = 64;  // passes over the working set per trial

// Keeps a value alive without a memory round trip on gcc / clang.
template <class T>
inline void keep(T& v) {
#if defined(__GNUC__)
    asm volatile("" : "+m"(v) : : "memory");
#else
    static volatile char sink;
    sink = *reinterpret_cast<volatile char*>(&v);
#endif
}

template <class F>
double best_ns_per_elem(F&& kernel) {
    double best = 1e30;
    for (int t = 0; t < kTrials; ++t) {
        const auto t0 = std::chrono::steady_clock::now();
        for (int r = 0; r < kReps; ++r) kernel();
        const auto t1 = std::chrono::steady_clock::now();
        const double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
        const double per = ns / (static_cast<double>(kReps) * kCount);
        if (per < best) best = per;
    }
    return best;
}

const char* backend() {
#if defined(SHADER_MATH_ISA_NEON)
#  if defined(SHADER_MATH_HAS_FP16)
    return "NEON+FP16";
#  else
    return "NEON";
#  endif
#elif defined(SHADER_MATH_ISA_SSE2)
#  if defined(SHADER_MATH_HAS_AVX_2ROW)
    return "SSE2+SSE4.1+FMA+AVX";
#  elif defined(SHADER_MATH_HAS_FMA)
    return "SSE2+FMA";
#  elif defined(SHADER_MATH_HAS_SSE41)
    return "SSE2+SSE4.1";
#  else
    return "SSE2";
#  endif
#else
    return "scalar";
#endif
}

float seed_value(int i) { return static_cast<float>((i * 37) % 101) * 0.03125f - 1.5f; }

}  // namespace

int main(int argc, char** argv) {
    const bool markdown = argc > 1 && std::strcmp(argv[1], "--markdown") == 0;
    const bool json = argc > 1 && std::strcmp(argv[1], "--json") == 0;
    namespace h = shader_math::hlsl;
    namespace g = shader_math::glsl;

    std::vector<h::float4> v(kCount);
    std::vector<h::float4> out(kCount);
    std::vector<h::float4x4> m(kCount);
    std::vector<h::float4x4> mo(kCount);
    std::vector<g::mat4> gm(kCount);
    std::vector<g::mat4> gmo(kCount);
    std::vector<float> s(kCount);
    for (int i = 0; i < kCount; ++i) {
        const float a = seed_value(i);
        v[i] = h::float4(a, a + 0.25f, a - 0.5f, 1.0f);
        m[i] = h::float4x4(a, 0.5f, 0.0f, 0.25f, 0.125f, a, 0.5f, 0.0f, 0.0f, 0.25f, a, 0.5f,
                           0.5f, 0.0f, 0.125f, 1.0f);
        gm[i] = g::mat4(m[i]._11, m[i]._21, m[i]._31, m[i]._41, m[i]._12, m[i]._22, m[i]._32,
                        m[i]._42, m[i]._13, m[i]._23, m[i]._33, m[i]._43, m[i]._14, m[i]._24,
                        m[i]._34, m[i]._44);
    }

    struct Row {
        const char* name;
        double ns;
    };
    std::vector<Row> rows;

    rows.push_back({"hlsl mul(float4x4, float4x4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) mo[i] = h::mul(m[i], m[(i + 1) % kCount]);
                        keep(mo[0]);
                    })});
    rows.push_back({"glsl mat4 * mat4", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) gmo[i] = gm[i] * gm[(i + 1) % kCount];
                        keep(gmo[0]);
                    })});
    rows.push_back({"hlsl mul(float4, float4x4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) out[i] = h::mul(v[i], m[i]);
                        keep(out[0]);
                    })});
    rows.push_back({"hlsl mul(float4x4, float4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) out[i] = h::mul(m[i], v[i]);
                        keep(out[0]);
                    })});
    rows.push_back({"hlsl inverse(float4x4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) mo[i] = shader_math::ext::inverse(m[i]);
                        keep(mo[0]);
                    })});
    h::float4x4 chain_m = m[7];
    rows.push_back({"hlsl inverse(float4x4) latency", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) chain_m = shader_math::ext::inverse(chain_m);
                        keep(chain_m);
                    })});
    rows.push_back({"dot(float4, float4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) s[i] = h::dot(v[i], v[(i + 1) % kCount]);
                        keep(s[0]);
                    })});
    rows.push_back({"normalize(float3)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) out[i].xyz = h::normalize(v[i].xyz);
                        keep(out[0]);
                    })});
    h::float3 chain_v(0.3f, -0.4f, 0.5f);
    const h::float3 nudge(0.125f, 0.25f, -0.0625f);
    rows.push_back({"normalize(float3) latency", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) chain_v = h::normalize(chain_v + nudge);
                        keep(chain_v);
                    })});
    rows.push_back({"floor(float4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) out[i] = h::floor(v[i]);
                        keep(out[0]);
                    })});
    rows.push_back({"round(float4)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i) out[i] = h::round(v[i]);
                        keep(out[0]);
                    })});
    rows.push_back({"lerp(float4, float4, float)", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i)
                            out[i] = h::lerp(v[i], v[(i + 1) % kCount], 0.25f);
                        keep(out[0]);
                    })});
#if defined(SHADER_MATH_HAS_HALF)
    std::vector<h::half4> hv(kCount);
    std::vector<h::half4> ho(kCount);
    for (int i = 0; i < kCount; ++i) hv[i] = h::half4(v[i]);
    rows.push_back({"half4 a * b + c", best_ns_per_elem([&] {
                        for (int i = 0; i < kCount; ++i)
                            ho[i] = hv[i] * hv[(i + 1) % kCount] + hv[(i + 2) % kCount];
                        keep(ho[0]);
                    })});
#endif

    if (json) {
        std::printf("[\n");
        for (std::size_t i = 0; i < rows.size(); ++i)
            std::printf("  {\"name\": \"%s [%s]\", \"unit\": \"ns/elem\", \"value\": %.3f}%s\n",
                        rows[i].name, backend(), rows[i].ns, i + 1 < rows.size() ? "," : "");
        std::printf("]\n");
    } else if (markdown) {
        std::printf("| kernel (%s) | ns / element |\n| --- | ---: |\n", backend());
        for (const Row& r : rows) std::printf("| %s | %.2f |\n", r.name, r.ns);
    } else {
        std::printf("backend: %s  (best of %d, %d elements x %d passes)\n", backend(), kTrials,
                    kCount, kReps);
        for (const Row& r : rows) std::printf("  %-32s %8.2f ns/elem\n", r.name, r.ns);
    }
    return 0;
}
