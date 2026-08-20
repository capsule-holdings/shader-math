# Slang ⇔ shader-math 対応表

同一の数式ソース（`tests/interop/shared/math_cases.hlsli`）を
(a) Slang コンパイラ（`slangc -target cpp`、CPU 実行）と
(b) C++（shader-math の `hlsl` 面）の両方でビルド・実行し、数値を照合するための
「共有可能サブセット」の定義。dual compile 自体がこの表の lint になる:
サブセット外の構文はどちらかのコンパイラで必ずエラーになる。

検証ハーネスの実行方法は `tests/interop/` と README の「ビルドとテスト」を参照。
検証済み Slang バージョン: **v2026.16**（CI で固定ダウンロード）。

## 共有可能サブセット（両方でそのままコンパイル・一致確認済み）

| 分類 | 内容 |
| --- | --- |
| ベクトル型 | `float2` / `float3` / `float4`（コンストラクタ・要素アクセス含む） |
| 成分ごと演算子 | `+` `-` `*` `/`（ベクトル同士、ベクトル×スカラー） |
| swizzle | `.xyzw` の読み書き（重複書き込みは両方でエラー） |
| 行列 | `floatRxC`（R,C ∈ {2,3,4}）、行優先 ctor、Hadamard `*` |
| 行列演算 | `mul` 全形（vec×mat / mat×vec / mat×mat）、`transpose`、`determinant` |
| ベクトル演算 | `dot` / `cross` / `length` / `normalize` |
| 比較 | 成分ごと `==` `<` 等 + `all` / `any` での還元 |
| 成分ごと関数（v2） | `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc` `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` + HLSL 別名 `lerp` `frac` `rcp` `rsqrt` `saturate`（**ベクトル引数のみ**、下記除外参照） |
| リテラル | float リテラルは必ず `f` サフィックスを付ける |

## 除外（共有ファイルに書いてはいけないもの）

| 項目 | 理由 |
| --- | --- |
| `inverse` | shader-math の拡張（GLSL 由来）。HLSL/Slang に同名 intrinsic が無い |
| リソース型・テクスチャ | `StructuredBuffer` 等はラッパ（`kernels.slang`）側のみ。共有数式は純関数に限る |
| Slang のジェネリクス / `interface` | C++ 側に対応物が無い（このハーネスの対象は数式サブセットのみ） |
| `v.x` の可変長引数直渡し | shader-math の `v.x` はプロキシ型。`printf` 等には `float(v.x)`（共有数式では非該当だが注意） |
| **スカラーへの intrinsic 適用** | HLSL/Slang は `saturate(dot(n,l))` のようにスカラーにも intrinsic が効くが、shader-math は純スカラーを意図的に非対応にして `std::` に譲る。共有ファイルでは **intrinsic はベクトル引数のみ** に使う（スカラー値は `float3(d,d,d)` のようにベクトルへ載せて計算するか、三項演算子で書く。例: `case_near_lighting`） |
| 行列への成分ごと intrinsic | shader-math 未実装（`abs(m)` 等） |
| **half（`half2/3/4`）** | **SPIR-V 実行フェーズ（lavapipe）送り**。実測（Slang v2026.16）: CPU ターゲットの C++ prelude が half ベクトル演算を未実装（`Vector<_Float16, 4>` の `operator*` 等が無い）ため、half 演算を含む生成コードがコンパイルできない。共有ファイルには `SM_INTEROP_ENABLE_HALF` ガード付きで `case_half_chain` を保持済み |

## 意味差（コンパイルは通るが結果・規則が異なりうるもの）

- **half/float の暗黙混合**: Slang（HLSL 規則）は暗黙昇格を許すが、shader-math は
  コンパイルエラーにする。共有ファイルでは**明示変換必須**（`half4(f4)` / `float4(h4)`）。
  C++ 側のコンパイルがこの規則を機械的に強制する = dual compile が lint。
- **fp contraction（fmadd 融合）**: ターゲット・コンパイラ依存で丸めが変わる。
  ハーネスは Slang 生成コードを `-ffp-contract=off` でビルドし、`near_` 階層は
  相対 1e-5 で吸収する（`compare.py` の階層許容誤差: `exact_` = ビット一致、
  `near_` = 相対 1e-5、`half_` = 相対 2e-3）。
- **`round` の半端値（.5）**: shader-math は half-away-from-zero（`std::round` 同等）。
  GPU / Slang ターゲットは RNE のことが多く、正確な .5 の丸めは共有サブセットで
  当てにしない（共有ケースでは .5 ちょうどを避ける入力にする）。
- **`rcp` / `rsqrt`**: shader-math は近似命令を使わない正確版。GPU 実行では
  近似命令になりうる（CPU 照合では一致、SPIR-V フェーズでは許容緩和が必要）。
- **メモリレイアウト**: 言語互換であって ABI 互換ではない（`sizeof(float3) == 16`、
  cbuffer パッキング等）。本検証の対象外。
