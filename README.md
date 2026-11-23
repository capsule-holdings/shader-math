# shader-math

English | [日本語](README.ja.md)

A vector and matrix library compatible with HLSL / GLSL (C++20, header-only, SIMD): float and half vectors and matrices, and int / uint vectors.

The goal is to let you write an expression from a shader in a `.cpp` file with the same meaning. Because `operator*` has opposite meanings in HLSL (component-wise) and GLSL (matrix product), the public API is split into two namespaces.

```cpp
#include <shader_math/shader_math.hpp>

void hlsl_side() {
    using namespace hlsl;              // * is component-wise, the matrix product is mul()
    float3x3 a = {1,2,3, 4,5,6, 7,8,9};  // filled in row-major order
    float3x3 h = a * a;                // Hadamard (component-wise)
    float3x3 p = mul(a, a);            // matrix product
    float3   v = mul(float3(1,2,3), a);  // row vector × matrix
    if (all(h == h)) { /* comparison yields bool3x3; reduce with all()/any() */ }
}

void glsl_side() {
    using namespace glsl;              // * is the matrix product
    mat3 m = {1,4,7, 2,5,8, 3,6,9};    // filled in column-major order
    mat3 id(1.0f);                     // scalar ctor sets the diagonal = identity matrix
    vec3 mv = m * vec3(1,2,3);         // matrix × column vector
    mat3 h  = matrixCompMult(m, m);    // component-wise is a function
    if (m == m) { /* == is a scalar bool */ }
}
```

Do not bring both namespaces into the same scope with `using` (`mul` / `transpose` and others collide).
Operators and each language's functions (`frac`, `mix`, etc.) are found for that language's types even without `using` (ADL).
For this, each namespace has `hlsl::lang_tag` / `glsl::lang_tag`, which represent the language of the types, and they become visible through `using namespace`
(using them in the same scope as your own `lang_tag` makes the name ambiguous).

## Types

- Vectors: `hlsl::float2/3/4` and `glsl::vec2/3/4` (**distinct types**; the contents and the component-wise operations are the same).
  Conversion between them is explicit (`glsl::vec3(f)` / `hlsl::float3(v)`), and mixing them in an operation is a compile error
- Half vectors: `hlsl::half2/3/4` and `glsl::f16vec2/3/4` (also distinct types). The scalars are `hlsl::half` / `glsl::float16_t`
- Integer vectors: `hlsl::int2/3/4` / `uint2/3/4` and `glsl::ivec2/3/4` / `uvec2/3/4` (distinct types; the scalar aliases are
  `uint`, and `dword` in HLSL). Arithmetic wraps, and the cases the languages leave undefined give the same result on every backend:
  `x / 0` and `x % 0` give all bits set (`-1` for int), `INT_MIN / -1` wraps, shift counts are taken modulo 32, and float → int
  truncates with NaN → 0 and saturation ([docs/design/int_vectors.md](docs/design/int_vectors.md))
  - Operators: `+ - * / %` (truncating), `& | ^ ~`, `<< >>` (the count is an int / uint vector of the same width or an integer
    scalar; `>>` is arithmetic on int and logical on uint), comparisons as for float vectors, and in HLSL `!` (true where zero) and
    `scalar << vector`
  - **Each language's implicit conversions and mixed operands**: HLSL converts between int, uint and float either way
    (`float3 f = i;`, `int3 t = a;` truncates); GLSL only widens (int → uint, int / uint → float; `ivec3 t = a;` is an error).
    Mixed operands compute in float when one side is float and in uint for int with uint, in both languages (`int3 * 2.5` and
    `int3 + float3` are `float3`, `int3 + uint3` is `uint3`); a compound assignment keeps the left type, so it needs the implicit
    conversion back (HLSL `int3 += float3` truncates, GLSL `ivec3 += vec3` is an error). In HLSL the floating-point functions
    take int vectors as float (`sqrt(int3)` is a `float3`). bool ↔ int, half ↔ int and the other language stay explicit.
    Constructors convert (`int3(a)`, `int4(i.xy, 2.9f, k)`)
  - Functions: `abs` `sign` (int only) `min` `max` `clamp`; HLSL `dot` `mul(v, v)` `all` `any` `select` `asint` `asuint`
    `asfloat`; GLSL `equal` / `lessThan` / … `mix(x, y, bvec)` `floatBitsToInt` `floatBitsToUint` `intBitsToFloat`
    `uintBitsToFloat`. In GLSL the floating-point functions (`sqrt`, `length`, …) do not take integer vectors
- Half matrices: `hlsl::halfRxC` and `glsl::f16matCxR` (as with float, **distinct types** with explicit conversion; the correspondence of non-square shapes is also the same as for float).
  `determinant` / `inverse` are also computed in half. Real fp16 underflows / overflows early, so do not pass ill-conditioned matrices
  - `sizeof(half3) == 8` (four packed binary16 values). As with `sizeof(float3) == 16`, this is **language compatibility, not GPU ABI compatibility**
  - **Implicit mixed half / float arithmetic is not allowed** (`half3 * float3` / `half3 * 2.0f` are compile errors; conversion is explicit). Constructor arguments are converted (`half3(1.0f, 2.0f, 3.0f)` and `float4(v2, h2)` are allowed)
  - Minimum compilers: gcc/clang need a `_Float16` that works in C++ (gcc 12+ / clang 15+ on x86, gcc 13+ on AArch64). MSVC is supported with a software type that stores the bits. On unsupported compilers the half types are not defined (the float surface is unaffected)
  - On ARM targets, `arm_neon.h` defines a global `float16_t`, so an unqualified `float16_t` under `using namespace glsl` is ambiguous. Qualify it as `glsl::float16_t` or use `hlsl::half`
- Matrices: all 9 shapes with R, C ∈ {2,3,4}. `hlsl::floatRxC` (stored by rows, row-major ctor) and `glsl::matCxR` (stored by columns, column-major ctor) are **distinct types**; conversion between them is explicit
- The correspondence swaps for non-square shapes: `float3x4` (3 rows, 4 columns) ≡ `mat4x3` (4 columns, 3 rows)
- Comparison results: `hlsl::bool2/3/4` and `glsl::bvec2/3/4` (distinct types), and `boolRxC`. No arithmetic and no implicit conversion to bool.
  They can be converted explicitly to and from float vectors (`float3(b)` gives 1 where true; `bool3(v)` / `bvec3(v)` are true where nonzero). There is no implicit conversion
  (GLSL has none; HLSL's implicit conversion is intentionally not included)
- `hlsl::vector<T, N>` (`vector<float, 3>` = `float3`). Using `using namespace std` and `using namespace hlsl` in the same scope
  makes `vector` ambiguous, so qualify it as `std::vector`
  Bool vectors can be created with `bool3(true, false, true)` / `bool3(true)` (the arguments must be bool; they cannot be created from int or float.
  HLSL also allows `bool3 b = true;`).
  Components are read with `b[i]` and the single-component `.x` to `.w` / `.r` to `.a`. A bool or another component can be assigned to `.x` and the like
  (`b.x = true`, `b.x = c.y`, `b[i] = true`; not an int). Writing to multi-component swizzles is not available
- For the subset of types and operations that can be shared with Slang / HLSL, see [docs/slang_compat.md](docs/slang_compat.md)
  (checked in CI with the Slang cross-compilation harness)

## Language semantics compatibility table

Only the main differences are summarized here. The full list of types, constructors, component access, operators and built-in functions (what can be written,
what cannot, and why) is in [docs/compat_mapping.md](docs/compat_mapping.md).

| Item | hlsl:: | glsl:: |
| --- | --- | --- |
| `*` between matrices | component-wise (same shape only) | matrix product (matching inner dimensions enforced by the type) |
| Matrix product | `mul(a, b)` | `a * b` |
| Component-wise product | `a * b` | `matrixCompMult(a, b)` |
| Vector transform | `mul(v, m)` / `mul(m, v)` | `v * m` / `m * v` (same meaning) |
| Matrix ctor from a single scalar | splat to all elements | diagonal (`mat3(1.0)` = identity matrix) |
| Matrix-from-matrix ctor | shrink only (upper-left block) | grow (filled from the identity matrix) / shrink |
| `==` | component-wise bool matrix/vector | scalar bool that is true when all components are equal |
| `<` `>` `<=` `>=` | component-wise bool | not available for vectors (`lessThan()` etc.) |
| Component access | `m[row][col]`, `_m00`, `_11` (multi-element `_m00_m11` is opt-in, see below) | `m[col][row]` |
| swizzle | every combination of `.xyzw` `.rgba` (read and write) | same as hlsl, plus `.stpq` |
| Nested swizzle (`v.xy.yx`) | opt-in (see below) | same as hlsl |
| Scalar → vector | implicit conversion (`float3 v = 1.0f`) | constructor only (`vec3(1.0)`) |
| `.length()` | none | number of components (number of columns for a matrix) |
| Ctor mixing vectors and scalars | exact component count (`float2x2(v2, 3.0, 4.0)`, `float2x2 m = v4`) | only the last argument may have components left over (`vec3(v2, w2)`, `mat2(v4)`, `mat2(v3, v3)`) |
| Unary `+`, `++` / `--` | vectors and matrices | same as hlsl |
| Compound assignment | `%=` is also allowed | `v *= m` (square matrix) is also allowed |

`transpose` / `determinant` / `dot` / `cross` / `length` / `normalize` / `all` / `any` exist in both. `inverse` exists only in GLSL;
for HLSL matrices, use the extension `shader_math::ext::inverse(m)` (features that the language does not have are placed in `shader_math::ext` and are available only
when called qualified). The `hlsl::` / `glsl::` functions take only arguments of their own language (`hlsl::dot(glsl vector)` is an error).

## Restricted by design (undefined operations are compile errors)

- `float3x4 * float4x3` under `hlsl` → error (use `mul`)
- Mismatched inner dimensions `mat3 * mat4x2` under `glsl` → error
- `float3 + float4` → error (size conversion only through an explicit ctor)
- `if (a == b)` (hlsl) → error (reduce with `all()` / `any()`)
- `v.xx = ...` (writing to a swizzle with repeated components) → error
- Implicit conversion between `hlsl::matrix` ⇔ `glsl::matrix` or `hlsl::float3` ⇔ `glsl::vec3`, and operations mixing them → error (explicit only)
- `vec3 v = 1.0;` / `mat2(1.0, 2.0)` under `glsl` → error (write `vec3(1.0)`)

## Standard library integration (opt-in)

Adding `#include <shader_math/std.hpp>` lets you use the types together with the C++ standard library (this is a separate header because `shader_math.hpp`
does not pull in `<ostream>` / `<format>`).

```cpp
#include <shader_math/std.hpp>
using namespace hlsl;

std::cout << float3(1, 2, 3);              // (1, 2, 3). Swizzles (v.zyx) and bool3 can be printed too
std::format("{:.2f}", float2(1, 0.5f));    // (1.00, 0.50). The format spec applies to each component
auto [x, y, z] = v;                        // structured binding; with auto& the components can be modified
auto [row0, row1] = m;                     // matrices bind per m[i] (rows in hlsl, columns in glsl)
std::unordered_map<float3, int> ids;       // std::hash and std::equal_to (value equality over N components)
auto p = shader_math::load<float3>(ptr);   // reads N values from a float*
shader_math::store(m, ptr);                // matrices in constructor order (row-major in hlsl, column-major in glsl)
```

- Matrix output is also laid out per `m[i]`: hlsl prints `((row0), (row1))`, glsl prints `((col0), (col1))`
- `std::format` is available only where `<format>` exists (gcc 13+ etc.). half is printed and hashed as float
- `std::hash` / `std::equal_to` treat -0 and +0 as the same key, and NaN keys never match (the same as float keys)
- `std::tuple_size` and the like are specializations in std, so always include this header in translation units that use structured bindings or `std::hash`

## SIMD

- NEON (AArch64) and SSE2 (x86-64) are first-class. Other architectures (RISC-V, etc.) build by falling back to a scalar 4-wide implementation with the same API
- `-DSHADER_MATH_FORCE_SCALAR=ON` forces scalar. CI runs all tests with both SIMD and scalar
- Boost is used header-only (ISA detection through Config / Predef; Predef 1.60+). If it is not installed,
  or if the system Predef is too old, CMake fetches it automatically

### Sub-features (compile time only, no runtime switching)

- `SHADER_MATH_HAS_FMA` — when FMA3 is detected, `ops::fmadd` becomes `_mm_fmadd_ps` (single rounding).
  Enabled by `__FMA__` on gcc/clang (e.g. `-march=x86-64-v3`) and by `/arch:AVX2` on MSVC
- `SHADER_MATH_HAS_SSE41` — when SSE4.1 is detected, `floor` / `ceil` / `trunc` / `roundEven` / `round` become
  `roundps` (one instruction). Other x86 targets call libm per lane.
  Enabled by `__SSE4_1__` on gcc/clang (e.g. `-march=x86-64-v2`) and by `/arch:AVX` or higher on MSVC. Results are bit-identical
  to libm (including signed zeros and NaN)
- `SHADER_MATH_HAS_F16C` — when F16C is detected (SSE2 backend on x86 only), widening half to f32 and narrowing it back
  each become one instruction (`vcvtph2ps` / `vcvtps2ph`). Enabled by `__F16C__` on gcc/clang (e.g. `-march=x86-64-v3`)
  and by `/arch:AVX2` on MSVC. Results are bit-identical to the plain scalar conversion (round to nearest even)
- `SHADER_MATH_HAS_AVX` — when AVX1 is detected, **and only together with FMA**, the float 4x4 matrix product internally
  packs 2 rows (2 columns in GLSL) into one YMM register (16 fmadds → 8 on YMM).
  The width of the public types does not change (`float4x4` stays four 128-bit registers), and results are bit-identical to the 128-bit path.
  AVX1 without FMA (Sandy/Ivy Bridge) stays at 128-bit. 2x2 / 3x3 / non-square also stay at 128-bit
- `SHADER_MATH_HAS_FP16` — when ARM FEAT_FP16 (`-march=armv8.2-a+fp16`) is detected, half arithmetic runs on a real
  16-bit vector ALU (`float16x4_t`, `fmla v.4h`).
  **On other hosts (x86 / scalar / older ARM), half widens the lanes to f32, computes, and narrows them back**.
  The widening / narrowing conversion is done four lanes at a time, with base instructions on ARMv8.0 (`vcvt_f32_f16` / `vcvt_f16_f32`) and with F16C or
  SSE2 integer operations on x86 (per lane on scalar).
  Rounding on this path matches neither a GPU half ALU nor native ARM (one f32 rounding is added per operation)
- The default baseline (SSE2 / ARMv8.0) is not raised. **Recommended**: `-march=x86-64-v3` (FMA + SSE4.1) on x86,
  and `-march=armv8.2-a+fp16` on ARM if you want faster half
- AVX-512 / ZMM / AVX512-FP16 are not supported (no plans to support them)
- Whether FMA is available changes the rounding of fmadd-type operations (fused = single rounding). The tests absorb this with tolerance comparisons

## Component-wise functions (intrinsics)

Component-wise functions on vectors (float / half). Scalar second / third arguments follow the same rules as operators
(float vectors accept arithmetic scalars, half vectors accept half only).

They also work on scalars (`max(dot(n, l), 0.0)`, `saturate(v.x)`, `lerp(a, b, 0.5f)`). A call with no vector argument and
at least one float / half / one-component swizzle (`v.x`) / matrix element (`m._11`) argument becomes the scalar version,
and the other arguments are converted by the same rules as above (a call with only `2.0` is not covered, so write `2.0f`). Internally it runs the vector version
on one lane, so rounding and edge-case handling are the same as for vectors. `dot` / `length` / `distance`,
`modf` / `frexp` / `sincos` (the output arguments are scalars too), `select(bool, a, b)` and `mix(x, y, bool)` can also be used on scalars.
`isnan` / `isinf` / `isfinite` and the geometric functions `normalize` / `reflect` / `refract` / `faceforward` can also be used on scalars.
HLSL's `all` / `any` also take numeric vectors, matrices and scalars (they test for nonzero; NaN is nonzero). `select` can
choose between vectors with a scalar condition (`select(c, v, w)`). GLSL's `all` / `any` take bool vectors only.
Scalar `round` is the one exception: libc++ / MSVC declare a global `::round(float)` (which rounds .5 away from zero), so
an unqualified `round(x)` under `using namespace hlsl` is ambiguous and fails to compile. Write `hlsl::round(x)`

Component-wise application to matrices is **HLSL only** (HLSL intrinsics also take matrices, but GLSL's generic functions take only
genType = scalar / vector, so `abs(mat3)` under `glsl` is a compile error). The functions covered are
`abs` `sign` `floor` `ceil` `trunc` `round` `frac` `saturate` `sqrt` `rsqrt` `rcp` `exp` `exp2`
`log` `log2` `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` `radians` `degrees`,
`min` `max` `pow` `fmod` (matrix / scalar), `atan2`, `step` (`step(scalar, matrix)` is also allowed),
`lerp` (`t` is a matrix / scalar), `clamp`, `smoothstep`, `mad`, and `isnan` `isinf` `isfinite`
(which return `boolRxC`). Only matrices of the same shape can be combined (`float2x3` and `float3x2` cannot be mixed).

- Both languages: `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc`
  `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` `asin` `acos` `atan`
  `sinh` `cosh` `tanh` `radians` `degrees` `modf` `isnan` `isinf`
- Geometry (both languages): `distance` `reflect` `refract` `faceforward` (like `dot` `cross` `length`
  `normalize`, they return a scalar / vector)
- HLSL only: `lerp` `frac` `rcp` `rsqrt` `saturate` `atan2` `fmod` `mad` `isfinite` `sincos`
  `log10` `ldexp` (float exponent) `frexp` (returns the exponent as float) `select` (HLSL 2021) `dst` `lit`
  (`lit` takes scalar arguments and returns `float4`, as in HLSL), and `%` on float vectors / matrices (same as `fmod`)
- GLSL only: `mix` `fract` `inversesqrt` `roundEven` `atan(y, x)` `mod` `fma` `asinh` `acosh` `atanh`,
  `mix(x, y, bvec)` (y in the components where the selector is true), `frexp` / `ldexp` (the exponent is an int vector) (GLSL has no
  `saturate` / `rcp`, so they are not defined. The same holds in the other direction: names that do not exist in the language are compile errors)
- Not covered: HLSL 2021's
  `and()` / `or()` (they cannot be defined because they have the same names as the C++ alternative tokens `and` / `or`. As in HLSL 2021,
  `&&` / `||` on bool vectors are not available either)

Semantic notes:

- `round` rounds .5 to even (`round(2.5) = 2`, `round(-0.5) = -0`). For HLSL this matches DXC's DXIL (`Round_ne`) and
  SPIR-V (`RoundEven`). GLSL leaves the direction of .5 to the implementation, so `glsl::round` is also the same as `roundEven`
- `sign` returns the same vector type in both languages (HLSL's original int return type is not adopted)
- `min` / `max` return the other operand when one is NaN (IEEE minNum / maxNum; the D3D rule). Therefore `saturate(NaN) = 0` and
  `clamp(NaN, lo, hi) = lo`. This is the same on every backend (GLSL leaves it undefined, so this behavior is within the spec)
- `rcp` / `rsqrt` / `inversesqrt` are exact versions that do not use the approximation instructions (`rcpps` / `rsqrtps`)
- Transcendental functions call libm per lane (no SIMD polynomial approximation). `pow(negative, non-integer)` is NaN
- `select(c, a, b)` gives a in the components where c is true; `mix(x, y, bvec)` gives y where true (the argument order is reversed)
- `lerp` is `x + s*(y - x)` (the same as DXC's DXIL), and `mix` is `x*(1 - a) + y*a` (GLSL's defining formula; `mix(x, y, 1) = y` is
  exact regardless of the magnitudes of x and y). They differ only in rounding
- `hlsl::fmod` and `glsl::mod` **have different meanings**: `fmod` is based on trunc and the result has the sign of x
  (`fmod(-7, 3) = -1`); `mod` is based on floor and the result has the sign of y (`mod(-7, 3) = 2`)
- The two-argument arctangent is `atan2(y, x)` in HLSL and `atan(y, x)` in GLSL. In both, y comes first
- `mad` / `fma` are fused (single rounding) if the backend has FMA, and are a multiply plus an add otherwise
  (neither language guarantees fusion unless `precise` is specified)
- `refract` returns the zero vector on total internal reflection (k < 0). `reflect` / `refract` assume the normal is normalized
- `modf(x, ip)` returns the fractional part, with the same sign as x, and stores the integer part in `ip`. For ±inf the fractional part is ±0
- `isnan` / `isinf` / `isfinite` return `bool2/3/4` (under `-ffast-math`, NaN checks may be optimized away)
- When using `<windows.h>` on Windows, `NOMINMAX` is required (it collides with the `min` / `max` macros)

## Migrating from 0.5

0.6 contains incompatible changes (during 0.x, only the same minor version is compatible).

- `hlsl::float3` and `glsl::vec3` (and the half and bool vectors) became distinct types. Where they were mixed, convert explicitly
  (`glsl::vec3(f)`). `detail::fvec<N>` / `detail::bvec<N>` are now `fvec<N, T, Lg>` / `bvec<N, Lg>`
- GLSL no longer converts a scalar to a vector implicitly (`vec3 v = 1.0;` becomes `vec3 v = vec3(1.0);`)
- `.stpq` always exists on GLSL vectors and does not exist on HLSL vectors. The CMake option `SHADER_MATH_GLSL_STPQ` has been removed
- `hlsl::inverse` moved to `shader_math::ext::inverse` (HLSL has no `inverse`)
- The `hlsl::` / `glsl::` functions take only arguments of their own language (`hlsl::dot(glsl vector)` is an error). Operators and each language's
  functions are found by ADL without `using`

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/sandbox                                        # playground
cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON  # force scalar
```

`-DSHADER_MATH_WARNINGS_AS_ERRORS=ON` turns warnings in the tests and samples (`-Wall -Wextra -Wpedantic` / `/W4`) into errors (always ON in CI).

### Using it from other projects (CMake)

The target is `shader_math::shader_math` with every method.

```cmake
# A) Subproject (add_subdirectory / FetchContent)
include(FetchContent)
FetchContent_Declare(shader_math
    GIT_REPOSITORY https://github.com/capsule-holdings/shader-math.git
    GIT_TAG v0.6.0)
FetchContent_MakeAvailable(shader_math)

# B) Installed package
#    point CMAKE_PREFIX_PATH at what cmake --install build --prefix <prefix> installed
find_package(shader_math 0.6 REQUIRED CONFIG)

target_link_libraries(app PRIVATE shader_math::shader_math)
```

- When it is included as a subproject, no test, sample or install rules are generated
  (this can be overridden with `SHADER_MATH_BUILD_TESTS` / `SHADER_MATH_INSTALL`)
- Boost (the Config / Predef headers) is not bundled with the package. It is searched for on the consumer side at `find_package` time;
  if it is not found, set `SHADER_MATH_BOOST_INCLUDE` (and also
  `SHADER_MATH_BOOST_PREDEF_INCLUDE` if Predef is in a different location)
- Forcing scalar: `-DSHADER_MATH_FORCE_SCALAR=ON` for a subproject; for an installed package,
  `set(SHADER_MATH_FORCE_SCALAR ON)` before `find_package`. In both cases the macro is attached to `shader_math::shader_math`,
  so it is consistent across all targets that link it (mixing SIMD and scalar translation units is an ODR violation).
  `SHADER_MATH_NESTED_SWIZZLE` / `SHADER_MATH_MATRIX_SWIZZLE(3)` are enabled the same way
- pkg-config: `<prefix>/share/pkgconfig/shader_math.pc` (`pkg-config --cflags shader_math`). Boost is
  included only when the package was installed using the system Boost (an automatically fetched Boost is not installed)
- During 0.x, versions are compatible only within the same minor version (`SameMinorVersion`)

Benchmarks: `./build/shader_math_bench` (SIMD) and `./build/shader_math_bench_scalar`
(scalar forced) compare ns / element for the main kernels (`--markdown` for table output, `--json` for
the github-action-benchmark format). CI runs them on `linux-x64-gcc` / `linux-x64-gcc-v3` / `linux-arm-gcc`
and shows the results in the job summary. The values from each push to `main` are accumulated on the `gh-pages` branch, and a graph of the trend
is published at <https://capsule-holdings.github.io/shader-math/bench/> (the values come from shared runners, so they are for reference only;
there is no pass/fail judgment).
Header compile time: `python3 bench/compile_time.py <compiler> [--markdown]` measures the front-end time of a translation unit that only
includes the library, for the default and for each opt-in macro. CI runs it on
`linux-x64-gcc` / `linux-x64-clang` and shows the results in the job summary (also for reference only).
Conformance to HLSL / GLSL: `python3 tests/conformance/run.py --cxx <compiler>` measures, for shader statements pasted as-is
into C++, the rate of agreement with DXC / glslang on what is accepted and what is rejected (not run in CI).
See [docs/conformance.md](docs/conformance.md) for details.

Demo: `-DSHADER_MATH_DEMO_SDLGPU=ON` builds a sample with SDL_GPU + FBX (ufbx) + CPU skinning
([examples/sdl_gpu_skinning/](examples/sdl_gpu_skinning/README.md)).

Requirements: C++20 (concepts). gcc 10+ / clang 14+ / MSVC 19.30+ / Apple Clang.
gcc 10 and clang 14 are checked in CI as the minimum versions (clang 11 to 12 hit internal compiler errors, and 13 produces false warnings).
MSVC and Apple Clang are checked only at the CI runners' default versions.

## Known deviations (differences from HLSL/GLSL)

- **Multi-element matrix swizzles** (`_m00_m11` etc.) are **opt-in**. The single-element `_m00` to `_m33` / `_11` to `_44` are
  all supported by default (out-of-range ones are a compile error when used). `SHADER_MATH_MATRIX_SWIZZLE` enables 2-element ones
  (`m._m00_m11`, `m._41_42`), and `SHADER_MATH_MATRIX_SWIZZLE3` also enables 3-element ones (`m._m30_m31_m32`).
  Both are limited to the range of the matrix's shape, and those without repeated elements are writable. There are no 4-element ones (write `m[3]` for a row, and
  `float4(m._m00, m._m11, m._m22, m._m33)` otherwise). Because the member declarations multiply combinatorially,
  in `bench/compile_time.py` (a translation unit that uses every vector and matrix type of both languages) 2-element costs about 1.3x, and 3-element costs
  about 6.7x on gcc and about 4.5x on clang
- **Nested swizzles** (`v.xy.yx`, `v.xyz.zx`) are **opt-in**: defining `SHADER_MATH_NESTED_SWIZZLE`
  enables reads only, on 2- and 3-component swizzles (also `.stpq` in GLSL). Nesting on a 4-component swizzle
  (`v.wzyx.xy`) and writing to a nested swizzle are not available (write `float4(v.wzyx).xy`). Because the member declarations
  multiply combinatorially, enabling it makes the front-end time of a translation unit that uses every vector type about 3.2x
  (measured with `bench/compile_time.py`: 3.2x on both gcc and clang). This is why it is disabled by default
- GLSL's `not()` is `not_()` because `not` is a C++ alternative token (for `!`)
- `v.x` is a proxy type (when passing it directly to variadic arguments, write `float(v.x)`)
- The covered types are float and half (vectors and matrices) and int / uint (vectors). Integer matrices, 16- / 64-bit integers, `double`,
  `1xN` matrices, the bit-manipulation and packing functions (`countbits`, `bitCount`, `packHalf2x16`, …) and GPU-only features (textures, derivatives, atomics) are out of scope
- Language-compatible, not memory-ABI-compatible (`sizeof(float3) == 16`; this is unrelated to constant buffer packing)
- Component access and swizzles are implemented with type punning through a union with the SIMD register.
  This is undefined behavior in ISO C++ and depends on how gcc / clang / MSVC handle it in practice (on gcc / clang the proxy types
  are marked `may_alias`; verified in every CI configuration).
  For this reason the types cannot be used in `constexpr`. See [CONTEXT.md](CONTEXT.md) for details
- `floatN` (N < 4) and matrix rows / columns are stored in 4-lane registers, and **the values of lanes N and above are unspecified**
  (they may contain NaN / Inf as a result of scalar operations or division). Results are always determined only by the valid N lanes.
  Shader languages have no 4th component either, so there is no semantic difference, but in environments that trap floating-point exceptions
  (`feenableexcept` etc.), these unspecified lanes can raise exceptions

## Cross-checking against GLSL

The `glsl` surface is checked against the results of compiling the same expressions to SPIR-V with glslang and running them on Vulkan (lavapipe)
(column-major ctor, `mat3(1.0)` = identity matrix, `*` = matrix product, floor-based `mod`, etc.).
See [docs/glsl_interop.md](docs/glsl_interop.md) for how to run it and what it covers.

## Cross-checking against DXC

In addition to Slang, the HLSL surface is also checked with DXC, the official HLSL compiler, in two ways: compiling to SPIR-V with DXC and running on lavapipe,
and compiling to DXIL on Windows and running on D3D12 WARP. `frexp` / `dst`, which cannot be checked with Slang,
are also checked here. See the DXC section of [docs/slang_compat.md](docs/slang_compat.md) for details.

## Cross-checking against Slang

A harness in `tests/interop/` builds and runs the same expression source with both Slang (`slangc -target cpp`, run on the CPU) and shader-math
and compares the numeric results. For the table of the shareable subset,
exclusions and semantic differences, see [docs/slang_compat.md](docs/slang_compat.md).

```bash
cmake -S . -B build-interop -DCMAKE_BUILD_TYPE=Release \
  -DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<extracted-slang-release-dir>
cmake --build build-interop
ctest --test-dir build-interop -R slang_interop --output-on-failure
```

## License

[MIT](LICENSE)
