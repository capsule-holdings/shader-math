window.BENCHMARK_DATA = {
  "lastUpdate": 1792885330847,
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
          "id": "77496ab6ca2e788c21c4622e8641ca48f9e2f33e",
          "message": "Split the matrix accessor union per first element on gcc (#77)\n\nWith SHADER_MATH_MATRIX_SWIZZLE3 the header's front-end time on gcc was\nabout 12x the default build. gcc slows down faster than linearly with the\ndata member count of one class scope (8192 members in one anonymous union:\n3.7 s; split into nested unions of 512: 0.44 s), and a 4x4 declared all of\nits 2 x 16^3 accessor lists in one union. On gcc each first element now gets\nits own nested anonymous union (SM_MX_GROUP), which brings\nbench/compile_time.cpp from 7.7 s to 2.9 s (4.4x the default). Nested\nanonymous unions are an extension that gcc accepts silently; clang warns\nunder -Wpedantic and measured slower with them, so clang and MSVC keep one\nflat union (4.1x on clang). README (ja / en) known deviations: about 4x on\nboth compilers.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:03:33+09:00",
          "tree_id": "862842d1b0e87b98c799c65a2ebf2d444c92b815",
          "url": "https://github.com/capsule-holdings/shader-math/commit/77496ab6ca2e788c21c4622e8641ca48f9e2f33e"
        },
        "date": 1792800552443,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.747,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.812,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.813,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.863,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 8.191,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 12.976,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.612,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.504,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.973,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 3.043,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 5.188,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.393,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.116,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.887,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.985,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.816,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.541,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.184,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.397,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.762,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.132,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.493,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.953,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.906,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.187,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 87.401,
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
          "id": "9adf550203bbe190bdd2363dacd21883483d6776",
          "message": "Invert float 4x4 matrices in registers on SSE2 (#76)\n\ninverse() of a float 4x4 wrote each register out lane by lane, ran the\nscalar formula, and wrote the result back lane by lane. On SSE2 it now stays\nin registers: inv4_regs applies Cramer's rule over a transposed, pair-rotated\ncopy (the Intel AP-928 arrangement), so every lane move is a single shuffle.\nhlsl and glsl share it (the inverse of the transpose is the transpose of the\ninverse). Halves, 2x2 / 3x3, NEON and the scalar backend keep the dense path.\n\nThe float backend contract (SimdFloatOps) gains six lane moves (cat_lo,\ncat_hi, even, odd, swap_pairs, swap_halves) and fnmadd (c - a*b), implemented\non SSE2, NEON and scalar.\n\nCI runners, ns (array loop / dependent chain): x64 SSE2 8.9 / 14.0 -> 5.7 /\n11.6, x86-64-v3 7.4 / 18.8 -> 5.6 / 14.0. On AArch64 (gcc) the same kernel\nmeasured 17.7 / 21.2 against 16.5 / 21.4, so NEON is left out. Accuracy over\n200k random matrices matches the dense path (mean relative error about 1e-7\non well-conditioned input).\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:08:54+09:00",
          "tree_id": "2d213a7fa1f1235f2ae7d8b64624508db17c3f13",
          "url": "https://github.com/capsule-holdings/shader-math/commit/9adf550203bbe190bdd2363dacd21883483d6776"
        },
        "date": 1792800887313,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 3.453,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 3.786,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.992,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 2.475,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 6.608,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 13.042,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 1.173,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.944,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 9.016,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 4.77,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 8.484,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.585,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.513,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 4.887,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 4.925,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.149,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.728,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 6.59,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 16.412,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.906,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.359,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.21,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 4.358,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 6.099,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.9,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 104.283,
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
          "id": "e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed",
          "message": "Release 0.4.0 (#78)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.4.0. 0.4 adds the opt-in\nmulti-element matrix accessors (SHADER_MATH_MATRIX_SWIZZLE / _SWIZZLE3, #70)\nwith a gcc compile-time fix (#77), converts halves with NEON on ARMv8.0 (#66),\ninverts float 4x4 matrices in registers on SSE2 (#76), cross-checks HLSL\nagainst DXC through SPIR-V (#69) and DXIL on D3D12 WARP (#73), moves to\nSlang v2026.20 (#72), adds the MIT license (#71), and records the benchmark\nhistory (#74, #75). Default builds keep 0.3's API and layout.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:15:44+09:00",
          "tree_id": "65d7fc5797a6eb7e60888dbfd16044694603cecd",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed"
        },
        "date": 1792801301051,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 5.8,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 5.734,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 1.553,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 4.109,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 10.936,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 19.132,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 1.296,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 4.17,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 11.502,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 5.684,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 13.409,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.786,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 16.049,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 7.618,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 7.683,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.603,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 7.383,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.105,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 21.028,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.146,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 4.289,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 10.393,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 5.472,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 13.738,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.466,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 127.866,
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
          "id": "d03f993b2a0f7b42a1e26e1c996a5b513ab971a3",
          "message": "Name the nested swizzle lanes as template parameters (#79)\n\nEach nested swizzle member named its lanes through the enclosing proxy's\nstatic constexpr array (nswz<T, N, L_[i0], L_[i1]>), a constant evaluation\nper member across tens of thousands of members per TU. The 2- and\n3-component swz_store specializations now take the lanes as parameters\n(I0, I1, I2) and the members paste them in directly.\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T12:55:29+09:00",
          "tree_id": "4202b20e6a09479a2f72ec0a64ca3a3dc876b4b1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d03f993b2a0f7b42a1e26e1c996a5b513ab971a3"
        },
        "date": 1792814436715,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.601,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.512,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.745,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.623,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.207,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.627,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.558,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.373,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.256,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.733,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 4.714,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.308,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.391,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.533,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.637,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.747,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.169,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.734,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.309,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.806,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.069,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.078,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.788,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.663,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.085,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 82.739,
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
          "id": "b24601b342fc1db4f3617baca298633190b4524d",
          "message": "Release 0.4.1 (#80)\n\nBump project(VERSION), version.hpp and the README FetchContent examples to\n0.4.1 (find_package keeps 0.4). 0.4.1 cuts the header front-end time with\nSHADER_MATH_NESTED_SWIZZLE (gcc 2.7x -> 2.0x, clang 4.1x -> 2.4x the default\nbuild, #79). API, layout and the generated members are unchanged.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:32:56+09:00",
          "tree_id": "23e506c3dae30e2f46eaa19158cf07ef423f6895",
          "url": "https://github.com/capsule-holdings/shader-math/commit/b24601b342fc1db4f3617baca298633190b4524d"
        },
        "date": 1792816731251,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.947,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.93,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.774,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 2.374,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.539,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.108,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.53,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.002,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 6.521,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.949,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 6.475,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.335,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 6.699,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.756,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.778,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.949,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 3.111,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.265,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 10.002,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.012,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.854,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.182,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.626,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.996,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.076,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 62.19,
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
          "id": "c87e2a2ee76e2df93a3a63f72f15d956a2e929c9",
          "message": "Add opt-in standard library integration (<shader_math/std.hpp>) (#81)\n\nIncluding <shader_math/std.hpp> adds, for both language surfaces:\n- operator<< for vectors, swizzles, bool vectors and matrices\n- std::formatter where <format> exists (__cpp_lib_format); the format spec\n  applies to each component\n- the tuple protocol for structured bindings (components; matrices by m[i]:\n  hlsl rows, glsl columns)\n- std::hash / std::equal_to for vectors (value equality over N components,\n  -0 and +0 as one key), so they work as unordered container keys\n- shader_math::load<V>(const T*) / shader_math::store(x, T*), matrices in\n  their constructor order (hlsl row-major, glsl column-major)\nHalf components print, format and hash through float. shader_math.hpp is\nunchanged and pulls in neither <ostream> nor <format>.\n\ntests/test_std.cpp joins the main suite (SIMD and scalar), and\ntest_restrict.cpp checks that operator<< needs the header. README (ja / en)\ngains a section.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:45:06+09:00",
          "tree_id": "80c2ab16a836e8f63a3e838bc522ab95cea4a9be",
          "url": "https://github.com/capsule-holdings/shader-math/commit/c87e2a2ee76e2df93a3a63f72f15d956a2e929c9"
        },
        "date": 1792817526905,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.979,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.822,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.84,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.823,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.748,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.234,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.591,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.474,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 8.332,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 3.131,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 5.415,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.358,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.65,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 4.045,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 4.142,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.866,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.916,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.451,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.782,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.343,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.323,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.54,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 3.548,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.909,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.351,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 96.874,
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
          "id": "eda55c30f347bd66d401d1213d4a695766178e94",
          "message": "Round halves to even, as DXC compiles HLSL round (#82)\n\nround sent halves away from zero (std::round). DXC compiles HLSL round to\nRound_ne in DXIL and RoundEven in SPIR-V, so round(0.5) = 0, round(2.5) = 2\nand round(-0.5) = -0 there, where shader-math returned 1, 3 and -1. GLSL\nleaves the halfway direction to the implementation, so glsl::round keeps\nsharing the function and now matches roundEven.\n\nround uses rnd_even; the unused rnd_away leaves the backend contract and\nevery backend (on SSE4.1 round is one roundps). DXC-only interop case\ncase_exact_round_ties checks exact halves in SPIR-V and DXIL (Slang's CPU\ntarget uses ::roundf, so the shared cases avoid halves). Tests, README\n(ja / en) and docs/slang_compat.md follow.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:35:05+09:00",
          "tree_id": "9daab9c183c8f9195eb09e621be881a3794810e0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/eda55c30f347bd66d401d1213d4a695766178e94"
        },
        "date": 1792824028094,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.782,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.775,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.796,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.737,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.564,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.364,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.597,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.467,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.76,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.94,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.984,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.35,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.891,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.785,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.879,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.796,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.561,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.056,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.052,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.99,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.141,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.539,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.973,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.973,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.128,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.181,
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
          "id": "f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a",
          "message": "Accept scalar arguments in the component-wise functions (#83)\n\nmax(dot(n, l), 0.0f) and saturate(dot(n, l)), the most common shader idioms,\ndid not compile: the component-wise functions took vectors only. A call with\nno vector argument and at least one float / half / one-component swizzle /\nmatrix element (detail::ScalarCall) now takes a scalar path that runs the\nvector function on one lane, so rounding and edge cases match the vectors.\nOther arguments follow the vector-scalar operand rules (any arithmetic type\nnext to float, exactly half next to half); double- or int-only calls are not\ncovered. The overloads are constrained templates, so they do not clash with\nthe C library's same-named functions and abs(-2.5f) no longer reaches the C\nint abs.\n\nCovered: the shared component-wise functions, dot / length / distance and\nmodf in both languages; HLSL lerp saturate frac rcp rsqrt log10 atan2 fmod\nldexp mad, frexp, sincos, select(bool, a, b); GLSL mix fract inversesqrt\nroundEven asinh acosh atanh atan(y, x) mod fma, mix(x, y, bool). Scalar\nround is a plain function instead: libc++ and MSVC declare a global\n::round(float) (halves away from zero) that would beat a template, so an\nunqualified call is ambiguous there rather than quietly wrong; hlsl::round(x)\nalways resolves to shader-math.\n\nTests in test_funcs.cpp; interop cases case_near_scalar (Slang, DXC SPIR-V\nand DXIL) and case_approx_scalar (GLSL). README (ja / en), CONTEXT.md and\ndocs/slang_compat.md drop the vector-only rule.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:45:43+09:00",
          "tree_id": "9d03c8c2d368878d0b8e9de20e469c826bc9ffd1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a"
        },
        "date": 1792824660914,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.744,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.731,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.783,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.704,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.288,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.805,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.566,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.389,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.276,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.757,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.786,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.309,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.512,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.604,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.691,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.755,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.234,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.831,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.58,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.814,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.128,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.325,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.924,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.909,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.114,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 84.098,
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
          "id": "2895f46ecb6f57f1b3930695fc0a6a94e396acfb",
          "message": "Make min / max ignore a NaN operand, as D3D defines them (#84)\n\nmin / max followed each backend: SSE2 and scalar returned NaN when the second\noperand was NaN, NEON returned NaN whenever either was, so saturate(NaN) was\n0 on x86 and NaN on ARM. D3D defines min / max as IEEE minNum / maxNum and\nsaturate(NaN) = 0 (DXC emits FMin / FMax / Saturate in DXIL, NMin / NMax /\nFClamp in SPIR-V); GLSL leaves it undefined, which this satisfies. Every\nbackend now ignores a NaN operand: SSE2 replaces the lanes where b is NaN\n(cmpunordps + select), NEON uses vminnmq / vmaxnmq (vminnm_f16 / vmaxnm_f16\nfor native half), scalar checks b != b. clamp / saturate / smoothstep build\non them, so saturate(NaN) = 0 and clamp(NaN, lo, hi) = lo everywhere.\n\nTests in test_simd.cpp and test_funcs.cpp (vectors, scalars, half). Interop\ncase case_exact_nan_rules runs in the Slang and DXC SPIR-V checks; not in\nDXIL, which DXC compiles with fast-math flags that assume no NaN. README\n(ja / en) updated.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:53:14+09:00",
          "tree_id": "411b120f91c758fc2b398c4a1c1446ebf5efd5fc",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2895f46ecb6f57f1b3930695fc0a6a94e396acfb"
        },
        "date": 1792825101352,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.525,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.527,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.734,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.577,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.04,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.261,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.539,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.327,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.004,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.644,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.614,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.359,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.178,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.435,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.54,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.821,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.001,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.603,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 10.988,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.809,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.035,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 6.825,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.69,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.615,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.06,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 79.803,
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
          "id": "17d0f3115690cedcd685b66f777ff51f51ba9862",
          "message": "Compute glsl::mix with GLSL's defining formula (#85)\n\nglsl::mix shared hlsl::lerp's x + a*(y - x). GLSL defines mix as\nx*(1 - a) + y*a, and the two differ at the ends: mix(1e8, 1, 1) is 1 on\nglslang / lavapipe and was 0 here, because x + a*(y - x) loses y next to a\nmuch larger x. glsl::mix now uses detail::mix_impl with the defining\nformula (vector or scalar a, float or half), exact at a = 0 and a = 1.\nhlsl::lerp keeps x + s*(y - x), which is what DXC emits for DXIL.\n\nTests in test_funcs.cpp; GLSL interop case case_exact_mix_ends (bit-exact\nagainst lavapipe). README (ja / en) and docs/glsl_interop.md describe the two\nformulas.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:59:30+09:00",
          "tree_id": "da5e0dc312a8e150a64bee7fa51e838e865ce17f",
          "url": "https://github.com/capsule-holdings/shader-math/commit/17d0f3115690cedcd685b66f777ff51f51ba9862"
        },
        "date": 1792825490306,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 3.014,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 3.007,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.864,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.886,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 6.031,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 12.276,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.647,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.589,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 8.286,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 3.17,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 4.288,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.377,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.504,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 4.075,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 4.093,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.856,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 7.069,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.423,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.947,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.067,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.224,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.099,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 3.19,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.266,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.26,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 93.842,
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
          "id": "6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3",
          "message": "interop: check edge values; compare infinities strictly (#86)\n\nThe cross-checks ran one fixed input set with no edge values. compare.py\nalso scaled its relative tolerance by max(1, |a|, |b|), so with an infinity\ntwo equal infinities failed and an infinity against a finite value passed;\nan infinity now matches only the same infinity, in every tier.\n\nNew cases: case_near_edges (shared HLSL; Slang, DXC SPIR-V and DXIL) for\nfmod with negative operands, step on equal bounds, clamp with reversed\nbounds, frac(-0) and smoothstep with reversed edges; case_exact_inf_edges\n(Slang and DXC SPIR-V only, as DXC compiles DXIL with fast-math flags that\nassume no infinities) for rcp / rsqrt of signed zeros; and case_near_edges\nfor GLSL (mod with negative operands, step on equal bounds, fract(-0); GLSL\nleaves inversesqrt(0) undefined). docs/glsl_interop.md and\ndocs/slang_compat.md list them.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:07:33+09:00",
          "tree_id": "ace6c4271d3683a656c56afb52218dfb80ddbd88",
          "url": "https://github.com/capsule-holdings/shader-math/commit/6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3"
        },
        "date": 1792825977129,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.618,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.548,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.76,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.661,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.273,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.723,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.561,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.375,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.286,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.769,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.744,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.347,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.411,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.575,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.752,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.779,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.163,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.809,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.66,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.881,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.078,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.1,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.814,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.965,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.066,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 83.067,
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
          "id": "8f78b44fbe3699d5417df9c4df91c8a12caf785f",
          "message": "Release 0.5.0 (#87)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.5.0. 0.5 fixes compatibility\ngaps found by checking edge values against DXC and glslang: round sends\nhalves to even (#82), min / max ignore a NaN operand as D3D defines them\n(#84), glsl::mix uses GLSL's defining formula (#85). It adds scalar\narguments to the component-wise functions (#83) and the opt-in\n<shader_math/std.hpp> (#81), and checks edge values in the cross-checks\n(#86). round and the NaN handling change results, hence the minor bump.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:25:54+09:00",
          "tree_id": "7549fa7cd2e45863df97a7f5b31e50cb3be681a7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/8f78b44fbe3699d5417df9c4df91c8a12caf785f"
        },
        "date": 1792827082460,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.952,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.989,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.751,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 2.422,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.943,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.699,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.559,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.055,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 6.853,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.948,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 1.751,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.353,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.019,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.916,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.911,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.003,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 3.246,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.49,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 10.368,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.052,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.885,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.572,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.765,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 1.777,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.137,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 65.38,
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
          "id": "bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e",
          "message": "chore: wait 14 days before Renovate updates and align labels (#88)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:26:42+09:00",
          "tree_id": "2e45eaeb5415712cdcf919188c256eff85daa9b7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e"
        },
        "date": 1792827426501,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.766,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.525,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.767,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.674,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.254,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.158,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.595,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.46,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.717,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.908,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.987,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.518,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.537,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.827,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.949,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.789,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.208,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.295,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.964,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.87,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.125,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.491,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.851,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.823,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.116,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 84.017,
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
          "id": "2a781a8bac65e42af88c1f71b874c9c4920edf71",
          "message": "Fill the HLSL / GLSL gaps found in the flat review (#89)\n\nScalar isnan / isinf (both languages) and isfinite (hlsl); HLSL all / any on\nnumeric vectors, matrices and scalars; select(bool, a, b) with vector\noperands; explicit bool <-> float vector conversions; hlsl::vector<T, N>.\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T17:16:25+09:00",
          "tree_id": "66c2c5b316b429ebf626f70cdd523d4916f44550",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2a781a8bac65e42af88c1f71b874c9c4920edf71"
        },
        "date": 1792830089499,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.776,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.719,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.795,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.728,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.545,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.376,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.596,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.465,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.751,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.918,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.978,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.326,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 7.892,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.781,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.883,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.796,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 6.55,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.052,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.074,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.992,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.141,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.559,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.975,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.978,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.133,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.337,
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
          "id": "3bae00e9ab6556c461e891a0c9fad0ad39e26666",
          "message": "Give hlsl and glsl their own vector types (#90)\n\n* Spike: language tag on vector types (hlsl / glsl distinct)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* Make GLSL .stpq unconditional on glsl vectors; update docs\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* README: compile-time ratios from CI\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T19:59:09+09:00",
          "tree_id": "b05d0fcf89a1209095e1162b9c481ad939ff09e6",
          "url": "https://github.com/capsule-holdings/shader-math/commit/3bae00e9ab6556c461e891a0c9fad0ad39e26666"
        },
        "date": 1792839885122,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.857,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.829,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.814,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.81,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.693,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.748,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.616,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.512,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.993,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 3.117,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 4.104,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.519,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.978,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.873,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.954,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.711,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.896,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.333,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.366,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.648,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.167,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.747,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 3.082,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.121,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.192,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 90.622,
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
          "id": "0c5cc245aa1d5d3d198a315484a92fa7a380f266",
          "message": "Add an HLSL / GLSL conformance corpus measured against DXC and glslang (#91)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:30:47+09:00",
          "tree_id": "c35deab23e42a222476b91a6fecdcfb32ac7aced",
          "url": "https://github.com/capsule-holdings/shader-math/commit/0c5cc245aa1d5d3d198a315484a92fa7a380f266"
        },
        "date": 1792845376489,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.844,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.834,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.812,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.775,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.679,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.996,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.578,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.423,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.532,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.828,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.9,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.318,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.338,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.632,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.699,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.663,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.777,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.042,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.8,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.621,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.101,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.232,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.876,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.914,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.147,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 85.634,
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
          "id": "25a9357e449eac114368ea17b780488193bdb8e4",
          "message": "ci: use the org-level Blacksmith runner variable for runs-on (#93)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T21:36:36+09:00",
          "tree_id": "2ac0980647ca4906c9f0469b3b813e95d1889546",
          "url": "https://github.com/capsule-holdings/shader-math/commit/25a9357e449eac114368ea17b780488193bdb8e4"
        },
        "date": 1792845731818,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.506,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.512,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.726,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.582,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.07,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.22,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.538,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.33,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 6.989,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.633,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.619,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.331,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.725,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.37,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.485,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.666,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.656,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.674,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 10.994,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.579,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.042,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 6.809,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.682,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.613,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.069,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 79.525,
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
          "id": "d509e9242feacdd3e40b514367507b9554eb6715",
          "message": "Fill the gaps the conformance corpus found (#92)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:37:18+09:00",
          "tree_id": "ceb4c8dffe4d61d36a57bb771c09582abe6d5ba0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d509e9242feacdd3e40b514367507b9554eb6715"
        },
        "date": 1792846168291,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.557,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.569,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.738,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.607,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.152,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.365,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.543,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.343,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.092,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.666,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.658,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.34,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.844,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.424,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.523,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.691,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.721,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.791,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.138,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.587,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.057,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 6.906,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.721,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.663,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.056,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 80.665,
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
          "id": "a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68",
          "message": "ci: build and test in parallel (#94)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T06:00:00+09:00",
          "tree_id": "8c5cf48375014e926612c158ea1e973a6359c735",
          "url": "https://github.com/capsule-holdings/shader-math/commit/a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68"
        },
        "date": 1792875913166,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.887,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 3.302,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.889,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 2.159,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.567,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.414,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.598,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.471,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.768,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.929,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.989,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.334,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.679,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.736,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.821,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.688,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.837,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.178,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.093,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.635,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.145,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.572,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.986,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.993,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.148,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.373,
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
          "id": "f066b80792484067f322dc5584f4fcc083ebc958",
          "message": "docs: HLSL / GLSL / shader-math mapping table (#95)\n\n* docs: HLSL / GLSL / shader-math mapping table; more conformance cases\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* docs: what is tolerated, what goes beyond the languages, what depends on the toolchain\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T07:49:49+09:00",
          "tree_id": "148e8bee7d282a384330390e42d6f8f835612795",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f066b80792484067f322dc5584f4fcc083ebc958"
        },
        "date": 1792882416403,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.882,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.917,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.837,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.828,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.508,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 11.115,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.795,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.573,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.659,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.892,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.907,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.335,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 9.563,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.685,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.953,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.701,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.868,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.307,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.227,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.649,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.166,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.373,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.968,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.975,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.125,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 88.041,
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
          "id": "074bbfa38cc462f0297b71a3d30673545cdbc697",
          "message": "Extensions under shader_math::ext; language namespaces take their own language only (#96)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T08:38:42+09:00",
          "tree_id": "82cef50ea7136909ae624d6f41b777f26374234e",
          "url": "https://github.com/capsule-holdings/shader-math/commit/074bbfa38cc462f0297b71a3d30673545cdbc697"
        },
        "date": 1792885324502,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2]",
            "value": 2.553,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2]",
            "value": 2.47,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2]",
            "value": 0.734,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2]",
            "value": 1.615,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2]",
            "value": 5.14,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2]",
            "value": 10.494,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2]",
            "value": 0.549,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2]",
            "value": 1.355,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2]",
            "value": 7.077,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2]",
            "value": 2.69,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2]",
            "value": 3.702,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2]",
            "value": 0.311,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2]",
            "value": 8.92,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.438,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.548,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.639,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 1.709,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.806,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.247,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.591,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.063,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 6.909,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 2.72,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 3.7,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.053,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 80.96,
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
          "id": "77496ab6ca2e788c21c4622e8641ca48f9e2f33e",
          "message": "Split the matrix accessor union per first element on gcc (#77)\n\nWith SHADER_MATH_MATRIX_SWIZZLE3 the header's front-end time on gcc was\nabout 12x the default build. gcc slows down faster than linearly with the\ndata member count of one class scope (8192 members in one anonymous union:\n3.7 s; split into nested unions of 512: 0.44 s), and a 4x4 declared all of\nits 2 x 16^3 accessor lists in one union. On gcc each first element now gets\nits own nested anonymous union (SM_MX_GROUP), which brings\nbench/compile_time.cpp from 7.7 s to 2.9 s (4.4x the default). Nested\nanonymous unions are an extension that gcc accepts silently; clang warns\nunder -Wpedantic and measured slower with them, so clang and MSVC keep one\nflat union (4.1x on clang). README (ja / en) known deviations: about 4x on\nboth compilers.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:03:33+09:00",
          "tree_id": "862842d1b0e87b98c799c65a2ebf2d444c92b815",
          "url": "https://github.com/capsule-holdings/shader-math/commit/77496ab6ca2e788c21c4622e8641ca48f9e2f33e"
        },
        "date": 1792800555547,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.256,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.3,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.837,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.655,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.075,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 17.65,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.413,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.522,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.846,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.2,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.565,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.518,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.828,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.84,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.949,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.689,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.487,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.642,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.054,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.787,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.935,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.11,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.621,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.137,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.037,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.225,
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
          "id": "9adf550203bbe190bdd2363dacd21883483d6776",
          "message": "Invert float 4x4 matrices in registers on SSE2 (#76)\n\ninverse() of a float 4x4 wrote each register out lane by lane, ran the\nscalar formula, and wrote the result back lane by lane. On SSE2 it now stays\nin registers: inv4_regs applies Cramer's rule over a transposed, pair-rotated\ncopy (the Intel AP-928 arrangement), so every lane move is a single shuffle.\nhlsl and glsl share it (the inverse of the transpose is the transpose of the\ninverse). Halves, 2x2 / 3x3, NEON and the scalar backend keep the dense path.\n\nThe float backend contract (SimdFloatOps) gains six lane moves (cat_lo,\ncat_hi, even, odd, swap_pairs, swap_halves) and fnmadd (c - a*b), implemented\non SSE2, NEON and scalar.\n\nCI runners, ns (array loop / dependent chain): x64 SSE2 8.9 / 14.0 -> 5.7 /\n11.6, x86-64-v3 7.4 / 18.8 -> 5.6 / 14.0. On AArch64 (gcc) the same kernel\nmeasured 17.7 / 21.2 against 16.5 / 21.4, so NEON is left out. Accuracy over\n200k random matrices matches the dense path (mean relative error about 1e-7\non well-conditioned input).\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:08:54+09:00",
          "tree_id": "2d213a7fa1f1235f2ae7d8b64624508db17c3f13",
          "url": "https://github.com/capsule-holdings/shader-math/commit/9adf550203bbe190bdd2363dacd21883483d6776"
        },
        "date": 1792800890090,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.626,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.473,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.04,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.8,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 5.139,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 13.251,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.714,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.599,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.195,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.209,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.579,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.36,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.783,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.946,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.489,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.695,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.746,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 5.139,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.744,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.808,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.963,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.571,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.669,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.566,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.154,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.507,
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
          "id": "e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed",
          "message": "Release 0.4.0 (#78)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.4.0. 0.4 adds the opt-in\nmulti-element matrix accessors (SHADER_MATH_MATRIX_SWIZZLE / _SWIZZLE3, #70)\nwith a gcc compile-time fix (#77), converts halves with NEON on ARMv8.0 (#66),\ninverts float 4x4 matrices in registers on SSE2 (#76), cross-checks HLSL\nagainst DXC through SPIR-V (#69) and DXIL on D3D12 WARP (#73), moves to\nSlang v2026.20 (#72), adds the MIT license (#71), and records the benchmark\nhistory (#74, #75). Default builds keep 0.3's API and layout.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:15:44+09:00",
          "tree_id": "65d7fc5797a6eb7e60888dbfd16044694603cecd",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed"
        },
        "date": 1792801303740,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.181,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.221,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.797,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.566,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.57,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.786,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.392,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.415,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.442,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.192,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.529,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.312,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.667,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.591,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.65,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.652,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.335,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.359,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.141,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.633,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.825,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.671,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.611,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.823,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.11,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.033,
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
          "id": "d03f993b2a0f7b42a1e26e1c996a5b513ab971a3",
          "message": "Name the nested swizzle lanes as template parameters (#79)\n\nEach nested swizzle member named its lanes through the enclosing proxy's\nstatic constexpr array (nswz<T, N, L_[i0], L_[i1]>), a constant evaluation\nper member across tens of thousands of members per TU. The 2- and\n3-component swz_store specializations now take the lanes as parameters\n(I0, I1, I2) and the members paste them in directly.\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T12:55:29+09:00",
          "tree_id": "4202b20e6a09479a2f72ec0a64ca3a3dc876b4b1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d03f993b2a0f7b42a1e26e1c996a5b513ab971a3"
        },
        "date": 1792814439358,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.25,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.244,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.764,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.506,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.343,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.075,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.427,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.327,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.013,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.181,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.499,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.306,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.631,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.481,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.545,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.598,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.04,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.118,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.437,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.536,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.77,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.204,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.54,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 4.477,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.93,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.873,
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
          "id": "b24601b342fc1db4f3617baca298633190b4524d",
          "message": "Release 0.4.1 (#80)\n\nBump project(VERSION), version.hpp and the README FetchContent examples to\n0.4.1 (find_package keeps 0.4). 0.4.1 cuts the header front-end time with\nSHADER_MATH_NESTED_SWIZZLE (gcc 2.7x -> 2.0x, clang 4.1x -> 2.4x the default\nbuild, #79). API, layout and the generated members are unchanged.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:32:56+09:00",
          "tree_id": "23e506c3dae30e2f46eaa19158cf07ef423f6895",
          "url": "https://github.com/capsule-holdings/shader-math/commit/b24601b342fc1db4f3617baca298633190b4524d"
        },
        "date": 1792816734189,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.238,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.237,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.92,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.639,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.759,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 12.337,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.706,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.507,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.81,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.201,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.352,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.699,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.726,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.76,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.647,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.472,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.693,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.71,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.856,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.011,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.601,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.016,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.026,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.234,
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
          "id": "c87e2a2ee76e2df93a3a63f72f15d956a2e929c9",
          "message": "Add opt-in standard library integration (<shader_math/std.hpp>) (#81)\n\nIncluding <shader_math/std.hpp> adds, for both language surfaces:\n- operator<< for vectors, swizzles, bool vectors and matrices\n- std::formatter where <format> exists (__cpp_lib_format); the format spec\n  applies to each component\n- the tuple protocol for structured bindings (components; matrices by m[i]:\n  hlsl rows, glsl columns)\n- std::hash / std::equal_to for vectors (value equality over N components,\n  -0 and +0 as one key), so they work as unordered container keys\n- shader_math::load<V>(const T*) / shader_math::store(x, T*), matrices in\n  their constructor order (hlsl row-major, glsl column-major)\nHalf components print, format and hash through float. shader_math.hpp is\nunchanged and pulls in neither <ostream> nor <format>.\n\ntests/test_std.cpp joins the main suite (SIMD and scalar), and\ntest_restrict.cpp checks that operator<< needs the header. README (ja / en)\ngains a section.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:45:06+09:00",
          "tree_id": "80c2ab16a836e8f63a3e838bc522ab95cea4a9be",
          "url": "https://github.com/capsule-holdings/shader-math/commit/c87e2a2ee76e2df93a3a63f72f15d956a2e929c9"
        },
        "date": 1792817529728,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.34,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.346,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.95,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.781,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 5.154,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 13.179,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.431,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.583,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.419,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.218,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.602,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.368,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.756,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.948,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.971,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.72,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.877,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.921,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.716,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.842,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.928,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.655,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.674,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 5.415,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.152,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.427,
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
          "id": "eda55c30f347bd66d401d1213d4a695766178e94",
          "message": "Round halves to even, as DXC compiles HLSL round (#82)\n\nround sent halves away from zero (std::round). DXC compiles HLSL round to\nRound_ne in DXIL and RoundEven in SPIR-V, so round(0.5) = 0, round(2.5) = 2\nand round(-0.5) = -0 there, where shader-math returned 1, 3 and -1. GLSL\nleaves the halfway direction to the implementation, so glsl::round keeps\nsharing the function and now matches roundEven.\n\nround uses rnd_even; the unused rnd_away leaves the backend contract and\nevery backend (on SSE4.1 round is one roundps). DXC-only interop case\ncase_exact_round_ties checks exact halves in SPIR-V and DXIL (Slang's CPU\ntarget uses ::roundf, so the shared cases avoid halves). Tests, README\n(ja / en) and docs/slang_compat.md follow.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:35:05+09:00",
          "tree_id": "9daab9c183c8f9195eb09e621be881a3794810e0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/eda55c30f347bd66d401d1213d4a695766178e94"
        },
        "date": 1792824030757,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.177,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.217,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.875,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.561,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.553,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.842,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.388,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.417,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.503,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.193,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.193,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.326,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.672,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.622,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.626,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.649,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.335,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.366,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.113,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.644,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.822,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.662,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.598,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.151,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.02,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.058,
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
          "id": "f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a",
          "message": "Accept scalar arguments in the component-wise functions (#83)\n\nmax(dot(n, l), 0.0f) and saturate(dot(n, l)), the most common shader idioms,\ndid not compile: the component-wise functions took vectors only. A call with\nno vector argument and at least one float / half / one-component swizzle /\nmatrix element (detail::ScalarCall) now takes a scalar path that runs the\nvector function on one lane, so rounding and edge cases match the vectors.\nOther arguments follow the vector-scalar operand rules (any arithmetic type\nnext to float, exactly half next to half); double- or int-only calls are not\ncovered. The overloads are constrained templates, so they do not clash with\nthe C library's same-named functions and abs(-2.5f) no longer reaches the C\nint abs.\n\nCovered: the shared component-wise functions, dot / length / distance and\nmodf in both languages; HLSL lerp saturate frac rcp rsqrt log10 atan2 fmod\nldexp mad, frexp, sincos, select(bool, a, b); GLSL mix fract inversesqrt\nroundEven asinh acosh atanh atan(y, x) mod fma, mix(x, y, bool). Scalar\nround is a plain function instead: libc++ and MSVC declare a global\n::round(float) (halves away from zero) that would beat a template, so an\nunqualified call is ambiguous there rather than quietly wrong; hlsl::round(x)\nalways resolves to shader-math.\n\nTests in test_funcs.cpp; interop cases case_near_scalar (Slang, DXC SPIR-V\nand DXIL) and case_approx_scalar (GLSL). README (ja / en), CONTEXT.md and\ndocs/slang_compat.md drop the vector-only rule.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:45:43+09:00",
          "tree_id": "9d03c8c2d368878d0b8e9de20e469c826bc9ffd1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a"
        },
        "date": 1792824663607,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.253,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.2,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.794,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.533,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.803,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.408,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.489,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.861,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.202,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.202,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.329,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.703,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.745,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.775,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.651,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.497,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.612,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.859,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.727,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.863,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.096,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.607,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.149,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.037,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.198,
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
          "id": "2895f46ecb6f57f1b3930695fc0a6a94e396acfb",
          "message": "Make min / max ignore a NaN operand, as D3D defines them (#84)\n\nmin / max followed each backend: SSE2 and scalar returned NaN when the second\noperand was NaN, NEON returned NaN whenever either was, so saturate(NaN) was\n0 on x86 and NaN on ARM. D3D defines min / max as IEEE minNum / maxNum and\nsaturate(NaN) = 0 (DXC emits FMin / FMax / Saturate in DXIL, NMin / NMax /\nFClamp in SPIR-V); GLSL leaves it undefined, which this satisfies. Every\nbackend now ignores a NaN operand: SSE2 replaces the lanes where b is NaN\n(cmpunordps + select), NEON uses vminnmq / vmaxnmq (vminnm_f16 / vmaxnm_f16\nfor native half), scalar checks b != b. clamp / saturate / smoothstep build\non them, so saturate(NaN) = 0 and clamp(NaN, lo, hi) = lo everywhere.\n\nTests in test_simd.cpp and test_funcs.cpp (vectors, scalars, half). Interop\ncase case_exact_nan_rules runs in the Slang and DXC SPIR-V checks; not in\nDXIL, which DXC compiles with fast-math flags that assume no NaN. README\n(ja / en) updated.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:53:14+09:00",
          "tree_id": "411b120f91c758fc2b398c4a1c1446ebf5efd5fc",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2895f46ecb6f57f1b3930695fc0a6a94e396acfb"
        },
        "date": 1792825103936,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.23,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.232,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.824,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.617,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.724,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 12.261,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.401,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.466,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.769,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.2,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.2,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.326,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.717,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.745,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.652,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.464,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.535,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.62,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.696,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.848,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.963,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.608,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.152,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.038,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.14,
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
          "id": "17d0f3115690cedcd685b66f777ff51f51ba9862",
          "message": "Compute glsl::mix with GLSL's defining formula (#85)\n\nglsl::mix shared hlsl::lerp's x + a*(y - x). GLSL defines mix as\nx*(1 - a) + y*a, and the two differ at the ends: mix(1e8, 1, 1) is 1 on\nglslang / lavapipe and was 0 here, because x + a*(y - x) loses y next to a\nmuch larger x. glsl::mix now uses detail::mix_impl with the defining\nformula (vector or scalar a, float or half), exact at a = 0 and a = 1.\nhlsl::lerp keeps x + s*(y - x), which is what DXC emits for DXIL.\n\nTests in test_funcs.cpp; GLSL interop case case_exact_mix_ends (bit-exact\nagainst lavapipe). README (ja / en) and docs/glsl_interop.md describe the two\nformulas.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:59:30+09:00",
          "tree_id": "da5e0dc312a8e150a64bee7fa51e838e865ce17f",
          "url": "https://github.com/capsule-holdings/shader-math/commit/17d0f3115690cedcd685b66f777ff51f51ba9862"
        },
        "date": 1792825492948,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.217,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.212,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.793,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.576,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.546,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.757,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.39,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.41,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.432,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.193,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.193,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.313,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.669,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.611,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.639,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.628,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.244,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.349,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.102,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.628,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.815,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.634,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.574,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.142,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.985,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.027,
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
          "id": "6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3",
          "message": "interop: check edge values; compare infinities strictly (#86)\n\nThe cross-checks ran one fixed input set with no edge values. compare.py\nalso scaled its relative tolerance by max(1, |a|, |b|), so with an infinity\ntwo equal infinities failed and an infinity against a finite value passed;\nan infinity now matches only the same infinity, in every tier.\n\nNew cases: case_near_edges (shared HLSL; Slang, DXC SPIR-V and DXIL) for\nfmod with negative operands, step on equal bounds, clamp with reversed\nbounds, frac(-0) and smoothstep with reversed edges; case_exact_inf_edges\n(Slang and DXC SPIR-V only, as DXC compiles DXIL with fast-math flags that\nassume no infinities) for rcp / rsqrt of signed zeros; and case_near_edges\nfor GLSL (mod with negative operands, step on equal bounds, fract(-0); GLSL\nleaves inversesqrt(0) undefined). docs/glsl_interop.md and\ndocs/slang_compat.md list them.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:07:33+09:00",
          "tree_id": "ace6c4271d3683a656c56afb52218dfb80ddbd88",
          "url": "https://github.com/capsule-holdings/shader-math/commit/6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3"
        },
        "date": 1792825979815,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.306,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.339,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.765,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.513,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.369,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.129,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.405,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.348,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.083,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.182,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.183,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.301,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.636,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.498,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.622,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.111,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.176,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.578,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.553,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.778,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.307,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.547,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.146,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.955,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.912,
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
          "id": "8f78b44fbe3699d5417df9c4df91c8a12caf785f",
          "message": "Release 0.5.0 (#87)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.5.0. 0.5 fixes compatibility\ngaps found by checking edge values against DXC and glslang: round sends\nhalves to even (#82), min / max ignore a NaN operand as D3D defines them\n(#84), glsl::mix uses GLSL's defining formula (#85). It adds scalar\narguments to the component-wise functions (#83) and the opt-in\n<shader_math/std.hpp> (#81), and checks edge values in the cross-checks\n(#86). round and the NaN handling change results, hence the minor bump.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:25:54+09:00",
          "tree_id": "7549fa7cd2e45863df97a7f5b31e50cb3be681a7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/8f78b44fbe3699d5417df9c4df91c8a12caf785f"
        },
        "date": 1792827085108,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.315,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.315,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.929,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.775,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 5.077,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 13.171,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.429,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.571,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 8.309,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.215,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.215,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.361,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.746,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 3.549,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 3.115,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.856,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.811,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.856,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 13.535,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.819,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.916,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.476,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.663,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.165,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.132,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.369,
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
          "id": "bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e",
          "message": "chore: wait 14 days before Renovate updates and align labels (#88)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:26:42+09:00",
          "tree_id": "2e45eaeb5415712cdcf919188c256eff85daa9b7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e"
        },
        "date": 1792827429162,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.224,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.224,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.865,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.631,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.708,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.522,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.386,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.383,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.328,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.19,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.212,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.321,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.661,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.58,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.598,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.62,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.223,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.31,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.954,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.687,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.846,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.957,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.609,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.152,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.04,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.184,
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
          "id": "2a781a8bac65e42af88c1f71b874c9c4920edf71",
          "message": "Fill the HLSL / GLSL gaps found in the flat review (#89)\n\nScalar isnan / isinf (both languages) and isfinite (hlsl); HLSL all / any on\nnumeric vectors, matrices and scalars; select(bool, a, b) with vector\noperands; explicit bool <-> float vector conversions; hlsl::vector<T, N>.\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T17:16:25+09:00",
          "tree_id": "66c2c5b316b429ebf626f70cdd523d4916f44550",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2a781a8bac65e42af88c1f71b874c9c4920edf71"
        },
        "date": 1792830092109,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.193,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.187,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.789,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.538,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.474,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.414,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.384,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.37,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.204,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.187,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.187,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.304,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.652,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.624,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.586,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.606,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.164,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.296,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.756,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.579,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.791,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.386,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.554,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.137,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.946,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.945,
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
          "id": "3bae00e9ab6556c461e891a0c9fad0ad39e26666",
          "message": "Give hlsl and glsl their own vector types (#90)\n\n* Spike: language tag on vector types (hlsl / glsl distinct)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* Make GLSL .stpq unconditional on glsl vectors; update docs\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* README: compile-time ratios from CI\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T19:59:09+09:00",
          "tree_id": "b05d0fcf89a1209095e1162b9c481ad939ff09e6",
          "url": "https://github.com/capsule-holdings/shader-math/commit/3bae00e9ab6556c461e891a0c9fad0ad39e26666"
        },
        "date": 1792839887735,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.211,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.229,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.978,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.545,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.414,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.505,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.381,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.394,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.278,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.19,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.189,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.365,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.248,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.58,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.539,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.648,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.71,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.16,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.799,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.72,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.796,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.42,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.563,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.133,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.039,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.962,
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
          "id": "0c5cc245aa1d5d3d198a315484a92fa7a380f266",
          "message": "Add an HLSL / GLSL conformance corpus measured against DXC and glslang (#91)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:30:47+09:00",
          "tree_id": "c35deab23e42a222476b91a6fecdcfb32ac7aced",
          "url": "https://github.com/capsule-holdings/shader-math/commit/0c5cc245aa1d5d3d198a315484a92fa7a380f266"
        },
        "date": 1792845379304,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.224,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.228,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.757,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.489,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.336,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.196,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.41,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.337,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.051,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.182,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.182,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.297,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.17,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.516,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.505,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.65,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.692,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.123,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.54,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.7,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.783,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.245,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.564,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.137,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.97,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.882,
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
          "id": "25a9357e449eac114368ea17b780488193bdb8e4",
          "message": "ci: use the org-level Blacksmith runner variable for runs-on (#93)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T21:36:36+09:00",
          "tree_id": "2ac0980647ca4906c9f0469b3b813e95d1889546",
          "url": "https://github.com/capsule-holdings/shader-math/commit/25a9357e449eac114368ea17b780488193bdb8e4"
        },
        "date": 1792845734820,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.195,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.2,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.803,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.579,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.61,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.904,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.389,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.428,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.419,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.194,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.194,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.317,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.299,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.652,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.632,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.672,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.797,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.321,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.123,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.733,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.815,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.656,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.576,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.135,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.986,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.027,
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
          "id": "d509e9242feacdd3e40b514367507b9554eb6715",
          "message": "Fill the gaps the conformance corpus found (#92)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:37:18+09:00",
          "tree_id": "ceb4c8dffe4d61d36a57bb771c09582abe6d5ba0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d509e9242feacdd3e40b514367507b9554eb6715"
        },
        "date": 1792846171267,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.184,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.199,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.873,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.566,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.57,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.791,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.386,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.412,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.469,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.192,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.192,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.312,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.46,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.651,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.608,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.667,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.809,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.283,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 12.182,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.736,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.819,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.671,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.575,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.135,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.066,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 3.074,
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
          "id": "a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68",
          "message": "ci: build and test in parallel (#94)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T06:00:00+09:00",
          "tree_id": "8c5cf48375014e926612c158ea1e973a6359c735",
          "url": "https://github.com/capsule-holdings/shader-math/commit/a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68"
        },
        "date": 1792875916044,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.543,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.543,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.006,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.554,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 5.306,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.296,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.568,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.106,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.138,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.205,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.205,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.41,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 3.538,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 5.435,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 5.454,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.929,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 3.654,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 7.183,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.427,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.086,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 1.028,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 8.155,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.717,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.228,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 0.993,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 5.917,
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
          "id": "f066b80792484067f322dc5584f4fcc083ebc958",
          "message": "docs: HLSL / GLSL / shader-math mapping table (#95)\n\n* docs: HLSL / GLSL / shader-math mapping table; more conformance cases\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* docs: what is tolerated, what goes beyond the languages, what depends on the toolchain\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T07:49:49+09:00",
          "tree_id": "148e8bee7d282a384330390e42d6f8f835612795",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f066b80792484067f322dc5584f4fcc083ebc958"
        },
        "date": 1792882419286,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.252,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.346,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.78,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.553,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.453,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.533,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.416,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.38,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.296,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.188,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.188,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.306,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.226,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.601,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.55,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.657,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.743,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.196,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.85,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.716,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.799,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.5,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.573,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.136,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.06,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.977,
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
          "id": "074bbfa38cc462f0297b71a3d30673545cdbc697",
          "message": "Extensions under shader_math::ext; language namespaces take their own language only (#96)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T08:38:42+09:00",
          "tree_id": "82cef50ea7136909ae624d6f41b777f26374234e",
          "url": "https://github.com/capsule-holdings/shader-math/commit/074bbfa38cc462f0297b71a3d30673545cdbc697"
        },
        "date": 1792885327440,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.241,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.279,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.803,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.514,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 4.357,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 11.288,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.404,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [SSE2+SSE4.1+FMA+AVX]",
            "value": 1.353,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [SSE2+SSE4.1+FMA+AVX]",
            "value": 7.123,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.184,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.184,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [SSE2+SSE4.1+FMA+AVX]",
            "value": 0.309,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [SSE2+SSE4.1+FMA+AVX]",
            "value": 2.187,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 2.541,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 2.533,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 0.648,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 2.704,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 4.14,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 11.636,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 0.705,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 0.787,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 7.309,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.559,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.133,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 1.081,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 2.908,
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
        "date": 1792799272413,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.444,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.445,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.564,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.155,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.667,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.617,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.264,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.423,
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
            "value": 2.666,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.623,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.507,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.773,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.585,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.007,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.743,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.621,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.729,
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
            "value": 2.496,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.538,
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
          "id": "77496ab6ca2e788c21c4622e8641ca48f9e2f33e",
          "message": "Split the matrix accessor union per first element on gcc (#77)\n\nWith SHADER_MATH_MATRIX_SWIZZLE3 the header's front-end time on gcc was\nabout 12x the default build. gcc slows down faster than linearly with the\ndata member count of one class scope (8192 members in one anonymous union:\n3.7 s; split into nested unions of 512: 0.44 s), and a 4x4 declared all of\nits 2 x 16^3 accessor lists in one union. On gcc each first element now gets\nits own nested anonymous union (SM_MX_GROUP), which brings\nbench/compile_time.cpp from 7.7 s to 2.9 s (4.4x the default). Nested\nanonymous unions are an extension that gcc accepts silently; clang warns\nunder -Wpedantic and measured slower with them, so clang and MSVC keep one\nflat union (4.1x on clang). README (ja / en) known deviations: about 4x on\nboth compilers.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:03:33+09:00",
          "tree_id": "862842d1b0e87b98c799c65a2ebf2d444c92b815",
          "url": "https://github.com/capsule-holdings/shader-math/commit/77496ab6ca2e788c21c4622e8641ca48f9e2f33e"
        },
        "date": 1792800558149,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.435,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.441,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.142,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.514,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.558,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.259,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.403,
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
            "value": 2.666,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.617,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.516,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.755,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.656,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.04,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.758,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.009,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.61,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.725,
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
            "value": 2.475,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.543,
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
          "id": "9adf550203bbe190bdd2363dacd21883483d6776",
          "message": "Invert float 4x4 matrices in registers on SSE2 (#76)\n\ninverse() of a float 4x4 wrote each register out lane by lane, ran the\nscalar formula, and wrote the result back lane by lane. On SSE2 it now stays\nin registers: inv4_regs applies Cramer's rule over a transposed, pair-rotated\ncopy (the Intel AP-928 arrangement), so every lane move is a single shuffle.\nhlsl and glsl share it (the inverse of the transpose is the transpose of the\ninverse). Halves, 2x2 / 3x3, NEON and the scalar backend keep the dense path.\n\nThe float backend contract (SimdFloatOps) gains six lane moves (cat_lo,\ncat_hi, even, odd, swap_pairs, swap_halves) and fnmadd (c - a*b), implemented\non SSE2, NEON and scalar.\n\nCI runners, ns (array loop / dependent chain): x64 SSE2 8.9 / 14.0 -> 5.7 /\n11.6, x86-64-v3 7.4 / 18.8 -> 5.6 / 14.0. On AArch64 (gcc) the same kernel\nmeasured 17.7 / 21.2 against 16.5 / 21.4, so NEON is left out. Accuracy over\n200k random matrices matches the dense path (mean relative error about 1e-7\non well-conditioned input).\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:08:54+09:00",
          "tree_id": "2d213a7fa1f1235f2ae7d8b64624508db17c3f13",
          "url": "https://github.com/capsule-holdings/shader-math/commit/9adf550203bbe190bdd2363dacd21883483d6776"
        },
        "date": 1792800892581,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.427,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.441,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.127,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.531,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.56,
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
            "value": 23.433,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.557,
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
            "value": 9.634,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.515,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.772,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.586,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.041,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.751,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.612,
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
            "value": 2.504,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.542,
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
          "id": "e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed",
          "message": "Release 0.4.0 (#78)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.4.0. 0.4 adds the opt-in\nmulti-element matrix accessors (SHADER_MATH_MATRIX_SWIZZLE / _SWIZZLE3, #70)\nwith a gcc compile-time fix (#77), converts halves with NEON on ARMv8.0 (#66),\ninverts float 4x4 matrices in registers on SSE2 (#76), cross-checks HLSL\nagainst DXC through SPIR-V (#69) and DXIL on D3D12 WARP (#73), moves to\nSlang v2026.20 (#72), adds the MIT license (#71), and records the benchmark\nhistory (#74, #75). Default builds keep 0.3's API and layout.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T09:15:44+09:00",
          "tree_id": "65d7fc5797a6eb7e60888dbfd16044694603cecd",
          "url": "https://github.com/capsule-holdings/shader-math/commit/e0efaf1d0b26e2fa2424d8bbf2b64412ee6570ed"
        },
        "date": 1792801306101,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.427,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.436,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.147,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.55,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.586,
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
            "name": "normalize(float3) latency [NEON]",
            "value": 23.4,
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
            "value": 2.663,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.611,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.502,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.792,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.633,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.015,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.777,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.005,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.618,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.714,
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
            "value": 2.482,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.535,
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
          "id": "d03f993b2a0f7b42a1e26e1c996a5b513ab971a3",
          "message": "Name the nested swizzle lanes as template parameters (#79)\n\nEach nested swizzle member named its lanes through the enclosing proxy's\nstatic constexpr array (nswz<T, N, L_[i0], L_[i1]>), a constant evaluation\nper member across tens of thousands of members per TU. The 2- and\n3-component swz_store specializations now take the lanes as parameters\n(I0, I1, I2) and the members paste them in directly.\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T12:55:29+09:00",
          "tree_id": "4202b20e6a09479a2f72ec0a64ca3a3dc876b4b1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d03f993b2a0f7b42a1e26e1c996a5b513ab971a3"
        },
        "date": 1792814441650,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.456,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.559,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.151,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.575,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.581,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.259,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.42,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
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
            "value": 2.664,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.619,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.497,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.75,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.585,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.024,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.767,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.009,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.624,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.713,
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
            "value": 2.49,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.545,
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
          "id": "b24601b342fc1db4f3617baca298633190b4524d",
          "message": "Release 0.4.1 (#80)\n\nBump project(VERSION), version.hpp and the README FetchContent examples to\n0.4.1 (find_package keeps 0.4). 0.4.1 cuts the header front-end time with\nSHADER_MATH_NESTED_SWIZZLE (gcc 2.7x -> 2.0x, clang 4.1x -> 2.4x the default\nbuild, #79). API, layout and the generated members are unchanged.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:32:56+09:00",
          "tree_id": "23e506c3dae30e2f46eaa19158cf07ef423f6895",
          "url": "https://github.com/capsule-holdings/shader-math/commit/b24601b342fc1db4f3617baca298633190b4524d"
        },
        "date": 1792816736756,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.43,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.554,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.136,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.459,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.56,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.256,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.406,
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
            "value": 2.661,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.616,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.49,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.769,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.575,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.997,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.752,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.622,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.713,
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
            "value": 2.492,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.54,
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
          "id": "c87e2a2ee76e2df93a3a63f72f15d956a2e929c9",
          "message": "Add opt-in standard library integration (<shader_math/std.hpp>) (#81)\n\nIncluding <shader_math/std.hpp> adds, for both language surfaces:\n- operator<< for vectors, swizzles, bool vectors and matrices\n- std::formatter where <format> exists (__cpp_lib_format); the format spec\n  applies to each component\n- the tuple protocol for structured bindings (components; matrices by m[i]:\n  hlsl rows, glsl columns)\n- std::hash / std::equal_to for vectors (value equality over N components,\n  -0 and +0 as one key), so they work as unordered container keys\n- shader_math::load<V>(const T*) / shader_math::store(x, T*), matrices in\n  their constructor order (hlsl row-major, glsl column-major)\nHalf components print, format and hash through float. shader_math.hpp is\nunchanged and pulls in neither <ostream> nor <format>.\n\ntests/test_std.cpp joins the main suite (SIMD and scalar), and\ntest_restrict.cpp checks that operator<< needs the header. README (ja / en)\ngains a section.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T13:45:06+09:00",
          "tree_id": "80c2ab16a836e8f63a3e838bc522ab95cea4a9be",
          "url": "https://github.com/capsule-holdings/shader-math/commit/c87e2a2ee76e2df93a3a63f72f15d956a2e929c9"
        },
        "date": 1792817532040,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.426,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.15,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.543,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.253,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.383,
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
            "value": 2.668,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.618,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.504,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.742,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.662,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.01,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.753,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.009,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.62,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.715,
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
            "value": 2.486,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.537,
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
          "id": "eda55c30f347bd66d401d1213d4a695766178e94",
          "message": "Round halves to even, as DXC compiles HLSL round (#82)\n\nround sent halves away from zero (std::round). DXC compiles HLSL round to\nRound_ne in DXIL and RoundEven in SPIR-V, so round(0.5) = 0, round(2.5) = 2\nand round(-0.5) = -0 there, where shader-math returned 1, 3 and -1. GLSL\nleaves the halfway direction to the implementation, so glsl::round keeps\nsharing the function and now matches roundEven.\n\nround uses rnd_even; the unused rnd_away leaves the backend contract and\nevery backend (on SSE4.1 round is one roundps). DXC-only interop case\ncase_exact_round_ties checks exact halves in SPIR-V and DXIL (Slang's CPU\ntarget uses ::roundf, so the shared cases avoid halves). Tests, README\n(ja / en) and docs/slang_compat.md follow.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:35:05+09:00",
          "tree_id": "9daab9c183c8f9195eb09e621be881a3794810e0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/eda55c30f347bd66d401d1213d4a695766178e94"
        },
        "date": 1792824033140,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.426,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.554,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.138,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.531,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.544,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.54,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.257,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.38,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.422,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.665,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.607,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.498,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.787,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.631,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.992,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.761,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.007,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.603,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.742,
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
            "value": 2.482,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.537,
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
          "id": "f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a",
          "message": "Accept scalar arguments in the component-wise functions (#83)\n\nmax(dot(n, l), 0.0f) and saturate(dot(n, l)), the most common shader idioms,\ndid not compile: the component-wise functions took vectors only. A call with\nno vector argument and at least one float / half / one-component swizzle /\nmatrix element (detail::ScalarCall) now takes a scalar path that runs the\nvector function on one lane, so rounding and edge cases match the vectors.\nOther arguments follow the vector-scalar operand rules (any arithmetic type\nnext to float, exactly half next to half); double- or int-only calls are not\ncovered. The overloads are constrained templates, so they do not clash with\nthe C library's same-named functions and abs(-2.5f) no longer reaches the C\nint abs.\n\nCovered: the shared component-wise functions, dot / length / distance and\nmodf in both languages; HLSL lerp saturate frac rcp rsqrt log10 atan2 fmod\nldexp mad, frexp, sincos, select(bool, a, b); GLSL mix fract inversesqrt\nroundEven asinh acosh atanh atan(y, x) mod fma, mix(x, y, bool). Scalar\nround is a plain function instead: libc++ and MSVC declare a global\n::round(float) (halves away from zero) that would beat a template, so an\nunqualified call is ambiguous there rather than quietly wrong; hlsl::round(x)\nalways resolves to shader-math.\n\nTests in test_funcs.cpp; interop cases case_near_scalar (Slang, DXC SPIR-V\nand DXIL) and case_approx_scalar (GLSL). README (ja / en), CONTEXT.md and\ndocs/slang_compat.md drop the vector-only rule.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:45:43+09:00",
          "tree_id": "9d03c8c2d368878d0b8e9de20e469c826bc9ffd1",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f20397e9dcbf3d43e4ad1c59f0ad7f9915e1320a"
        },
        "date": 1792824666057,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.428,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.44,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.147,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.588,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.587,
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
            "name": "normalize(float3) latency [NEON]",
            "value": 23.421,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.421,
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
            "value": 9.617,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.503,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.755,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.645,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.985,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.768,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.009,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.621,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.729,
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
            "value": 2.492,
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
          "id": "2895f46ecb6f57f1b3930695fc0a6a94e396acfb",
          "message": "Make min / max ignore a NaN operand, as D3D defines them (#84)\n\nmin / max followed each backend: SSE2 and scalar returned NaN when the second\noperand was NaN, NEON returned NaN whenever either was, so saturate(NaN) was\n0 on x86 and NaN on ARM. D3D defines min / max as IEEE minNum / maxNum and\nsaturate(NaN) = 0 (DXC emits FMin / FMax / Saturate in DXIL, NMin / NMax /\nFClamp in SPIR-V); GLSL leaves it undefined, which this satisfies. Every\nbackend now ignores a NaN operand: SSE2 replaces the lanes where b is NaN\n(cmpunordps + select), NEON uses vminnmq / vmaxnmq (vminnm_f16 / vmaxnm_f16\nfor native half), scalar checks b != b. clamp / saturate / smoothstep build\non them, so saturate(NaN) = 0 and clamp(NaN, lo, hi) = lo everywhere.\n\nTests in test_simd.cpp and test_funcs.cpp (vectors, scalars, half). Interop\ncase case_exact_nan_rules runs in the Slang and DXC SPIR-V checks; not in\nDXIL, which DXC compiles with fast-math flags that assume no NaN. README\n(ja / en) updated.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:53:14+09:00",
          "tree_id": "411b120f91c758fc2b398c4a1c1446ebf5efd5fc",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2895f46ecb6f57f1b3930695fc0a6a94e396acfb"
        },
        "date": 1792825106290,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.428,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.443,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.147,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.565,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.562,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.258,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.392,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.419,
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
            "value": 9.609,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.494,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.766,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.595,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.016,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.725,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.608,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.723,
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
            "value": 2.481,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.535,
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
          "id": "17d0f3115690cedcd685b66f777ff51f51ba9862",
          "message": "Compute glsl::mix with GLSL's defining formula (#85)\n\nglsl::mix shared hlsl::lerp's x + a*(y - x). GLSL defines mix as\nx*(1 - a) + y*a, and the two differ at the ends: mix(1e8, 1, 1) is 1 on\nglslang / lavapipe and was 0 here, because x + a*(y - x) loses y next to a\nmuch larger x. glsl::mix now uses detail::mix_impl with the defining\nformula (vector or scalar a, float or half), exact at a = 0 and a = 1.\nhlsl::lerp keeps x + s*(y - x), which is what DXC emits for DXIL.\n\nTests in test_funcs.cpp; GLSL interop case case_exact_mix_ends (bit-exact\nagainst lavapipe). README (ja / en) and docs/glsl_interop.md describe the two\nformulas.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T15:59:30+09:00",
          "tree_id": "da5e0dc312a8e150a64bee7fa51e838e865ce17f",
          "url": "https://github.com/capsule-holdings/shader-math/commit/17d0f3115690cedcd685b66f777ff51f51ba9862"
        },
        "date": 1792825495328,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.427,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.435,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.141,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.502,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.599,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.258,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.401,
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
            "value": 9.631,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.49,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.763,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.583,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.048,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.756,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.007,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.628,
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
            "value": 2.483,
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
          "id": "6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3",
          "message": "interop: check edge values; compare infinities strictly (#86)\n\nThe cross-checks ran one fixed input set with no edge values. compare.py\nalso scaled its relative tolerance by max(1, |a|, |b|), so with an infinity\ntwo equal infinities failed and an infinity against a finite value passed;\nan infinity now matches only the same infinity, in every tier.\n\nNew cases: case_near_edges (shared HLSL; Slang, DXC SPIR-V and DXIL) for\nfmod with negative operands, step on equal bounds, clamp with reversed\nbounds, frac(-0) and smoothstep with reversed edges; case_exact_inf_edges\n(Slang and DXC SPIR-V only, as DXC compiles DXIL with fast-math flags that\nassume no infinities) for rcp / rsqrt of signed zeros; and case_near_edges\nfor GLSL (mod with negative operands, step on equal bounds, fract(-0); GLSL\nleaves inversesqrt(0) undefined). docs/glsl_interop.md and\ndocs/slang_compat.md list them.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:07:33+09:00",
          "tree_id": "ace6c4271d3683a656c56afb52218dfb80ddbd88",
          "url": "https://github.com/capsule-holdings/shader-math/commit/6cc6dd9f38782b8b09afe9ba4b7ce1b3c550bbd3"
        },
        "date": 1792825982070,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.433,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.448,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.143,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.587,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.591,
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
            "name": "normalize(float3) latency [NEON]",
            "value": 23.424,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.421,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.661,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.611,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.498,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.785,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.638,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.986,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.778,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.007,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.608,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.736,
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
            "value": 2.496,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.534,
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
          "id": "8f78b44fbe3699d5417df9c4df91c8a12caf785f",
          "message": "Release 0.5.0 (#87)\n\nBump project(VERSION), version.hpp, the README FetchContent / find_package\nexamples and the package consumer test to 0.5.0. 0.5 fixes compatibility\ngaps found by checking edge values against DXC and glslang: round sends\nhalves to even (#82), min / max ignore a NaN operand as D3D defines them\n(#84), glsl::mix uses GLSL's defining formula (#85). It adds scalar\narguments to the component-wise functions (#83) and the opt-in\n<shader_math/std.hpp> (#81), and checks edge values in the cross-checks\n(#86). round and the NaN handling change results, hence the minor bump.\n\nCo-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:25:54+09:00",
          "tree_id": "7549fa7cd2e45863df97a7f5b31e50cb3be681a7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/8f78b44fbe3699d5417df9c4df91c8a12caf785f"
        },
        "date": 1792827087309,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.425,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.558,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.128,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.599,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.6,
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
            "name": "normalize(float3) latency [NEON]",
            "value": 23.41,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.557,
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
            "value": 2.664,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.606,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.489,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.769,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.593,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.045,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.791,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.612,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.766,
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
            "value": 2.49,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.549,
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
          "id": "bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e",
          "message": "chore: wait 14 days before Renovate updates and align labels (#88)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T16:26:42+09:00",
          "tree_id": "2e45eaeb5415712cdcf919188c256eff85daa9b7",
          "url": "https://github.com/capsule-holdings/shader-math/commit/bb2132003ffba354ec0a96b5dcd0b7fba3e4b66e"
        },
        "date": 1792827431408,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.427,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.441,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.137,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.513,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.576,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.258,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.405,
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
            "value": 2.659,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.622,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.504,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.767,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.594,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.061,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.761,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.008,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.619,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.732,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.42,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.557,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.499,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.546,
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
          "id": "2a781a8bac65e42af88c1f71b874c9c4920edf71",
          "message": "Fill the HLSL / GLSL gaps found in the flat review (#89)\n\nScalar isnan / isinf (both languages) and isfinite (hlsl); HLSL all / any on\nnumeric vectors, matrices and scalars; select(bool, a, b) with vector\noperands; explicit bool <-> float vector conversions; hlsl::vector<T, N>.\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T17:16:25+09:00",
          "tree_id": "66c2c5b316b429ebf626f70cdd523d4916f44550",
          "url": "https://github.com/capsule-holdings/shader-math/commit/2a781a8bac65e42af88c1f71b874c9c4920edf71"
        },
        "date": 1792830094475,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.428,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.434,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.553,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.123,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.542,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.584,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.258,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.423,
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
            "value": 2.668,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.604,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.5,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.754,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 5.627,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.01,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.79,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 2.007,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.611,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.739,
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
            "value": 2.485,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.542,
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
          "id": "3bae00e9ab6556c461e891a0c9fad0ad39e26666",
          "message": "Give hlsl and glsl their own vector types (#90)\n\n* Spike: language tag on vector types (hlsl / glsl distinct)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* Make GLSL .stpq unconditional on glsl vectors; update docs\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* README: compile-time ratios from CI\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T19:59:09+09:00",
          "tree_id": "b05d0fcf89a1209095e1162b9c481ad939ff09e6",
          "url": "https://github.com/capsule-holdings/shader-math/commit/3bae00e9ab6556c461e891a0c9fad0ad39e26666"
        },
        "date": 1792839889958,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.433,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.431,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.133,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.596,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.585,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.54,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.256,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.413,
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
            "value": 2.663,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.615,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.494,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.208,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.029,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.787,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.454,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.611,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.478,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.518,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.543,
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
          "id": "0c5cc245aa1d5d3d198a315484a92fa7a380f266",
          "message": "Add an HLSL / GLSL conformance corpus measured against DXC and glslang (#91)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:30:47+09:00",
          "tree_id": "c35deab23e42a222476b91a6fecdcfb32ac7aced",
          "url": "https://github.com/capsule-holdings/shader-math/commit/0c5cc245aa1d5d3d198a315484a92fa7a380f266"
        },
        "date": 1792845381418,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.426,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.448,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.151,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.519,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.614,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.54,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.258,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.426,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.42,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.66,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.629,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.493,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.206,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.022,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.785,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.455,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.602,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.496,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.51,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.535,
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
          "id": "25a9357e449eac114368ea17b780488193bdb8e4",
          "message": "ci: use the org-level Blacksmith runner variable for runs-on (#93)\n\nCo-authored-by: Claude Opus 5.5 <noreply@anthropic.com>",
          "timestamp": "2026-10-24T21:36:36+09:00",
          "tree_id": "2ac0980647ca4906c9f0469b3b813e95d1889546",
          "url": "https://github.com/capsule-holdings/shader-math/commit/25a9357e449eac114368ea17b780488193bdb8e4"
        },
        "date": 1792845737369,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.428,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.438,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.556,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.152,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.502,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.577,
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
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.42,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.696,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.667,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.617,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.501,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.552,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.207,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.992,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.757,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.455,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.608,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.444,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.42,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.483,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.537,
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
          "id": "d509e9242feacdd3e40b514367507b9554eb6715",
          "message": "Fill the gaps the conformance corpus found (#92)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-24T21:37:18+09:00",
          "tree_id": "ceb4c8dffe4d61d36a57bb771c09582abe6d5ba0",
          "url": "https://github.com/capsule-holdings/shader-math/commit/d509e9242feacdd3e40b514367507b9554eb6715"
        },
        "date": 1792846174019,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.428,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.444,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.126,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.589,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.578,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.256,
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
            "value": 2.661,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.619,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.497,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.554,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.216,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.991,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.723,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.453,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.605,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.499,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.505,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.54,
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
          "id": "a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68",
          "message": "ci: build and test in parallel (#94)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T06:00:00+09:00",
          "tree_id": "8c5cf48375014e926612c158ea1e973a6359c735",
          "url": "https://github.com/capsule-holdings/shader-math/commit/a7e4dfa8a1f7ca8eff58c3809d3dd21ac333bd68"
        },
        "date": 1792875918675,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.427,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.44,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.139,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.532,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.567,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.257,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.387,
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
            "value": 9.611,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.494,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.551,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.212,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 9.995,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.78,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.454,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.613,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.467,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.418,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.488,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.546,
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
          "id": "f066b80792484067f322dc5584f4fcc083ebc958",
          "message": "docs: HLSL / GLSL / shader-math mapping table (#95)\n\n* docs: HLSL / GLSL / shader-math mapping table; more conformance cases\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n* docs: what is tolerated, what goes beyond the languages, what depends on the toolchain\n\nCo-authored-by: Cursor <cursoragent@cursor.com>\n\n---------\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T07:49:49+09:00",
          "tree_id": "148e8bee7d282a384330390e42d6f8f835612795",
          "url": "https://github.com/capsule-holdings/shader-math/commit/f066b80792484067f322dc5584f4fcc083ebc958"
        },
        "date": 1792882421853,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.426,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.44,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.151,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.546,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.579,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.541,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.273,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.442,
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
            "value": 0.71,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.664,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.619,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.484,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.555,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.198,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.01,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.758,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.453,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.592,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.47,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.556,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.419,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.517,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.53,
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
          "id": "074bbfa38cc462f0297b71a3d30673545cdbc697",
          "message": "Extensions under shader_math::ext; language namespaces take their own language only (#96)\n\nCo-authored-by: Cursor <cursoragent@cursor.com>",
          "timestamp": "2026-10-25T08:38:42+09:00",
          "tree_id": "82cef50ea7136909ae624d6f41b777f26374234e",
          "url": "https://github.com/capsule-holdings/shader-math/commit/074bbfa38cc462f0297b71a3d30673545cdbc697"
        },
        "date": 1792885330115,
        "tool": "customSmallerIsBetter",
        "benches": [
          {
            "name": "hlsl mul(float4x4, float4x4) [NEON]",
            "value": 3.439,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [NEON]",
            "value": 3.434,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [NEON]",
            "value": 1.557,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [NEON]",
            "value": 5.132,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [NEON]",
            "value": 16.579,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [NEON]",
            "value": 25.48,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [NEON]",
            "value": 1.537,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [NEON]",
            "value": 5.244,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [NEON]",
            "value": 23.359,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [NEON]",
            "value": 0.555,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [NEON]",
            "value": 0.417,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [NEON]",
            "value": 0.694,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [NEON]",
            "value": 2.659,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4x4) [scalar]",
            "value": 9.597,
            "unit": "ns/elem"
          },
          {
            "name": "glsl mat4 * mat4 [scalar]",
            "value": 9.486,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4, float4x4) [scalar]",
            "value": 1.552,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl mul(float4x4, float4) [scalar]",
            "value": 4.208,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) [scalar]",
            "value": 10.028,
            "unit": "ns/elem"
          },
          {
            "name": "hlsl inverse(float4x4) latency [scalar]",
            "value": 20.703,
            "unit": "ns/elem"
          },
          {
            "name": "dot(float4, float4) [scalar]",
            "value": 1.451,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) [scalar]",
            "value": 3.603,
            "unit": "ns/elem"
          },
          {
            "name": "normalize(float3) latency [scalar]",
            "value": 21.459,
            "unit": "ns/elem"
          },
          {
            "name": "floor(float4) [scalar]",
            "value": 0.555,
            "unit": "ns/elem"
          },
          {
            "name": "round(float4) [scalar]",
            "value": 0.419,
            "unit": "ns/elem"
          },
          {
            "name": "lerp(float4, float4, float) [scalar]",
            "value": 2.492,
            "unit": "ns/elem"
          },
          {
            "name": "half4 a * b + c [scalar]",
            "value": 6.528,
            "unit": "ns/elem"
          }
        ]
      }
    ]
  }
}