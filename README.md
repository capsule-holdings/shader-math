# shader-math

日本語 | [English](README.en.md)

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
- half ベクトル: `hlsl::half2/3/4` = `glsl::f16vec2/3/4`（同じく同一型）。スカラーは `hlsl::half` / `glsl::float16_t`
- half 行列: `hlsl::halfRxC` と `glsl::f16matCxR`（float と同じく**別型**・explicit 変換。非正方の形対応も float と同一）。
  `determinant` / `inverse` も half のまま計算する。真の fp16 は underflow / overflow が早いので、条件の悪い行列を渡さないこと
  - `sizeof(half3) == 8`（4×binary16 パック）。`sizeof(float3) == 16` と同様、**言語互換であり GPU ABI ではない**
  - half と float の**暗黙混合演算は不可**（`half3 * float3` / `half3 * 2.0f` はコンパイルエラー、変換は explicit）。コンストラクタ引数は変換する（`half3(1.0f, 2.0f, 3.0f)` は可）
  - コンパイラ下限: gcc/clang は C++ で使える `_Float16` が必要（x86 では gcc 12+ / clang 15+、AArch64 では gcc 13+）。MSVC はビット格納のソフトウェア型で対応。未対応コンパイラでは half 型群は定義されない（float 面は無影響）
  - ARM ターゲットでは `arm_neon.h` がグローバル `float16_t` を定義するため、`using namespace glsl` 下の非修飾 `float16_t` は曖昧になる。`glsl::float16_t` と修飾するか `hlsl::half` を使う
- 行列: R,C ∈ {2,3,4} の全9形。`hlsl::floatRxC`（行格納・行優先 ctor）と `glsl::matCxR`（列格納・列優先 ctor）は **別型**、相互変換は explicit
- 対応関係は非正方で入れ替わる: `float3x4`（3行4列）≡ `mat4x3`（4列3行）
- 比較結果: `bool2/3/4`（= `bvec2/3/4`）と `boolRxC`。算術なし・bool への暗黙変換なし。
  `bool3(true, false, true)` / `bool3(true)` で作れる（引数は bool のみ。int や float からは作れない）。
  成分は `b[i]` と単成分の `.x` 〜 `.w` / `.r` 〜 `.a` で**読み出しのみ**（複数成分の swizzle と書き込みは無し）
- Slang / HLSL と共有できる型・演算のサブセットは [docs/slang_compat.md](docs/slang_compat.md) 参照
  （Slang との相互コンパイル検証ハーネスで CI 照合済み）

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
| 成分アクセス | `m[row][col]`, `_m00`, `_11`（複数成分 `_m00_m11` はオプトイン、下記） | `m[col][row]` |
| swizzle | `.xyzw` `.rgba` 全組み合わせ（読み書き） | 同左。`.stpq` はオプトイン（下記） |
| swizzle の重ねがけ（`v.xy.yx`） | オプトイン（下記） | 同左 |

`transpose` / `determinant` / `inverse` / `dot` / `cross` / `length` / `normalize` / `all` / `any` は両方にある（`inverse` は GLSL 公式・HLSL 側は拡張）。

## Restrict 設計（未定義の演算はコンパイルエラー）

- `hlsl` の `float3x4 * float4x3` → エラー（`mul` を使う）
- `glsl` の内次元不一致 `mat3 * mat4x2` → エラー
- `float3 + float4` → エラー（サイズ変換は明示 ctor のみ）
- `if (a == b)`（hlsl）→ エラー（`all()` / `any()` で還元）
- `v.xx = ...`（重複 swizzle への書き込み）→ エラー
- `hlsl::matrix` ⇔ `glsl::matrix` の暗黙変換 → エラー（explicit のみ）

## 標準ライブラリとの連携（オプトイン）

`#include <shader_math/std.hpp>` を足すと、C++ の標準ライブラリと組み合わせて使える（`shader_math.hpp` は
`<ostream>` / `<format>` を引き込まないので別ヘッダーにしている）。

```cpp
#include <shader_math/std.hpp>
using namespace hlsl;

std::cout << float3(1, 2, 3);              // (1, 2, 3)。swizzle（v.zyx）や bool3 も出力できる
std::format("{:.2f}", float2(1, 0.5f));    // (1.00, 0.50)。書式は各成分に効く
auto [x, y, z] = v;                        // 構造化束縛。auto& なら成分を書き換えられる
auto [row0, row1] = m;                     // 行列は m[i] の単位（hlsl は行、glsl は列）
std::unordered_map<float3, int> ids;       // std::hash と std::equal_to（N 成分の値の一致）
auto p = shader_math::load<float3>(ptr);   // float* から N 個読む
shader_math::store(m, ptr);                // 行列はコンストラクタの順（hlsl は行優先、glsl は列優先）
```

- 行列の出力も `m[i]` の単位で並ぶ: hlsl は `((行0), (行1))`、glsl は `((列0), (列1))`
- `std::format` は `<format>` のある環境だけ（gcc 13+ など）。half は float として出力・ハッシュする
- `std::hash` / `std::equal_to` は -0 と +0 を同じキーとして扱い、NaN のキーは一致しない（float のキーと同じ）
- `std::tuple_size` などは std の特殊化なので、構造化束縛や `std::hash` を使う翻訳単位ではこのヘッダーを必ず include する

## SIMD

- NEON（AArch64）と SSE2（x86-64）が第一級。その他アーキ（RISC-V 等）は同一 API の scalar 4-wide にフォールバックしてビルド可能
- `-DSHADER_MATH_FORCE_SCALAR=ON` で scalar を強制。CI では SIMD と scalar の両方で全テストを実行する
- Boost はヘッダのみ利用（Config / Predef による ISA 検出、Predef は 1.60+）。未インストール時、
  またはシステムの Predef が古い場合は CMake が自動取得する

### サブ機能（コンパイル時のみ、実行時切替なし）

- `SHADER_MATH_HAS_FMA` — FMA3 検出時に `ops::fmadd` が `_mm_fmadd_ps`（単一丸め）になる。
  gcc/clang は `__FMA__`（例: `-march=x86-64-v3`）、MSVC は `/arch:AVX2` で有効
- `SHADER_MATH_HAS_SSE41` — SSE4.1 検出時、`floor` / `ceil` / `trunc` / `roundEven` / `round` が
  `roundps`（1 命令）になる。それ以外の x86 はレーンごとの libm 呼び出し。
  gcc/clang は `__SSE4_1__`（例: `-march=x86-64-v2`）、MSVC は `/arch:AVX` 以上で有効。結果は libm と
  ビット一致（符号付きゼロ・NaN 含む）
- `SHADER_MATH_HAS_F16C` — F16C 検出時（x86 の SSE2 バックエンドのみ）、half を f32 に広げる / 戻す変換が
  それぞれ 1 命令（`vcvtph2ps` / `vcvtps2ph`）になる。gcc/clang は `__F16C__`（例: `-march=x86-64-v3`）、
  MSVC は `/arch:AVX2` で有効。結果は従来のスカラー変換とビット一致（丸めは最近接偶数）
- `SHADER_MATH_HAS_AVX` — AVX1 検出時、**FMA と併用の場合のみ** float 4x4 の行列積の内部で
  2 行（GLSL は 2 列）を 1 本の YMM に詰めて計算する（fmadd 16 回 → YMM 8 回）。
  公開型の幅は変わらず（`float4x4` は 4 本の 128-bit のまま）、結果は 128-bit 経路とビット一致。
  FMA なしの AVX1 のみ（Sandy/Ivy Bridge）は 128-bit のまま。2x2 / 3x3 / 非正方も 128-bit のまま
- `SHADER_MATH_HAS_FP16` — ARM FEAT_FP16（`-march=armv8.2-a+fp16`）検出時、half 演算が本物の
  16-bit ベクトル ALU（`float16x4_t`、`fmla v.4h`）で実行される。
  **それ以外のホスト（x86 / scalar / 古い ARM）では half はレーンを f32 に拡げて計算して戻す**。
  拡げる / 戻す変換は、ARMv8.0 では基本命令（`vcvt_f32_f16` / `vcvt_f16_f32`）、x86 では F16C か
  SSE2 の整数演算で 4 レーンまとめて行う（MSVC の ARM64 と scalar はレーンごと）。
  この経路の丸めは GPU の half ALU とも ARM ネイティブとも一致しない（1 演算ごとに f32 丸めが 1 回入る）
- 既定ベースライン（SSE2 / ARMv8.0）は上げない。**推奨**: x86 は `-march=x86-64-v3`（FMA + SSE4.1）、
  ARM で half を速くしたい場合は `-march=armv8.2-a+fp16`
- AVX-512 / ZMM / AVX512-FP16 は非対応（対応予定なし）
- FMA の有無で fmadd 系演算の丸めが変わる（fused = 単一丸め）。テストは許容比較でこれを吸収している

## 成分ごと関数（intrinsics）

ベクトル（float / half）に対する成分ごと関数。スカラー第 2/3 引数は演算子と同じ規則
（float ベクトルは算術スカラー可、half ベクトルは half のみ）。

スカラーにも効く（`max(dot(n, l), 0.0)`、`saturate(v.x)`、`lerp(a, b, 0.5f)`）。ベクトル引数が無く、
少なくとも 1 つが float / half / 1 成分の swizzle（`v.x`）/ 行列の要素（`m._11`）の呼び出しがスカラー版になり、
ほかの引数は上と同じ規則で変換される（`2.0` だけの呼び出しは対象外なので `2.0f` と書く）。中身はベクトル版を
1 レーンで実行するので、丸めや境界値の扱いはベクトルと同じ。`dot` / `length` / `distance` と、
`modf` / `frexp` / `sincos`（出力引数もスカラー）、`select(bool, a, b)`、`mix(x, y, bool)` もスカラーで使える。
`isnan` / `isinf` / `isfinite` と幾何関数（`normalize` / `reflect` など）のスカラー版は無い。
スカラーの `round` だけは、libc++ / MSVC がグローバルに `::round(float)`（.5 は 0 から遠い方へ）を宣言しているので、
`using namespace hlsl` 下の修飾なしの `round(x)` が曖昧でコンパイルエラーになる。`hlsl::round(x)` と書く

行列への成分ごと適用は **HLSL のみ**（HLSL の intrinsic は行列も取るが、GLSL の汎用関数は
genType = スカラー / ベクトルだけなので `glsl` で `abs(mat3)` はコンパイルエラー）。対象は
`abs` `sign` `floor` `ceil` `trunc` `round` `frac` `saturate` `sqrt` `rsqrt` `rcp` `exp` `exp2`
`log` `log2` `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` `radians` `degrees`、
`min` `max` `pow` `fmod`（行列 / スカラー）、`atan2`、`step`（`step(スカラー, 行列)` も可）、
`lerp`（`t` は行列 / スカラー）、`clamp`、`smoothstep`、`mad`、`isnan` `isinf` `isfinite`
（`boolRxC` を返す）。同じ形の行列同士のみ（`float2x3` と `float3x2` は混ぜられない）。

- 両言語共通: `abs` `sign` `min` `max` `clamp` `step` `smoothstep` `floor` `ceil` `trunc`
  `round` `sqrt` `exp` `exp2` `log` `log2` `pow` `sin` `cos` `tan` `asin` `acos` `atan`
  `sinh` `cosh` `tanh` `radians` `degrees` `modf` `isnan` `isinf`
- 幾何（両言語共通）: `distance` `reflect` `refract` `faceforward`（`dot` `cross` `length`
  `normalize` と同じくスカラー / ベクトルを返す）
- HLSL のみ: `lerp` `frac` `rcp` `rsqrt` `saturate` `atan2` `fmod` `mad` `isfinite` `sincos`
  `log10` `ldexp`（指数は float）`frexp`（指数を float で返す）`select`（HLSL 2021）`dst` `lit`
  （`lit` は HLSL と同じくスカラー引数で `float4` を返す）、float ベクトル / 行列の `%`（`fmod` と同じ）
- GLSL のみ: `mix` `fract` `inversesqrt` `roundEven` `atan(y, x)` `mod` `fma` `asinh` `acosh` `atanh`、
  `mix(x, y, bvec)`（選択子が真の成分は y）（GLSL に
  `saturate` / `rcp` は無い → 定義していない。逆も同様で、言語に無い名前はコンパイルエラー）
- 対象外: GLSL の `ldexp` / `frexp`（int ベクトルを取るため。int 型は範囲外）。HLSL 2021 の
  `and()` / `or()`（C++ の代替トークン `and` / `or` と同名で定義できない。bool ベクトルの
  `&&` / `||` も HLSL 2021 と同じく使えない）

意味論の注記:

- `round` は .5 を偶数へ丸める（`round(2.5) = 2`、`round(-0.5) = -0`）。HLSL は DXC の DXIL（`Round_ne`）・
  SPIR-V（`RoundEven`）と同じ。GLSL は .5 の向きを実装に任せているので `glsl::round` も `roundEven` と同じ
- `sign` は両言語とも同じベクトル型を返す（HLSL 本家の int 返しは採用しない）
- `min` / `max` / `saturate` の NaN 挙動は backend 依存（シェーダ言語仕様も未定義）
- `rcp` / `rsqrt` / `inversesqrt` は近似命令（`rcpps` / `rsqrtps`）を使わない正確版
- 超越関数は per-lane で libm を呼ぶ（SIMD 多項式近似はしない）。`pow(負, 非整数)` は NaN
- `select(c, a, b)` は c が真の成分で a、`mix(x, y, bvec)` は真の成分で y（引数の順が逆）
- `lerp` は `x + s*(y - x)`（DXC の DXIL と同じ）、`mix` は `x*(1 - a) + y*a`（GLSL の定義式。`mix(x, y, 1) = y` が
  x と y の大きさに関係なく正確）。違いは丸めだけ
- `hlsl::fmod` と `glsl::mod` は**意味が違う**: `fmod` は trunc 基準で結果は x の符号
  （`fmod(-7, 3) = -1`）、`mod` は floor 基準で y の符号（`mod(-7, 3) = 2`）
- 2 引数の逆正接は HLSL が `atan2(y, x)`、GLSL が `atan(y, x)`。どちらも y が先
- `mad` / `fma` は backend に FMA があれば融合（単一丸め）、無ければ乗算 + 加算
  （両言語とも `precise` 指定時以外は融合を保証しない）
- `refract` は全反射（k < 0）でゼロベクトル。`reflect` / `refract` の法線は正規化済みを前提
- `modf(x, ip)` の戻り値は x と同符号の小数部、`ip` に整数部。±inf は小数部 ±0
- `isnan` / `isinf` / `isfinite` は `bool2/3/4` を返す（`-ffast-math` 下では NaN 判定が消えうる）
- Windows で `<windows.h>` を使う場合は `NOMINMAX` が必要（`min` / `max` マクロと衝突）

## ビルドとテスト

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
./build/sandbox                                        # 遊び場
cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON  # scalar 強制
```

`-DSHADER_MATH_WARNINGS_AS_ERRORS=ON` でテスト・サンプルの警告（`-Wall -Wextra -Wpedantic` / `/W4`）をエラーにする（CI は常に ON）。

### 他プロジェクトからの利用（CMake）

ターゲットはどの方法でも `shader_math::shader_math`。

```cmake
# A) サブプロジェクト（add_subdirectory / FetchContent）
include(FetchContent)
FetchContent_Declare(shader_math
    GIT_REPOSITORY https://github.com/capsule-holdings/shader-math.git
    GIT_TAG v0.4.1)
FetchContent_MakeAvailable(shader_math)

# B) インストール済みパッケージ
#    cmake --install build --prefix <prefix> したものを CMAKE_PREFIX_PATH で指す
find_package(shader_math 0.4 REQUIRED CONFIG)

target_link_libraries(app PRIVATE shader_math::shader_math)
```

- サブプロジェクトとして取り込んだ場合、テスト・サンプル・install ルールは生成しない
  （`SHADER_MATH_BUILD_TESTS` / `SHADER_MATH_INSTALL` で上書き可）
- Boost（Config / Predef ヘッダ）はパッケージに同梱しない。`find_package` 時に利用側で探し、
  見つからなければ `SHADER_MATH_BOOST_INCLUDE`（Predef が別の場所なら
  `SHADER_MATH_BOOST_PREDEF_INCLUDE` も）を指定する
- scalar 強制: サブプロジェクトなら `-DSHADER_MATH_FORCE_SCALAR=ON`、インストール済みパッケージなら
  `find_package` の前に `set(SHADER_MATH_FORCE_SCALAR ON)`。どちらもマクロが `shader_math::shader_math`
  に付くので、リンクする全ターゲットで揃う（SIMD と scalar の翻訳単位が混ざると ODR 違反になる）。
  `SHADER_MATH_GLSL_STPQ` / `SHADER_MATH_NESTED_SWIZZLE` / `SHADER_MATH_MATRIX_SWIZZLE(3)` も同じ方法で有効にする
- pkg-config: `<prefix>/share/pkgconfig/shader_math.pc`（`pkg-config --cflags shader_math`）。Boost は
  システムの Boost を使ってインストールした場合だけ含まれる（自動取得した Boost はインストールされない）
- バージョンは 0.x の間は同一マイナーのみ互換（`SameMinorVersion`）

ベンチマーク: `./build/shader_math_bench`（SIMD）と `./build/shader_math_bench_scalar`
（scalar 強制）で主要カーネルの ns / 要素を比較できる（`--markdown` で表形式、`--json` で
github-action-benchmark の形式）。CI では `linux-x64-gcc` / `linux-x64-gcc-v3` / `linux-arm-gcc` で
実行し、ジョブサマリーに出す。`main` への push ごとの値は `gh-pages` ブランチに積み、推移のグラフを
<https://capsule-holdings.github.io/shader-math/bench/> で公開する（共有ランナーの値なので参考値。
合否判定はしない）。
ヘッダのコンパイル時間: `python3 bench/compile_time.py <コンパイラ> [--markdown]` で、ライブラリを
include するだけの翻訳単位のフロントエンド時間を既定とオプトインのマクロごとに測る。CI では
`linux-x64-gcc` / `linux-x64-clang` で実行し、ジョブサマリーに出す（同じく参考値）。

デモ: `-DSHADER_MATH_DEMO_SDLGPU=ON` で SDL_GPU + FBX（ufbx）+ CPU スキニングの
サンプルをビルドできる（[examples/sdl_gpu_skinning/](examples/sdl_gpu_skinning/README.md)）。

要件: C++20（concepts）。gcc 10+ / clang 14+ / MSVC 19.30+ / Apple Clang。
gcc 10 と clang 14 は CI で下限として確認している（clang 11〜12 はコンパイラ内部エラー、13 は誤警告が出る）。
MSVC と Apple Clang は CI ランナーの既定版のみ確認。

## 既知の逸脱（HLSL/GLSL との差分）

- **行列の複数成分 swizzle**（`_m00_m11` 等）は**オプトイン**。単成分 `_m00`〜`_m33` / `_11`〜`_44` は
  既定で全対応（範囲外は使用時にコンパイルエラー）。`SHADER_MATH_MATRIX_SWIZZLE` で 2 成分
  （`m._m00_m11`、`m._41_42`）、`SHADER_MATH_MATRIX_SWIZZLE3` で 3 成分（`m._m30_m31_m32`）も有効になる。
  どちらも行列の形の範囲内だけで、重複の無いものは書き込める。4 成分は無い（行は `m[3]`、それ以外は
  `float4(m._m00, m._m11, m._m22, m._m33)` と書く）。メンバ宣言が組み合わせで増えるため、
  `bench/compile_time.py`（全行列型を使う翻訳単位）で 2 成分は約 1.2 倍、3 成分は gcc・clang とも約 4 倍
- GLSL の `.stpq` swizzle は**オプトイン**: `SHADER_MATH_GLSL_STPQ` を定義すると有効
  （CMake では `-DSHADER_MATH_GLSL_STPQ=ON`、インストール済みパッケージは `find_package` の前に
  `set(SHADER_MATH_GLSL_STPQ ON)`。どちらも `shader_math::shader_math` に付くので全翻訳単位で揃う）。
  ベクトル型は `hlsl` と共有なので、有効にすると HLSL 側でも `.stp` が通る（HLSL には無い綴り。意味は
  `.xyz` と同じ）。既定で無効なのはこのため。コンパイル時間は gcc で +5〜11%、clang では誤差程度（実測）
- **swizzle の重ねがけ**（`v.xy.yx`、`v.xyz.zx`）は**オプトイン**: `SHADER_MATH_NESTED_SWIZZLE` を定義すると
  2・3 成分の swizzle に対して読み出しだけ有効（`.stpq` も有効ならそれも使える）。4 成分の swizzle からの
  重ねがけ（`v.wzyx.xy`）と重ねがけ先への書き込みは無い（`float4(v.wzyx).xy` と書く）。メンバ宣言が
  組み合わせで増えるため、有効にするとライブラリを include するだけの翻訳単位のフロントエンド時間が
  約 2 倍になる（`bench/compile_time.py` の実測で gcc 2.0 倍、clang 2.4 倍）。既定で無効なのはこのため
- GLSL の `not()` は C++ キーワードのため `not_()`
- `v.x` はプロキシ型（可変長引数に直接渡すときは `float(v.x)` にする）
- 対象は float と half（ベクトル）。`int` / `double`、`1xN` 行列、GPU 専用機能（テクスチャ・微分・atomic）は対象外
- 言語互換であってメモリ ABI 互換ではない（`sizeof(float3) == 16`。定数バッファのパッキングとは別物）
- 成分アクセス・swizzle は SIMD レジスタとの union による型パンニングで実装している。
  ISO C++ では未定義動作で、gcc / clang / MSVC の実装上の扱いに依存する（gcc / clang ではプロキシ型に
  `may_alias` を付けている。CI の全構成で検証）。
  このため型は `constexpr` で使えない。詳細は [CONTEXT.md](CONTEXT.md)
- `floatN`（N < 4）や行列の行・列は 4 レーンのレジスタに格納され、**N 以上のレーンの値は不定**
  （スカラー演算や除算の結果、NaN / Inf が入ることもある）。演算結果は常に有効な N レーンだけで決まる。
  シェーダ言語でも 4 番目の成分は存在しないので意味の差はないが、浮動小数点例外をトラップする
  （`feenableexcept` 等）環境では、この不定レーンで例外が上がりうる

## GLSL との相互検証

`glsl` 面は、同じ数式を glslang で SPIR-V にして Vulkan（lavapipe）で実行した結果と照合している
（列優先 ctor、`mat3(1.0)` = 単位行列、`*` = 行列積、`mod` の floor 基準など）。
実行方法と照合範囲は [docs/glsl_interop.md](docs/glsl_interop.md)。

## DXC との相互検証

HLSL 面は Slang に加えて、公式の HLSL コンパイラ DXC でも照合している。DXC で SPIR-V にして lavapipe で
実行するものと、Windows で DXIL にして D3D12 の WARP で実行するものの 2 通り。Slang では照合できない
`frexp` / `dst` もここで照合する。詳細は [docs/slang_compat.md](docs/slang_compat.md) の DXC 節。

## Slang との相互検証

同一の数式ソースを Slang（`slangc -target cpp`、CPU 実行）と shader-math の両方で
ビルド・実行して数値照合するハーネスが `tests/interop/` にある。共有可能サブセット・
除外・意味差の対応表は [docs/slang_compat.md](docs/slang_compat.md) を参照。

```bash
cmake -S . -B build-interop -DCMAKE_BUILD_TYPE=Release \
  -DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<slangリリース展開先>
cmake --build build-interop
ctest --test-dir build-interop -R slang_interop --output-on-failure
```

## ライセンス

[MIT](LICENSE)
