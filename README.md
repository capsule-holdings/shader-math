# shader-math

HLSL / GLSL 互換の float ベクトル・行列ライブラリ（C++20、ヘッダオンリー、SIMD）。

シェーダに書いた式を、そのままの意味で `.cpp` に書けることを目的とする。`operator*` の意味が HLSL（成分ごと）と GLSL（行列積）で逆であるため、公開 API を二重名前空間に分離している。

```cpp
#include <shader_math/shader_math.hpp>

void hlsl_side() {
    using namespace hlsl;              // * は成分ごと、行列積は mul()
    float3x3 a = {1,2,3, 4,5,6, 7,8,9};  // 行優先で埋まる
    float3x3 h = a * a;                // Hadamard（成分ごと）
    float3x3 p = mul(a, a);            // 行列積
    float3   v = mul(float3(1,2,3), a);  // 行ベクトル × 行列
    if (all(h == h)) { /* 比較は bool3x3、all()/any() で還元 */ }
}

void glsl_side() {
    using namespace glsl;              // * が行列積
    mat3 m = {1,4,7, 2,5,8, 3,6,9};    // 列優先で埋まる
    mat3 id(1.0f);                     // スカラー ctor は対角 = 単位行列
    vec3 mv = m * vec3(1,2,3);         // 行列 × 列ベクトル
    mat3 h  = matrixCompMult(m, m);    // 成分ごとは関数
    if (m == m) { /* == はスカラー bool */ }
}
```

同一スコープに両方の名前空間を `using` しないこと（`mul` / `transpose` などが衝突する）。

## 型

- ベクトル: `hlsl::float2/3/4` = `glsl::vec2/3/4`（**同一型のエイリアス**。両言語とも `*` は成分ごとなので共有できる）
- 行列: R,C ∈ {2,3,4} の全9形。`hlsl::floatRxC`（行格納・行優先 ctor）と `glsl::matCxR`（列格納・列優先 ctor）は **別型**、相互変換は explicit
- 対応関係は非正方で入れ替わる: `float3x4`（3行4列）≡ `mat4x3`（4列3行）
- 比較結果: `bool2/3/4`（= `bvec2/3/4`）と `boolRxC`。算術なし・bool への暗黙変換なし

## 言語セマンティクスの互換表

| 項目 | hlsl:: | glsl:: |
| --- | --- | --- |
| 行列同士の `*` | 成分ごと（同形のみ） | 行列積（内次元一致を型で強制） |
| 行列積 | `mul(a, b)` | `a * b` |
| 成分ごと積 | `a * b` | `matrixCompMult(a, b)` |
| ベクトル変換 | `mul(v, m)` / `mul(m, v)` | `v * m` / `m * v`（同じ意味） |
| スカラー1つの行列 ctor | 全成分 splat | 対角（`mat3(1.0)` = 単位行列） |
| 行列⇔行列 ctor | 縮小のみ（左上切り出し） | 拡大（単位行列で補完）/ 縮小 |
| `==` | 成分ごと bool 行列/ベクトル | 全成分一致のスカラー bool |
| `<` `>` `<=` `>=` | 成分ごと bool | ベクトルには無し（`lessThan()` 等） |
| 成分アクセス | `m[row][col]`, `_m00`, `_11` | `m[col][row]` |
| swizzle | `.xyzw` `.rgba` 全組み合わせ（読み書き） | 同左 |

`transpose` / `determinant` / `inverse` / `dot` / `cross` / `length` / `normalize` / `all` / `any` は両方にある（`inverse` は GLSL 公式・HLSL 側は拡張）。

## Restrict 設計（未定義の演算はコンパイルエラー）

- `hlsl` の `float3x4 * float4x3` → エラー（`mul` を使う）
- `glsl` の内次元不一致 `mat3 * mat4x2` → エラー
- `float3 + float4` → エラー（サイズ変換は明示 ctor のみ）
- `if (a == b)`（hlsl）→ エラー（`all()` / `any()` で還元）
- `v.xx = ...`（重複 swizzle への書き込み）→ エラー
- `hlsl::matrix` ⇔ `glsl::matrix` の暗黙変換 → エラー（explicit のみ）

## SIMD

- NEON（AArch64）と SSE2（x86-64）が第一級。その他アーキ（RISC-V 等）は同一 API の scalar 4-wide にフォールバックしてビルド可能
- `-DSHADER_MATH_FORCE_SCALAR=ON` で scalar を強制。CI では SIMD と scalar の両方で全テストを実行する
- Boost はヘッダのみ利用（Config / Predef による ISA 検出）。未インストール時は CMake が自動取得する

### サブ機能（コンパイル時のみ、実行時切替なし）

- `SHADER_MATH_HAS_FMA` — FMA3 検出時に `ops::fmadd` が `_mm_fmadd_ps`（単一丸め）になる。
  gcc/clang は `__FMA__`（例: `-march=x86-64-v3`）、MSVC は `/arch:AVX2` で有効
- 既定ベースライン（SSE2 / ARMv8.0）は上げない。**推奨**: x86 では `-march=x86-64-v3` でビルドすると FMA が有効になる
- AVX-512 / ZMM は非対応（対応予定なし）
- FMA の有無で fmadd 系演算の丸めが変わる（fused = 単一丸め）。テストは許容比較でこれを吸収している

## ビルドとテスト

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/sandbox                                        # 遊び場
cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON  # scalar 強制
```

要件: C++20（concepts）。gcc 10+ / clang 10+ / MSVC 19.30+ / Apple Clang 12+。

## 既知の逸脱（HLSL/GLSL との差分）

- **行列の複数成分 swizzle**（`_m00_m11` 等）は未実装。単成分 `_m00`〜`_m33` / `_11`〜`_44` は全対応（範囲外は使用時にコンパイルエラー）。複数成分はメンバ宣言の組み合わせが最大 16⁴ 通りで現実的でないため
- GLSL の `not()` は C++ キーワードのため `not_()`
- `v.x` はプロキシ型（可変長引数に直接渡すときは `float(v.x)` にする）
- 対象は float のみ。`int` / `double` / 半精度、`1xN` 行列、GPU 専用機能（テクスチャ・微分・atomic）は対象外
- 言語互換であってメモリ ABI 互換ではない（`sizeof(float3) == 16`。定数バッファのパッキングとは別物）
