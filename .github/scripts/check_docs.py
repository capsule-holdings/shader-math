#!/usr/bin/env python3
"""Documentation checks (run by .github/workflows/docs.yml).

1. README.ja.md follows README.md: the same sequence of headings (by level),
   code blocks and table rows, and the same code apart from comments and
   <placeholders>. The prose is not compared.
2. Every relative link in a tracked Markdown file points to an existing file.

Exits non-zero and prints each problem when a check fails.
"""

import difflib
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def readme_structure(path):
    items, code, block = [], [], None
    with open(path, encoding="utf-8") as f:
        for line in f:
            if line.startswith("```"):
                if block is None:
                    block = []
                    items.append("code block")
                else:
                    code.append(block)
                    block = None
                continue
            if block is not None:
                s = re.sub(r"/\*.*?\*/", "", line.rstrip())
                s = re.sub(r"\s*(//|#(?!include|define|if|endif|pragma)).*$", "", s)
                s = re.sub(r"<[^<>\s]*>", "<...>", s)
                if s.strip():
                    block.append(s.rstrip())
                continue
            m = re.match(r"(#+) ", line)
            if m:
                items.append("heading level %d" % len(m.group(1)))
            elif line.startswith("|"):
                items.append("table row")
    return items, code


def check_readme_ja():
    en_items, en_code = readme_structure(os.path.join(ROOT, "README.md"))
    ja_items, ja_code = readme_structure(os.path.join(ROOT, "README.ja.md"))
    problems = []
    if en_items != ja_items:
        diff = difflib.unified_diff(en_items, ja_items, "README.md", "README.ja.md", lineterm="")
        problems.append("README.ja.md does not follow README.md's structure:\n" + "\n".join(diff))
    for i, (en, ja) in enumerate(zip(en_code, ja_code)):
        if en != ja:
            diff = difflib.unified_diff(en, ja, "README.md", "README.ja.md", lineterm="")
            problems.append("code block %d differs:\n" % (i + 1) + "\n".join(diff))
    return problems


def check_links():
    files = subprocess.run(["git", "ls-files", "*.md"], cwd=ROOT, capture_output=True,
                           text=True, check=True).stdout.split()
    problems = []
    for rel in files:
        with open(os.path.join(ROOT, rel), encoding="utf-8") as f:
            text = re.sub(r"```.*?```", "", f.read(), flags=re.S)
        for target in re.findall(r"\]\(([^)#\s]+)(?:#[^)]*)?\)", text):
            if re.match(r"[a-z]+:", target):
                continue
            path = os.path.normpath(os.path.join(ROOT, os.path.dirname(rel), target))
            if not os.path.exists(path):
                problems.append("%s: broken link to %s" % (rel, target))
    return problems


def main():
    problems = check_readme_ja() + check_links()
    for p in problems:
        print(p)
    print("docs: %d problem(s)" % len(problems))
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
