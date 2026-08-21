#!/usr/bin/env bash
# Regenerate the committed SPIR-V binaries from shader.slang.
# Usage: build_shaders.sh [path-to-slangc]   (default: slangc on PATH)
set -eu
cd "$(dirname "$0")"
SLANGC="${1:-slangc}"
"$SLANGC" shader.slang -target spirv -entry vsMain -stage vertex -o shader.vert.spv
"$SLANGC" shader.slang -target spirv -entry fsMain -stage fragment -o shader.frag.spv
echo "wrote shader.vert.spv shader.frag.spv"
