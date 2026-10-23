#!/usr/bin/env python3
"""Front-end time of a TU that only includes shader-math (informational).

Compiles bench/compile_time.cpp with -fsyntax-only under each opt-in macro
set and reports the best of N runs in seconds, plus the ratio to the default
build. Nothing is asserted: shared CI runners are noisy, the ratios are the
useful part.

  compile_time.py <c++ compiler> [--markdown] [--runs N] [-- extra flags...]
"""

import os
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SOURCE = os.path.join(ROOT, "bench", "compile_time.cpp")

CONFIGS = [
    ("default", []),
    ("SHADER_MATH_GLSL_STPQ", ["-DSHADER_MATH_GLSL_STPQ"]),
    ("SHADER_MATH_NESTED_SWIZZLE", ["-DSHADER_MATH_NESTED_SWIZZLE"]),
    ("NESTED_SWIZZLE + GLSL_STPQ", ["-DSHADER_MATH_NESTED_SWIZZLE", "-DSHADER_MATH_GLSL_STPQ"]),
    ("SHADER_MATH_MATRIX_SWIZZLE", ["-DSHADER_MATH_MATRIX_SWIZZLE"]),
    ("SHADER_MATH_MATRIX_SWIZZLE3", ["-DSHADER_MATH_MATRIX_SWIZZLE3"]),
]


def best_seconds(cmd, runs):
    best = float("inf")
    for _ in range(runs):
        t0 = time.perf_counter()
        subprocess.run(cmd, check=True)
        best = min(best, time.perf_counter() - t0)
    return best


def main():
    args = sys.argv[1:]
    extra = []
    if "--" in args:
        extra = args[args.index("--") + 1:]
        args = args[: args.index("--")]
    if not args:
        raise SystemExit(__doc__)
    cxx = args[0]
    markdown = "--markdown" in args
    runs = int(args[args.index("--runs") + 1]) if "--runs" in args else 5

    base = [cxx, "-std=c++20", "-fsyntax-only", "-I" + os.path.join(ROOT, "include"), *extra]
    rows = []
    for name, defs in CONFIGS:
        rows.append((name, best_seconds([*base, *defs, SOURCE], runs)))

    ref = rows[0][1]
    if markdown:
        print(f"Header front-end time ({os.path.basename(cxx)}, -fsyntax-only, best of {runs})")
        print()
        print("| config | seconds | vs default |")
        print("| --- | ---: | ---: |")
        for name, sec in rows:
            print(f"| {name} | {sec:.2f} | {sec / ref:.2f}x |")
    else:
        for name, sec in rows:
            print(f"{name:28s} {sec:6.2f} s  {sec / ref:5.2f}x")
    return 0


if __name__ == "__main__":
    sys.exit(main())
