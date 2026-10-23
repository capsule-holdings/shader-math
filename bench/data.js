window.BENCHMARK_DATA = {
  "lastUpdate": 1792796402330,
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
      }
    ]
  }
}