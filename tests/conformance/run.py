#!/usr/bin/env python3
"""Conformance of shader-math with HLSL / GLSL, measured on a corpus.

Every case in cases.py is a shader statement pasted verbatim into a C++
function. The real compilers decide whether the statement is valid HLSL
(DXC) or GLSL (glslang); their verdicts are recorded in expected.tsv. A case
agrees when shader-math accepts what the language accepts and rejects what
it rejects. Values are not compared here (the interop harnesses do that).

  run.py [--cxx g++] [--include DIR] [--compare DIR] [--jobs N] [--verbose]
         [--check FILE [--strict] | --update-known FILE]
         [-- extra C++ flags, e.g. -I<boost>]
      compile the C++ side against include/ (or DIR) and report agreement;
      --compare DIR also measures another header tree (e.g. an older
      checkout's include/) and lists the cases that changed;
      --check FILE exits non-zero on any mismatch not listed in FILE (and
      with --strict, also on resolved or unknown entries in FILE);
      --update-known FILE writes the current mismatches to FILE
  run.py --refresh --dxc <dxc> --glslang <glslangValidator>
      recompile the corpus with the real compilers and rewrite expected.tsv
"""

import argparse
import collections
import concurrent.futures
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
sys.path.insert(0, HERE)
sys.dont_write_bytecode = True
import cases  # noqa: E402

EXPECTED = os.path.join(HERE, "expected.tsv")
CATEGORIES = ["ctor", "swizzle", "operator", "intrinsic", "scalar", "matrix_fn", "int", "width", "pack", "out_of_scope"]


def corpus():
    for cid, cat, body in cases.HLSL:
        yield f"h_{cid}", "hlsl", cat, body
    for cid, cat, body in cases.GLSL:
        yield f"g_{cid}", "glsl", cat, body


def shader_source(lang, body):
    if lang == "hlsl":
        return f"void f({cases.H_PARAMS}) {{ {body} }}\n[numthreads(1, 1, 1)] void main() {{}}\n"
    return ("#version 450\n#extension GL_EXT_shader_explicit_arithmetic_types : require\n"
            "layout(local_size_x = 1) in;\n"
            f"void f({cases.G_PARAMS}) {{ {body} }}\nvoid main() {{}}\n")


def cpp_source(lang, body, include_header=True):
    inc = "#include <shader_math/shader_math.hpp>\n" if include_header else ""
    if lang == "hlsl":
        return (f"{inc}using namespace hlsl;\n"
                f"void f({cases.H_PARAMS}) {{ {body} }}\n")
    return (f"{inc}using namespace glsl;\n"
            f"void f({cases.G_CPP_PARAMS}) {{ {cases.G_CPP_PRELUDE} {body} }}\n")


def first_error(text):
    for line in text.splitlines():
        m = re.search(r"error\W*(.*)", line, re.IGNORECASE)
        if m:
            # drop the temporary directory, so the message is stable
            msg = re.sub(r"\S*/(?=[hg]_\w+\.(?:comp|hlsl|cpp)\b)", "", m.group(1))
            return " ".join(msg.split())[:120]
    return ""


def compile_one(cmd):
    p = subprocess.run(cmd, capture_output=True, text=True)
    return p.returncode == 0, first_error(p.stdout + p.stderr)


def run_all(jobs, items):
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        futs = {cid: ex.submit(compile_one, cmd) for cid, cmd in items}
        return {cid: f.result() for cid, f in futs.items()}


def refresh(args, tmp):
    for tool in (args.dxc, args.glslang):
        if not tool:
            raise SystemExit("--refresh needs --dxc and --glslang")
    items = []
    for cid, lang, _, body in corpus():
        ext = "hlsl" if lang == "hlsl" else "comp"
        path = os.path.join(tmp, f"{cid}.{ext}")
        with open(path, "w") as f:
            f.write(shader_source(lang, body))
        if lang == "hlsl":
            cmd = [args.dxc, "-T", "cs_6_2", "-E", "main", "-HV", "2021",
                   "-enable-16bit-types", "-Fo", os.devnull, path]
        else:
            cmd = [args.glslang, "-V", "--target-env", "vulkan1.1", "-o", os.devnull, path]
        items.append((cid, cmd))
    res = run_all(args.jobs, items)
    versions = []
    for cmd in ([args.dxc, "--version"], [args.glslang, "--version"]):
        p = subprocess.run(cmd, capture_output=True, text=True)
        out = (p.stdout + p.stderr).strip().splitlines()
        versions.append(out[0] if out else cmd[0])
    with open(EXPECTED, "w") as f:
        f.write(f"# verdicts of {versions[0]} (-HV 2021 -enable-16bit-types) and {versions[1]}\n")
        for cid, _, _, _ in corpus():
            ok, err = res[cid]
            f.write(f"{cid}\t{'ok' if ok else 'err'}\t{err}\n")
    n = len(res)
    print(f"wrote {EXPECTED}: {n} cases, {sum(1 for ok, _ in res.values() if ok)} accepted")


def load_expected():
    truth = {}
    for line in open(EXPECTED):
        if line.startswith("#"):
            continue
        cid, verdict, err = line.rstrip("\n").split("\t")
        truth[cid] = (verdict == "ok", err)
    return truth


def prepare_pch(args, tmp, include, tag):
    """Precompile shader_math.hpp once per header tree so cases skip reparsing it."""
    hdr = os.path.join(tmp, f"sm_{tag}.hpp")
    with open(hdr, "w") as f:
        f.write("#include <shader_math/shader_math.hpp>\n")
    ver = subprocess.run([args.cxx, "--version"], capture_output=True, text=True)
    is_clang = "clang" in (ver.stdout + ver.stderr).lower()
    pch = hdr + (".pch" if is_clang else ".gch")
    cmd = [args.cxx, "-std=c++20", "-x", "c++-header", "-I" + include,
           *args.extra, hdr, "-o", pch]
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode != 0:
        return None
    return ["-include-pch", pch] if is_clang else ["-include", hdr]


def measure(args, tmp, include, tag):
    pch_flags = prepare_pch(args, tmp, include, tag)
    items = []
    for cid, lang, _, body in corpus():
        path = os.path.join(tmp, f"{cid}.{tag}.cpp")
        with open(path, "w") as f:
            f.write(cpp_source(lang, body, include_header=pch_flags is None))
        items.append((cid, [args.cxx, "-std=c++20", "-fsyntax-only", "-I" + include,
                            *(pch_flags or []), *args.extra, path]))
    return run_all(args.jobs, items)


def load_known(path):
    ids = []
    seen = set()
    with open(path) as f:
        for line in f:
            cid = line.split("#", 1)[0].strip()
            if cid and cid not in seen:
                ids.append(cid)
                seen.add(cid)
    return ids


def write_known(path, truth, cur):
    groups = [
        ("hlsl", True, "HLSL: language accepts, shader-math rejects"),
        ("hlsl", False, "HLSL: language rejects, shader-math accepts"),
        ("glsl", True, "GLSL: language accepts, shader-math rejects"),
        ("glsl", False, "GLSL: language rejects, shader-math accepts"),
    ]
    rows = [(cid, lang, truth[cid][0], cur[cid][0]) for cid, lang, _, _ in corpus()]
    count = 0
    with open(path, "w") as f:
        f.write("# Known conformance mismatches on Linux GCC (g++, libstdc++).\n"
                "# Checked in CI with:\n"
                "#   python3 tests/conformance/run.py --cxx g++ --check tests/conformance/known_mismatches.txt\n"
                "# Update after intentional changes or conformance improvements with:\n"
                "#   python3 tests/conformance/run.py --cxx g++ --update-known tests/conformance/known_mismatches.txt\n"
                "#\n"
                "# Blank lines and comments (# ...) are ignored.\n")
        for lang, want_real, heading in groups:
            cids = [cid for cid, l, real, sm in rows
                    if l == lang and real != sm and real == want_real]
            if not cids:
                continue
            f.write(f"\n# {heading}\n")
            for cid in cids:
                f.write(f"{cid}\n")
                count += 1
    print(f"\nwrote {path}: {count} known mismatch(es)")


def check_known(args, truth, cur):
    meta = {cid: (lang, cat, body) for cid, lang, cat, body in corpus()}
    known = load_known(args.check)
    known_set = set(known)
    actual = [cid for cid in meta if truth[cid][0] != cur[cid][0]]
    actual_set = set(actual)

    unexpected = [cid for cid in actual if cid not in known_set]
    resolved = [cid for cid in known if cid in meta and cid not in actual_set]
    unknown = [cid for cid in known if cid not in meta]

    if unexpected:
        print(f"\n## conformance check: {len(unexpected)} unexpected mismatch(es)")
        for cid in unexpected:
            _, cat, body = meta[cid]
            real = truth[cid][0]
            kind = "under" if real else "over"
            why = cur[cid][1] if real else truth[cid][1]
            print(f"- ({kind}) [{cat}] `{cid}`: `{body}` -- {why}")
    if resolved:
        print(f"\n## conformance check: {len(resolved)} known mismatch(es) now agree")
        for cid in resolved:
            print(f"- `{cid}`")
        print(f"(remove from {args.check} or run --update-known {args.check} to tighten the baseline)")
    if unknown:
        print(f"\n## conformance check: {len(unknown)} entry/entries not in cases.py")
        for cid in unknown:
            print(f"- `{cid}`")

    failed = bool(unexpected) or (args.strict and bool(resolved or unknown))
    status = "FAIL" if failed else "PASS"
    print(f"\nconformance check: {status} ({len(actual)} mismatches, "
          f"{len(unexpected)} unexpected, {len(resolved)} resolved, {len(unknown)} unknown)")
    return 1 if failed else 0


def pct(a, t):
    return f"{100.0 * a / t:.1f}%" if t else "-"


def report(args, truth, results):
    meta = {cid: (lang, cat, body) for cid, lang, cat, body in corpus()}
    names = list(results)

    def tally(name, pred):
        t = agree = under = over = 0
        for cid, (lang, cat, _) in meta.items():
            if not pred(lang, cat):
                continue
            t += 1
            real, sm = truth[cid][0], results[name][cid][0]
            agree += real == sm
            under += real and not sm
            over += sm and not real
        return t, agree, under, over

    head = " | ".join(f"{n} agree | {n} under | {n} over" for n in names)
    print(f"| scope | cases | {head} |")
    print("|" + " --- |" * (2 + 3 * len(names)))
    for lang in ("hlsl", "glsl"):
        for label, pred in (("in scope", lambda l, c, g=lang: l == g and c != "out_of_scope"),
                            ("all", lambda l, c, g=lang: l == g)):
            cols = []
            for n in names:
                t, a, u, o = tally(n, pred)
                cols += [f"{a} ({pct(a, t)})", str(u), str(o)]
            print(f"| {lang} {label} | {t} | " + " | ".join(cols) + " |")
    print()
    print("| lang | category | cases | " + " | ".join(names) + " |")
    print("|" + " --- |" * (3 + len(names)))
    for lang in ("hlsl", "glsl"):
        for cat in CATEGORIES:
            pred = lambda l, c, g=lang, k=cat: l == g and c == k
            t = tally(names[0], pred)[0]
            if t:
                print(f"| {lang} | {cat} | {t} | "
                      + " | ".join(pct(tally(n, pred)[1], t) for n in names) + " |")

    if len(names) == 2:
        print(f"\n## changed: {names[1]} -> {names[0]}")
        for cid, (_, _, body) in meta.items():
            new, old = results[names[0]][cid][0], results[names[1]][cid][0]
            if new != old:
                tag = "fixed" if new == truth[cid][0] else "regressed"
                print(f"- {tag} `{cid}`: `{body}`")

    cur = names[0]
    for kind, title in (("under", "language accepts, shader-math rejects"),
                        ("over", "language rejects, shader-math accepts")):
        print(f"\n## {cur}: {title}")
        for cid, (_, cat, body) in meta.items():
            real, sm = truth[cid][0], results[cur][cid][0]
            if real == sm or (kind == "under") != real:
                continue
            why = results[cur][cid][1] if kind == "under" else truth[cid][1]
            if args.verbose:
                print(f"- [{cat}] `{cid}`: `{body}` -- {why}")
            else:
                print(f"- [{cat}] `{cid}`: `{body}`")


def main():
    argv = sys.argv[1:]
    extra = []
    if "--" in argv:
        extra = argv[argv.index("--") + 1:]
        argv = argv[: argv.index("--")]
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--cxx", default=os.environ.get("CXX", "c++"))
    ap.add_argument("--include", default=os.path.join(ROOT, "include"))
    ap.add_argument("--compare", metavar="DIR", help="another include/ to measure alongside")
    ap.add_argument("--jobs", type=int, default=os.cpu_count() or 4)
    ap.add_argument("--verbose", action="store_true", help="show the first error of mismatches")
    ap.add_argument("--check", metavar="FILE",
                    help="fail on mismatches not listed in FILE")
    ap.add_argument("--strict", action="store_true",
                    help="with --check, also fail on resolved or unknown entries in FILE")
    ap.add_argument("--update-known", metavar="FILE",
                    help="write current mismatches to FILE")
    ap.add_argument("--refresh", action="store_true")
    ap.add_argument("--dxc")
    ap.add_argument("--glslang")
    args = ap.parse_args(argv)
    args.extra = extra

    if args.strict and not args.check:
        ap.error("--strict requires --check")
    if args.check and args.update_known:
        ap.error("--check and --update-known are mutually exclusive")
    if args.refresh and (args.check or args.update_known):
        ap.error("--refresh cannot be combined with --check or --update-known")

    with tempfile.TemporaryDirectory() as tmp:
        if args.refresh:
            refresh(args, tmp)
            return
        truth = load_expected()
        missing = [cid for cid, *_ in corpus() if cid not in truth]
        if missing:
            raise SystemExit(f"expected.tsv lacks {len(missing)} cases; run --refresh")
        results = {"current": measure(args, tmp, args.include, "cur")}
        if args.compare:
            results["compare"] = measure(args, tmp, args.compare, "cmp")
        report(args, truth, results)
        if args.update_known:
            write_known(args.update_known, truth, results["current"])
        elif args.check:
            sys.exit(check_known(args, truth, results["current"]))


if __name__ == "__main__":
    main()
