# Slang ⇔ shader-math 対応表

同一の数式ソース（`tests/interop/shared/math_cases.hlsli`）を
(a) Slang コンパイラ（`slangc -target cpp`、CPU 実行）と
(b) C++（shader-math の `hlsl` 面）の両方でビルド・実行し、数値を照合するための
「共有可能サブセット」の定義。dual compile 自体がこの表の lint になる:
サブセット外の構文はどちらかのコンパイラで必ずエラーになる。

検証ハーネスの実行方法は `tests/interop/` と README の「ビルドとテスト」を参照。
検証済み Slang バージョン: **v2026.20**（CI で固定ダウンロード）。

## 共有可能サブセット（両方でそのままコンパイル・一致確認済み）

| 分類 | 内容 |
| --- | --- |
| ベクトル型 | `float2` / `float3` / `float4`（コンストラクタ・要素アクセス含む） |
| 成分ごと演算子 | `+` `-` `*` `/`（ベクトル同士、ベクトル×スカラー） |
| swizzle | `.xyzw` の読み書き（重複書き込みは両方でエラー） |
| swizzle の重ねがけ | 2・3 成分の swizzle に対する読み出し（`a.zw.yx`、`b.bgr.rrgb`）。shader-math ではオプトインなので、`shared/nested_cases.hlsli` を `kernels_nested.slang` と `SHADER_MATH_NESTED_SWIZZLE` 付きの C++ 側で照合する（`slang_interop_nested_check`） |
| 行列の複数成分アクセス | `m._m30_m31_m32`、`m._11_22_33`、`m._m00_m11` の読み出しと書き込み（重複無し）。shader-math ではオプトインなので、`shared/matrix_swizzle_cases.hlsli` を `kernels_matrix_swizzle.slang` と `SHADER_MATH_MATRIX_SWIZZLE3` 付きの C++ 側で照合する（`slang_interop_matrix_swizzle_check`） |
| 行列 | `floatRxC`（R,C ∈ {2,3,4}）、行優先 ctor、Hadamard `*` |
| 行列演算 | `mul` 全形（vec×mat / mat×vec / mat×mat）、`transpose`、`determinant` |
| ベクトル演算 | `dot` / `cross` / `length` / `normalize` |
| 比較 | 成分ごと `==` `<` 等 + `all` / `any` での還元 |
| 成分ごと関数（v2） | `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc` `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` + HLSL 別名 `lerp` `frac` `rcp` `rsqrt` `saturate`（ベクトルとスカラー。スカラーは `case_near_scalar`） |
| 逆三角・双曲線・角度 | `asin` `acos` `atan` `atan2(y, x)` `sinh` `cosh` `tanh` `radians` `degrees` |
| 剰余・積和・分解 | `fmod`（trunc 基準、x の符号）、`mad`、`modf(x, out ip)`、`sincos(x, out s, out c)` |
| 幾何 | `distance` `reflect` `refract`（全反射でゼロベクトル）`faceforward` |
| 判定 | `isnan` `isinf` `isfinite`（結果の bool ベクトルは**添字**で読む。下記除外参照） |
| 行列への成分ごと関数 | 上記の成分ごと関数の行列版（`abs(m)` `lerp(a, b, t)` `clamp` `min(m, s)` `step(s, m)` `atan2` `fmod` `mad` など） |
| リテラル | float リテラルは必ず `f` サフィックスを付ける |

## 除外（共有ファイルに書いてはいけないもの）

| 項目 | 理由 |
| --- | --- |
| `inverse` | shader-math の拡張（GLSL 由来）。HLSL/Slang に同名 intrinsic が無い |
| リソース型・テクスチャ | `StructuredBuffer` 等はラッパ（`kernels.slang`）側のみ。共有数式は純関数に限る |
| Slang のジェネリクス / `interface` | C++ 側に対応物が無い（このハーネスの対象は数式サブセットのみ） |
| `v.x` の可変長引数直渡し | shader-math の `v.x` はプロキシ型。`printf` 等には `float(v.x)`（共有数式では非該当だが注意） |
| bool ベクトルの `.xyzw` | shader-math の `boolN` は成分名アクセスを持たない（比較結果のマスク型）。`isnan(v)[2]` のように添字で読む |
| **half（`half2/3/4`）の CPU 実行** | Slang（v2026.20）の CPU ターゲットの C++ prelude が half ベクトル演算を未実装（`Vector<_Float16, 4>` の `operator*` 等が無い）。half のケースは `SM_INTEROP_HALF_CASES` に分け、`kernels_half.slang` を `-target spirv` で SPIR-V にして Vulkan（lavapipe）で実行し照合する（`slang_interop_half_check`、`shaderFloat16` の無いデバイスではスキップ） |

## 意味差（コンパイルは通るが結果・規則が異なりうるもの）

- **`frexp` の指数の型**: HLSL（DXC）と shader-math は指数を float で返すが、Slang（v2026.20）は int
  ベクトルで返す（`out` 引数の型が違うので共有ファイルでは呼べない）。`ldexp` も Slang は int の指数を
  取る（float は暗黙変換）ので、共有ケースでは整数値の指数だけを渡す
- **`dst`**: Slang（v2026.20）では CPU ターゲットの C++ prelude に実装が無く、`-target spirv` でも
  コア側の組み込みコードの解析に失敗する（`snippet parsing failed`）。共有ファイルには書かない
  （shader-math 側は単体テストのみ）。`frexp` も SPIR-V 経由で同じく int の指数を取る

- **half/float の暗黙混合**: Slang（HLSL 規則）は暗黙昇格を許すが、shader-math は
  コンパイルエラーにする。共有ファイルでは**明示変換必須**（`half4(f4)` / `float4(h4)`）。
  C++ 側のコンパイルがこの規則を機械的に強制する = dual compile が lint。
- **fp contraction（fmadd 融合）**: ターゲット・コンパイラ依存で丸めが変わる。
  ハーネスは Slang 生成コードを `-ffp-contract=off` でビルドし、`near_` 階層は
  相対 1e-5 で吸収する（`compare.py` の階層許容誤差: `exact_` = ビット一致、
  `near_` = 相対 1e-5、`half_` = 相対 2e-3）。
- **`round` の半端値（.5）**: HLSL の `round` は .5 を偶数へ丸める（DXC は DXIL で `Round_ne`、SPIR-V で
  `RoundEven`）。shader-math も同じ。Slang の CPU ターゲットは `::roundf`（0 から遠い方へ）なので、
  共有ケースでは .5 ちょうどを避け、.5 は DXC 専用の `case_exact_round_ties` で照合する。
- **`rcp` / `rsqrt`**: shader-math は近似命令を使わない正確版。GPU 実行では
  近似命令になりうる（CPU 照合では一致、SPIR-V フェーズでは許容緩和が必要）。
- **メモリレイアウト**: 言語互換であって ABI 互換ではない（`sizeof(float3) == 16`、
  cbuffer パッキング等）。本検証の対象外。

## DXC（公式 HLSL コンパイラ）との照合

同じ共有ケースを DXC（v1.9.2609）で 2 通りに照合する。

- **SPIR-V**（Linux 版）: Vulkan（lavapipe）で実行する（`dxc_interop_check`、half は
  `-enable-16bit-types` 付きで `dxc_interop_half_check`）
- **DXIL**（Windows 版）: D3D12 の WARP（Windows 標準のソフトウェア実装）で実行する
  （`dxil_interop_check` / `dxil_interop_half_check`、`tests/interop/d3d12_runner.cpp`）。
  DXC は DXIL の生成時に定数入力の組み込み関数を畳み込むので、入力を出力バッファ（0 初期化）の
  ビットと XOR してコンパイル時には分からない値にしている（`SM_INTEROP_OPAQUE_INPUTS`）

Slang では照合できない `frexp` / `dst` / `round` の .5 は `tests/interop/dxc/dxc_cases.hlsli` で照合する。
オプトインの行列の複数成分アクセスも、`SM_INTEROP_MATRIX_SWIZZLE` 付きで `shared/matrix_swizzle_cases.hlsli`
を DXC で SPIR-V にして照合する（`dxc_interop_matrix_swizzle_check`）。

```bash
# SPIR-V: DXC の Linux リリースを展開して（x86_64 のみ）
cmake -S . -B build-dxc -DCMAKE_BUILD_TYPE=Release \
  -DSHADER_MATH_INTEROP_DXC=ON -DSHADER_MATH_DXC_DIR=<dxc 展開先>
cmake --build build-dxc
ctest --test-dir build-dxc -R dxc_interop --output-on-failure

# DXIL（Windows）: DXC の Windows リリース（zip）を展開して。bin/x64 か bin/arm64 の dxc を使う
cmake -S . -B build-dxil -DSHADER_MATH_INTEROP_DXIL=ON -DSHADER_MATH_DXC_DIR=<dxc 展開先>
cmake --build build-dxil --config Release
ctest --test-dir build-dxil -C Release -R dxil_interop --output-on-failure
```

DXC で分かった差:

- **スカラー 1 つの行列 ctor**: DXC は `float3x3(0.25f)` を「要素が足りない」とエラーにする。HLSL で
  全成分に広げる綴りはキャスト `(float3x3)0.25f` か代入 `float3x3 m = 0.25f;`。Slang と shader-math は
  ctor の綴りも受け付ける（共有ケースはキャストで書く）
- **`lit` の鏡面項**: Microsoft の `lit` のドキュメントは `(n·h)^m`、Slang と shader-math も累乗だが、
  DXC v1.9.2609 の SPIR-V 出力（`SpirvEmitter::processIntrinsicLit`）は `(n·h) * m`。DXIL 出力は
  `Exp(Log(n·h) * m)` で累乗になっており、WARP での実行も shader-math と一致する。SPIR-V の照合からだけ
  `lit` のケース（`case_near_lit`）を外している
- **`frexp` の仮数の符号（DXIL）**: DXC の DXIL への変換は仮数を `(bits & 0x007fffff) | 0x3f000000` で
  作るので、負の入力でも仮数が正になる（`frexp(-7.5)` の仮数が 0.9375）。SPIR-V 出力（GLSL の `Frexp`）・
  Slang・shader-math は C の `frexp` と同じく符号付き（-0.9375）。DXIL の照合からだけ `case_exact_frexp` を
  外している
- **`frac` の定数畳み込み（DXIL）**: DXC が DXIL を生成するときの定数畳み込みは `frac(-0.75)` を 0.75 にする
  （`x - floor(x)` なら 0.25）。実行時の入力なら DXIL の `Frc` 命令になり、WARP の結果は 0.25 で shader-math と
  一致する。照合が畳み込みでなく実行を見るよう、DXIL 版は入力を不透明にしている（上記）
- **WARP の逆三角関数の精度**: WARP の `asin` / `acos` / `atan`（`atan2` も）は libm と最大 1e-3 程度ずれる
  （sin / cos / exp2 / log2 / pow などは `near_` の 1e-5 に収まる）。DXIL の照合だけ、これらを使う
  `near_inverse_trig` と `near_matrix_intrinsics` の許容誤差を相対 2e-3 にしている（`compare.py --tol=`）
- half: `-enable-16bit-types` を付けないと DXC の `half` は 32 ビット float（min precision）になる
- DXC は手動更新（`ci.yml` の `dxc_version` / `dxc_asset` / `dxc_sha256` を一緒に変える）

