# HLSL / GLSL / shader-math 対応表

シェーダで書けるものが、shader-math（`hlsl::` / `glsl::`）でどう書けるかの一覧。対象は float / half の
ベクトル・行列とその演算・組み込み関数（int / double・テクスチャなどは末尾の「対象外」）。

- 記号: **○** そのまま書けて同じ意味 / **△** 書けるが規則か書き方が違う（注記） / **×** 言語では書けるが
  shader-math では書けない（理由） / **−** その言語に無い（shader-math でもエラーになる）
- 「書けるか・エラーになるか」は `tests/conformance/` のケース集で DXC（`-HV 2021 -enable-16bit-types`）
  と glslang に照らして確かめている（[conformance.md](conformance.md)）。値の一致は `tests/interop/` の照合
  ハーネス（[slang_compat.md](slang_compat.md)、[glsl_interop.md](glsl_interop.md)）
- 1 つのスコープで `using namespace` するのは片方の言語だけ（`mul` / `transpose` などが衝突する）

## 型

| 概念 | HLSL | GLSL | shader-math | 注記 |
| --- | --- | --- | --- | --- |
| float ベクトル | `float2` 〜 `float4` | `vec2` 〜 `vec4` | ○ / ○ | `hlsl::float3` と `glsl::vec3` は別型。変換は explicit（`glsl::vec3(f)`） |
| テンプレート表記 | `vector<float, 3>` | − | ○ | `hlsl::vector<T = float, N = 4>`。`<>` 無しの `vector` は △（clang は可、gcc は不可） |
| half ベクトル | `half2` 〜 `half4`（16 ビット型有効時） | `f16vec2` 〜 `f16vec4`（`GL_EXT_shader_explicit_arithmetic_types`） | ○ / ○ | C++ で使える `_Float16` が必要（README の「型」） |
| half スカラー | `half` | `float16_t` | ○ / △ | ARM では `arm_neon.h` の `::float16_t` と曖昧になるので `glsl::float16_t` と書く |
| bool ベクトル | `bool2` 〜 `bool4` | `bvec2` 〜 `bvec4` | ○ / ○ | 比較結果の型。算術は無い（どちらの言語も同じ） |
| 行列 | `floatRxC`（R 行 C 列） | `matCxR`（C 列 R 行） | ○ / ○ | `float3x4` と `mat4x3` が同じ形。両者は別型で変換は explicit |
| half 行列 | `halfRxC` | `f16matCxR` | ○ / ○ | |
| bool 行列 | `boolRxC` | − | ○ / − | HLSL の行列比較の結果 |
| 行列のテンプレート表記 | `matrix<float, R, C>` | − | × | `hlsl::matrix<R, C, T>` と引数の順が違う |
| 1 成分・1xN | `float1`、`float1x3` など | − | × / − | |
| 型の言語 | − | − | `hlsl::lang_tag` / `glsl::lang_tag` | shader-math の内部用。演算子と言語固有の関数を ADL で見つけるため |

## コンストラクタと変換

| 書き方 | HLSL | GLSL | shader-math | 注記 |
| --- | --- | --- | --- | --- |
| スカラーを並べる `float3(1, 2, 3)` / `vec3(1.0, 2.0, 3.0)` | ○ | ○ | ○ / ○ | int の引数も変換される |
| ベクトルとスカラーを混ぜる `float4(v2, 0, 1)` | ○ | ○ | ○ / ○ | |
| 成分数ちょうどでない `vec3(v2, w2)` | エラー | ○（最後の引数だけ余ってよい） | ○ / ○ | GLSL は全引数が使われる必要がある（`vec2(v2, s)` はエラー）。shader-math も同じ |
| 1 つのスカラー `float3(s)` / `vec3(s)` | エラー | ○（全成分） | △ / ○ | HLSL はキャスト `(float3)s` と暗黙変換 `float3 v = s` なら可。C++ では同じコンストラクタを通るので `float3(s)` も通る |
| スカラーの暗黙変換 `float3 v = 1.0` | ○ | エラー | ○ / ○ | GLSL は `vec3(1.0)` と書く |
| 切り詰め `float3(v4)` / `vec3(v4)` | エラー（キャスト `(float3)v4` は可） | ○ | △ / ○ | HLSL の `float3(v4)` は C++ ではキャストと区別できず通る |
| 暗黙の切り詰め `float3 v = v4` | ○（DXC は警告） | エラー | × / ○ | 意図して弾いている（`(float3)v4` / `float3(v4)` と書く） |
| 拡張 `float4 v = v3` | エラー | エラー | ○ / ○ | |
| 行列: スカラーを並べる | ○（行優先で埋まる） | ○（列優先で埋まる） | ○ / ○ | |
| 行列: 行 / 列ベクトル `float2x2(r0, r1)` / `mat2(c0, c1)` | ○ | ○ | ○ / ○ | |
| 行列: 混ぜる `float2x2(v2, 3, 4)` / `mat2(v2, 3.0, 4.0)` | ○（成分数ちょうど） | ○（最後の引数だけ余ってよい） | ○ / ○ | `mat2(v4)`、`mat2(v3, v3)` も GLSL では可 |
| 行列: 1 つのスカラー | エラー（キャストと暗黙変換は全成分） | ○（対角、`mat3(1.0)` = 単位行列） | △ / ○ | HLSL の `float3x3(s)` は C++ では通る |
| 行列: ベクトル 1 つから暗黙に `float2x2 m = v4` | ○ | エラー | ○ / ○ | |
| 行列 → 行列 | 縮小キャスト `(float2x2)m3` | 拡大（単位行列で補完）・縮小 `mat2(m3)` | ○ / ○ | HLSL の `float2x2(m3)`（エラー）は C++ では △ で通る |
| 行列 → ベクトル `float4 v = m22` / `vec4(m2)` | ○ | ○ | × / × | |
| half と float の明示変換 `half3(f)` / `f16vec3(v)` | ○ | ○ | ○ / ○ | |
| half と float の暗黙変換・混ぜた演算 `h * a`、`float3 v = h` | ○ | 一部○（f16 → f32 方向） | × / × | 意図して弾いている（丸めが混ざるため） |
| half と float のベクトルを 1 つの ctor に混ぜる `float4(v2, h2)` | ○ | ○ | × / × | 未対応 |
| bool と float の明示変換 `float3(b)` / `bool3(v)` | ○ | ○ | ○ / ○ | 0 以外が真（NaN も真）、真は 1 |
| bool と float の暗黙変換 `float3 v = b` | ○ | エラー | × / ○ | 意図して弾いている |
| bool の暗黙 splat `bool3 b = true` | ○ | エラー | ○ / ○ | |
| 初期化子リスト `float3 v = {1, 2, 3}` | ○ | ○ | ○ / ○ | HLSL の平坦化 `float4 v = {v3, 1}`、`float2x2 m = {v2, 3, 4}` も可。GLSL の `mat2 m = {v2, 3.0, 4.0}` はエラー（shader-math も同じ） |
| C 形式のキャスト `(float3)x` | ○ | エラー（`vec3(x)` と書く） | ○ / △ | GLSL の `(vec3)x` は C++ の文法なので通ってしまう |
| HLSL ⇔ GLSL の変換 | − | − | 明示のみ | `glsl::vec3(f)`、`hlsl::float3x3(gm)`（転置して同じ数学的対象） |

## 成分アクセス

| 書き方 | HLSL | GLSL | shader-math | 注記 |
| --- | --- | --- | --- | --- |
| `.xyzw` / `.rgba`（全組み合わせ、読み書き） | ○ | ○ | ○ / ○ | 集合をまたぐ `a.xg`、範囲外、重複への書き込みはエラー（両言語・shader-math とも） |
| `.stpq` | − | ○ | − / ○ | |
| swizzle への複合代入 `v.yz += w` | ○ | ○ | ○ / ○ | |
| swizzle の重ねがけ `v.xy.yx` | ○ | ○ | △ / △ | `SHADER_MATH_NESTED_SWIZZLE` でオプトイン。2・3 成分の swizzle の読み出しのみ |
| スカラーへの swizzle `s.xxx` | ○ | ○ | × / × | C++ の float にメンバは無い |
| 1 成分への `++` `v.x++` | ○ | ○ | × / × | |
| 添字 `v[i]` | ○ | ○ | ○ / ○ | |
| 行列の添字 `m[i]` | ○（行） | ○（列） | ○ / ○ | |
| `m._m00` / `m._11` | ○ | − | ○ / − | |
| 行列の複数成分 `m._m00_m11` | ○ | − | △ / − | `SHADER_MATH_MATRIX_SWIZZLE`（2 成分）/ `SHADER_MATH_MATRIX_SWIZZLE3`（3 成分）でオプトイン |
| `.length()` | − | ○（成分数、行列は列数） | − / ○ | |
| bool ベクトルの成分の読み出し `b.x` / `b[i]` | ○ | ○ | ○ / ○ | |
| bool ベクトルの成分への書き込み `b.x = true` | ○ | ○ | × / × | 読み出しのみ |
| 可変長引数に `v.x` を渡す | − | − | △ | `v.x` はプロキシ型なので `printf` などには `float(v.x)` |

## 演算子

| 書き方 | HLSL | GLSL | shader-math | 注記 |
| --- | --- | --- | --- | --- |
| `+ - * /`（ベクトル同士、ベクトルとスカラー） | ○ | ○ | ○ / ○ | float ベクトルには int / double のスカラーも可。half ベクトルには half のスカラーのみ（HLSL の `h * 2.0` は ×） |
| 単項 `-` `+` | ○ | ○ | ○ / ○ | bool ベクトルの `+b` は × |
| `++` `--`（ベクトル・行列） | ○ | ○ | ○ / ○ | |
| 幅の違うベクトルの演算 `a3 + a4` | ○（切り詰め、DXC は警告） | エラー | × / ○ | 意図して弾いている |
| `%`、`%=`（float） | ○（`fmod` と同じ。x の符号） | − | ○ / − | GLSL は `mod()`（y の符号） |
| `==` `!=` | 成分ごとの bool ベクトル | 全成分一致の bool | ○ / ○ | `using` なしでも型に合った意味で見つかる |
| `<` `<=` `>` `>=` | 成分ごとの bool ベクトル | ベクトルには無い（`lessThan()` など） | ○ / ○ | |
| `if (a == b)`（ベクトル） | エラー（`all()` / `any()`） | ○（bool） | ○ / ○ | |
| bool ベクトルの `&&` `\|\|`、三項演算子の条件 | エラー（`and()` / `or()` / `select()`） | エラー（`mix(x, y, b)`） | ○ / ○ | HLSL の `and()` / `or()` は C++ の代替トークンと同名で × |
| bool ベクトルの `!` | ○ | エラー（`not()`） | ○ / ○ | GLSL の `not()` は C++ のキーワードなので `not_()`（△） |
| 行列同士の `*` | 成分ごと（同じ形のみ） | 行列積（内側の次元が一致） | ○ / ○ | 行列積は HLSL では `mul()` |
| 行列とベクトルの `*` | エラー（`mul()`） | `m * v` / `v * m` | ○ / ○ | |
| `mul(a, b)`（9 形） | ○ | − | ○ / − | 次元の違う `mul(v4, m3)` / `mul(m3, m23)` は DXC が切り詰めて通すが shader-math は × |
| 複合代入 `+= -= *= /=` | ○ | ○ | ○ / ○ | GLSL の `m *= n`（正方）と `v *= m` も可。HLSL の `v *= m` はエラー |

## 組み込み関数

ベクトル版はすべて、スカラー（`float` / `half`、`v.x`、`m._11`）にも使える。行列への成分ごと適用は HLSL のみ
（GLSL の関数は genType だけを取る）。

| HLSL | GLSL | shader-math | 注記 |
| --- | --- | --- | --- |
| `abs` `sign` `floor` `ceil` `trunc` | 同名 | ○ / ○ | HLSL の `sign` は int を返すが、shader-math は同じ型の float を返す（`float3 r = sign(a)` はどちらも可） |
| `round` | `round` / `roundEven` | ○ / ○ | .5 は偶数へ（DXC と同じ。GLSL の `round` は向きが実装依存なので `roundEven` と同じにしている）。スカラーの修飾なし `round(s)` は libc++ / MSVC で曖昧（△、`hlsl::round(s)` と書く） |
| `frac` | `fract` | ○ / ○ | |
| `fmod`（trunc 基準、x の符号） | `mod`（floor 基準、y の符号） | ○ / ○ | 意味が違う。`fmod(-7, 3) = -1`、`mod(-7, 3) = 2` |
| `min` `max` `clamp` | 同名 | ○ / ○ | NaN は IEEE minNum / maxNum（D3D の規則）。全 backend で同じ |
| `saturate` | − | ○ / − | GLSL は `clamp(x, 0.0, 1.0)` |
| `lerp`（`a + t*(b - a)`） | `mix`（`x*(1 - a) + y*a`） | ○ / ○ | 違いは丸めだけ |
| − | `mix(x, y, bvec)` | − / ○ | 真の成分で y |
| `select(c, a, b)`（HLSL 2021） | − | ○ / − | 真の成分で a（`mix` と引数の順が逆）。スカラーの条件も可 |
| `step` `smoothstep` | 同名 | ○ / ○ | `step(edge, x)` の引数順は両言語とも同じ |
| `sqrt` | 同名 | ○ / ○ | |
| `rsqrt` | `inversesqrt` | ○ / ○ | 近似命令を使わない正確版 |
| `rcp` | − | ○ / − | 正確な 1/x |
| `exp` `exp2` `log` `log2` `pow` | 同名 | ○ / ○ | libm をレーンごとに呼ぶ。`pow(負, 非整数)` は NaN |
| `log10` | − | ○ / − | |
| `sin` `cos` `tan` `asin` `acos` `atan` `sinh` `cosh` `tanh` | 同名 | ○ / ○ | |
| − | `asinh` `acosh` `atanh` | − / ○ | |
| `atan2(y, x)` | `atan(y, x)` | ○ / ○ | どちらも y が先 |
| `degrees` `radians` | 同名 | ○ / ○ | |
| `mad` | `fma` | ○ / ○ | FMA のある backend では融合。HLSL の `fma` は double 専用（float ではエラー、shader-math も同じ） |
| `modf(x, out ip)` | 同名 | ○ / ○ | ±inf の小数部は ±0 |
| `frexp(x, out e)`（指数は float） | `frexp(x, out ivec e)` | ○ / × | GLSL 版は int ベクトルを取る（対象外） |
| `ldexp(x, e)`（指数は float） | `ldexp(x, ivec e)` | ○ / × | 同上 |
| `isnan` `isinf` | 同名 | ○ / ○ | スカラーも可。`-ffast-math` では NaN 判定が消えうる |
| `isfinite` | − | ○ / − | |
| `sincos(x, out s, out c)` | − | ○ / − | |
| `dot` `cross` `length` `distance` `normalize` `reflect` `refract` `faceforward` | 同名 | ○ / ○ | スカラー版も可。`refract` は全反射でゼロ |
| `transpose` `determinant` | 同名 | ○ / ○ | |
| −（`inverse` は無い） | `inverse` | △ / ○ | HLSL 側は拡張として `hlsl::inverse` がある（HLSL ではエラーになる書き方） |
| − | `matrixCompMult` `outerProduct` | − / ○ | HLSL の成分ごと積は `*` |
| `all` `any`（数値・bool のベクトル・行列・スカラー） | `all` `any`（bvec のみ） | ○ / ○ | 数値は 0 以外が真（NaN も真）。HLSL のスカラーの bool `all(c)` は × |
| − | `lessThan` `lessThanEqual` `greaterThan` `greaterThanEqual` `equal` `notEqual` | − / ○ | |
| − | `not` | − / △ | `not_()`（C++ のキーワード） |
| `dst` `lit` | − | ○ / − | `lit` はスカラー引数で `float4` を返す |
| `abs(m)` など行列への成分ごと適用 | − | ○ / − | `abs` `sign` `floor` `ceil` `trunc` `round` `frac` `saturate` `sqrt` `rsqrt` `rcp` `exp` `exp2` `log` `log2` 三角・双曲線 `radians` `degrees` `min` `max` `pow` `fmod` `atan2` `step` `lerp` `clamp` `smoothstep` `mad` `isnan` `isinf` `isfinite` |

C++ の文法による違い:

- double のリテラルだけの呼び出し（`max(1.0, 2.0)`）はスカラー版にならない（`1.0f` と書く。float の引数や
  ベクトルがあれば `1.0` も可）
- `out` / `inout` 引数は参照で受ける（`modf(x, ip)` の `ip` は左辺値）

## 互換のために許しているもの

C++ では区別できないので、言語ではエラーになるのに通るもの:

| 書き方 | 言語 | 理由 |
| --- | --- | --- |
| `float3(s)`、`float3(v4)`、`float3x3(s)`、`float2x2(m3)` | HLSL | 同じ変換をキャスト（`(float3)v4`）や暗黙変換（`float3 v = s`）で書くのは正しい HLSL。C++ ではどれも同じコンストラクタを通る |
| `(vec3)x` | GLSL | C 形式のキャストは C++ の文法 |
| `if (s)`、`!s`、float の `&&`、float の条件の `?:`、`int i = 1.5` | GLSL | スカラーは C++ の `float` そのもので、C++ の bool への変換・縮小変換が効く |
| `vec3{1, 2, 3}`、`float3{1, 2, 3}` | 両方 | C++ の波括弧初期化（どちらの言語にも無い） |

C++ の文法のために書けないもの: スカラーへの swizzle（`s.xxx`）、`float1`、`not()` / `and()` / `or()`、double のリテラル
だけの呼び出し（`max(1.0, 2.0)`）、HLSL のスカラーの `s % t`（`fmod` と書く）、可変長引数への `v.x`、`out` / `inout`
（参照で受ける）。

値だけに出る差: スカラーだけの式の `1.0` は C++ では double なので、`s * (1.0 / 3.0)` は double で計算されて
から float に丸まる（シェーダは float で計算する）。ベクトルとの演算はスカラーを要素型に変換してから計算するので
この差は出ない。

## 言語に無い機能

| 機能 | 有効になる条件 | 注記 |
| --- | --- | --- |
| `hlsl::inverse` | 常に | HLSL に無い（DXC はエラー）。GLSL の `inverse` は公式 |
| `hlsl::matrix::identity()` | 常に | `float4x4::identity()`。シェーダの書き方とは紛れない |
| `v.store(p)`（ベクトルのメンバー） | 常に | 成分を `p[0..N)` に書く |
| HLSL ⇔ GLSL の明示変換 | 常に | `glsl::vec3(f)`、`hlsl::float3x3(gm)` |
| `using` なしの ADL | 常に | 書ける式は増えない（シェーダには名前空間が無い） |
| 言語が決めていない挙動の固定 | 常に | `glsl::round` の偶数丸め、`min` / `max` の NaN、`rcp` / `rsqrt` の精度、`sign` の戻り値の型 |
| 内部のメンバー | 常に（public） | `q_` `comp_` `row_` `col_` `flat_` `m_` とレジスタからのコンストラクタ。union の都合で public。利用者向けではない |
| 出力・`std::format`・構造化束縛・`std::hash`・`load` / `store` | `#include <shader_math/std.hpp>` | |
| swizzle の重ねがけ、行列の複数成分アクセス | `SHADER_MATH_NESTED_SWIZZLE` / `SHADER_MATH_MATRIX_SWIZZLE(3)` | 言語にある機能。コンパイル時間のためオプトイン |
| scalar backend の強制、`hlsl` / `glsl` のグローバル別名の抑止 | `SHADER_MATH_FORCE_SCALAR` / `SHADER_MATH_NO_ALIAS` | |

言語をまたいだ修飾付きの呼び出し: 両言語に共通の関数（`dot`、`abs` など）は `hlsl::dot(glsl のベクトル)` のように
修飾して呼ぶともう片方の言語のベクトルでも通る（言語固有の `hlsl::frac` などは通らない）。

## 処理系による差

コンパイルが通るかどうか:

| 項目 | 差 |
| --- | --- |
| 修飾なしのスカラーの `round(s)` | libc++ と MSVC はグローバルの `::round(float)` があるので曖昧（エラー）。libstdc++ は通る |
| `float16_t` | ARM では `arm_neon.h` の `::float16_t` と曖昧 |
| `vector r = v4`（`<>` なし） | clang は通る、gcc は通らない |
| half の型 | C++ で使える `_Float16` が要る（x86: gcc 12+ / clang 15+、AArch64: gcc 13+）。MSVC はソフトウェア実装。無ければ型が無い |
| `std::format` | `<format>` がある環境だけ（gcc 13+ など） |
| `<windows.h>` | `min` / `max` マクロと衝突するので `NOMINMAX` が要る |

値:

- FMA の有無（SSE2 だけか、`x86-64-v3` / NEON か）で、`mad` / `fma`、`lerp`、`smoothstep`、行列積の丸めが変わる
- 利用者のスカラーの式の融合（`-ffp-contract`）はコンパイラによる（clang は既定で融合、gcc は ISO モードでは
  しない）
- 超越関数は各処理系の libm（glibc / Apple / UCRT）なので ulp 単位で違う。GPU とも違う
- half は ARMv8.2 の FEAT_FP16 では 1 演算ごとに fp16 で丸める。それ以外は f32 に広げて計算して戻す（丸めが 2 回）
- `-ffast-math` では NaN の判定や `min` / `max` の NaN の規則が崩れうる（DXC の DXIL も fast-math）
- 非正規化数: CPU は既定で扱う。GPU は 0 にすることが多い
- `floatN`（N < 4）の使わないレーンは値が不定。浮動小数点例外をトラップする設定では例外が上がりうる

表現: 成分アクセスは union の型パンニング（ISO C++ では未定義動作）で、gcc / clang / MSVC の扱いに依存する
（`constexpr` にできない）。`sizeof(float3) == 16` は言語上の互換で GPU の ABI ではない。シェーダのコンパイラ同士
にも差がある（Slang の CPU の `round`、DXC の SPIR-V の `lit`、DXIL の `frexp`。[slang_compat.md](slang_compat.md)）。
shader-math は DXC と公式のドキュメントに合わせる。

## 対象外

| 分類 | 例 |
| --- | --- |
| int / uint / double の型 | `int3` `uint` `ivec3` `dvec3`、`1.0L` / `1.0lf` |
| ビット表現 | `asfloat` `asuint` `f32tof16` `floatBitsToInt` `packHalf2x16` `countbits` `bitCount` |
| GPU 専用 | 微分（`ddx` / `dFdx`）、テクスチャ、wave / subgroup、atomic、`clip` / `discard` |
| 修飾子・レイアウト | `precise`、semantics、`cbuffer` / `uniform` のパッキング（`sizeof(float3) == 16` は GPU の ABI ではない） |

## 準拠率（2026-10-25）

`tests/conformance/` の 529 ケース（HLSL 280、GLSL 249）で、言語と同じものを受け付け・弾く割合。
対象外の分類を除いた値。

| | v0.5.0 | main（`a7e4dfa`） |
| --- | --- | --- |
| HLSL（271） | 78.2% | 88.6% |
| GLSL（242） | 81.8% | 94.2% |

g++ 13（Linux）で測った値。Apple Clang では GLSL が 93.8%（libc++ の `::round(float)` でスカラーの
`round(s)` が曖昧になる 1 件）。残りの不一致は上の表の × と △ に対応する。測り直しは
`python3 tests/conformance/run.py --cxx <コンパイラ>`。
