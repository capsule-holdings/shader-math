#!/usr/bin/env bash
# Disassembly check for the codegen probe kernels (tests/codegen.cpp).
#
# Primary check (stable): fmadd-based kernels must contain a fused
# multiply-add instruction on backends that promise one (vfmadd on
# SSE2+FMA, fmla on NEON).
# Auxiliary check (best effort): no conditional branch inside a kernel,
# i.e. unroll<M> really unrolled. If this ever misfires on a new
# compiler version, relax the auxiliary check, not the primary one.
#
# Exit codes: 0 = pass, 1 = fail, 77 = skipped (ctest SKIP_RETURN_CODE).
set -u

BIN="${1:?usage: check_codegen.sh <codegen-binary>}"

if ! command -v objdump > /dev/null 2>&1; then
    echo "SKIP: objdump not found"
    exit 77
fi

BACKEND="$("$BIN")" || { echo "FAIL: could not run $BIN"; exit 1; }
echo "backend: $BACKEND"

case "$BACKEND" in
    SCALAR)
        # No SIMD promises to verify on the scalar reference backend.
        echo "SKIP: scalar backend"
        exit 77
        ;;
esac

DIS="$(objdump -d --no-show-raw-insn "$BIN")" || exit 1

# Print the disassembly body of one symbol (empty if the symbol is missing).
disasm_of() {
    printf '%s\n' "$DIS" | awk -v s="<${1}>:" 'index($0, s) {f=1; next} f && /^$/ {exit} f'
}

KERNELS="sm_cg_hlsl_mul_m4m4 sm_cg_hlsl_mul_v4m4 sm_cg_glsl_mul_m4m4 sm_cg_glsl_mul_m4v4"
# The vector kernels iterate exclusively via the compile-time unroll<M>
# template, so they must be straight-line code. The matrix*matrix kernels
# have an outer runtime `for` over rows/columns in the library itself; the
# compiler may legitimately keep that loop, so they are exempt from the
# auxiliary branch check.
STRAIGHT_LINE_KERNELS=" sm_cg_hlsl_mul_v4m4 sm_cg_glsl_mul_m4v4 "
X86_COND_JUMPS='j(e|ne|b|nb|ae|nae|a|na|be|nbe|l|nl|ge|nge|g|ng|le|nle|s|ns|p|np|o|no|c|nc|z|nz)'
ARM_COND_BRANCHES='b\.(eq|ne|lt|le|gt|ge|hi|hs|ls|lo|cc|cs|mi|pl|vs|vc)|cbn?z|tbn?z'

fail=0
for sym in $KERNELS; do
    symfail=0
    body="$(disasm_of "$sym")"
    if [ -z "$body" ]; then
        echo "FAIL: symbol $sym not found in disassembly"
        fail=1
        continue
    fi

    # Primary: the promised fused multiply-add instruction is present.
    case "$BACKEND" in
        SSE2+FMA*)
            if ! printf '%s\n' "$body" | grep -q 'vfmadd'; then
                echo "FAIL: $sym: no vfmadd instruction on $BACKEND"
                symfail=1
            fi
            ;;
        NEON*)
            if ! printf '%s\n' "$body" | grep -qw 'fmla'; then
                echo "FAIL: $sym: no fmla instruction on NEON"
                symfail=1
            fi
            ;;
    esac

    # Auxiliary: fully template-unrolled kernels are straight-line code.
    case "$STRAIGHT_LINE_KERNELS" in
        *" $sym "*)
            if printf '%s\n' "$body" | grep -Eq "[[:space:]](${X86_COND_JUMPS}|${ARM_COND_BRANCHES})[[:space:]]"; then
                echo "FAIL: $sym: conditional branch found (unroll<M> not unrolled?)"
                printf '%s\n' "$body" | grep -E "[[:space:]](${X86_COND_JUMPS}|${ARM_COND_BRANCHES})[[:space:]]" | head -3
                symfail=1
            fi
            ;;
    esac

    if [ "$symfail" -eq 0 ]; then
        echo "OK: $sym"
    else
        fail=1
    fi
done

# AVX two-row packing: on SSE2+FMA+AVX the float 4x4 product kernels must
# use YMM-width fused multiply-adds.
if [ "$BACKEND" = "SSE2+FMA+AVX" ]; then
    for ysym in sm_cg_hlsl_mul_m4m4 sm_cg_glsl_mul_m4m4; do
        ybody="$(disasm_of "$ysym")"
        if [ -z "$ybody" ]; then
            echo "FAIL: symbol $ysym not found in disassembly"
            fail=1
        elif ! printf '%s\n' "$ybody" | grep -Eq 'vfmadd[^ ]*[[:space:]].*ymm'; then
            echo "FAIL: $ysym: no YMM-width vfmadd on SSE2+FMA+AVX"
            fail=1
        else
            echo "OK: $ysym (ymm)"
        fi
    done
fi

# Half kernels: on NEON+FP16 half math must use 16-bit lanes (.4h). The
# vector mad must additionally be straight-line (the matrix product has a
# runtime outer for loop, same exemption as the float kernels). On every
# other backend half math is a widened f32 fallback with no
# instruction-level promise, so there is nothing to check.
if [ "$BACKEND" = "NEON+FP16" ]; then
    for hsym in sm_cg_half4_mad sm_cg_hlsl_mul_h4h4; do
        hbody="$(disasm_of "$hsym")"
        if [ -z "$hbody" ]; then
            echo "FAIL: symbol $hsym not found in disassembly"
            fail=1
            continue
        fi
        hfail=0
        if ! printf '%s\n' "$hbody" | grep -Eq '\.4h'; then
            echo "FAIL: $hsym: no 16-bit (.4h) lanes on NEON+FP16"
            hfail=1
        fi
        if [ "$hsym" = "sm_cg_half4_mad" ] &&
            printf '%s\n' "$hbody" | grep -Eq "[[:space:]](${ARM_COND_BRANCHES})[[:space:]]"; then
            echo "FAIL: $hsym: conditional branch found"
            hfail=1
        fi
        if [ "$hfail" -eq 0 ]; then
            echo "OK: $hsym"
        else
            fail=1
        fi
    done
fi

exit "$fail"
