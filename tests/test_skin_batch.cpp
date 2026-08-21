// Approximate-equivalence proof for the batch skinning backends
// (examples/sdl_gpu_skinning/skin_batch.hpp). Pixel-level identity is NOT the
// goal; instead each backend must satisfy, on deterministic random data:
//
//   1. oracle bound   : per-component error vs a double-precision LBS oracle
//                       within an operation-count-derived bound
//   2. pairwise bound : all available backends agree with each other within
//                       the same bound (they may differ in rounding: fmadd
//                       use and operation order are backend-specific)
//   3. properties     : identity palette reproduces the rest pose; a rigid
//                       (rotation+translation) palette preserves distances
//
// Runs without SDL/GPU: this is a pure CPU numeric test.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "../examples/sdl_gpu_skinning/skin_batch.hpp"
#include "framework.hpp"

using smdemo::Corner;
using smdemo::GpuVertex;
using smdemo::SkinBackend;
using shader_math::hlsl::float3;
using shader_math::hlsl::float4x4;

// deterministic PRNG (xorshift) so every platform tests identical data
static uint32_t g_state = 0x12345678u;
static float frand(float lo, float hi) {
    g_state ^= g_state << 13;
    g_state ^= g_state >> 17;
    g_state ^= g_state << 5;
    return lo + (hi - lo) * (float)(g_state & 0xFFFFFFu) / (float)0xFFFFFFu;
}

// double-precision rigid transform (rotation about an axis + translation)
struct Mat34d {
    double m[3][4];
};

static Mat34d random_rigid() {
    const double ax = frand(-1, 1), ay = frand(-1, 1), az = frand(-1, 1);
    const double len = std::sqrt(ax * ax + ay * ay + az * az) + 1e-9;
    const double x = ax / len, y = ay / len, z = az / len;
    const double a = frand(-3.14159f, 3.14159f);
    const double c = std::cos(a), s = std::sin(a), t = 1.0 - c;
    Mat34d r;
    r.m[0][0] = t * x * x + c;     r.m[0][1] = t * x * y - s * z; r.m[0][2] = t * x * z + s * y;
    r.m[1][0] = t * x * y + s * z; r.m[1][1] = t * y * y + c;     r.m[1][2] = t * y * z - s * x;
    r.m[2][0] = t * x * z - s * y; r.m[2][1] = t * y * z + s * x; r.m[2][2] = t * z * z + c;
    r.m[0][3] = frand(-2, 2);
    r.m[1][3] = frand(-2, 2);
    r.m[2][3] = frand(-2, 2);
    return r;
}

static float4x4 to_f32(const Mat34d& d) {
    return float4x4((float)d.m[0][0], (float)d.m[0][1], (float)d.m[0][2], (float)d.m[0][3],
                    (float)d.m[1][0], (float)d.m[1][1], (float)d.m[1][2], (float)d.m[1][3],
                    (float)d.m[2][0], (float)d.m[2][1], (float)d.m[2][2], (float)d.m[2][3],
                    0.0f, 0.0f, 0.0f, 1.0f);
}

// double-precision oracle (uses the float palette values so only the
// ARITHMETIC differs, not the inputs)
static void oracle(const std::vector<Corner>& in, const std::vector<float4x4>& pal,
                   std::vector<double>& pos, std::vector<double>& nrm) {
    pos.resize(in.size() * 3);
    nrm.resize(in.size() * 3);
    for (size_t i = 0; i < in.size(); ++i) {
        const Corner& c = in[i];
        double p[3] = {0, 0, 0}, n[3] = {0, 0, 0};
        for (int w = 0; w < 4; ++w) {
            const double wt = c.weight[w];
            if (wt <= 0.0) continue;
            const float4x4& m = pal[c.joint[w]];
            for (int r = 0; r < 3; ++r) {
                p[r] += wt * ((double)m[r][0] * c.rest_pos[0] +
                              (double)m[r][1] * c.rest_pos[1] +
                              (double)m[r][2] * c.rest_pos[2] + (double)m[r][3]);
                n[r] += wt * ((double)m[r][0] * c.rest_nrm[0] +
                              (double)m[r][1] * c.rest_nrm[1] +
                              (double)m[r][2] * c.rest_nrm[2]);
            }
        }
        const double len = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
        for (int r = 0; r < 3; ++r) {
            pos[i * 3 + r] = p[r];
            nrm[i * 3 + r] = len > 0 ? n[r] / len : 0.0;
        }
    }
}

static double max_rel_err_pos(const std::vector<GpuVertex>& got,
                              const std::vector<double>& want) {
    double worst = 0;
    for (size_t i = 0; i < got.size(); ++i)
        for (int r = 0; r < 3; ++r) {
            const double w = want[i * 3 + r];
            const double scale = std::fabs(w) > 1.0 ? std::fabs(w) : 1.0;
            const double e = std::fabs((double)got[i].pos[r] - w) / scale;
            if (e > worst) worst = e;
        }
    return worst;
}

static double max_abs_err_nrm(const std::vector<GpuVertex>& got,
                              const std::vector<double>& want) {
    double worst = 0;
    for (size_t i = 0; i < got.size(); ++i)
        for (int r = 0; r < 3; ++r) {
            const double e = std::fabs((double)got[i].nrm[r] - want[i * 3 + r]);
            if (e > worst) worst = e;
        }
    return worst;
}

void test_skin_batch() {
    constexpr size_t kJoints = 8;
    constexpr size_t kCorners = 2000;
    // Error budget: LBS is <= 4 weighted mat*vec accumulations (~16 fmadd-class
    // roundings per component) plus a normalize; 1e-5 relative is ~84x the
    // f32 epsilon and holds for every rounding mode combination in play.
    constexpr double kPosBound = 1e-5;
    constexpr double kNrmBound = 1e-4;  // normalize amplifies near-degenerate blends

    std::vector<Mat34d> pal_d(kJoints);
    std::vector<float4x4> pal(kJoints);
    for (size_t j = 0; j < kJoints; ++j) {
        pal_d[j] = random_rigid();
        pal[j] = to_f32(pal_d[j]);
    }

    std::vector<Corner> corners(kCorners);
    for (Corner& c : corners) {
        c.rest_pos = float3(frand(-2, 2), frand(-2, 2), frand(-2, 2));
        const float3 n(frand(-1, 1), frand(-1, 1), frand(-1, 1) + 1.5f);
        c.rest_nrm = shader_math::hlsl::normalize(n);
        const int influences = 1 + (int)(frand(0, 3.999f));
        float total = 0;
        for (int w = 0; w < 4; ++w) {
            if (w < influences) {
                c.joint[w] = (int)(frand(0, (float)kJoints - 0.001f));
                c.weight[w] = frand(0.05f, 1.0f);
                total += c.weight[w];
            } else {
                c.joint[w] = 0;
                c.weight[w] = 0.0f;
            }
        }
        for (int w = 0; w < 4; ++w) c.weight[w] /= total;
    }

    std::vector<double> want_pos, want_nrm;
    oracle(corners, pal, want_pos, want_nrm);

    const SkinBackend backends[] = {SkinBackend::Scalar, SkinBackend::Simd4,
                                    SkinBackend::Avx2Pair};
    std::vector<GpuVertex> results[3];
    int available = 0;

    // 1. oracle bound per backend
    for (int b = 0; b < 3; ++b) {
        if (!smdemo::skin_backend_available(backends[b])) continue;
        results[b].resize(kCorners);
        smdemo::skin_batch(backends[b], corners.data(), kCorners, pal.data(), kJoints,
                           results[b].data());
        const double pe = max_rel_err_pos(results[b], want_pos);
        const double ne = max_abs_err_nrm(results[b], want_nrm);
        std::printf("[skin %s] max pos rel err %.3g, max nrm abs err %.3g\n",
                    smdemo::skin_backend_name(backends[b]), pe, ne);
        CHECK(pe <= kPosBound);
        CHECK(ne <= kNrmBound);
        ++available;
    }
    CHECK(available >= 2);  // scalar + simd4 always; avx2pair on FMA+AVX builds

    // 2. pairwise agreement of the available backends
    for (int a = 0; a < 3; ++a)
        for (int b = a + 1; b < 3; ++b) {
            if (results[a].empty() || results[b].empty()) continue;
            double worst = 0;
            for (size_t i = 0; i < kCorners; ++i)
                for (int r = 0; r < 3; ++r) {
                    const double w = results[a][i].pos[r];
                    const double scale = std::fabs(w) > 1.0 ? std::fabs(w) : 1.0;
                    const double e =
                        std::fabs((double)results[b][i].pos[r] - w) / scale;
                    if (e > worst) worst = e;
                }
            CHECK(worst <= kPosBound);
        }

    // 3a. identity palette + convex weights reproduces the rest pose
    {
        std::vector<float4x4> ident(kJoints);
        for (size_t j = 0; j < kJoints; ++j) ident[j] = float4x4::identity();
        std::vector<GpuVertex> out(kCorners);
        for (int b = 0; b < 3; ++b) {
            if (!smdemo::skin_backend_available(backends[b])) continue;
            smdemo::skin_batch(backends[b], corners.data(), kCorners, ident.data(),
                               kJoints, out.data());
            double worst = 0;
            for (size_t i = 0; i < kCorners; ++i)
                for (int r = 0; r < 3; ++r) {
                    const double e =
                        std::fabs((double)out[i].pos[r] - (double)corners[i].rest_pos[r]);
                    if (e > worst) worst = e;
                }
            CHECK(worst <= 1e-6);  // only weight-sum rounding remains
        }
    }

    // 3b. a single rigid transform preserves pairwise distances (isometry)
    {
        std::vector<Corner> rigid = corners;
        for (Corner& c : rigid) {
            c.joint[0] = 1;
            c.weight[0] = 1.0f;
            for (int w = 1; w < 4; ++w) c.weight[w] = 0.0f;
        }
        std::vector<GpuVertex> out(kCorners);
        for (int b = 0; b < 3; ++b) {
            if (!smdemo::skin_backend_available(backends[b])) continue;
            smdemo::skin_batch(backends[b], rigid.data(), kCorners, pal.data(), kJoints,
                               out.data());
            double worst = 0;
            for (size_t i = 0; i + 7 < kCorners; i += 7) {
                const Corner& a = rigid[i];
                const Corner& c = rigid[i + 7];
                double d0 = 0, d1 = 0;
                for (int r = 0; r < 3; ++r) {
                    const double dr = (double)a.rest_pos[r] - (double)c.rest_pos[r];
                    d0 += dr * dr;
                    const double ds = (double)out[i].pos[r] - (double)out[i + 7].pos[r];
                    d1 += ds * ds;
                }
                d0 = std::sqrt(d0);
                d1 = std::sqrt(d1);
                const double e = std::fabs(d1 - d0) / (d0 > 1.0 ? d0 : 1.0);
                if (e > worst) worst = e;
            }
            CHECK(worst <= 1e-5);
        }
    }
}

int main() {
    test_skin_batch();
    std::printf("[skin-batch] %d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
