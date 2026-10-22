#!/usr/bin/env bash
# Smoke-test the demo against third-party FBX files from the wild.
#
# The files come from the ufbx test suite (MIT-licensed repository) and cover
# three different exporters and two FBX format generations:
#   - Blender 2.93 (FBX 7400 binary): skinned character + rigid prop meshes
#   - Maya        (FBX 7700 ascii):  skinned mesh with an advanced pivot setup
#   - 3ds Max     (FBX 7500 binary): transformed-skin edge case
# Files are downloaded into a scratch dir, never committed.
#
# Usage: test_external_assets.sh <demo-binary> <out-dir>
set -eu

BIN="${1:?usage: test_external_assets.sh <demo-binary> <out-dir>}"
OUT="${2:?usage: test_external_assets.sh <demo-binary> <out-dir>}"
BASE="https://raw.githubusercontent.com/ufbx/ufbx/v0.23.1/data"
ASSETS="blender_293_barbarian_7400_binary maya_advanced_skinned_pivot_7700_ascii max_transformed_skin_7500_binary"

mkdir -p "$OUT"
fail=0
for name in $ASSETS; do
    fbx="$OUT/$name.fbx"
    [ -f "$fbx" ] || curl -fsSL "$BASE/$name.fbx" -o "$fbx"
    dir="$OUT/$name"
    mkdir -p "$dir"
    echo "=== $name"
    if ! "$BIN" --headless 8 --fbx "$fbx" --out "$dir"; then
        echo "FAIL: $name"
        fail=1
    fi
done
exit "$fail"
