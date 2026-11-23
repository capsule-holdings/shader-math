# shader-math development context

A record of the research and design decisions made during the planning phase. This document exists to trace the background of the implementation.
Created: 2026-08-20

## Origin and evolution of the requirements

1. Initial request: "A library identical to CUDA (HLSL) matrix math. float3x3 to float4x4. Fully compatible. Inline expansion with Boost + SIMD × templates. Something that compiles in a .cpp file."
2. Research findings: CUDA Runtime / `vector_types.h` has no matrix types. What "CUDA (HLSL) matrices" actually refers to is the matrix language specification of Microsoft HLSL.
3. Updated requirement: "I want HLSL×GLSL-compatible floatNxM SIMD with no Windows dependency" → this led to the dual-namespace design.
4. Scope expansion: float2 / 2x2 added → all 9 shapes with R,C ∈ {2,3,4} → arbitrary swizzles also in v1 (holding to full compatibility).
5. Toolchain: no accommodation is made for old environments. C++20 concepts are required (no fallback layer).

## Key design decisions

### Dual namespaces (most important)

HLSL and GLSL give `operator*` opposite meanings:

- HLSL: `mat1 * mat2` is **component-wise** (Hadamard). Matrix multiplication is `mul()`.
- GLSL: `mat1 * mat2` is **matrix multiplication**. Component-wise multiplication is `matrixCompMult()`.

→ A single `*` cannot be compatible with both languages. The library is split into `shader_math::hlsl` / `shader_math::glsl`,
and each translation unit applies `using namespace` to only one of them. The global `namespace hlsl = ...;`
alias can be suppressed with `SHADER_MATH_NO_ALIAS`.

**Rejected alternative: switching the behavior of `*` with a build setting** — the meaning could no longer be determined by reading the code,
mixing TUs built with different settings would cause ODR violations, and the setting could propagate across dependencies by accident. What "wanting to change it with a setting" really means is
"wanting to write expressions in either language", and namespaces express the same thing through the type system.
The only knob allowed to change behavior is FORCE_SCALAR (results are identical; only speed differs).

### Why existing libraries were not used

- [hlslpp](https://github.com/redorav/hlslpp): the best existing implementation in terms of HLSL compatibility (the strongest competitor). It has no GLSL side.
- [NVIDIA MathLib](https://github.com/NVIDIA-RTX/MathLib): HLSL-compatible + SIMD, but float3x3 is not implemented.
- [DirectXMath](https://github.com/Microsoft/DirectXMath): `XMMATRIX::operator*` is matrix multiplication, the opposite of HLSL.
  The only first-class type is 4x4. **Not suitable for wrapping** (opposite language contract, no GLSL support, oriented toward Win/D3D).
- [GLM](https://github.com/g-truc/glm): GLSL only. Incompatible with HLSL's `*`.
- Boost.QVM: `*` is matrix multiplication, and there is no SIMD. The former Boost.SIMD is no longer maintained.

The reason for writing our own library is that "expressions in both languages can be written on top of a single SIMD core".

### Type correspondence (pitfalls found in compatibility audits)

Incompatibilities found in audits that cross-checked against the official specifications of both languages (performed twice):

1. **Comparison operators have opposite meanings in the two languages** (the same kind of pitfall as `*`): HLSL `==` yields a component-wise bool matrix
   (it cannot be used in `if`; reduce it with `all()`/`any()`), while GLSL `==` yields a scalar bool (component-wise comparison is the `equal()` family).
2. **The non-square type correspondence is swapped**: GLSL `matCxR` is C columns × R rows → `float3x4` (3 rows, 4 columns) ≡ `mat4x3` (4 columns, 3 rows).
3. **Scalar ctors have different meanings**: HLSL `float3x3(1)` = splat to all components / GLSL `mat3(1)` = identity matrix (diagonal).
4. **Matrix-to-matrix ctors**: GLSL fills with the identity matrix when enlarging and takes the upper-left part when shrinking / HLSL only has truncating casts.
5. **GLSL `<` `>` are scalar-only**: they are not defined for vectors (defining them would be over-compatible).
6. vec×mat has the same meaning in both languages (GLSL `v*m` = HLSL `mul(v,m)`) → the `mul` implementation can be shared.

### How the types are split

- **One template for vectors, separate types per language**: the `Lg` in `detail::fvec<N, T, Lg>` is the language tag
  (`hlsl::lang_tag` / `glsl::lang_tag`). `hlsl::float3` = `fvec<3, float, hlsl::lang_tag>` and
  `glsl::vec3` = `fvec<3, float, glsl::lang_tag>` are distinct types. The implementation of the operations (`+` / `dot` etc. in `detail`) is
  shared, and the result has the same language type as the arguments. Operations across languages are errors, and conversions are explicit.
- What differs by language: the swizzle spellings placed in the same union as the register (only GLSL has `.stpq`), implicit conversion
  from scalars (HLSL allows `float3 v = 1.0f`; GLSL allows only the ctor), and GLSL's `.length()`. Union
  members cannot be added in a derived class, so the base `fvec_store<N, T, Lg>` that holds the union is specialized per language.
- **Rejected alternative: sharing one type for vectors**. Things that exist in only one language
  (`.stpq`, `.length()`, the ban on implicit scalar conversion in GLSL) would either leak into both or disappear from both.
  Comparison operators also could not determine the language from the type, and their meaning would change depending on whether a using-directive is present.
- **Rejected alternative: narrowing to a single language with a macro**. The class definition changes, so unless every translation unit agrees
  it is an ODR violation, and both languages can no longer be used in one program.
- Compile time: most of the front-end time is the instantiation of the swizzle proxy types (one type per combination of selected lanes; the same combination in `xyzw` /
  `rgba` / `stpq` is the same type). A vector of width K declares only the combinations of its own K letters
  (width 2: 30 types, 3: 120 types, 4: 340 types). Declaring 4-letter combinations for every width and making out-of-range ones an error at the point of use
  would result in 1020 types per element type per language, and a translation unit that uses all types of both languages would be about 1.5 times slower.
  Including the header alone does not instantiate any vectors (no vectors are created in the bodies of non-template functions). The copy operations of `fvec_store` are user-defined. If they were left implicit,
  the compiler would go and resolve the copy operations of every swizzle member of the union, which increases front-end time.
- **Matrices are separate types**: `hlsl::matrix<R,C>` (row storage, row-major ctor) and `glsl::matrix<C,R>` (column storage, column-major ctor).
  `operator*` and the ctor order differ, so they cannot be the same type. Conversion between them is explicit.
- The glsl side uses column-register storage (internally transposed) so that `m[col]` reads a register directly.
  Sharing row storage + gather is slow, so it was rejected.
- `bool2/3/4` / `boolRxC` are types used only for comparison results (mask registers). No arithmetic and no implicit conversion to bool.

### Component access through unions (an implementation-dependent assumption)

`fvec` is `union { reg_t q_; T comp_[4]; swz<...> x, xy, ...; }` and `hlsl::matrix` is
`union { fvec row_[R]; T flat_[R*4]; mswz<...> _m00, ...; }`, so the SIMD register, the component array, and the
swizzle proxies share the same storage (16 bytes per vector or matrix row). Operations write to `q_`, while `v[i]` / `v.x` / `v.xy = ...` read and write
through `comp_` / the proxy's `d[]`.

- **This is undefined behavior in ISO C++** (type punning that reads a union member other than the one last written).
- What gcc guarantees is only access through a union member expression. Proxy reads and writes go through the proxy type's
  `this->d[k]`, so they are outside that guarantee. gcc's type-based alias analysis compares the base types of the accesses
  (`fvec` and `mswz` / `swz`), and since neither contains the other, it treats them as distinct objects.
  In practice, `float2x2 m = a * b; return m._22;` forward-propagates the zero initialization of `row_{}` and returns 0
  (gcc 10 to 14, x86 / AArch64 / scalar backends, the same at -O2 and above and with LTO; clang does not reproduce it).
  Adding `may_alias` to the register type (x86's `__m128` already has it) does not change the base-type comparison,
  so it does not prevent this.
- Countermeasure: `swz` / `mswz` carry `SHADER_MATH_MAY_ALIAS` (`__attribute__((__may_alias__))` on gcc / clang),
  so that access through proxies is treated as possibly aliasing any type. `comp_` / `flat_` are accessed through
  union member expressions, so they are not covered. MSVC does not perform type-based alias analysis, so the macro is defined as empty.
  The benchmark difference (NEON / gcc 13) is within measurement error (within ±2%).
- Other major compilers: clang also treats type punning through unions the same way as gcc. The scalar backend's
  register is a struct that holds `float[4]`.
- Consequence: `constexpr` is not possible (constant evaluation rejects reads of inactive members).
  UBSan does not detect this kind of UB, so the ASan/UBSan job is not a guarantee.
- Assurance: all tests on all CI configurations (gcc / clang / MSVC / Apple Clang, -O3 and Debug).
  The union access check in `test_vector.cpp` alternates register writes with component / swizzle reads and writes
  in a loop inside a noinline function and confirms that the result matches plain scalar computation
  (if alias analysis reorders the reads and writes, the results do not match).
  `test_proxy_alias.cpp` contains, in noinline functions, the patterns in which the miscompilation above occurs (reading with `_mRC` immediately after per-row register writes,
  and register operations immediately after a proxy write), and it fails on the gcc configurations
  (linux-arm-gcc / linux-arm-gcc10-min / the linux-x64-gcc family / scalar) if `may_alias` is removed.
- Rejected alternatives: proxies that hold a reference to the parent vector (`sizeof` grows, and `v.x` cannot be held by value),
  accessor functions (they become `v.x()`, so HLSL / GLSL expressions cannot be written as is).
  To be reconsidered if this assumption breaks with a newer compiler.

### ADL and namespace placement

- Operations common to both languages (vector `+`, `dot`, etc.) → `detail` (the namespace where fvec is actually defined; found by ADL).
- Operations whose meaning differs by language (matrix `*`, comparisons) and language-specific names (`frac`, `mix`, etc.) → each language namespace.
  The language tag is a class in that namespace, so these are found by ADL even for vectors (no using-directive needed).
  They are not found for vectors of the other language, and even a qualified call is rejected by the constraints.
- Functions common to both languages are also placed in each language namespace, not with `using detail::X` but as forwarding templates that take only arguments of their own language
  (`SHADER_MATH_LANG_FORWARD`). With `using`, `hlsl::dot(a glsl vector)` would compile.
  `isnan` / `isinf` / `round` / `all` / `any` have explicit signatures because of interactions with MSVC UCRT's `isnan(_Ty)` and `::round(float)`.
  The scalar `round` / `isnan` / `isinf` are placed only in the language namespaces (if `detail` also had the same ones,
  a call that passes a component `v.x` would be ambiguous under ADL).
- Features that the languages do not have (such as an HLSL version of `inverse`) are in `shader_math::ext`. It is not an associated namespace of any type, so these functions are not
  found by ADL, and writing them as in a shader (`inverse(m)`) does not compile.
- Placing `hlsl` / `glsl` directly at the top level was rejected (risk of collisions, e.g. DirectXShaderCompiler uses an `hlsl` namespace).
  Resolved by placing them under the root `shader_math` + global aliases.

### Restriction design (undefined operations are compile errors)

A three-layer approach: concepts + not declaring the overloads + the equivalent of `= delete`:

- `float3x4 * float4x3` in `hlsl` → error (same shape only; matrix multiplication is `mul`)
- Inner-dimension mismatch in `glsl` (`mat3 * mat4x2`) → no matching signature
- `float3 + float4` → error (only scalars are broadcast)
- `if (a == b)` (hlsl) → error, because bvec does not convert to bool (as in the language specification)
- Duplicate swizzle write `v.xx = ...` → fails the requires clause
- No implicit conversion between hlsl and glsl matrices
- Tests verify "does not compile" itself with requires expressions + static_assert

### SIMD / toolchain

- NEON (AArch64) and SSE2 (x86-64) are first-class. Unknown architectures (RISC-V, etc.) fall back to scalar 4-wide
  and can still be built. RVV support is possible, but CI coverage would be thin, so it was deferred from the first release.
- PS5 = prospero-clang (Clang-based, x86-64/SSE2), Switch = GCC-based (AArch64/NEON).
  The requirements are gcc 10+ / clang 14+ / MSVC 19.30+ / Apple Clang. gcc 10 and clang 14 are verified by the CI minimum-version jobs
  (clang 11 to 12 hit internal compiler errors, and 13 emits false warnings). MSVC and Apple Clang are tested only with the runner's default version.
- x86 sub-features (compile time only; the default baseline of SSE2 is not raised): FMA3, SSE4.1 (`roundps`),
  F16C (half ⇔ f32 conversion in a single instruction), AVX1 (packing two rows for the 4x4 product, only when combined with FMA). MSVC does not have the GCC-style
  macros, so detection uses `__AVX2__` → FMA / F16C and `__AVX__` → SSE4.1 / AVX. On ARM, FEAT_FP16 is used to compute half in
  native 16-bit lanes.
- Constraints are written only with core-language `requires` clauses, without depending on the `<concepts>` library header.
- Boost is used header-only (Config = FORCEINLINE, Predef = ISA detection). It is not a polyfill for concepts.
- scalar is not given reduced coverage: the same full test suite runs with both SIMD and FORCE_SCALAR.
  Operations that involve rounding (the fmadd family) use approximate comparison; operations without rounding (Hadamard, transpose, ctor) require an exact match.

### CI

The job list in `.github/workflows/ci.yml` is authoritative (only the categories are described here). `fail-fast: false`, and
all jobs use `-DSHADER_MATH_WARNINGS_AS_ERRORS=ON`. push triggers only on `main`; branches are verified by a single
pull_request run, and an additional push to the same PR cancels the older run. Changes that touch only `*.md` and `docs/`
do not run CI (`paths-ignore`; `release.yml` accepts a commit that has no CI run, as long as the diff from the
nearest ancestor that passed CI contains only documentation).

- Backends: Linux x64 (SSE2 / x86-64-v2 = SSE4.1 / ivybridge = AVX+F16C without FMA /
  x86-64-v3 = FMA+AVX), Linux ARM
  (NEON / armv8.2-a+fp16), FORCE_SCALAR (x64 / ARM)
- Compilers: g++ / clang++, MSVC (SSE2 / `/arch:AVX2` / ARM64), Apple Clang (macOS arm64),
  minimum versions (gcc 10 x64 / ARM, clang 14)
- Quality: Debug ASan/UBSan, codegen checks (disassembly is used to confirm fmadd / roundps / inlining),
  consumer-side validation of the CMake package (install + `find_package` / `add_subdirectory`)
- Other: Slang cross-validation (pinned version + sha256; regenerating the committed SPIR-V must produce identical output), GLSL cross-validation
  (glslang → SPIR-V → lavapipe), DXC cross-validation (SPIR-V → lavapipe, and DXIL → D3D12 WARP on Windows), headless rendering of the SDL_GPU demo
  on lavapipe, benchmarks (reference values are written to the job summary, with no pass/fail; on pushes to `main`,
  the `publish-bench` job accumulates history in `bench/` on `gh-pages`, and the graphs are published on GitHub Pages)
- `release.yml` is a separate workflow (runs only when a `vX.Y.Z` tag is pushed; see "Versioning and releases")

The runners are Blacksmith by default (prerequisite: the Blacksmith app is installed in the org).
macOS and Windows ARM64 are not available on Blacksmith, so they use GitHub-hosted runners (free because the repository is public).

### Versioning and releases

- The version is recorded in two places: `project(shader_math VERSION x.y.z)` and `include/shader_math/version.hpp`
  (`SHADER_MATH_VERSION_MAJOR/MINOR/PATCH`, for users who do not use CMake).
  If they disagree, CMake configure stops with an error.
- While the version is 0.x, only the same minor version is compatible (`SameMinorVersion` in `write_basic_package_version_file`).
- Procedure: merge the version-bump PR into `main` → wait for ci (push) on that `main` to pass →
  push the `vX.Y.Z` tag. `.github/workflows/release.yml` checks that the tag matches the version in both places, that the tag is
  on `main`, and that ci succeeded for that commit, and then generates release notes from the merged PRs
  and creates a GitHub Release. If the job fails while waiting for ci, re-run it after ci completes.

## Known deviations (intentional)

- Multi-component matrix swizzles (`_m00_m11`) are not implemented. All single components `_m00` to `_44` are supported
  (multi-component swizzles would need up to 16⁴ member declarations, a combinatorial explosion. They are also rarely used in real shaders).
- GLSL `not()` → `not_()` (C++ keyword)
- `v.x` is a proxy type (use `float(v.x)` for varargs)
- The scope is float and half vectors (half is native only on ARM FEAT_FP16; elsewhere it is promoted to f32).
  `int` / `double`, 1xN, and GPU-only features (textures, derivatives, atomics) are out of scope.
- `%` is an HLSL-only operation (GLSL float does not have it) → out of scope. `++`/`--` are also out of scope.
- The library provides language compatibility, not memory ABI compatibility (`sizeof(float3) == 16`; constant buffer packing is a separate matter).

## v2 candidates

- **Implemented**: inverse trigonometric / hyperbolic / geometric (`distance` `reflect` `refract` `faceforward`) /
  `atan2` (GLSL `atan(y, x)`) / `fmod` (HLSL, trunc-based) and `mod` (GLSL, floor-based) /
  `mad` and `fma` / `isnan` `isinf` `isfinite` / `modf` / `sincos` / `radians` `degrees`.
  `fmod` / `mod` differ in meaning in the same way as `*`, so they are separated by language, and the other language's name is a compile error.
- **Implemented**: component-wise functions (the 23 functions from `abs` to `tan` + `roundEven`; for vectors;
  language-specific names: HLSL `lerp`/`frac`/`rcp`/`rsqrt`/`saturate` ⇔ GLSL `mix`/`fract`/`inversesqrt`.
  Transcendental functions use per-lane libm; rounding is per-lane only on SSE2 and native on the other backends).
  The two-row register packing optimization (4x4 with AVX1+FMA3).
- **Implemented**: component-wise intrinsics for matrices (HLSL only; `abs(m)` `lerp(a, b, t)` `isnan(m)` etc.
  The vector version is applied to each row). GLSL's generic functions take only genType, so they are not provided on the `glsl` side.
- **Implemented**: intrinsics on scalars (`max(dot(n, l), 0.0)`). A call that has no vector argument and includes a float / half / single-component
  swizzle runs the vector version on one lane and returns the result (`detail::ScalarCall`). These are constrained templates,
  so they do not collide with the C functions of the same name that are visible under `using namespace hlsl`, and `abs(-2.5f)` does not turn into C's int `abs`.
- **Not implemented**: RVV backend.

## Slang cross-validation harness (added 2026-08-20)

The same `tests/interop/shared/math_cases.hlsli` is built and run both with Slang (`slangc -target cpp` → g++,
executed on the CPU) and with shader-math (`cpp_harness.cpp`), and
`tests/interop/compare.py` compares the results with tiered tolerances (`exact_` = bit-exact / `near_` = relative 1e-5 /
`half_` = relative 2e-3). The dual compile itself serves as a lint for the subset.
How to run: configure with `-DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<extracted Slang directory>`
and run `ctest -R slang_interop`. In CI, the `linux-x64-slang-interop` job
downloads the Slang version pinned by `slang_tag` in `ci.yml` and runs it. Renovate updates `slang_tag` and
`slang_sha256` (the sha256 of the Linux asset) together (after waiting 14 days).

Measurement record (Slang v2026.16; the same with v2026.20): the C++ prelude of the CPU target does not implement half vector operations
(`Vector<_Float16, 4>` has no `operator*` etc.), so the half cases are kept in the shared file
behind the `SM_INTEROP_ENABLE_HALF` guard but excluded from comparison, and deferred to the SPIR-V execution
phase (lavapipe). The subset definition, exclusions, and semantic differences are documented in `docs/slang_compat.md`.

## Verification record (2026-08-20, Cloud Agent VM)

The same suite of 397 checks was run in 5 configurations, all with 0 failures:

| Configuration | Backend |
| --- | --- |
| g++ 13 (x64) | SSE2 |
| clang++ 18 (x64) | SSE2 |
| g++ 13 scalar twin | scalar |
| g++ 13 FORCE_SCALAR configuration | scalar |
| aarch64-linux-gnu-g++ 13 + qemu-aarch64 | NEON (real SIMD) |

Bug encountered during implementation: the swizzle generation macro's parameter names `a`/`b` collided with the letter tokens of rgba
(`p##a` was replaced with the parameter value) → resolved by renaming the parameters to `i0/i1/i2`.
