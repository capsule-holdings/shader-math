# HLSL / GLSL / shader-math compatibility mapping

This is a list of how the constructs you can write in shaders are written in shader-math (`hlsl::` / `glsl::`). The
scope is float / half vectors and matrices, int / uint vectors, their operations, and built-in functions (double,
textures, and so on are listed under "Out of scope" at the end).

- Symbols: **○** can be written as is, with the same meaning / **△** can be written, but the rules or the syntax
  differ (see the notes) / **×** valid in the language but cannot be written in shader-math (reason given) / **−**
  does not exist in that language (also an error in shader-math)
- Whether something compiles or is an error is checked against DXC (`-HV 2021 -enable-16bit-types`) and glslang
  using the case collection in `tests/conformance/` ([conformance.md](conformance.md)). Agreement of values is checked
  by the comparison harness in `tests/interop/` ([slang_compat.md](slang_compat.md), [glsl_interop.md](glsl_interop.md))
- Use `using namespace` for only one of the two languages in a given scope (`mul`, `transpose`, and others collide)

## Types

| Concept | HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- | --- |
| float vector | `float2` to `float4` | `vec2` to `vec4` | ○ / ○ | `hlsl::float3` and `glsl::vec3` are distinct types. Conversion is explicit (`glsl::vec3(f)`) |
| Template syntax | `vector<float, 3>` | − | ○ | `hlsl::vector<T = float, N = 4>`. `vector` without `<>` is △ (accepted by clang, rejected by gcc) |
| half vector | `half2` to `half4` (when 16-bit types are enabled) | `f16vec2` to `f16vec4` (`GL_EXT_shader_explicit_arithmetic_types`) | ○ / ○ | Requires `_Float16` usable in C++ ("Types" in the README) |
| half scalar | `half` | `float16_t` | ○ / △ | On ARM this is ambiguous with `::float16_t` from `arm_neon.h`, so write `glsl::float16_t` |
| bool vector | `bool2` to `bool4` | `bvec2` to `bvec4` | ○ / ○ | Type of comparison results. No arithmetic (same in both languages) |
| Matrix | `floatRxC` (R rows, C columns) | `matCxR` (C columns, R rows) | ○ / ○ | `float3x4` and `mat4x3` have the same shape. They are distinct types, and conversion is explicit |
| half matrix | `halfRxC` | `f16matCxR` | ○ / ○ | |
| bool matrix | `boolRxC` | − | ○ / − | Result of HLSL matrix comparisons |
| Matrix template syntax | `matrix<float, R, C>`, `matrix<>` (float4x4) | − | ○ / − | |
| Single component | `float1`, `int1`, `vector<float, 1>`, etc. | − | ○ / − | A scalar spelled as a vector; scalar results stay scalars (`float1 + float1` is a `float`) |
| 1xN, Nx1, 1x1 matrices | `float1x3`, `float3x1`, `float1x1`, etc. | − | ○ / − | `float`, `half`, `int`, `uint`, `bool` (`matrix<T, R, C>` with R, C ∈ {1..4}) |
| Language of a type | − | − | `hlsl::lang_tag` / `glsl::lang_tag` | Internal to shader-math. Used so that operators and language-specific functions are found by ADL |

## Constructors and conversions

| Syntax | HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- | --- |
| List of scalars `float3(1, 2, 3)` / `vec3(1.0, 2.0, 3.0)` | ○ | ○ | ○ / ○ | int arguments are converted too |
| Mixing vectors and scalars `float4(v2, 0, 1)` | ○ | ○ | ○ / ○ | |
| Component count not exact `vec3(v2, w2)` | Error | ○ (only the last argument may have components left over) | ○ / ○ | GLSL requires every argument to be used (`vec2(v2, s)` is an error). shader-math does the same |
| Single scalar `float3(s)` / `vec3(s)` | Error | ○ (all components) | △ / ○ | HLSL allows the cast `(float3)s` and the implicit conversion `float3 v = s`. In C++ these go through the same constructor, so `float3(s)` also compiles |
| Implicit conversion from a scalar `float3 v = 1.0` | ○ | Error | ○ / ○ | In GLSL, write `vec3(1.0)` |
| Truncation `float3(v4)` / `vec3(v4)` | Error (the cast `(float3)v4` is allowed) | ○ | △ / ○ | HLSL `float3(v4)` cannot be distinguished from a cast in C++, so it compiles |
| Implicit truncation `float3 v = v4` | ○ (DXC warns) | Error | × / ○ | Rejected on purpose (write `(float3)v4` / `float3(v4)`) |
| Widening `float4 v = v3` | Error | Error | ○ / ○ | An error in shader-math as well |
| Matrix: list of scalars | ○ (filled in row-major order) | ○ (filled in column-major order) | ○ / ○ | |
| Matrix: row / column vectors `float2x2(r0, r1)` / `mat2(c0, c1)` | ○ | ○ | ○ / ○ | |
| Matrix: mixed `float2x2(v2, 3, 4)` / `mat2(v2, 3.0, 4.0)` | ○ (exact component count) | ○ (only the last argument may have components left over) | ○ / ○ | `mat2(v4)` and `mat2(v3, v3)` are also allowed in GLSL |
| Matrix: single scalar | Error (casts and implicit conversions fill all components) | ○ (diagonal; `mat3(1.0)` = identity matrix) | △ / ○ | HLSL `float3x3(s)` compiles in C++ |
| Matrix: implicitly from a single vector `float2x2 m = v4` | ○ | Error | ○ / ○ | |
| Matrix → matrix | Narrowing cast `(float2x2)m3` | Widening (filled in from the identity matrix) and narrowing `mat2(m3)` | ○ / ○ | HLSL `float2x2(m3)` (an error) compiles in C++ as △ |
| Matrix → vector `float4 v = m22` / `vec4(m2)` | ○ (same component count, row order) | ○ (first N components in column order) | ○ / ○ | HLSL allows this whenever R×C ∈ {1..4} (implicitly as well: `2x2` ↔ `float4`, `1x3`/`3x1` ↔ `float3`, `1x1` ↔ `float1`/`float`). GLSL allows it only explicitly (`vec3(m2)`, `vec4(m3)`) |
| Matrices mixed into constructor arguments | In a matrix ctor, with an exact component count (`float3x3(m22, s, s, s, s, s)`, row order) | In a vector ctor, only the last argument may have components left over (`vec4(s, m2)`, column order). Not allowed in a matrix ctor | ○ / ○ | An HLSL vector ctor (`float4(s, m2)`) is an error because the component count does not match |
| Explicit conversion between half and float `half3(f)` / `f16vec3(v)` | ○ | ○ | ○ / ○ | |
| Implicit conversion between half and float, and mixed operations `h * a`, `float3 v = h` | ○ | Partly ○ (f16 → f32 direction) | × / × | Rejected on purpose (because rounding would be mixed in) |
| Mixing half and float vectors in one ctor `float4(v2, h2)` | ○ | ○ | ○ / ○ | Single-argument implicit conversion (`float2x2 m = h4`) remains × |
| Explicit conversion between bool and float `float3(b)` / `bool3(v)` | ○ | ○ | ○ / ○ | Nonzero is true (NaN is also true); true becomes 1 |
| Implicit conversion between bool and float `float3 v = b` | ○ | Error | × / ○ | Rejected on purpose |
| Implicit bool splat `bool3 b = true` | ○ | Error | ○ / ○ | |
| Initializer list `float3 v = {1, 2, 3}` | ○ | ○ | ○ / ○ | HLSL flattening `float4 v = {v3, 1}` and `float2x2 m = {v2, 3, 4}` are also allowed. GLSL `mat2 m = {v2, 3.0, 4.0}` is an error (same in shader-math) |
| C-style cast `(float3)x` | ○ | Error (write `vec3(x)`) | ○ / △ | GLSL `(vec3)x` is valid C++ syntax, so it compiles |
| HLSL ⇔ GLSL conversion | − | − | Explicit only | `glsl::vec3(f)`, `hlsl::float3x3(gm)` (transposed, so that it is the same mathematical object) |

## Component access

| Syntax | HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- | --- |
| `.xyzw` / `.rgba` (all combinations, read and write) | ○ | ○ | ○ / ○ | Mixing sets (`a.xg`), out-of-range components, and writes to repeated components are errors (in both languages and in shader-math) |
| `.stpq` | − | ○ | − / ○ | |
| Compound assignment to a swizzle `v.yz += w` | ○ | ○ | ○ / ○ | |
| Nested swizzle `v.xy.yx` | ○ | ○ | △ / △ | Opt-in via `SHADER_MATH_NESTED_SWIZZLE`. Reading 2- and 3-component swizzles only |
| Swizzle on a scalar `s.xxx` | ○ | ○ | × / × | A C++ float has no members |
| `++` / `--` on a swizzle (`v.x++`, `v.xy--`) | ○ | ○ | ○ / ○ | HLSL matrix elements `m._11++` are also allowed |
| Index `v[i]` | ○ | ○ | ○ / ○ | |
| Matrix index `m[i]` | ○ (row) | ○ (column) | ○ / ○ | |
| `m._m00` / `m._11` | ○ | − | ○ / − | |
| Multiple matrix components `m._m00_m11` | ○ | − | △ / − | Opt-in via `SHADER_MATH_MATRIX_SWIZZLE` (2 components) / `SHADER_MATH_MATRIX_SWIZZLE3` (3 components) |
| `.length()` | − | ○ (component count; column count for matrices) | − / ○ | |
| Reading bool vector components `b.x` / `b[i]` | ○ | ○ | ○ / ○ | |
| Writing bool vector components `b.x = true`, `b.x = c.y`, `b[i] = true` | ○ | ○ | ○ / ○ | Assign a bool or another component (HLSL `b.x = 1` / `b[i] = 1` is ×; as with `bool3(1)`, bools are not created from int) |
| Passing `v.x` to variadic arguments | − | − | △ | `v.x` is a proxy type, so pass `float(v.x)` to `printf` and similar functions |

## Operators

| Syntax | HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- | --- |
| `+ - * /` (vector with vector, vector with scalar) | ○ | ○ | ○ / ○ | float vectors also accept int / double scalars. half vectors accept only half scalars (HLSL `h * 2.0` is ×) |
| Unary `-` `+` | ○ | ○ | ○ / ○ | `+b` on a bool vector is also allowed |
| `++` `--` (vectors and matrices) | ○ | ○ | ○ / ○ | |
| Operations on vectors of different widths `a3 + a4` | ○ (truncated; DXC warns) | Error | × / ○ | Rejected on purpose |
| `%`, `%=` (float) | ○ (same as `fmod`; sign of x) | − | ○ / − | GLSL uses `mod()` (sign of y) |
| `==` `!=` | Per-component bool vector | bool that is true when all components are equal | ○ / ○ | Found with the meaning that matches the type, even without `using` |
| `<` `<=` `>` `>=` | Per-component bool vector | Not available for vectors (`lessThan()` and others) | ○ / ○ | |
| `if (a == b)` (vectors) | Error (`all()` / `any()`) | ○ (bool) | ○ / ○ | |
| `&&` `\|\|` on bool vectors, condition of the ternary operator | Error (`and()` / `or()` / `select()`) | Error (`mix(x, y, b)`) | ○ / ○ | HLSL `and()` / `or()` have the same names as C++ alternative tokens, so they are × |
| `!` on bool vectors | ○ | Error (`not()`) | ○ / ○ | GLSL `not()` is a C++ keyword, so use `not_()` (△) |
| `*` between matrices | Per component (same shape only) | Matrix product (inner dimensions must match) | ○ / ○ | In HLSL, the matrix product is `mul()` |
| `*` between a matrix and a vector | Error (`mul()`) | `m * v` / `v * m` | ○ / ○ | |
| `mul(a, b)` (9 forms) | ○ | − | ○ / − | `mul(v4, m3)` / `mul(m3, m23)` with mismatched dimensions are truncated and accepted by DXC, but are × in shader-math |
| Compound assignment `+= -= *= /=` | ○ | ○ | ○ / ○ | GLSL `m *= n` (square) and `v *= m` are also allowed. HLSL `v *= m` is an error |

## Built-in functions

Every vector version can also be used on scalars (`float` / `half`, `v.x`, `m._11`). Component-wise application to
matrices is HLSL only (GLSL functions take only genType).

| HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- |
| `abs` `sign` `floor` `ceil` `trunc` | Same names | ○ / ○ | HLSL `sign` returns int, but shader-math returns a float of the same type (`float3 r = sign(a)` works in both cases) |
| `round` | `round` / `roundEven` | ○ / ○ | .5 rounds to even (same as DXC; GLSL `round` has an implementation-defined direction, so it is made the same as `roundEven`). An unqualified scalar `round(s)` is ambiguous with libc++ / MSVC (△; write `hlsl::round(s)`) |
| `frac` | `fract` | ○ / ○ | |
| `fmod` (trunc-based, sign of x) | `mod` (floor-based, sign of y) | ○ / ○ | The meanings differ. `fmod(-7, 3) = -1`, `mod(-7, 3) = 2` |
| `min` `max` `clamp` | Same names | ○ / ○ | NaN follows IEEE minNum / maxNum (the D3D rule). Same on all backends |
| `saturate` | − | ○ / − | In GLSL, use `clamp(x, 0.0, 1.0)` |
| `lerp` (`a + t*(b - a)`) | `mix` (`x*(1 - a) + y*a`) | ○ / ○ | They differ only in rounding |
| − | `mix(x, y, bvec)` | − / ○ | y for true components |
| `select(c, a, b)` (HLSL 2021) | − | ○ / − | a for true components (the argument order is the reverse of `mix`). A scalar condition is also allowed |
| `step` `smoothstep` | Same names | ○ / ○ | The argument order of `step(edge, x)` is the same in both languages |
| `sqrt` | Same name | ○ / ○ | |
| `rsqrt` | `inversesqrt` | ○ / ○ | Exact version that does not use approximation instructions |
| `rcp` | − | ○ / − | Exact 1/x |
| `exp` `exp2` `log` `log2` `pow` | Same names | ○ / ○ | Calls libm per lane. `pow(negative, non-integer)` is NaN |
| `log10` | − | ○ / − | |
| `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` | Same names | ○ / ○ | |
| − | `asinh` `acosh` `atanh` | − / ○ | |
| `atan2(y, x)` | `atan(y, x)` | ○ / ○ | y comes first in both |
| `degrees` `radians` | Same names | ○ / ○ | |
| `mad` | `fma` | ○ / ○ | Fused on backends with FMA. HLSL `fma` is double-only (an error for float; same in shader-math) |
| `modf(x, out ip)` | Same name | ○ / ○ | The fractional part of ±inf is ±0 |
| `frexp(x, out e)` (exponent is float) | `frexp(x, out ivec e)` | ○ / × | The GLSL version takes an int vector (out of scope) |
| `ldexp(x, e)` (exponent is float) | `ldexp(x, ivec e)` | ○ / × | Same as above |
| `isnan` `isinf` | Same names | ○ / ○ | Scalars are also allowed. Under `-ffast-math`, NaN checks may be removed |
| `isfinite` | − | ○ / − | |
| `sincos(x, out s, out c)` | − | ○ / − | |
| `dot` `cross` `length` `distance` `normalize` `reflect` `refract` `faceforward` | Same names | ○ / ○ | Scalar versions are also allowed. `refract` returns zero on total internal reflection |
| `transpose` `determinant` | Same names | ○ / ○ | |
| − (there is no `inverse`) | `inverse` | − / ○ | For HLSL matrices, use the extension `shader_math::ext::inverse(m)` (available only when called qualified) |
| − | `matrixCompMult` `outerProduct` | − / ○ | In HLSL, the component-wise product is `*` |
| `all` `any` (numeric and bool vectors, matrices, scalars) | `all` `any` (bvec only) | ○ / ○ | For numeric values, nonzero is true (NaN is also true). HLSL also accepts a scalar bool (`all(c)`) |
| − | `lessThan` `lessThanEqual` `greaterThan` `greaterThanEqual` `equal` `notEqual` | − / ○ | |
| − | `not` | − / △ | `not_()` (C++ keyword) |
| `dst` `lit` | − | ○ / − | `lit` takes scalar arguments and returns `float4` |
| Component-wise application to matrices, such as `abs(m)` | − | ○ / − | `abs` `sign` `floor` `ceil` `trunc` `round` `frac` `saturate` `sqrt` `rsqrt` `rcp` `exp` `exp2` `log` `log2` trigonometric / hyperbolic `radians` `degrees` `min` `max` `pow` `fmod` `atan2` `step` `lerp` `clamp` `smoothstep` `mad` `isnan` `isinf` `isfinite` |

Differences caused by C++ syntax:

- A call with only double literals (`max(1.0, 2.0)`) does not resolve to the scalar version (write `1.0f`; `1.0` is
  also fine if there is a float argument or a vector)
- `out` / `inout` parameters are taken by reference (`ip` in `modf(x, ip)` must be an lvalue)

## Integer vectors

| Construct | HLSL | GLSL | shader-math | Notes |
| --- | --- | --- | --- | --- |
| Types | `int3` `uint3` `vector<int, 3>`, `uint` `dword` | `ivec3` `uvec3`, `uint` | ○ / ○ | Distinct types per language, as for float |
| Splat | `int3 v = 1;` | `ivec3(1)` | ○ / ○ | Implicit in HLSL only, as for float |
| Constructors | `int4(v.xy, 2.9, k)` | same | ○ / ○ | Arguments convert; float → int truncates toward zero |
| float → int conversion | `int3(a)`, `int3 v = a;` | `ivec3(a)` (no implicit form) | ○ / ○ | NaN gives 0 and out-of-range values saturate (both languages leave them undefined) |
| int → float, int ↔ uint | implicit either way | implicit int → uint, int / uint → float only | ○ / ○ | |
| bool ↔ int | implicit | constructor only | △ / ○ | HLSL's implicit form is × (`int3(b)` / `bool3(i)`), as for bool ↔ float |
| `+ - * /` | ○ | ○ | ○ / ○ | Wrap around. `x / 0` gives all bits set, `INT_MIN / -1` wraps (undefined in the languages) |
| `%` | truncating | truncating | ○ / ○ | Sign of the dividend; `x % 0` gives all bits set |
| `& \| ^ ~` | ○ | ○ | ○ / ○ | |
| `<< >>` | count masked to 5 bits | undefined for counts ≥ 32 | ○ / ○ | Count modulo 32 in both. The count may have the other signedness (`i >> u`). `k << v` is HLSL only |
| Mixed operands | `i * 2.5` → float3, `i + u` → uint3 | `i * 2.5` → vec3, `i + u` → uvec3 | ○ / ○ | Compound assignment keeps the left type: HLSL `i += a` truncates, GLSL `i += a` is an error |
| Integer scalar operands | `i * 2u` → uint3, `s16 * 2` → int16_t3 | `i * 2u` → uvec3, `s16 * 2` → ivec3 | ○ / ○ | GLSL types every integer; HLSL types `2u` and components, an unsuffixed literal takes the vector's type. An HLSL `int` variable (`s16 * k` is an `int3`) counts as such a literal here. The same for `min` `max` `clamp`, HLSL `select` `mul`, and HLSL's integer matrices (`im * 2u` → uint3x3) |
| `!` | `!i` → bool3 | − | ○ / ○ | |
| Comparisons | `i < j` → bool3 | `lessThan(i, j)`, `i == j` → bool | ○ / ○ | As for float vectors |
| `abs` `min` `max` `clamp` `sign` | ○ | ○ | ○ / ○ | `abs(INT_MIN)` wraps; `sign` takes int, not uint |
| `dot` | ○ | − (floats only) | ○ / ○ | Wraps |
| Float functions on int | `sqrt(i)` → float3 | `sqrt(i)` → ambiguous (error) | ○ / ○ | |
| Bit casts | `asint` `asuint` `asfloat` | `floatBitsToInt` `floatBitsToUint` `intBitsToFloat` `uintBitsToFloat` | ○ / ○ | Vectors and scalars. GLSL converts the argument implicitly (`intBitsToFloat(i16vec3)`, `uintBitsToFloat(ivec3)`) |
| 16-bit casts | `asint16` `asuint16` `asfloat16` | `float16BitsToInt16` `float16BitsToUint16` `int16BitsToFloat16` `uint16BitsToFloat16` | ○ / ○ | Between `int16_t`, `uint16_t` and half |
| `frexp` / `ldexp` | float exponent | int exponent | ○ / ○ | |
| Bit counts and positions | `countbits` `firstbithigh` `firstbitlow` (uint) | `bitCount` `findMSB` `findLSB` (int of the argument's width) | ○ / ○ | 16, 32 and 64 bits, counted at the lane's width. On signed integers, the highest bit that differs from the sign bit; -1 where none. HLSL also takes float and half (converted to int) |
| Bit reversal | `reversebits` (same integer type) | `bitfieldReverse` (int or uint) | ○ / ○ | HLSL: 16, 32 and 64 bits; float and half give int. GLSL: a 16-bit argument converts to int |
| Bit fields | − | `bitfieldExtract` `bitfieldInsert` (int offset / bits) | ○ / ○ | Sign-extended on int. A field outside the 32 bits (undefined in GLSL) ends at bit 31; negative values count as 0. 16-bit arguments compute as int (glslang takes int for `u16vec` too), 64-bit ones do not compile |
| Half bits | `f32tof16` / `f16tof32` (uint low 16 bits) | `packHalf2x16` / `unpackHalf2x16` | ○ / ○ | Round to nearest even |
| unorm / snorm packing | − | `packUnorm2x16` `packSnorm2x16` `packUnorm4x8` `packSnorm4x8`, `unpack*` | ○ / ○ | Ties to even (implementation-defined in GLSL); NaN packs as 0 |
| Integer and float16 packing | − | `packInt2x16` `packUint2x16` `packInt4x16` `packUint4x16` `packInt2x32` `packUint2x32` `packFloat2x16`, `unpack*` | − / ○ | The first component in the low bits |
| 16- / 64-bit integers | `int16_t3` `uint64_t3` (`-enable-16bit-types` for 16 bits) | `i16vec3` `u64vec3` | ○ / ○ | Operators, int functions and bit functions at their width |
| Mixed widths | the wider, unsigned when either is | one side converts implicitly | ○ / ○ | |
| 16-bit integer with half | `int16_t3 + half3` → half3 | `i16vec3 + f16vec3` → f16vec3 | × / × | half converts explicitly, as with float |
| Integer matrices | `int3x3` `uint3x3` | − | ○ / − | `mul` wraps; `%`, bitwise operators, comparisons and the int functions row by row; float functions and `determinant` compute in float |
| Matrix compared with a scalar | `m > 1.0` → bool3x3 | − (`==` only, whole matrix) | ○ / − | Every element type |
| Carry, borrow, 64-bit product | − | `uaddCarry` `usubBorrow` `umulExtended` (uint), `imulExtended` (int) | ○ / ○ | |

## Tolerated for compatibility

Constructs that are errors in the language but compile, because C++ cannot distinguish them:

| Syntax | Language | Reason |
| --- | --- | --- |
| `float3(s)`, `float3(v4)`, `float3x3(s)`, `float2x2(m3)` | HLSL | Writing the same conversion as a cast (`(float3)v4`) or an implicit conversion (`float3 v = s`) is valid HLSL. In C++ they all go through the same constructor |
| `(vec3)x` | GLSL | C-style casts are C++ syntax |
| `if (s)`, `!s`, `&&` on floats, `?:` with a float condition, `int i = 1.5` | GLSL | Scalars are plain C++ `float`, so C++ conversions to bool and narrowing conversions apply |
| `vec3{1, 2, 3}`, `float3{1, 2, 3}` | Both | C++ brace initialization (exists in neither language) |

Constructs that cannot be written because of C++ syntax: swizzles on scalars (`s.xxx`), `float1`, `not()` / `and()` /
`or()`, calls with only double literals (`max(1.0, 2.0)`), HLSL scalar `s % t` (write `fmod`), `v.x` passed to
variadic arguments, and `out` / `inout` (taken by reference).

Differences that appear only in values: in a scalar-only expression, `1.0` is a double in C++, so `s * (1.0 / 3.0)` is
computed in double and then rounded to float (shaders compute in float). Operations with vectors convert the scalar to
the element type before computing, so this difference does not occur there.

## Features not in the languages

| Feature | Enabled when | Notes |
| --- | --- | --- |
| `shader_math::ext::inverse` | When called qualified | Inverse of an HLSL matrix (not in HLSL). Features that are not in the languages are placed in `shader_math::ext` so that they do not compile when written the way a shader is written |
| `hlsl::matrix::identity()` | Always | `float4x4::identity()`. Cannot be confused with shader syntax |
| `v.store(p)` (vector member) | Always | Writes the components to `p[0..N)` |
| Explicit HLSL ⇔ GLSL conversion | Always | `glsl::vec3(f)`, `hlsl::float3x3(gm)` |
| ADL without `using` | Always | Does not increase the set of expressions that can be written (shaders have no namespaces) |
| Fixing behavior the languages leave unspecified | Always | Round-to-even in `glsl::round`, NaN handling in `min` / `max`, precision of `rcp` / `rsqrt`, return type of `sign` |
| Internal members | Always (public) | `q_` `comp_` `row_` `col_` `flat_` `m_` and the constructors from registers. Public because of union constraints. Not intended for users |
| Output, `std::format`, structured bindings, `std::hash`, `load` / `store` | `#include <shader_math/std.hpp>` | |
| Nested swizzles, multi-component matrix access | `SHADER_MATH_NESTED_SWIZZLE` / `SHADER_MATH_MATRIX_SWIZZLE(3)` | Features that exist in the languages. Opt-in because of compile time |
| Forcing the scalar backend, suppressing the global aliases `hlsl` / `glsl` | `SHADER_MATH_FORCE_SCALAR` / `SHADER_MATH_NO_ALIAS` | |

Functions in a language namespace accept only arguments of their own language, even when called qualified
(`hlsl::dot(glsl vector)` and `glsl::abs(hlsl vector)` are errors).

## Toolchain differences

Whether code compiles:

| Item | Difference |
| --- | --- |
| Unqualified scalar `round(s)` | Ambiguous (an error) with libc++ and MSVC, because they have a global `::round(float)`. Compiles with libstdc++ |
| `float16_t` | Ambiguous on ARM with `::float16_t` from `arm_neon.h` |
| `vector r = v4` (without `<>`) | Compiles with clang, does not compile with gcc |
| half types | Require `_Float16` usable in C++ (x86: gcc 12+ / clang 15+, AArch64: gcc 13+). MSVC uses a software implementation. Without it, the types do not exist |
| `std::format` | Only in environments that have `<format>` (gcc 13+ and others) |
| `<windows.h>` | Requires `NOMINMAX`, because it conflicts with the `min` / `max` macros |

Values:

- Whether FMA is available (SSE2 only, or `x86-64-v3` / NEON) changes the rounding of `mad` / `fma`, `lerp`,
  `smoothstep`, and matrix products
- Contraction of the user's own scalar expressions (`-ffp-contract`) depends on the compiler (clang contracts by
  default; gcc does not in ISO mode)
- Transcendental functions use each platform's libm (glibc / Apple / UCRT), so results differ by some ulps. They also
  differ from GPUs
- With ARMv8.2 FEAT_FP16, half is rounded to fp16 after every operation. Otherwise it is widened to f32, computed, and
  converted back (rounded twice)
- Under `-ffast-math`, NaN checks and the NaN rules of `min` / `max` may break (DXC's DXIL is also fast-math)
- Denormals: CPUs handle them by default. GPUs often flush them to zero
- The unused lanes of `floatN` (N < 4) have unspecified values. With floating-point exception trapping enabled,
  exceptions may be raised

Representation: component access uses type punning through a union (undefined behavior in ISO C++) and depends on how
gcc / clang / MSVC handle it (it cannot be `constexpr`). `sizeof(float3) == 16` is language-level compatibility, not the
GPU ABI. Shader compilers also differ from each other (Slang's CPU `round`, DXC's SPIR-V `lit`, DXIL's `frexp`;
[slang_compat.md](slang_compat.md)). shader-math follows DXC and the official documentation.

## Out of scope

| Category | Examples |
| --- | --- |
| Other numeric types | `double` / `dvec3`, `1.0L` / `1.0lf`, 16- / 64-bit integer matrices, 8-bit integers |
| 8-bit packing (HLSL SM 6.6) | `pack_u8` `pack_s8` `unpack_s8s32` `int8_t4_packed` |
| GPU-only | Derivatives (`ddx` / `dFdx`), textures, wave / subgroup, atomics, `clip` / `discard` |
| Qualifiers and layout | `precise`, semantics, `cbuffer` / `uniform` packing (`sizeof(float3) == 16` is not the GPU ABI) |

## Conformance rate (2026-11-23)

The fraction of the 826 cases in `tests/conformance/` (HLSL 432, GLSL 394) for which shader-math accepts and rejects
the same code as the language. The values exclude the out-of-scope categories. The int / uint cases were added with
the integer vectors; the earlier columns cover the float / half cases only.

| | v0.5.0 | before int / uint | main |
| --- | --- | --- | --- |
| HLSL float / half (305) | 74.8% | 91.6% | 92.5% |
| GLSL float / half (256) | 78.5% | 96.1% | 96.9% |
| HLSL int / uint (77) | − | − | 98.7% |
| GLSL int / uint (63) | − | − | 100.0% |
| HLSL 16- / 64-bit (40) | − | − | 97.5% |
| GLSL 16- / 64-bit (58) | − | − | 98.3% |
| HLSL packing (8) | − | − | 100.0% |
| GLSL packing (15) | − | − | 100.0% |
| HLSL total (430) | − | − | 94.2% |
| GLSL total (392) | − | − | 97.7% |

The one int / uint mismatch is HLSL's implicit bool → int (`int3 r = bb;`), kept explicit like bool → float. The
16- / 64-bit mismatches are 16-bit integers mixed with half (both languages). GLSL's float / half cases went up with `frexp` / `ldexp` taking int exponents.

Measured with Apple Clang. With g++ (libstdc++), one more GLSL case matches (the case where the scalar `round(s)`
becomes ambiguous because of libc++'s `::round(float)`). The remaining mismatches correspond to the × and △ entries in
the tables above. To measure again, run `python3 tests/conformance/run.py --cxx <compiler>`.
