# shader-math

[日本語](README.md) | English

HLSL / GLSL compatible float vector and matrix library for C++20: header-only, SIMD (SSE2 / NEON, with a scalar fallback).

The goal is to write a shader expression in a `.cpp` file and have it mean exactly what it means in the shader. Because `operator*` means opposite things in the two languages (component-wise in HLSL, the matrix product in GLSL), the public API lives in two namespaces.

```cpp
#include <shader_math/shader_math.hpp>

void hlsl_side() {
    using namespace hlsl;                // * is component-wise, mul() is the product
    float3x3 a = {1,2,3, 4,5,6, 7,8,9};  // fills row by row
    float3x3 h = a * a;                  // Hadamard (component-wise)
    float3x3 p = mul(a, a);              // matrix product
    float3   v = mul(float3(1,2,3), a);  // row vector x matrix
    if (all(h == h)) { /* comparisons give bool3x3; reduce with all() / any() */ }
}

void glsl_side() {
    using namespace glsl;                // * is the matrix product
    mat3 m = {1,4,7, 2,5,8, 3,6,9};      // fills column by column
    mat3 id(1.0f);                       // a single scalar makes a diagonal (identity)
    vec3 mv = m * vec3(1,2,3);           // matrix x column vector
    mat3 h  = matrixCompMult(m, m);      // component-wise is a function
    if (m == m) { /* == is a scalar bool */ }
}
```

Never import both namespaces into the same scope (`mul`, `transpose` and others collide).

## Types

- Vectors: `hlsl::float2/3/4` = `glsl::vec2/3/4` (**the same type**: `*` is component-wise in both languages)
- Half vectors: `hlsl::half2/3/4` = `glsl::f16vec2/3/4` (also one type); scalars `hlsl::half` / `glsl::float16_t`
- Matrices: every shape with R, C in {2, 3, 4}. `hlsl::floatRxC` (stored by rows, row-major constructor) and `glsl::matCxR` (stored by columns, column-major constructor) are **distinct types** with explicit conversion only. Non-square names swap: `float3x4` (3 rows x 4 columns) is `mat4x3` (4 columns x 3 rows)
- Half matrices: `hlsl::halfRxC` / `glsl::f16matCxR`, same rules as float
- Comparison results: `bool2/3/4` (= `bvec2/3/4`) and `boolRxC`; no arithmetic, no implicit conversion to `bool`. Build them from bools (`bool3(true, false, true)`, `bool3(true)`; int / float arguments do not compile) and read components with `b[i]` or the single-component `.x` ... `.w` / `.r` ... `.a` (read-only)
- Half and float never mix implicitly (`half3 * float3` and `half3 * 2.0f` do not compile); constructor arguments convert (`half3(1.0f, 2.0f, 3.0f)` is fine)
- Half needs a compiler whose `_Float16` works in C++ (x86: gcc 12+ / clang 15+, AArch64: gcc 13+); MSVC uses a software binary16. Without it, the half types are simply absent
- On ARM, `arm_neon.h` declares a global `float16_t`, so an unqualified `float16_t` under `using namespace glsl` is ambiguous; write `glsl::float16_t` or `hlsl::half`

## Language semantics

| | `hlsl::` | `glsl::` |
| --- | --- | --- |
| matrix `*` matrix | component-wise (same shape only) | matrix product (inner dimensions checked by the type) |
| matrix product | `mul(a, b)` | `a * b` |
| component-wise product | `a * b` | `matrixCompMult(a, b)` |
| vector transform | `mul(v, m)` / `mul(m, v)` | `v * m` / `m * v` |
| single-scalar matrix constructor | every element | diagonal (`mat3(1.0)` = identity) |
| matrix from matrix | shrink only (upper-left block) | grow (identity fill) or shrink |
| `==` | component-wise bool vector / matrix | scalar bool (all components equal) |
| `<` `>` `<=` `>=` | component-wise bool | not on vectors (`lessThan()` and friends) |
| element access | `m[row][col]`, `_m00`, `_11` | `m[col][row]` |
| swizzles | every `.xyzw` / `.rgba` combination, read and write | same; `.stpq` is opt-in (below) |
| swizzle of a swizzle (`v.xy.yx`) | opt-in (below) | same |

`transpose`, `determinant`, `inverse`, `dot`, `cross`, `length`, `normalize`, `all` and `any` exist in both (`inverse` is official GLSL and an extension on the HLSL side).

## Restricted by design: undefined operations do not compile

- `float3x4 * float4x3` under `hlsl` (use `mul`)
- `mat3 * mat4x2` under `glsl` (inner dimensions differ)
- `float3 + float4` (size changes need an explicit constructor)
- `if (a == b)` under `hlsl` (reduce with `all()` / `any()`)
- `v.xx = ...` (writing a swizzle with a repeated component)
- implicit `hlsl::matrix` <-> `glsl::matrix` conversion (explicit only)
- names that do not exist in the language (`saturate` under `glsl`, `mix` under `hlsl`, ...)

## Component-wise functions

Vector (float / half) arguments; scalar second / third arguments follow the operator rules (float vectors accept any arithmetic scalar, half vectors only `half`). Pure scalars are left to `std::`.

- Both languages: `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc` `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` `radians` `degrees` `modf` `isnan` `isinf`
- Geometry (both): `dot` `cross` `length` `normalize` `distance` `reflect` `refract` `faceforward`
- HLSL only: `lerp` `frac` `rcp` `rsqrt` `saturate` `atan2` `fmod` `mad` `isfinite` `sincos` `log10` `ldexp` (float exponent) `frexp` (float exponent out) `select` (HLSL 2021) `dst` `lit` (scalar inputs, returns `float4`), and `%` on float vectors / matrices (same as `fmod`)
- GLSL only: `mix` `fract` `inversesqrt` `roundEven` `atan(y, x)` `mod` `fma` `asinh` `acosh` `atanh`, `mix(x, y, bvec)` (y where the selector is true)
- HLSL also applies the component-wise functions to matrices (`abs(m)`, `lerp(a, b, t)`, `isnan(m)` -> `boolRxC`, ...); GLSL's generic functions take scalars / vectors only, so `abs(mat3)` does not compile under `glsl`

Semantic notes:

- `hlsl::fmod` and `glsl::mod` **differ**: `fmod` truncates and keeps the sign of x (`fmod(-7, 3) = -1`), `mod` floors and keeps the sign of y (`mod(-7, 3) = 2`)
- `round` rounds halves away from zero (like `std::round`); GPUs often round to even, so use `glsl::roundEven` when that matters
- `sign` returns the vector type in both languages (HLSL's int result is not replicated)
- `rcp` / `rsqrt` / `inversesqrt` are exact (no `rcpps` / `rsqrtps` approximations); transcendentals call libm per lane
- `mad` / `fma` fuse when the backend has FMA (neither language guarantees fusion unless `precise`)
- `refract` returns the zero vector on total internal reflection
- `select(c, a, b)` picks a where c is true; `mix(x, y, bvec)` picks y where true (opposite operand order)
- With `<windows.h>`, define `NOMINMAX` (its `min` / `max` macros collide)

## SIMD

- NEON (AArch64) and SSE2 (x86-64) are first-class; other architectures build on a scalar 4-wide fallback with the same API. `-DSHADER_MATH_FORCE_SCALAR=ON` forces the scalar backend; CI runs every test on both
- Optional compile-time features (no runtime dispatch; the SSE2 / ARMv8.0 baseline is not raised):
  - FMA3 (`__FMA__`, e.g. `-march=x86-64-v3`; MSVC `/arch:AVX2`): fused multiply-add
  - SSE4.1 (`__SSE4_1__`, `-march=x86-64-v2`; MSVC `/arch:AVX`): `roundps` for the rounding functions
  - F16C (`__F16C__`, `-march=x86-64-v3`; MSVC `/arch:AVX2`): one-instruction half <-> float conversion
  - AVX1 together with FMA: the 4x4 float product packs two rows per YMM register (results unchanged)
  - ARM FEAT_FP16 (`-march=armv8.2-a+fp16`): half math on real 16-bit lanes; elsewhere half widens to f32 per operation, converting four lanes at once (ARMv8.0 base `vcvt_f32_f16` / `vcvt_f16_f32`, x86 F16C or SSE2 integer ops)
- Boost is used header-only (Config / Predef 1.60+ for ISA detection); CMake fetches it when it is not installed

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/sandbox                    # playground
./build/shader_math_bench          # micro-benchmarks (and shader_math_bench_scalar)
python3 bench/compile_time.py c++  # header front-end time per opt-in macro set
```

`-DSHADER_MATH_WARNINGS_AS_ERRORS=ON` turns warnings into errors for this repo's own targets (CI always does).

Requirements: C++20 (concepts). gcc 10+ / clang 14+ / MSVC 19.30+ / Apple Clang. gcc 10 and clang 14 are the floors CI checks; MSVC and Apple Clang are checked at the runners' default versions.

## Using it from another CMake project

The target is `shader_math::shader_math` either way.

```cmake
# A) as a subproject (add_subdirectory / FetchContent)
include(FetchContent)
FetchContent_Declare(shader_math
    GIT_REPOSITORY https://github.com/capsule-holdings/shader-math.git
    GIT_TAG v0.3.0)
FetchContent_MakeAvailable(shader_math)

# B) an installed package (cmake --install build --prefix <prefix>)
find_package(shader_math 0.3 REQUIRED CONFIG)

target_link_libraries(app PRIVATE shader_math::shader_math)
```

- As a subproject it builds no tests, examples or install rules (override with `SHADER_MATH_BUILD_TESTS` / `SHADER_MATH_INSTALL`)
- Boost is not bundled: `find_package` looks for it on the consumer side; set `SHADER_MATH_BOOST_INCLUDE` (and `SHADER_MATH_BOOST_PREDEF_INCLUDE` if Predef lives elsewhere) when it is not found
- 0.x versions are compatible within the same minor version only
- Force the scalar backend with `-DSHADER_MATH_FORCE_SCALAR=ON` (subproject) or `set(SHADER_MATH_FORCE_SCALAR ON)` before `find_package`; enable the GLSL `.stpq` swizzles and swizzles of swizzles the same way with `SHADER_MATH_GLSL_STPQ` / `SHADER_MATH_NESTED_SWIZZLE`. All land on `shader_math::shader_math`, so every translation unit agrees (mixing them would break the ODR)
- pkg-config: `<prefix>/share/pkgconfig/shader_math.pc` (`pkg-config --cflags shader_math`); Boost is listed only when a system Boost was used at install time

## Cross-checked against real compilers

- HLSL side: the same source is built by Slang (`slangc -target cpp`, run on the CPU) and by shader-math, and the outputs are compared ([docs/slang_compat.md](docs/slang_compat.md)); it is also compiled by DXC, the official HLSL compiler, to SPIR-V and run on lavapipe, which covers `frexp` / `dst` as well
- GLSL side: the same source is compiled to SPIR-V by glslang and run on Vulkan (lavapipe), and compared with shader-math's `glsl` surface ([docs/glsl_interop.md](docs/glsl_interop.md))
- Half: Slang's CPU target cannot compile half vector math, so the HLSL half cases are compiled to SPIR-V by Slang and run on lavapipe; GLSL `f16vec` cases likewise (both need the device's `shaderFloat16`)

## Known deviations

- Multi-component matrix swizzles (`_m00_m11`) are not implemented; every single-component accessor is
- GLSL `.stpq` swizzles are opt-in (`SHADER_MATH_GLSL_STPQ`): the vector type is shared with `hlsl`, which has no stpq, so enabling them lets HLSL code accept `.stp` too (same lanes as `.xyz`). Measured compile-time cost: +5 to 11% with gcc, within noise with clang
- Swizzles of swizzles (`v.xy.yx`, `v.xyz.zx`) are opt-in (`SHADER_MATH_NESTED_SWIZZLE`): reads on 2- and 3-component swizzles (`.stpq` too when enabled). There is no nesting on a 4-component swizzle (`v.wzyx.xy`) and no writing through a nested one; write `float4(v.wzyx).xy`. The member declarations multiply, so a translation unit that only includes the library takes 3-4x the front-end time with it (`bench/compile_time.py`: gcc 3.0x, clang 4.0x); hence off by default
- GLSL's `not()` is `not_()` (`not` is a C++ alternative token); HLSL 2021's `and()` / `or()` cannot exist for the same reason, and bool-vector `&&` / `||` are unavailable as in HLSL 2021
- `v.x` is a proxy type: pass `float(v.x)` to varargs
- float and half only: no `int` / `double`, no `1xN` matrices, no GPU-only features (textures, derivatives, atomics)
- Language-compatible, not ABI-compatible: `sizeof(float3) == 16`, unrelated to constant-buffer packing
- Component access and swizzles rely on union type punning, which ISO C++ leaves undefined; it holds on gcc / clang / MSVC as CI checks, and it rules out `constexpr` ([CONTEXT.md](CONTEXT.md), Japanese)
- Lanes past N of an N < 4 vector (and of matrix rows / columns) are unspecified and may hold NaN / Inf; results only ever depend on the first N lanes, but a floating-point-exception trap can fire on them
