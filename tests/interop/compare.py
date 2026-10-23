#!/usr/bin/env python3
"""Run a reference runner (Slang-CPU or Vulkan/GLSL) and the shader-math
harness, compare outputs.

  compare.py <reference_runner> <cpp_harness> [label] [runner args...]

Line format from both binaries:  name[i] 0xBITS
Tolerance is chosen from the case-name prefix:
  exact_  bit-identical
  near_   relative 1e-5 (inner products, divisions, transcendentals)
  approx_ relative 1e-4 (GPU transcendentals with relaxed GLSL precision)
  half_   relative 2e-3 (binary16 ulp + promoted-path rounding)
NaN == NaN is accepted (bit pattern may differ).

Exit codes: 0 = match, 1 = mismatch/error.
"""

import math
import re
import struct
import subprocess
import sys

LINE = re.compile(r"^(\w+)\[(\d)\] 0x([0-9a-fA-F]{8})$")


SKIP = 77  # ctest SKIP_RETURN_CODE: the reference runner lacks a device feature


def run(binary, *args):
    proc = subprocess.run([binary, *args], capture_output=True, text=True)
    if proc.returncode == SKIP:
        print(proc.stderr.strip() or f"SKIP: {binary}")
        sys.exit(SKIP)
    if proc.returncode != 0:
        raise SystemExit(f"{binary} failed ({proc.returncode}): {proc.stderr.strip()}")
    out = proc.stdout
    values = {}
    for line in out.strip().splitlines():
        m = LINE.match(line.strip())
        if not m:
            raise SystemExit(f"unparseable line from {binary}: {line!r}")
        bits = int(m.group(3), 16)
        values[f"{m.group(1)}[{m.group(2)}]"] = bits
    return values


def as_float(bits):
    return struct.unpack("<f", struct.pack("<I", bits))[0]


def tolerance(key):
    if key.startswith("exact_"):
        return 0.0
    if key.startswith("half_"):
        return 2e-3
    if key.startswith("approx_"):
        return 1e-4
    return 1e-5


def main():
    if len(sys.argv) < 3:
        raise SystemExit("usage: compare.py <reference_runner> <cpp_harness> [label] [runner args...]")
    label = sys.argv[3] if len(sys.argv) > 3 else "slang"
    slang = run(sys.argv[1], *sys.argv[4:])
    cpp = run(sys.argv[2])
    if slang.keys() != cpp.keys():
        raise SystemExit(
            f"case sets differ: only-slang={sorted(slang.keys() - cpp.keys())} "
            f"only-cpp={sorted(cpp.keys() - slang.keys())}"
        )

    failures = 0
    for key in sorted(slang):
        sb, cb = slang[key], cpp[key]
        sf, cf = as_float(sb), as_float(cb)
        tol = tolerance(key)
        if math.isnan(sf) or math.isnan(cf):
            ok = math.isnan(sf) and math.isnan(cf)
        elif tol == 0.0:
            ok = sb == cb
        else:
            scale = max(1.0, abs(sf), abs(cf))
            ok = abs(sf - cf) <= tol * scale
        status = "OK  " if ok else "FAIL"
        if not ok:
            failures += 1
        print(f"{status} {key:28s} {label}=0x{sb:08x} ({sf!r}) cpp=0x{cb:08x} ({cf!r})")

    total = len(slang)
    print(f"{total - failures}/{total} slots matched")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
