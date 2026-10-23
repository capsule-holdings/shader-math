window.BENCHMARK_DATA = {
  "lastUpdate": 1792799270592,
  "repoUrl": "https://github.com/capsule-holdings/shader-math",
  "entries": {
    "linux-x64-gcc": [
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "27731254dfb152bc2961e913468453c9b81d21b9",
          "message": "CI: publish the benchmark history to GitHub Pages (#74)\n\nbench.cpp gets --json (github-action-benchmark's customSmallerIsBetter\narray; names carry the backend so SIMD and scalar share a chart). The\nbenchmark step merges the SIMD and scalar JSON and, on push, uploads it as an\nartifact. A new publish-bench job (main pushes only, contents: write, after\nbuild-test unless cancelled) appends each available result to bench/ on the\ngh-pages branch with github-action-benchmark v1.22.2 (pinned), which also\nwrites bench/index.html. Nothing is gated on the numbers. README (ja / en)\nand CONTEXT.md point to https://capsule-holdings.github.io/shader-math/bench/.",
          "timestamp": "2026-10-24T07:48:49+09:00",
          "tree_id": "922c427e8713421c362bea622fc914cb344de096",
          "url": "https://github.com/capsule-holdings/shader-math/commit/27731254dfb152bc2961e913468453c9b81d21b9"
        },
        "date": 1792796396733,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.897,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.773,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.831,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.673,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 8.164,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.602,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.471,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.953,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 5.06,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.347,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.325,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.871,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.901,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.839,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.829,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.057,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.793,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.171,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.905,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.088,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.45,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.757,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "40dccf5b846e15980af60dc8c6ec468419b7f854",
          "message": "bench: measure inverse and normalize latency too (#75)\n\nIn the scalar twin, gcc -O3 vectorizes the inverse / normalize array loops\nacross elements (-fopt-info-vec), which made the SIMD backend look slower\nthere. Two rows now feed each result into the next call (inverse(float4x4)\nlatency, normalize(float3) latency), so neither build can batch, and the\nfile comment explains how to read the array-loop numbers.",
          "timestamp": "2026-10-24T08:02:59+09:00",
          "tree_id": "d61289a3930098805225ccad304408efe45bfec3",
          "url": "https://github.com/capsule-holdings/shader-math/commit/40dccf5b846e15980af60dc8c6ec468419b7f854"
        },
        "date": 1792797264599,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 4.096,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 4.156,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 1.066,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 2.801,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 11.138,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 14.824,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 1.198,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.959,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 9.267,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 5.202,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 9.853,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.405,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.714,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 5.005,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 4.64,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.365,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 7.343,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 6.249,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 15.304,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.176,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.336,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.95,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 3.539,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 6.506,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.711,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 102.99,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e37f9cddd1b8578abc2f4b011bf60dd32ff7b805",
          "message": "test: cross-check HLSL through DXC's DXIL backend on D3D12 WARP (#73)\n\nThe DXC cross-check ran only through SPIR-V on lavapipe. The same HLSL now\nalso goes through DXC's DXIL backend (v1.9.2609, Windows release) and runs on\nWARP: tests/interop/d3d12_runner.cpp, SHADER_MATH_INTEROP_DXIL (Windows),\ndxil_interop_check / dxil_interop_half_check, and the windows-x64-dxil-interop\nCI job (DXC zip pinned by sha256). Every interop job now checks with\nctest --no-tests=error that its cross-check was registered, since CMake only\nwarns when a prerequisite is missing.\n\nFindings, recorded in docs/slang_compat.md:\n- lit: the DXIL backend computes (n.h)^m and matches shader-math on WARP, so\n  the SPIR-V-only defect stays excluded from the SPIR-V check alone\n- frexp: DXIL lowering builds the mantissa as (bits & 0x007fffff) | 0x3f000000\n  and drops the sign of a negative input; case_exact_frexp_dst is split into\n  case_exact_frexp (left out of DXIL) and case_exact_dst\n- frac: DXC's DXIL constant folding turns frac(-0.75) into 0.75; the DXIL\n  kernel xors its inputs with bits of the zero-initialized output buffer\n  (SM_INTEROP_OPAQUE_INPUTS) so the device evaluates every case, and WARP's\n  Frc gives 0.25\n- WARP's asin / acos / atan are about 1e-3 off libm; compare.py gains\n  --tol=<case>:<relative>, used for the two cases that call them\n\nAlso spells a half swizzle-to-float conversion as float(half(v.x)) in the\nshared cases for MSVC's software half.",
          "timestamp": "2026-10-24T08:03:56+09:00",
          "tree_id": "fd6968d65c526d8f65bf18f7e6daf1c88f49be94",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e37f9cddd1b8578abc2f4b011bf60dd32ff7b805"
        },
        "date": 1792797658725,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.616,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.614,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.745,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.706,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 7.53,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.876,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.561,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.378,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.284,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.738,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 4.92,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.31,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.46,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.562,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.645,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.749,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.169,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.752,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.369,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.89,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.075,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.129,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.796,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.703,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.07,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 82.583,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d268b9af19dccd1e5ecab4e92bddb113ffccd28b",
          "message": "Add opt-in multi-element matrix accessors (SHADER_MATH_MATRIX_SWIZZLE) (#70)\n\nHLSL's multi-element matrix accessor lists, opt-in like the other swizzle\nextensions. SHADER_MATH_MATRIX_SWIZZLE declares the 2-element lists in both\nspellings (m._m00_m11, m._41_42); SHADER_MATH_MATRIX_SWIZZLE3 adds the\n3-element ones (m._m30_m31_m32, m._11_22_33). Only lists inside the matrix's\nown shape exist; they convert to vectors, work with dot etc. through\nvec_traits, and are writable when free of repeats (gathered first, so\nm._m01_m10 = m._m10_m01 swaps). There are no 4-element lists. Under either\nmacro the hlsl::matrix union moves into a per-shape storage base\n(detail::hmat_store<R, C, T>); with neither, the class is unchanged.\n\nCMake options and find_package variables follow the other opt-ins, and\nbench/compile_time.py gains both configs: a TU using every matrix type takes\nabout 1.2x with 2-element lists and, with 3-element lists, about 12x on gcc\nand 4x on clang (README known deviations).\n\nTests: shader_math_tests_matrix_swizzle / _matrix_swizzle3 (reads, repeats,\nnon-square shapes, writes, swaps, compound ops, half, and static_asserts for\nthe shape limits, 4-element lists and duplicate writes), test_restrict.cpp\nfor the default build, and shared/matrix_swizzle_cases.hlsli cross-checked\nagainst Slang (CPU target) and DXC (SPIR-V on lavapipe), each against a C++\ntwin built with SHADER_MATH_MATRIX_SWIZZLE3.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T08:41:51+09:00",
          "tree_id": "f06a061a2cc669527ff97791e2b5bd07089278c8",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d268b9af19dccd1e5ecab4e92bddb113ffccd28b"
        },
        "date": 1792799266997,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.808,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.796,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.778,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.769,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 7.901,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 12.199,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.582,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.442,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.69,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 3.008,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 5.219,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.356,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.879,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.775,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.879,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.809,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.624,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.073,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.978,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.925,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.145,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.567,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.968,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.955,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.208,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.833,
            "unit": "ns/elem"
          }
        ]
      }
    ],
    "linux-x64-gcc-v3": [
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "27731254dfb152bc2961e913468453c9b81d21b9",
          "message": "CI: publish the benchmark history to GitHub Pages (#74)\n\nbench.cpp gets --json (github-action-benchmark's customSmallerIsBetter\narray; names carry the backend so SIMD and scalar share a chart). The\nbenchmark step merges the SIMD and scalar JSON and, on push, uploads it as an\nartifact. A new publish-bench job (main pushes only, contents: write, after\nbuild-test unless cancelled) appends each available result to bench/ on the\ngh-pages branch with github-action-benchmark v1.22.2 (pinned), which also\nwrites bench/index.html. Nothing is gated on the numbers. README (ja / en)\nand CONTEXT.md point to https://capsule-holdings.github.io/shader-math/bench/.",
          "timestamp": "2026-10-24T07:48:49+09:00",
          "tree_id": "922c427e8713421c362bea622fc914cb344de096",
          "url": "https://github.com/capsule-holdings/shader-math/commit/27731254dfb152bc2961e913468453c9b81d21b9"
        },
        "date": 1792796399255,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.841,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.464,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.05,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.496,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.154,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.952,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.727,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.229,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.628,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.37,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.806,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.917,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.386,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.011,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.367,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.751,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.05,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.882,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.681,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 7.32,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.603,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.441,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "40dccf5b846e15980af60dc8c6ec468419b7f854",
          "message": "bench: measure inverse and normalize latency too (#75)\n\nIn the scalar twin, gcc -O3 vectorizes the inverse / normalize array loops\nacross elements (-fopt-info-vec), which made the SIMD backend look slower\nthere. Two rows now feed each result into the next call (inverse(float4x4)\nlatency, normalize(float3) latency), so neither build can batch, and the\nfile comment explains how to read the array-loop numbers.",
          "timestamp": "2026-10-24T08:02:59+09:00",
          "tree_id": "d61289a3930098805225ccad304408efe45bfec3",
          "url": "https://github.com/capsule-holdings/shader-math/commit/40dccf5b846e15980af60dc8c6ec468419b7f854"
        },
        "date": 1792797267381,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.567,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.791,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.993,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.953,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.324,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 18.728,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.713,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.56,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.169,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.254,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.734,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.514,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.923,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 4.185,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.97,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.976,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.799,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.319,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.595,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.779,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.96,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.429,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.769,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 7.567,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.468,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 5.503,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e37f9cddd1b8578abc2f4b011bf60dd32ff7b805",
          "message": "test: cross-check HLSL through DXC's DXIL backend on D3D12 WARP (#73)\n\nThe DXC cross-check ran only through SPIR-V on lavapipe. The same HLSL now\nalso goes through DXC's DXIL backend (v1.9.2609, Windows release) and runs on\nWARP: tests/interop/d3d12_runner.cpp, SHADER_MATH_INTEROP_DXIL (Windows),\ndxil_interop_check / dxil_interop_half_check, and the windows-x64-dxil-interop\nCI job (DXC zip pinned by sha256). Every interop job now checks with\nctest --no-tests=error that its cross-check was registered, since CMake only\nwarns when a prerequisite is missing.\n\nFindings, recorded in docs/slang_compat.md:\n- lit: the DXIL backend computes (n.h)^m and matches shader-math on WARP, so\n  the SPIR-V-only defect stays excluded from the SPIR-V check alone\n- frexp: DXIL lowering builds the mantissa as (bits & 0x007fffff) | 0x3f000000\n  and drops the sign of a negative input; case_exact_frexp_dst is split into\n  case_exact_frexp (left out of DXIL) and case_exact_dst\n- frac: DXC's DXIL constant folding turns frac(-0.75) into 0.75; the DXIL\n  kernel xors its inputs with bits of the zero-initialized output buffer\n  (SM_INTEROP_OPAQUE_INPUTS) so the device evaluates every case, and WARP's\n  Frc gives 0.25\n- WARP's asin / acos / atan are about 1e-3 off libm; compare.py gains\n  --tol=<case>:<relative>, used for the two cases that call them\n\nAlso spells a half swizzle-to-float conversion as float(half(v.x)) in the\nshared cases for MSVC's software half.",
          "timestamp": "2026-10-24T08:03:56+09:00",
          "tree_id": "fd6968d65c526d8f65bf18f7e6daf1c88f49be94",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e37f9cddd1b8578abc2f4b011bf60dd32ff7b805"
        },
        "date": 1792797661472,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.699,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.711,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.126,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.938,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.509,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 18.412,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.691,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.626,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.051,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.208,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.586,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.516,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.828,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.036,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.634,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.73,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.897,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.769,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.985,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.759,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.88,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.273,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.621,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.204,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.377,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.431,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d268b9af19dccd1e5ecab4e92bddb113ffccd28b",
          "message": "Add opt-in multi-element matrix accessors (SHADER_MATH_MATRIX_SWIZZLE) (#70)\n\nHLSL's multi-element matrix accessor lists, opt-in like the other swizzle\nextensions. SHADER_MATH_MATRIX_SWIZZLE declares the 2-element lists in both\nspellings (m._m00_m11, m._41_42); SHADER_MATH_MATRIX_SWIZZLE3 adds the\n3-element ones (m._m30_m31_m32, m._11_22_33). Only lists inside the matrix's\nown shape exist; they convert to vectors, work with dot etc. through\nvec_traits, and are writable when free of repeats (gathered first, so\nm._m01_m10 = m._m10_m01 swaps). There are no 4-element lists. Under either\nmacro the hlsl::matrix union moves into a per-shape storage base\n(detail::hmat_store<R, C, T>); with neither, the class is unchanged.\n\nCMake options and find_package variables follow the other opt-ins, and\nbench/compile_time.py gains both configs: a TU using every matrix type takes\nabout 1.2x with 2-element lists and, with 3-element lists, about 12x on gcc\nand 4x on clang (README known deviations).\n\nTests: shader_math_tests_matrix_swizzle / _matrix_swizzle3 (reads, repeats,\nnon-square shapes, writes, swaps, compound ops, half, and static_asserts for\nthe shape limits, 4-element lists and duplicate writes), test_restrict.cpp\nfor the default build, and shared/matrix_swizzle_cases.hlsli cross-checked\nagainst Slang (CPU target) and DXC (SPIR-V on lavapipe), each against a C++\ntwin built with SHADER_MATH_MATRIX_SWIZZLE3.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T08:41:51+09:00",
          "tree_id": "f06a061a2cc669527ff97791e2b5bd07089278c8",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d268b9af19dccd1e5ecab4e92bddb113ffccd28b"
        },
        "date": 1792799269873,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.313,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.323,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.894,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.741,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 6.713,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 17.709,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.436,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.325,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.212,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.584,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.344,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.851,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.768,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.723,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.66,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.576,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.704,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.121,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.773,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.883,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.273,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.82,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.285,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.071,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.329,
            "unit": "ns/elem"
          }
        ]
      }
    ],
    "linux-arm-gcc": [
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "27731254dfb152bc2961e913468453c9b81d21b9",
          "message": "CI: publish the benchmark history to GitHub Pages (#74)\n\nbench.cpp gets --json (github-action-benchmark's customSmallerIsBetter\narray; names carry the backend so SIMD and scalar share a chart). The\nbenchmark step merges the SIMD and scalar JSON and, on push, uploads it as an\nartifact. A new publish-bench job (main pushes only, contents: write, after\nbuild-test unless cancelled) appends each available result to bench/ on the\ngh-pages branch with github-action-benchmark v1.22.2 (pinned), which also\nwrites bench/index.html. Nothing is gated on the numbers. README (ja / en)\nand CONTEXT.md point to https://capsule-holdings.github.io/shader-math/bench/.",
          "timestamp": "2026-10-24T07:48:49+09:00",
          "tree_id": "922c427e8713421c362bea622fc914cb344de096",
          "url": "https://github.com/capsule-holdings/shader-math/commit/27731254dfb152bc2961e913468453c9b81d21b9"
        },
        "date": 1792796401598,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.446,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.435,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.126,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.582,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.26,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.728,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.641,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.525,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.758,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.421,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.994,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.033,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.606,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.834,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.541,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "40dccf5b846e15980af60dc8c6ec468419b7f854",
          "message": "bench: measure inverse and normalize latency too (#75)\n\nIn the scalar twin, gcc -O3 vectorizes the inverse / normalize array loops\nacross elements (-fopt-info-vec), which made the SIMD backend look slower\nthere. Two rows now feed each result into the next call (inverse(float4x4)\nlatency, normalize(float3) latency), so neither build can batch, and the\nfile comment explains how to read the array-loop numbers.",
          "timestamp": "2026-10-24T08:02:59+09:00",
          "tree_id": "d61289a3930098805225ccad304408efe45bfec3",
          "url": "https://github.com/capsule-holdings/shader-math/commit/40dccf5b846e15980af60dc8c6ec468419b7f854"
        },
        "date": 1792797269883,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.431,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.438,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.152,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.616,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.616,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.261,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.397,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.423,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.663,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.648,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.51,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.756,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.579,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.987,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.741,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.006,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.617,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.72,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.48,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.539,
            "unit": "ns/elem"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "emuesuenu@hotmail.com",
            "name": "Hidenori Kondo",
            "username": "emuesuenu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "e37f9cddd1b8578abc2f4b011bf60dd32ff7b805",
          "message": "test: cross-check HLSL through DXC's DXIL backend on D3D12 WARP (#73)\n\nThe DXC cross-check ran only through SPIR-V on lavapipe. The same HLSL now\nalso goes through DXC's DXIL backend (v1.9.2609, Windows release) and runs on\nWARP: tests/interop/d3d12_runner.cpp, SHADER_MATH_INTEROP_DXIL (Windows),\ndxil_interop_check / dxil_interop_half_check, and the windows-x64-dxil-interop\nCI job (DXC zip pinned by sha256). Every interop job now checks with\nctest --no-tests=error that its cross-check was registered, since CMake only\nwarns when a prerequisite is missing.\n\nFindings, recorded in docs/slang_compat.md:\n- lit: the DXIL backend computes (n.h)^m and matches shader-math on WARP, so\n  the SPIR-V-only defect stays excluded from the SPIR-V check alone\n- frexp: DXIL lowering builds the mantissa as (bits & 0x007fffff) | 0x3f000000\n  and drops the sign of a negative input; case_exact_frexp_dst is split into\n  case_exact_frexp (left out of DXIL) and case_exact_dst\n- frac: DXC's DXIL constant folding turns frac(-0.75) into 0.75; the DXIL\n  kernel xors its inputs with bits of the zero-initialized output buffer\n  (SM_INTEROP_OPAQUE_INPUTS) so the device evaluates every case, and WARP's\n  Frc gives 0.25\n- WARP's asin / acos / atan are about 1e-3 off libm; compare.py gains\n  --tol=<case>:<relative>, used for the two cases that call them\n\nAlso spells a half swizzle-to-float conversion as float(half(v.x)) in the\nshared cases for MSVC's software half.",
          "timestamp": "2026-10-24T08:03:56+09:00",
          "tree_id": "fd6968d65c526d8f65bf18f7e6daf1c88f49be94",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e37f9cddd1b8578abc2f4b011bf60dd32ff7b805"
        },
        "date": 1792797664193,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.444,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.45,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.152,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.512,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.561,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.261,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.409,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.662,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.643,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.533,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.788,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.639,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.027,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.768,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.007,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.61,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.737,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.493,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.547,
            "unit": "ns/elem"
          }
        ]
      }
    ]
  }
}