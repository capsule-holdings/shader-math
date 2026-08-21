// Batch linear-blend skinning with switchable native backends.
//
// shader-math itself stays compile-time-only (no runtime dispatch), but this
// is application-layer code where runtime backend selection is fine:
//
//   Scalar   - plain float loop; doubles as the readable reference
//   Simd4    - shader-math per-vertex (horizontal 4-wide mul/normalize)
//   Avx2Pair - two vertices per YMM register (vertical), available only on
//              SSE2+FMA+AVX builds. Extends the avx2row trick: because
//              _mm256_permute_ps broadcasts per 128-bit half, the two halves
//              can be multiplied by two DIFFERENT matrices, so a vertex pair
//              with different joints is skinned in one accumulator chain.
//              Uses the row-vector form (v * M^T == M * v), so the palette is
//              transposed once per call.
//
// All backends implement the same math; they agree with a double-precision
// oracle within documented bounds (see tests/test_skin_batch.cpp).
#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

#include <shader_math/shader_math.hpp>

namespace smdemo {

using shader_math::hlsl::float3;
using shader_math::hlsl::float4;
using shader_math::hlsl::float4x4;

struct Corner {  // one triangle corner (expanded, no index reuse)
    float3 rest_pos;
    float3 rest_nrm;
    int joint[4];
    float weight[4];
};

// What the GPU sees (pitch 24). Deliberately plain floats, NOT shader-math
// float3: sizeof(float3) == 16 (register storage) is language compatibility,
// not a GPU ABI. fvec::store() is the sanctioned bridge (writes exactly N
// lanes).
struct GpuVertex {
    float pos[3];
    float nrm[3];
};

enum class SkinBackend { Scalar, Simd4, Avx2Pair };

inline const char* skin_backend_name(SkinBackend b) {
    switch (b) {
        case SkinBackend::Scalar: return "scalar";
        case SkinBackend::Simd4: return "simd4";
        case SkinBackend::Avx2Pair: return "avx2pair";
    }
    return "?";
}

inline bool skin_backend_available(SkinBackend b) {
#if defined(SHADER_MATH_HAS_AVX_2ROW)
    (void)b;
    return true;
#else
    return b != SkinBackend::Avx2Pair;
#endif
}

// ---- Scalar: plain float reference ------------------------------------------
inline void skin_scalar(const Corner* in, size_t n, const float4x4* palette,
                        GpuVertex* out) {
    for (size_t i = 0; i < n; ++i) {
        const Corner& c = in[i];
        const float rx = c.rest_pos[0], ry = c.rest_pos[1], rz = c.rest_pos[2];
        const float ux = c.rest_nrm[0], uy = c.rest_nrm[1], uz = c.rest_nrm[2];
        float px = 0, py = 0, pz = 0, nx = 0, ny = 0, nz = 0;
        for (int w = 0; w < 4; ++w) {
            const float wt = c.weight[w];
            if (wt <= 0.0f) continue;
            const float4x4& m = palette[c.joint[w]];
            px += wt * (m[0][0] * rx + m[0][1] * ry + m[0][2] * rz + m[0][3]);
            py += wt * (m[1][0] * rx + m[1][1] * ry + m[1][2] * rz + m[1][3]);
            pz += wt * (m[2][0] * rx + m[2][1] * ry + m[2][2] * rz + m[2][3]);
            nx += wt * (m[0][0] * ux + m[0][1] * uy + m[0][2] * uz);
            ny += wt * (m[1][0] * ux + m[1][1] * uy + m[1][2] * uz);
            nz += wt * (m[2][0] * ux + m[2][1] * uy + m[2][2] * uz);
        }
        const float len = std::sqrt(nx * nx + ny * ny + nz * nz);
        const float inv = len > 0.0f ? 1.0f / len : 0.0f;
        out[i] = {{px, py, pz}, {nx * inv, ny * inv, nz * inv}};
    }
}

// ---- Simd4: shader-math per-vertex (horizontal) ------------------------------
inline void skin_simd4(const Corner* in, size_t n, const float4x4* palette,
                       GpuVertex* out) {
    using namespace shader_math::hlsl;
    for (size_t i = 0; i < n; ++i) {
        const Corner& c = in[i];
        const float4 rp(c.rest_pos, 1.0f);
        const float4 rn(c.rest_nrm, 0.0f);
        float4 p = float4(0.0f);
        float4 nv = float4(0.0f);
        for (int w = 0; w < 4; ++w) {
            if (c.weight[w] <= 0.0f) continue;
            const float4x4& m = palette[c.joint[w]];
            p += mul(m, rp) * c.weight[w];
            nv += mul(m, rn) * c.weight[w];
        }
        float3(p).store(out[i].pos);
        normalize(float3(nv)).store(out[i].nrm);
    }
}

// ---- Avx2Pair: two vertices per YMM (vertical) --------------------------------
#if defined(SHADER_MATH_HAS_AVX_2ROW)
inline void skin_avx2pair(const Corner* in, size_t n, const float4x4* palette,
                          size_t palette_count, GpuVertex* out) {
    namespace d = shader_math::detail;
    using shader_math::hlsl::mul;
    using shader_math::hlsl::normalize;
    using shader_math::hlsl::transpose;
    using avx = d::avx2row;

    // Row-vector form needs M^T: transpose the palette once per call.
    std::vector<float4x4> pal_t(palette_count);
    for (size_t j = 0; j < palette_count; ++j) pal_t[j] = transpose(palette[j]);

    size_t i = 0;
    for (; i + 1 < n; i += 2) {
        const Corner& c0 = in[i];
        const Corner& c1 = in[i + 1];
        const float4 p0(c0.rest_pos, 1.0f), p1(c1.rest_pos, 1.0f);
        const float4 n0(c0.rest_nrm, 0.0f), n1(c1.rest_nrm, 0.0f);
        const avx::dreg P = avx::pack(p0.q_, p1.q_);
        const avx::dreg N = avx::pack(n0.q_, n1.q_);
        avx::dreg accP = avx::zero();
        avx::dreg accN = avx::zero();
        for (int w = 0; w < 4; ++w) {
            const float4x4& t0 = pal_t[c0.joint[w]];
            const float4x4& t1 = pal_t[c1.joint[w]];
            avx::dreg termP = avx::zero();
            avx::dreg termN = avx::zero();
            d::unroll<4>([&]<int J>() {
                // [t0.row_J | t1.row_J]: a different matrix per 128-bit half
                const avx::dreg rows = avx::pack(t0.row_[J].q_, t1.row_[J].q_);
                termP = avx::fmadd(avx::bcast_per_lane<J>(P), rows, termP);
                termN = avx::fmadd(avx::bcast_per_lane<J>(N), rows, termN);
            });
            const avx::dreg wgt = avx::pack(d::ops::splat(c0.weight[w]),
                                            d::ops::splat(c1.weight[w]));
            accP = avx::fmadd(termP, wgt, accP);
            accN = avx::fmadd(termN, wgt, accN);
        }
        const float4 rp0(avx::lo(accP)), rp1(avx::hi(accP));
        const float4 rn0(avx::lo(accN)), rn1(avx::hi(accN));
        float3(rp0).store(out[i].pos);
        float3(rp1).store(out[i + 1].pos);
        normalize(float3(rn0)).store(out[i].nrm);
        normalize(float3(rn1)).store(out[i + 1].nrm);
    }
    if (i < n) skin_simd4(in + i, n - i, palette, out + i);  // odd tail
}
#endif

// ---- dispatch ------------------------------------------------------------------
inline void skin_batch(SkinBackend backend, const Corner* in, size_t n,
                       const float4x4* palette, size_t palette_count, GpuVertex* out) {
    switch (backend) {
        case SkinBackend::Scalar: skin_scalar(in, n, palette, out); return;
        case SkinBackend::Simd4: skin_simd4(in, n, palette, out); return;
        case SkinBackend::Avx2Pair:
#if defined(SHADER_MATH_HAS_AVX_2ROW)
            skin_avx2pair(in, n, palette, palette_count, out);
            return;
#else
            (void)palette_count;
            skin_simd4(in, n, palette, out);  // graceful fallback
            return;
#endif
    }
}

}  // namespace smdemo
