# Measuring HLSL / GLSL conformance

`tests/conformance/` counts whether, when a statement written for a shader is pasted as-is into C++, shader-math
accepts the same statements as the real language and rejects the same statements. It is not run in CI (it is a
tool for re-measuring locally).

- Cases: `cases.py`. Each case is one statement, placed in the body of a function that has the same parameters for
  HLSL and GLSL (`float3 a`, `float3x3 m`, `bool3 bb`, `half3 h`, and so on). The categories are `ctor`
  (constructors and conversions), `swizzle`, `operator`, `intrinsic`, `scalar` (functions with scalar arguments),
  `matrix_fn`, and `out_of_scope` (int / double, bit casts, derivatives, and other things outside the scope of
  shader-math).
- Expected results: `expected.tsv`. It records whether DXC (`-T cs_6_2 -HV 2021 -enable-16bit-types`) and glslang
  (`#version 450` + `GL_EXT_shader_explicit_arithmetic_types`) accepted each case. Regenerate it when you add cases.
- Agreement: the language accepts and shader-math also accepts, or both reject. Values are not compared (that is
  done by the comparison harness in `tests/interop/`).
- The rate changes with how the cases are chosen. What should be compared is the difference between versions on the
  same case set, and the contents of the mismatches.
- The item-by-item mapping (which forms mismatch, and why) is in [compat_mapping.md](compat_mapping.md).

## Usage

```bash
# Measure with a C++ compiler only (if Boost is not in a standard location, pass -I after --)
python3 tests/conformance/run.py --cxx g++
python3 tests/conformance/run.py --cxx clang++ --verbose -- -I<boost>/include

# Compare against the include/ of another version and list the cases that changed
git worktree add /tmp/sm-old v0.5.0
python3 tests/conformance/run.py --cxx g++ --compare /tmp/sm-old/include

# After adding cases, regenerate the expected results with the real compilers
python3 tests/conformance/run.py --refresh --dxc <dxc> --glslang glslangValidator
```

The output is a Markdown table (agreement rate per language and agreement rate per category) and a list of
mismatches (statements the language accepts but shader-math rejects, and the reverse). `--verbose` also prints the
first error.

## Reading the results

- The results vary slightly with the compiler and standard library (for example, libc++ declares `::round(float)`,
  so the scalar call `round(s)` under `using namespace hlsl` becomes ambiguous).
- Statements that are rejected on purpose (HLSL's implicit truncation, implicit mixing of half and float, and HLSL's
  implicit conversion between bool and float) and statements that need an opt-in macro (nested swizzles and
  multi-component matrix swizzles) are also counted as mismatches.
- Things that cannot be expressed in C++ (swizzles on scalars such as `s.xxx`, `not()`, calls whose only arguments
  are `1.0`, which becomes double, and the fact that GLSL rejects the C-style cast `(vec3)v`) are also counted as
  mismatches.
