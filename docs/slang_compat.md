# Slang ⇔ shader-math mapping

This document defines the "shareable subset" used to build and run the same math source
(`tests/interop/shared/math_cases.hlsli`) with both
(a) the Slang compiler (`slangc -target cpp`, run on the CPU) and
(b) C++ (the `hlsl` surface of shader-math), and to compare the numeric results.
The dual compile itself acts as a lint for this table:
syntax outside the subset always fails to compile with one of the two compilers.

For how to run the verification harness, see `tests/interop/` and the "Build and test" section of the README.
The Slang version being checked is `slang_tag` in `ci.yml` (CI pins it and downloads it, and Renovate updates it).

## Shareable subset (compiles as-is with both, and verified to match)

| Category | Contents |
| --- | --- |
| Vector types | `float2` / `float3` / `float4` (including constructors and element access) |
| Component-wise operators | `+` `-` `*` `/` (vector with vector, and vector × scalar) |
| swizzle | Reading and writing `.xyzw` (writing the same component twice is an error in both) |
| Nested swizzles | Reading from 2- and 3-component swizzles (`a.zw.yx`, `b.bgr.rrgb`). These are opt-in in shader-math, so `shared/nested_cases.hlsli` is checked with `kernels_nested.slang` against a C++ side built with `SHADER_MATH_NESTED_SWIZZLE` (`slang_interop_nested_check`) |
| Multi-component matrix access | Reading and writing (without duplicates) `m._m30_m31_m32`, `m._11_22_33`, `m._m00_m11`. These are opt-in in shader-math, so `shared/matrix_swizzle_cases.hlsli` is checked with `kernels_matrix_swizzle.slang` against a C++ side built with `SHADER_MATH_MATRIX_SWIZZLE3` (`slang_interop_matrix_swizzle_check`) |
| Matrices | `floatRxC` (R,C ∈ {2,3,4}), row-major ctor, Hadamard `*` |
| Matrix operations | All forms of `mul` (vec×mat / mat×vec / mat×mat), `transpose`, `determinant` |
| Vector operations | `dot` / `cross` / `length` / `normalize` |
| Comparison | Component-wise `==` `<` and so on, plus reduction with `all` / `any` |
| Component-wise functions (v2) | `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc` `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` + the HLSL aliases `lerp` `frac` `rcp` `rsqrt` `saturate` (vector and scalar; the scalar forms are in `case_near_scalar`) |
| Inverse trigonometric, hyperbolic, and angle functions | `asin` `acos` `atan` `atan2(y, x)` `sinh` `cosh` `tanh` `radians` `degrees` |
| Remainder, multiply-add, and decomposition | `fmod` (trunc-based, sign of x), `mad`, `modf(x, out ip)`, `sincos(x, out s, out c)` |
| Geometry | `distance` `reflect` `refract` (zero vector on total internal reflection) `faceforward` |
| Classification | `isnan` `isinf` `isfinite` (read the resulting bool vector **by index**; see the exclusions below) |
| Component-wise functions on matrices | Matrix versions of the component-wise functions above (`abs(m)` `lerp(a, b, t)` `clamp` `min(m, s)` `step(s, m)` `atan2` `fmod` `mad`, and so on) |
| Literals | Float literals must always have the `f` suffix |

## Exclusions (things that must not be written in the shared files)

| Item | Reason |
| --- | --- |
| `inverse` | HLSL / Slang have no intrinsic with this name (in shader-math it is the extension `shader_math::ext::inverse`) |
| Resource types and textures | `StructuredBuffer` and the like appear only in the wrapper (`kernels.slang`). Shared math is limited to pure functions |
| Slang generics / `interface` | There is no counterpart on the C++ side (this harness covers only the math subset) |
| Passing `v.x` directly to variadic arguments | In shader-math, `v.x` is a proxy type. For `printf` and similar, use `float(v.x)` (this does not apply to shared math, but take care) |
| `.xyzw` on bool vectors | shader-math's `boolN` has no access by component name (it is a mask type for comparison results). Read it by index, as in `isnan(v)[2]` |
| **CPU execution of half (`half2/3/4`)** | The C++ prelude of the CPU target in Slang (v2026.20) does not implement half vector arithmetic (`operator*` and others for `Vector<_Float16, 4>` are missing). The half cases are separated into `SM_INTEROP_HALF_CASES`, and `kernels_half.slang` is compiled to SPIR-V with `-target spirv`, run on Vulkan (lavapipe), and compared (`slang_interop_half_check`; skipped on devices without `shaderFloat16`) |

## Semantic differences (things that compile but whose results or rules may differ)

- **Type of the `frexp` exponent**: HLSL (DXC) and shader-math return the exponent as float, but Slang (v2026.20)
  returns it as an int vector (the type of the `out` parameter differs, so it cannot be called in the shared files).
  Slang's `ldexp` also takes an int exponent (float is converted implicitly), so the shared cases pass only
  integer-valued exponents.
- **`dst`**: In Slang (v2026.20), the C++ prelude of the CPU target has no implementation of it, and with
  `-target spirv` parsing of the core built-in code fails (`snippet parsing failed`). Do not write it in the shared
  files (shader-math covers it with unit tests only). Via SPIR-V, `frexp` likewise takes an int exponent.
- **`sign` on int vectors**: the C++ prelude of the CPU target in Slang (v2026.20) has no `I32_sign`, so the generated
  code does not compile. It is checked against DXC only (`case_bits_int_sign`).

- **Implicit mixing of half and float**: Slang (following HLSL rules) allows implicit promotion, but shader-math makes
  it a compile error. The shared files **require explicit conversion** (`half4(f4)` / `float4(h4)`).
  Compiling the C++ side enforces this rule mechanically; that is, the dual compile is the lint.
- **fp contraction (fmadd fusion)**: Rounding changes depending on the target and compiler.
  The harness builds the Slang-generated code with `-ffp-contract=off`, and the `near_` tier absorbs the difference
  with a relative 1e-5 (the tiered tolerances in `compare.py`: `exact_` = bit-exact,
  `near_` = relative 1e-5, `half_` = relative 2e-3, `bits_` = identical bit patterns including NaN patterns, for
  int / uint results returned through `asfloat`).
- **Halfway values (.5) in `round`**: HLSL's `round` rounds .5 to even (DXC emits `Round_ne` in DXIL and `RoundEven`
  in SPIR-V). shader-math does the same. Slang's CPU target uses `::roundf` (away from zero), so the shared cases
  avoid exact .5 values, and .5 is checked in the DXC-only `case_exact_round_ties`.
- **`rcp` / `rsqrt`**: shader-math uses exact versions, not approximate instructions. On GPU execution they may become
  approximate instructions (they match in the CPU comparison; the SPIR-V phase needs relaxed tolerances).
- **Memory layout**: The compatibility is at the language level, not at the ABI level (`sizeof(float3) == 16`,
  cbuffer packing, and so on). This is out of scope for this verification.

## Cross-checking against DXC (the official HLSL compiler)

The same shared cases are checked against DXC (v1.9.2609) in two ways.

- **SPIR-V** (Linux build): run on Vulkan (lavapipe) (`dxc_interop_check`; half is checked with
  `-enable-16bit-types` in `dxc_interop_half_check`).
- **DXIL** (Windows build): run on D3D12 WARP (the software implementation that ships with Windows)
  (`dxil_interop_check` / `dxil_interop_half_check`, `tests/interop/d3d12_runner.cpp`).
  When generating DXIL, DXC folds built-in functions whose inputs are constants, so the inputs are XORed with the
  bits of the output buffer (zero-initialized) to turn them into values that are unknown at compile time
  (`SM_INTEROP_OPAQUE_INPUTS`).

`frexp` / `dst` / .5 in `round` / `sign` on int vectors, which cannot be checked with Slang, are checked in
`tests/interop/dxc/dxc_cases.hlsli`.
The opt-in multi-component matrix access is also checked by compiling `shared/matrix_swizzle_cases.hlsli` with
`SM_INTEROP_MATRIX_SWIZZLE` to SPIR-V with DXC (`dxc_interop_matrix_swizzle_check`).

```bash
# SPIR-V: extract the DXC Linux release (x86_64 only)
cmake -S . -B build-dxc -DCMAKE_BUILD_TYPE=Release \
  -DSHADER_MATH_INTEROP_DXC=ON -DSHADER_MATH_DXC_DIR=<dxc extract dir>
cmake --build build-dxc
ctest --test-dir build-dxc -R dxc_interop --output-on-failure

# DXIL (Windows): extract the DXC Windows release (zip). Use the dxc in bin/x64 or bin/arm64
cmake -S . -B build-dxil -DSHADER_MATH_INTEROP_DXIL=ON -DSHADER_MATH_DXC_DIR=<dxc extract dir>
cmake --build build-dxil --config Release
ctest --test-dir build-dxil -C Release -R dxil_interop --output-on-failure
```

Differences found with DXC:

- **Matrix ctor from a single scalar**: DXC rejects `float3x3(0.25f)` with an error saying there are not enough
  elements. In HLSL, the spellings that broadcast to all components are the cast `(float3x3)0.25f` or the assignment
  `float3x3 m = 0.25f;`. Slang and shader-math also accept the ctor spelling (the shared cases are written with the
  cast).
- **Specular term of `lit`**: Microsoft's documentation for `lit` gives `(n·h)^m`, and Slang and shader-math also use
  a power, but the SPIR-V output of DXC v1.9.2609 (`SpirvEmitter::processIntrinsicLit`) computes `(n·h) * m`. The
  DXIL output is `Exp(Log(n·h) * m)`, which is a power, and execution on WARP also matches shader-math. The `lit` case
  (`case_near_lit`) is excluded from the SPIR-V comparison only.
- **Sign of the `frexp` mantissa (DXIL)**: DXC's lowering to DXIL builds the mantissa as
  `(bits & 0x007fffff) | 0x3f000000`, so the mantissa is positive even for negative inputs (the mantissa of
  `frexp(-7.5)` is 0.9375). The SPIR-V output (GLSL's `Frexp`), Slang, and shader-math return a signed mantissa
  (-0.9375), like C's `frexp`. `case_exact_frexp` is excluded from the DXIL comparison only.
- **`++` / `--` on swizzles and matrix elements (SPIR-V)**: The SPIR-V output of DXC v1.9.2609 generates invalid
  SPIR-V for `v.yz--` and `m._12++` and fails with an error (`OpStore Pointer ... is not a logical pointer`). The
  single-component `v.x++` and the DXIL output have no problem. These forms are checked only in the DXIL comparison
  (`case_exact_step_writes`).
- **Constant folding of `frac` (DXIL)**: The constant folding that DXC performs when generating DXIL turns
  `frac(-0.75)` into 0.75 (`x - floor(x)` gives 0.25). With a run-time input it becomes the DXIL `Frc` instruction,
  and the result on WARP is 0.25, which matches shader-math. So that the comparison observes execution rather than
  folding, the DXIL version makes its inputs opaque (see above).
- **Precision of inverse trigonometric functions on WARP**: WARP's `asin` / `acos` / `atan` (and also `atan2`) differ
  from libm by up to about 1e-3 (sin / cos / exp2 / log2 / pow and others stay within the 1e-5 of `near_`). For the
  DXIL comparison only, the tolerance of `near_inverse_trig` and `near_matrix_intrinsics`, which use these functions,
  is set to relative 2e-3 (`compare.py --tol=`).
- **inf / NaN (DXIL)**: DXC generates DXIL with fast-math (which assumes there is no inf / NaN), so the cases that
  involve inf / NaN (such as `case_exact_inf_edges`) are checked only with Slang and with DXC's SPIR-V output.
- half: without `-enable-16bit-types`, DXC's `half` is a 32-bit float (min precision).
- The DXC version is `dxc_tag` in `ci.yml` (Renovate updates it). DXC releases cannot be changed (they are immutable),
  so CI finds the assets with a regular expression on their names and verifies them against the digest that GitHub
  records (it fails if a release is not immutable).
