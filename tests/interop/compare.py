#!/usr/bin/env python3
"""Run the Slang-CPU runner and the shader-math harness, compare outputs.

Line format from both binaries:  name[i] 0xBITS
Tolerance is chosen from the case-name prefix:
  exact_  bit-identical
  near_   relative 1e-5 (inner products, divisions, transcendentals)
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


def run(binary):
    out = subprocess.run([binary], capture_output=True, text=True, check=True).stdout
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
    return 1e-5


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: compare.py <slang_runner> <cpp_harness>")
    slang = run(sys.argv[1])
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
        print(f"{status} {key:28s} slang=0x{sb:08x} ({sf!r}) cpp=0x{cb:08x} ({cf!r})")

    total = len(slang)
    print(f"{total - failures}/{total} slots matched")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
