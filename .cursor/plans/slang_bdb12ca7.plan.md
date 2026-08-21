---
name: Slang 相互検証ハーネス
overview: 同一の数式ソースを Slang と C++（shader-math hlsl 面）の両方でビルド・実行して数値照合する CI ハーネスと、対応サブセットの対応表を追加する。
todos:
  - id: compat-doc
    content: docs/slang_compat.md 対応表（サブセット・除外・意味差）を作成
    status: pending
  - id: shared-cases
    content: tests/interop/shared/math_cases.hlsli + 両側ラッパ（kernels.slang / cpp_harness.cpp）
    status: pending
  - id: slang-path
    content: 'slangc -target cpp でのビルド・実行経路を確立（代替: host-callable）'
    status: pending
  - id: compare
    content: compare.py（階層許容誤差）+ CMake option + ctest 登録
    status: pending
  - id: ci-job
    content: linux-x64-slang-interop CI ジョブ（Slang リリース固定 DL）追加
    status: pending
  - id: docs-update
    content: README / AGENTS.md / CONTEXT.md への追記と main 向け PR
    status: pending
isProject: false
---
# Slang ⇔ shader-math 相互コンパイル検証

前提: **v2 成分ごと関数は別プラン**（v2 プラン参照）に切り分けた。本プランは現状の実装済みサブセットだけで独立に成立し、v2 完了後に共有ケースと対応表を拡張する（拡張は v2 プラン側の作業に含める）。

## 目的

「オレオレ言語なしで CPU/GPU の数式を単一ソース化する」ための基盤。同じ数式ファイルを (a) Slang コンパイラで、(b) C++（shader-math の hlsl 面）でビルド・実行し、CI で数値照合する。将来のマルチプラットフォームライブラリ（Slang 記述）の CPU 側数式共有の実証になる。

## 実行方式（GPU 不要）

Slang は公式に CPU 実行をサポートする（[cpu-target.html](http://shader-slang.org/slang/cpu-target.html)）。

- **主経路**: `slangc kernels.slang -target cpp` で C++ ソースを生成し、Slang リリース同梱の prelude ヘッダとともに g++ でビルドして実行（libslang へのリンク不要、slangc バイナリだけで完結）
- **代替**: libslang をリンクした小さなホスト（`SLANG_SHADER_HOST_CALLABLE` + slang-llvm JIT、公式 cpu-hello-world 例 ~100 行）。主経路の生成コードが扱いにくい場合に切替
- 将来の段階 2（今回スコープ外）: lavapipe（ソフトウェア Vulkan）で SPIR-V 経路も実行

## 構成（shader-math リポジトリ内に追加）

```
docs/slang_compat.md            対応表（サブセット定義・除外・意味差）
tests/interop/
  shared/math_cases.hlsli       共有数式（サブセットのみで書く純関数群）
  kernels.slang                 Slang 側ラッパ: include + compute エントリ（入力配列→出力配列）
  cpp_harness.cpp               C++ 側ラッパ: 同じ include を shader-math で評価、同形式ダンプ
  compare.py                    階層許容誤差つき照合
```

## 1. 対応表（docs/slang_compat.md）

- **共有可能サブセット**: `float2/3/4`・`halfN`、成分ごと演算子、swizzle（重複書き込み不可）、`mul` 全形、`dot` / `cross` / `length` / `normalize`、`floatRxC` と Hadamard `*`、`transpose` / `determinant` / `inverse`、比較 + `all` / `any`
- **除外（明記）**: 成分ごと intrinsics（`abs` / `min` / `lerp` 等）は **v2 プラン完了までは除外**（完了後にサブセットへ追加）、リソース型・テクスチャ、Slang のジェネリクス / `interface`、`v.x` の可変長引数直渡し
- **意味差**: half/float の暗黙混合は Slang（HLSL 規則）は許すが shader-math は禁止 → **共有ファイルでは明示変換必須**（C++ 側のコンパイルが規則を機械的に強制する = dual compile 自体が対応表の lint になる）。fp contraction・half 丸めはターゲット依存で不一致になり得る（許容誤差で吸収）。メモリレイアウト（cbuffer パッキング等）は本検証の対象外
- shader-math 側 README の型節からリンク

## 2. 共有数式ファイルと両側ラッパ

- `math_cases.hlsli`: プリプロセッサだけで両言語に取り込める純関数群。例: Lambert 照明、ベクトル変換チェーン（`mul(m, normalize(cross(a, b)))`）、swizzle 混じりの式、4x4 行列積 + `inverse` の往復、half 版（明示変換つき）
- `kernels.slang`: `#include "shared/math_cases.hlsli"` + `[numthreads(1,1,1)]` の compute エントリ。入力 StructuredBuffer から各ケースを評価し出力バッファへ
- `cpp_harness.cpp`: 同じ `#include` を `using namespace shader_math::hlsl` 下で行い、同一の決定的入力（ハードコード配列。通常値 + 0 / 負 / 小さい値）で評価

## 3. 照合（compare.py）

出力は両側とも「ケース名 + float のビットパターン（hex）」の行形式。許容誤差は既存テストの階層をそのまま採用:

- コピー・Hadamard・swizzle: **ビット一致**
- float の内積・行列積・normalize: 相対 1e-5（fmadd contraction 差を吸収）
- half を含むケース: 相対 2e-3（Slang CPU ターゲットの half が float エミュレーションの可能性があるため。実挙動を見て half をフェーズ 2 に後送する判断もこの段階で行う）

## 4. ビルド統合と CI

- CMake: `option(SHADER_MATH_INTEROP_SLANG OFF)`。ON のとき `SLANGC` を探し、Slang 側ビルド + ctest（`slang_interop_check`）を登録。slangc 不在なら exit 77 スキップ（codegen 検査と同じ流儀）
- CI: `linux-x64-slang-interop` を 1 本追加。Slang の GitHub リリース（**バージョン固定**、renovate で更新）をダウンロード → configure ON → ctest。既存 9 ジョブは不変
- AGENTS.md / CONTEXT.md に interop テストの実行方法を追記

## リスクと逃げ道

- `slangc -target cpp` の生成物・prelude の取り回しが版によって変わる → バージョン固定 + 代替経路（host-callable ホスト）を用意
- Slang CPU ターゲットでの half 精度が GPU と異なる → half ケースは許容緩和 or フェーズ 2 送り（対応表に実測結果を記録）
- 「Slang らしい機能」（ジェネリクス等）はこのハーネスの対象外と明記（共有するのは数式サブセットのみ）

## 進め方

main 向け単一 PR（squash 前提）。実装順 = 対応表 → 共有ファイル + C++ ハーネス（先にローカルで green）→ Slang 経路 → 照合 → CI。

v2 プランとの関係: 本プランは v2 と独立に着手可能（現行サブセットのみでケースを書く）。v2 マージ後、共有ケースへ `saturate(lerp(...))` / `smoothstep` 等の実戦形を追加し対応表を更新する（その作業は v2 プラン側の最終項目）。超越関数を含むケースの許容誤差は相対 1e-5、ビット一致要求は四則・コピー・swizzle に限定する。
