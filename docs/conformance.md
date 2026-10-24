# HLSL / GLSL 準拠率の測定

`tests/conformance/` は、シェーダに書く文をそのまま C++ に貼ったときに、shader-math が本物の言語と
同じものを受け付け、同じものを弾くかを数える。CI では実行しない（手元で測り直すための道具）。

- ケース: `cases.py`。1 ケースは 1 文で、HLSL / GLSL それぞれ同じ引数（`float3 a`、`float3x3 m`、`bool3 bb`、
  `half3 h` など）を持つ関数の本体に入れる。分類は `ctor`（コンストラクタ・変換）、`swizzle`、`operator`、
  `intrinsic`、`scalar`（スカラー引数の関数）、`matrix_fn`、`out_of_scope`（int / double・ビットキャスト・
  微分など、shader-math の対象外）
- 正解: `expected.tsv`。DXC（`-T cs_6_2 -HV 2021 -enable-16bit-types`）と glslang（`#version 450` +
  `GL_EXT_shader_explicit_arithmetic_types`）が受け付けたか。ケースを足したら作り直す
- 一致: 言語が受け付けて shader-math も受け付ける、または両方が弾く。値の一致は見ない（それは
  `tests/interop/` の照合ハーネス）
- 率はケースの選び方で変わる。比べるのは同じケース集での版の間の差と、不一致の中身
- 項目ごとの対応（不一致がどの書き方で、なぜか）は [compat_mapping.md](compat_mapping.md)

## 使い方

```bash
# C++ コンパイラだけで測る（Boost が標準の場所に無ければ -- の後に -I を渡す）
python3 tests/conformance/run.py --cxx g++
python3 tests/conformance/run.py --cxx clang++ --verbose -- -I<boost>/include

# 別の版の include/ と並べて比べ、変わったケースを出す
git worktree add /tmp/sm-old v0.5.0
python3 tests/conformance/run.py --cxx g++ --compare /tmp/sm-old/include

# ケースを足したら、本物のコンパイラで正解を作り直す
python3 tests/conformance/run.py --refresh --dxc <dxc> --glslang glslangValidator
```

出力は Markdown の表（言語ごとの一致率、分類ごとの一致率）と、不一致の一覧
（言語は受け付けるのに shader-math が弾くもの、その逆）。`--verbose` で最初のエラーも出す。

## 結果の読み方

- コンパイラと標準ライブラリで少し変わる（例: libc++ は `::round(float)` を宣言するので、
  `using namespace hlsl` 下のスカラーの `round(s)` が曖昧になる）
- 意図して弾いているもの（HLSL の暗黙の切り詰め、half と float の暗黙の混在、HLSL の bool と float の
  暗黙変換）や、オプトインのマクロが要るもの（重ねがけ swizzle、行列の複数成分 swizzle）も不一致に数える
- C++ で書けないもの（スカラーへの swizzle `s.xxx`、`not()`、double になる `1.0` だけの呼び出し、
  C 形式のキャスト `(vec3)v` を GLSL がエラーにすること）も不一致に数える
