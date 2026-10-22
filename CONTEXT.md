# shader-math 開発コンテキスト

計画フェーズ（Cursor Plan Mode）での調査・設計判断の記録。実装の背景を追うためのドキュメント。
作成: 2026-08-20

## 発端と要件の変遷

1. 初期要求: 「CUDA（HLSL）の行列計算と同じライブラリ。float3x3〜float4x4。完全互換。Boost + SIMD × template でインライン展開。.cpp でコンパイルできるもの」
2. 調査結果: CUDA Runtime / `vector_types.h` に行列型は存在しない。「CUDA（HLSL）の行列」の実体は Microsoft HLSL の行列言語仕様
3. 要件更新: 「Win 依存なしで HLSL×GLSL 互換の floatNxM SIMD が欲しい」→ 二重名前空間の設計へ
4. スコープ拡大: float2 / 2x2 追加 → R,C ∈ {2,3,4} の全9形 → 任意 swizzle も v1（完全互換を貫く）
5. ツールチェイン: 古い環境への忖度はしない。C++20 concepts 必須（フォールバック層なし）

## 主要な設計判断

### 二重名前空間（最重要）

HLSL と GLSL は `operator*` の意味が逆:

- HLSL: `mat1 * mat2` は**成分ごと**（Hadamard）。行列積は `mul()`
- GLSL: `mat1 * mat2` が**行列積**。成分ごとは `matrixCompMult()`

→ 1つの `*` で両言語互換は不可能。`shader_math::hlsl` / `shader_math::glsl` に分離し、
翻訳単位ごとに片方だけ `using namespace` する。グローバルの `namespace hlsl = ...;`
エイリアスは `SHADER_MATH_NO_ALIAS` で抑止可能。

**却下した代替案: `*` の挙動をビルド設定で切り替える** — コードを読んでも意味が決まらなくなる、
設定違いの TU 混在で ODR 違反、依存間の伝播事故。「設定で変えたい」の実体は
「どちらの言語の式も書きたい」であり、名前空間が型システムで同じことを表現する。
挙動を変えてよいノブは FORCE_SCALAR（結果同一・速度のみ）に限定。

### 既存ライブラリを使わなかった理由

- [hlslpp](https://github.com/redorav/hlslpp): HLSL 互換としては最良の既存実装（最有力競合）。GLSL 面がない
- [NVIDIA MathLib](https://github.com/NVIDIA-RTX/MathLib): HLSL 互換 + SIMD だが float3x3 未実装
- [DirectXMath](https://github.com/Microsoft/DirectXMath): `XMMATRIX::operator*` が行列積で HLSL と逆。
  第一級型は 4x4 のみ。**ラップは不適**（言語契約が逆、GLSL 非対応、Win/D3D 寄り）
- [GLM](https://github.com/g-truc/glm): GLSL 専用。HLSL の `*` と非互換
- Boost.QVM: `*` が行列積・SIMD なし。旧 Boost.SIMD はメンテ終了

自作の存在意義は「一つの SIMD 核で両言語の式が書ける」こと。

### 型の対応関係（互換性監査で発見した罠）

両言語の公式仕様との突合監査（2回実施）で見つけた非互換ポイント:

1. **比較演算子の意味が言語で逆**（`*` と同型の罠）: HLSL の `==` は成分ごと bool 行列
   （`if` 不可、`all()`/`any()` で還元）、GLSL の `==` はスカラー bool（成分ごとは `equal()` 系）
2. **非正方の型対応が入れ替わる**: GLSL `matCxR` は C列×R行 → `float3x4`(3行4列) ≡ `mat4x3`(4列3行)
3. **スカラー ctor の意味差**: HLSL `float3x3(1)` = 全成分 splat / GLSL `mat3(1)` = 単位行列（対角）
4. **行列⇔行列 ctor**: GLSL は拡大時に単位行列で補完・縮小は左上切り出し / HLSL は縮小キャストのみ
5. **GLSL の `<` `>` はスカラー限定**: ベクトルには定義しない（定義すると過剰互換）
6. vec×mat の意味は両言語で同一（GLSL `v*m` = HLSL `mul(v,m)`）→ `mul` 実装は共有可能

### 型の共有戦略

- **ベクトルは同一型のエイリアス**: `hlsl::float3` = `glsl::vec3` = `detail::fvec<3>`
  （`*` も `dot` も両言語で同じ意味なので共有できる）
- **行列は別型**: `hlsl::matrix<R,C>`（行格納・行優先 ctor）と `glsl::matrix<C,R>`（列格納・列優先 ctor）。
  `operator*` と ctor 順が違うため同一型にはできない。相互変換は explicit
- glsl 側を列レジスタ格納（内部転置）にしたのは `m[col]` をレジスタ直読みにするため。
  行格納共有 + gather は遅いので却下
- `bool2/3/4` / `boolRxC` は比較結果専用型（マスクレジスタ）。算術なし・bool 暗黙変換なし

### union による成分アクセス（実装依存の前提）

`fvec` は `union { reg_t q_; T comp_[4]; swz<...> x, xy, ...; }`、`hlsl::matrix` は
`union { fvec row_[R]; T flat_[R*4]; mswz<...> _m00, ...; }` で、SIMD レジスタと成分配列・
swizzle プロキシが同じ 16 バイトを共有する。演算は `q_` に書き、`v[i]` / `v.x` / `v.xy = ...` は
`comp_` / プロキシの `d[]` 経由で読み書きする。

- **ISO C++ では未定義動作**（最後に書いたのと別の union メンバを読む型パンニング）
- 実装上は主要 3 コンパイラで成立している: gcc は union 経由の型パンニングを拡張として文書化、
  clang も同じ扱い、MSVC は型ベースのエイリアス解析をしない。加えて `__m128` は gcc/clang で
  `may_alias` 付き、scalar バックエンドのレジスタは `float[4]` そのもの
- 帰結: `constexpr` 化はできない（定数評価は非アクティブメンバの読み出しを拒否する）。
  UBSan はこの種の UB を検出しないので、ASan/UBSan ジョブは保証にならない
- 担保: 全 CI 構成（gcc / clang / MSVC / Apple Clang、-O3・Debug）での全テスト。
  `test_vector.cpp` の union アクセス検査は、レジスタ書き込みと成分 / swizzle 読み書きを
  noinline 関数内のループで交互に行い、素のスカラー計算と一致することを確認する
  （エイリアス解析で読み書きが並べ替えられると不一致になる）
- 却下した代替: 親ベクトルへの参照を持つプロキシ（`sizeof` が増え、`v.x` を値で持てない）、
  アクセサ関数（`v.x()` になり HLSL / GLSL の式がそのまま書けない）。
  新しいコンパイラでこの前提が崩れた場合に再検討する

### ADL と名前空間配置

- 言語共通の演算（ベクトル `+`、`dot` 等）→ `detail`（fvec の実体名前空間、ADL で見つかる）
- 言語で意味が分かれる演算（行列 `*`、比較）→ 各言語名前空間
  （行列は実体がそこにあるので ADL 可。ベクトル比較は using-directive 経由）
- トップレベル `hlsl` / `glsl` の直置きは却下（DirectXShaderCompiler が `hlsl` 名前空間を使う等、
  衝突リスク）。ルート `shader_math` 配下 + グローバルエイリアスで解決

### restrict 設計（未定義の演算はコンパイルエラー）

concepts + オーバーロード不宣言 + `= delete` 相当の3段構え:

- `hlsl` の `float3x4 * float4x3` → エラー（同形のみ。行列積は `mul`）
- `glsl` の内次元不一致（`mat3 * mat4x2`）→ シグネチャで不成立
- `float3 + float4` → エラー（スカラーのみブロードキャスト）
- `if (a == b)`（hlsl）→ bvec は bool に変換しないのでエラー（言語仕様どおり）
- 重複 swizzle 書き込み `v.xx = ...` → requires で不成立
- hlsl⇔glsl 行列の暗黙変換なし
- テストは requires 式 + static_assert で「コンパイルできないこと」自体を検証

### SIMD / ツールチェイン

- NEON（AArch64）と SSE2（x86-64）が第一級。不明アーキ（RISC-V 等）は scalar 4-wide に
  フォールバックしてビルド可能。RVV 対応は可能だが CI が薄いので初版見送り
- PS5 = prospero-clang（Clang 系、x86-64/SSE2）、Switch = GCC 系（AArch64/NEON）。
  主要コンパイラ（gcc 10+ / clang 10+ / MSVC 19.30+ / Apple Clang 12+）は concepts 対応済み
- 制約は `<concepts>` ライブラリヘッダに依存せず、コア言語の `requires` 句のみで記述
- Boost はヘッダのみ（Config = FORCEINLINE、Predef = ISA 検出）。concepts のポリフィルではない
- scalar は間引かない: 同一テスト全件を SIMD と FORCE_SCALAR の両方で実行。
  丸めの入る演算（fmadd 系）は近似比較、丸めなし演算（Hadamard・転置・ctor）は厳密一致

### CI

Blacksmith ランナー（GitHub-hosted ではない）。`fail-fast: false`:

- `blacksmith-2vcpu-ubuntu-2404` × g++ / clang++（SSE2）
- `blacksmith-2vcpu-ubuntu-2404-arm` × g++ / clang++（NEON）
- `blacksmith-2vcpu-windows-2025` × MSVC（SSE2）
- x64 / ARM の g++ × `-DSHADER_MATH_FORCE_SCALAR=ON`

前提: org に Blacksmith app がインストールされていること。

## 既知の逸脱（意図的）

- 行列の複数成分 swizzle（`_m00_m11`）は未実装。単成分 `_m00`〜`_44` は全対応
  （複数成分はメンバ宣言が最大 16⁴ 通りで組み合わせ爆発するため。実シェーダでもほぼ未使用）
- GLSL `not()` → `not_()`（C++ キーワード）
- `v.x` はプロキシ型（varargs には `float(v.x)`）
- 対象は float と half ベクトル（half は ARM FEAT_FP16 のみネイティブ、他は f32 昇格）。
  `int` / `double`、1xN、GPU 専用機能（テクスチャ・微分・atomic）は対象外
- `%` は HLSL のみの演算（GLSL float に無い）→ 対象外。`++`/`--` も対象外
- 言語互換でありメモリ ABI 互換ではない（`sizeof(float3) == 16`、定数バッファのパッキングは別）

## v2 候補

- **実装済み**: 逆三角 / 双曲線 / 幾何（`distance` `reflect` `refract` `faceforward`）/
  `atan2`（GLSL `atan(y, x)`）/ `fmod`（HLSL, trunc 基準）と `mod`（GLSL, floor 基準）/
  `mad`・`fma` / `isnan` `isinf` `isfinite` / `modf` / `sincos` / `radians` `degrees`。
  `fmod` / `mod` は `*` と同型の意味差なので言語別に分離し、相手側の名前はコンパイルエラー。
- **実装済み**: 成分ごと関数（`abs`〜`tan` の 23 関数 + `roundEven`。ベクトル対象、
  言語別名: HLSL `lerp`/`frac`/`rcp`/`rsqrt`/`saturate` ⇔ GLSL `mix`/`fract`/`inversesqrt`。
  超越関数は per-lane libm、丸めは SSE2 のみ per-lane、他 backend はネイティブ）。
  2 行レジスタ詰め込み最適化（AVX1+FMA3 の 4x4）。
- **未実装**: RVV バックエンド。行列への成分ごと intrinsic（HLSL の `abs(m)` 等）。
  純スカラーへの intrinsic（`std::` を使う）。

## Slang 相互検証ハーネス（2026-08-20 追加）

同一の `tests/interop/shared/math_cases.hlsli` を Slang（`slangc -target cpp` → g++ で
CPU 実行）と shader-math（`cpp_harness.cpp`）の両方でビルド・実行し、
`tests/interop/compare.py` が階層許容誤差（`exact_` = ビット一致 / `near_` = 相対 1e-5 /
`half_` = 相対 2e-3）で照合する。dual compile 自体がサブセットの lint。
実行方法: `-DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<slang展開先>` で
configure し `ctest -R slang_interop`。CI は `linux-x64-slang-interop` ジョブが
Slang v2026.16（バージョン固定、renovate 対象外の手動更新）をダウンロードして実行する。
更新時は `ci.yml` の `slang_version` と `slang_sha256`（リリースアセットの digest）を一緒に変える。

実測記録（Slang v2026.16）: CPU ターゲットの C++ prelude は half ベクトル演算が未実装
（`Vector<_Float16, 4>` に `operator*` 等が無い）ため、half ケースは共有ファイルに
`SM_INTEROP_ENABLE_HALF` ガード付きで保持しつつ照合対象から除外し、SPIR-V 実行
フェーズ（lavapipe）送りとした。サブセット定義・除外・意味差は `docs/slang_compat.md`。

## 検証記録（2026-08-20、Cloud Agent VM）

同一スイート 397 checks を5構成で実行、すべて 0 failures:

| 構成 | バックエンド |
| --- | --- |
| g++ 13 (x64) | SSE2 |
| clang++ 18 (x64) | SSE2 |
| g++ 13 scalar twin | scalar |
| g++ 13 FORCE_SCALAR 構成 | scalar |
| aarch64-linux-gnu-g++ 13 + qemu-aarch64 | NEON（実SIMD） |

実装中に踏んだバグ: swizzle 生成マクロのパラメータ名 `a`/`b` が rgba の文字トークンと衝突
（`p##a` がパラメータ値に置換される）→ パラメータを `i0/i1/i2` に改名して解決。

## リポジトリ移送の経緯

本来のリポジトリは `capsule-holdings/shader-math`（git@github.com:capsule-holdings/shader-math.git）。
実装した Cloud Agent の GitHub 認証は kineca org 固定で capsule-holdings に届かなかったため、
`kineca/cabakuru-server` のブランチ `cursor/shader-math-9737`（PR #27011、移送用 draft）と
zip/tar.gz 成果物の2経路でコードを受け渡した。以後の Cloud Agent 開発は
`capsule-holdings/shader-math` を対象に新規エージェントを起動すること。
