# Design: 32-bit integer vectors (int / uint)

Status: accepted. Steps 1 and 2 of "Phasing" are implemented (the backend, then the public types with the
strict operators); the decisions are listed at the end.

## Goal and scope

Add the 32-bit signed and unsigned integer vectors of both shading languages,
with the same guarantees the float vectors have: one header-only type per
language, SIMD on SSE2 / NEON with a scalar fallback, language rules checked by
the conformance corpus, and results cross-checked against Slang, DXC and
glslang.

In scope (phase 1 and 2, see "Phasing"):

| | HLSL | GLSL |
|---|---|---|
| signed | `int2` `int3` `int4`, `vector<int, N>` | `ivec2` `ivec3` `ivec4` |
| unsigned | `uint2` `uint3` `uint4`, `vector<uint, N>`, `dword` | `uvec2` `uvec3` `uvec4` |
| scalar aliases | `uint`, `dword` | `uint` |

Out of scope for now: integer matrices (`int3x3`, HLSL only), 16-bit and 64-bit
integers (`int16_t`, `uint64_t`), `double` vectors, and the GLSL packing
functions (`packUnorm2x16` and friends). Each can follow on the same machinery.

## Representation

Integer vectors reuse `detail::fvec<N, T, Lg>` with `T = std::int32_t` or
`std::uint32_t`. Everything that is generic over `T` today (the 4-lane register,
the swizzle proxies, the component-list constructors, `vec_traits`, the
language tag and its ADL rules, GLSL `length()`) carries over unchanged.

- `vops_for<std::int32_t>` / `vops_for<std::uint32_t>` select new `iops` / `uops`
  backends over an integer register `ireg` (`__m128i` on SSE2, `int32x4_t` /
  `uint32x4_t` on NEON, a `std::uint32_t[4]` struct on the scalar backend).
- Comparisons return the existing `bvec<N, Lg>` (mask register `mreg`). On SSE2
  the integer compare result is reinterpreted with `_mm_castsi128_ps` (free); on
  NEON `mreg` is already `uint32x4_t`.
- The rule that lanes `>= N` are unspecified stays. Integer division must
  therefore never trap on a padding lane (see "Undefined results").
- `int` is asserted to be 32 bits (`static_assert(sizeof(int) == 4)`), so
  `hlsl::int3` holds `int`, and the literal `int3(1, 2, 3)` needs no casts.
- The name `fvec` becomes a misnomer. Renaming it is internal (`detail::`), so it
  is left for a separate cleanup and not tied to this work.

`hlsl::uint` / `glsl::uint` are aliases of `unsigned int`. glibc declares
`::uint` as the same type; two aliases of one type found by unqualified lookup
are not ambiguous, but this gets a test on all three compilers.

## Construction and conversion

Constructors follow the float vectors: component lists, scalar splat
(`explicit` in GLSL), and swizzle / vector arguments of the same language.
Element types may mix in constructor lists, as they already do for float and
half (`float4(v2, h2)`), because both languages convert in constructors.

| Conversion | HLSL | GLSL | Here |
|---|---|---|---|
| float -> int / uint | implicit (truncates) | constructor only | HLSL implicit, GLSL explicit |
| int / uint -> float | implicit | implicit (4.00+, not ES) | implicit in both |
| int <-> uint | implicit | int -> uint implicit, uint -> int constructor only | as the languages |
| bool -> int | implicit (0 / 1) | constructor only | HLSL implicit, GLSL explicit |
| int -> bool | implicit (`!= 0`) | constructor only | HLSL implicit, GLSL explicit |
| half <-> int | as float | as float | as float |

The language columns were checked with DXC 1.9.2609 (`-HV 2021`) and glslang
15.1.0. The implicit rows depend on the mixed-operand decision (question 1 below); with
the strict option they all become explicit.

Bit reinterpretation is separate from conversion: HLSL `asint` / `asuint` /
`asfloat`, GLSL `floatBitsToInt` / `floatBitsToUint` / `intBitsToFloat` /
`uintBitsToFloat`. These are register casts on every backend.

## Operators

| Operator | Element types | Semantics |
|---|---|---|
| `+ - *`, unary `-` | int, uint | wrap modulo 2^32 (shaders wrap; C++ signed overflow is avoided by computing in unsigned) |
| `/ %` | int, uint | truncate toward zero; `%` takes the sign of the dividend; zero divisors: see below |
| `& \| ^ ~` | int, uint | bitwise |
| `<< >>` | int, uint | the result has the left operand's type, the count may be of the other signedness (`ivec << uvec`); count masked to its low 5 bits; `>>` is arithmetic for int, logical for uint |
| `== != < <= > >=` | int, uint | return `bvec` (HLSL operators; GLSL keeps `lessThan` etc. for vectors) |
| `++ --`, compound forms | int, uint | as above |
| `!` | int, uint | HLSL only: `!int3` is a `bool3` (lane `== 0`); GLSL rejects it |
| `&& \|\|` | — | not on vectors in either language (HLSL 2021 asks for `and` / `or`) |

`<<` / `>>` take a vector or a scalar count. NEON shifts every lane by its own
count in one instruction; SSE2 only shifts all lanes by one count, so the
backend shifts one lane at a time there. A one-instruction path for scalar
counts (`pslld` / `psrad` / `psrld`) can be added when the operators land.

### Mixed operands

The float vectors accept any arithmetic scalar (`float3 * 2`). For integer
vectors the scalar rule is the main design choice:

- **Language rules (recommended).** Follow each language's usual arithmetic
  conversions: HLSL `int3 * 2.5f` is a `float3`, `int3 + uint3` is a `uint3`
  (both checked with DXC by overload resolution); GLSL `ivec3 * 2.5` is a `vec3`
  (implicit int -> float), `ivec3 + uvec3` is a `uvec3` (glslang accepts both). The binary operators compute the result element type from both
  operands instead of requiring `SameVec`. This is what the conformance corpus
  measures, so it is the only option that keeps HLSL / GLSL conformance high once
  integer cases are in the corpus.
- **Strict.** Only same-type vectors and integral scalars; anything mixed is an
  explicit conversion. Simpler and cheaper to compile, but HLSL code like
  `float3 f = id.xyz * 0.5;` does not compile, and conformance drops for every
  such case.

The recommended path still lands in two steps: phase 1 ships strict operators
(same element type, integral scalars), phase 2 adds the promotions. Phase 1 code
keeps compiling in phase 2; phase 2 only accepts more.

## Functions

Phase 1 (both languages unless noted):

- `abs`, `sign` (returns int), `min`, `max`, `clamp`
- `all`, `any` on integer vectors (HLSL; non-zero lanes)
- HLSL `select`, GLSL `mix(x, y, bvec)` on integer vectors
- GLSL `equal`, `notEqual`, `lessThan`, `lessThanEqual`, `greaterThan`,
  `greaterThanEqual` on `ivec` / `uvec`
- HLSL `dot` on integer vectors (HLSL has it; GLSL does not)
- the bit casts listed above

Phase 3 (bit manipulation):

- HLSL `countbits`, `firstbithigh`, `firstbitlow`, `reversebits`
- GLSL `bitCount`, `findMSB`, `findLSB`, `bitfieldReverse`, `bitfieldExtract`,
  `bitfieldInsert`, `uaddCarry`, `usubBorrow`, `umulExtended`, `imulExtended`

With the language-rules option, HLSL float functions also accept integer
arguments through the implicit int -> float conversion (`sqrt(int3)` is a
`float3`), and GLSL float functions accept them where GLSL 4.x converts
implicitly. That is phase 2. Until then, the shared float functions must
**reject** integer vectors in their constraints. Today they would accept them
and fail inside the body, so every float-only function in `detail::` gets a
`FloatElem` constraint as part of phase 1.

## Backends

A new `SimdIntOps` concept in `detail/simd_concepts.hpp` describes the integer
interface, and `simd.hpp` static-asserts each backend against it, like the
float and half backends.

| Operation | SSE2 | SSE4.1 | NEON | Scalar |
|---|---|---|---|---|
| add / sub / and / or / xor | 1 instr | — | 1 instr | loop |
| mul (low 32 bits) | 2× `pmuludq` + shuffles (~6) | `pmulld` | `vmulq` | loop in unsigned |
| eq / signed gt | `pcmpeqd` / `pcmpgtd` | — | `vceqq` / `vcgtq` | loop |
| unsigned compare | bias with `0x80000000`, then `pcmpgtd` | `pmaxud` + `pcmpeqd` | `vcgtq_u32` | loop |
| min / max | compare + blend | `pminsd` / `pminud` … | `vminq` / `vmaxq` | loop |
| abs | `(x ^ s) - s`, `s = x >> 31` | (SSSE3 `pabsd`) | `vabsq_s32` | loop |
| shift (per-lane count) | 4 scalar shifts | — | `vshlq` (negative = right) | loop |
| div / mod | per-lane scalar | — | per-lane scalar | loop |
| int -> float | `cvtdq2ps` | — | `vcvtq_f32_s32` | loop |
| uint -> float | split into 16-bit halves, two converts, one add (rounds once) | — | `vcvtq_f32_u32` | loop |
| float -> int | `cvttps2dq` + fix-ups (see below) | — | `vcvtq_s32_f32` | loop with clamps |
| float -> uint | emulated | — | `vcvtq_u32_f32` | loop with clamps |

## Undefined results

Several operations are undefined in the shading languages and either undefined
or trapping in C++. This library must not trap or invoke C++ undefined behaviour
on any input, padding lanes included. The question is whether results are the
same on every backend.

| Case | Shaders | C++ / hardware | Proposal (deterministic) |
|---|---|---|---|
| `x / 0`, `x % 0` (uint) | D3D: `0xFFFFFFFF` for both; GLSL undefined | UB; x86 `div` traps | `0xFFFFFFFF` |
| `x / 0`, `x % 0` (int) | undefined | UB; x86 `idiv` traps | `-1` (all bits set, as uint) |
| `INT_MIN / -1`, `INT_MIN % -1` | undefined | UB; x86 `idiv` traps | `INT_MIN`, `0` (wrap) |
| shift count >= 32 or < 0 | HLSL masks to 5 bits (DXC emits `and i32 %s, 31`); GLSL undefined | UB | mask to 5 bits (HLSL rule) |
| signed overflow in `+ - *` | wraps (SPIR-V / DXIL) | UB | wrap |
| float -> int, NaN / out of range | D3D `ftoi`: NaN -> 0, saturate | UB; SSE2 gives `INT_MIN` | NaN -> 0, saturate |
| float -> uint, NaN / negative / out of range | D3D `ftou`: NaN -> 0, saturate | UB | NaN -> 0, saturate |

The deterministic proposal costs a few extra instructions on SSE2 for float ->
int (NEON already saturates and maps NaN to 0) and one compare per lane in the
scalar division loop. The alternative is "whatever the backend does", documented
as a toolchain difference; that is faster on SSE2 but makes results differ
between x86 and ARM.

## Impact on the existing API

- No existing type or signature changes. `bvec`, the float and half vectors,
  and their operand rules stay as they are.
- `ScalarFor` gains the integer rule (integral scalars for integer vectors). The
  float rule ("any arithmetic scalar") is unchanged.
- Float-only functions in `detail::` get an element-type constraint (see
  "Functions"). This changes no accepted call; it only turns a hard error inside
  a body into an overload that does not match.
- `select` / `mix(bvec)` / `all` / `any` / comparison functions gain integer
  overloads.
- `std.hpp` (hash, formatting, stream output) covers the new element types.
- With the language-rules option in phase 2, the binary operators compute a
  common element type. That changes how `float3 + float3` is resolved
  internally (same result type), so its compile-time cost is measured.

## Compile time

The swizzle proxies (490 types per element type per language) are instantiated
only when a vector of that element type is. Code that uses no integer vectors
pays only to parse the new templates. Budgets, measured with
`bench/compile_time.py` on the CI compilers:

- including the headers without using integer vectors: under 5% slower
- a TU that uses `int4` and `uint4` in both languages: reported, not gated

## Tests and conformance

- Unit tests per operator and function, including wrap-around, shift masking,
  negative division, and the deterministic results above. They run on the
  SIMD and scalar builds, and the padding-lane checks of
  `tests/test_pad_lanes.cpp` cover the new types.
- Conformance corpus: new case families in `tests/conformance/cases.py` for
  integer constructors, implicit conversions, mixed operands, shifts, `%`, and
  the functions above. `expected.tsv` is refreshed with DXC and glslang.
- Interop: integer cases in the shared HLSL / GLSL case lists, run through Slang
  CPU, DXC (SPIR-V on lavapipe and DXIL on WARP), and glslang. Cases with
  undefined results (division by zero, out-of-range conversion) are excluded
  from cross-checks and covered by unit tests only.

## Phasing

1. Backend: `ireg`, `iops` / `uops`, `SimdIntOps`, per-backend unit tests. No
   public types yet.
2. Phase 1 types: aliases, constructors and conversions, swizzles, strict
   operators, comparisons, the phase 1 functions, `std.hpp`, unit tests,
   conformance cases, compile-time numbers.
3. Interop cases (Slang, DXC SPIR-V / DXIL, glslang).
4. Phase 2: mixed-operand promotions and float functions taking integer
   arguments (if the language-rules option is chosen).
5. Phase 3: bit-manipulation functions.

Each step is one PR.

## Decisions

1. Mixed operands follow the language rules. Phase 1 ships the strict
   operators; phase 2 adds the promotions.
2. Undefined results are deterministic on every backend, as proposed in
   "Undefined results".
3. This series covers 32-bit integer vectors only. Integer matrices and 16-bit /
   64-bit integers come later.
