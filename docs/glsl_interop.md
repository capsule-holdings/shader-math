# GLSL ⇔ shader-math cross-checking

The same math source (`tests/interop/glsl/math_cases.glsl`) is
(a) compiled to SPIR-V with glslang and run on Vulkan (in CI, lavapipe = llvmpipe), and
(b) built and run as C++ (the `glsl` surface of shader-math), and the numeric results are compared.
This is the GLSL counterpart of the Slang cross-checking on the HLSL side ([slang_compat.md](slang_compat.md)).

## How to run

```bash
# Ubuntu: glslang-tools libvulkan-dev mesa-vulkan-drivers python3
cmake -S . -B build-glsl -DCMAKE_BUILD_TYPE=Release -DSHADER_MATH_INTEROP_GLSL=ON
cmake --build build-glsl
ctest --test-dir build-glsl -R glsl_interop --output-on-failure
```

CI runs this in the `linux-x64-glsl-interop` job.

## GLSL-specific semantics that are checked

| Case | What it checks |
| --- | --- |
| `exact_ctor_layout` | Matrix ctors are column-major; `matCxR` is C columns × R rows (`mat2x3` / `mat3x2`) |
| `exact_ctor_special` | `mat3(s)` is diagonal, `mat4(mat2)` extends with the identity matrix, `mat2(mat3)` takes the upper-left part, and vector concatenation ctors |
| `exact_componentwise` | `matrixCompMult`, non-square `transpose`, `outerProduct`, and component-wise functions |
| `exact_rounding` | `roundEven` (.5 rounds to even), `trunc` `ceil` `min` `max` `clamp` `step` |
| `exact_compare` | `==` `!=` on vectors / matrices yield a scalar bool; `lessThan` `equal` `greaterThanEqual` + `all` `any` |
| `exact_mix_ends` | `mix` is `x*(1 - a) + y*a` (the defining formula in GLSL), so `a = 0` / `a = 1` return x / y exactly (even when x is orders of magnitude larger than y) |
| `near_matrix_product` | `*` is the matrix product, `v * m` is the row-vector form, and non-square products (`mat3x2 * mat2x3`) |
| `near_inverse` | `inverse`, `determinant` |
| `near_mod` | `mod` is floor-based and takes the sign of y (the opposite of HLSL's `fmod`); `mix`, `modf`, `fma` |
| `near_geometry` | `reflect` `refract` `faceforward` `distance` `inversesqrt` `smoothstep` |
| `near_edges` | `mod` with a negative dividend / divisor, `step` at an equal boundary (it returns 1), `fract(-0)` |
| `bits_int_*` / `bits_uint_wrap` | `ivec` / `uvec` arithmetic (wrapping on uint), `/` and `%` (non-negative operands for `%`), bitwise operators and shifts (vector and scalar counts, arithmetic `>>` on int), comparison functions and `mix(ivec, ivec, bvec)`, conversions to and from float, `floatBitsToUint`, `frexp` / `ldexp` with int exponents |
| `approx_transcend` | `atan(y, x)`, inverse trigonometric, trigonometric, exponential, and logarithmic functions, `pow`, `radians` `degrees` |

## half (`f16vec` / `f16mat`)

`kernels_half.comp` (`GL_EXT_shader_explicit_arithmetic_types_float16`) and `half_cases.glsl` check `f16vec`
arithmetic, `min` `abs` `clamp` `mix` `dot`, and `f16mat2 * f16vec2`
(`glsl_interop_half_check`). This requires `shaderFloat16` on the device; without it the check is skipped (exit code 77).
The shared files do not use `float16_t` (on ARM it is ambiguous with the global `float16_t` from `arm_neon.h`).

## `.stpq`

`.stpq` exists only on GLSL vectors (HLSL does not have it). `stpq_cases.glsl` (`kernels_stpq.comp`) is checked
against a dedicated C++ side (`glsl_interop_stpq_check`).

## Nested swizzles (opt-in)

Nested swizzles on 2- and 3-component swizzles (`a.zw.yx`, `b.stp.ps`) are also enabled only when
`SHADER_MATH_NESTED_SWIZZLE` is defined, so `nested_cases.glsl` (`kernels_nested.comp`) is checked against a C++ side
built with this macro (`glsl_interop_nested_check`).

## Tolerances (determined by the name prefix)

- `exact_`: bit-exact. Only operations with a single rounding are placed here (GLSL compilers may fuse `a * b + c`,
  and the precision of division and transcendental functions is also relaxed).
- `near_`: relative 1e-5 (multiply-add and division).
- `approx_`: relative 1e-4 (transcendental functions; the lavapipe implementation differs from libm by about 1e-5).
- `half_`: relative 2e-3 (the binary16 ulp, plus rounding in the path that widens to f32 for the computation).
- `bits_`: identical bit patterns, NaN patterns included. Integer results return through `intBitsToFloat` /
  `uintBitsToFloat`; the cases avoid what GLSL leaves undefined (division by zero, signed overflow, shifts by 32 or
  more, `%` with a negative operand).

## Writing the shared files

- Use only forms that compile in both GLSL 4.50 and C++ (literals with the `f` suffix, no definitions of functions
  with `out` parameters, and built-in functions with vector arguments only).
- Avoid denormal inputs (Vulkan implementations may flush them).
