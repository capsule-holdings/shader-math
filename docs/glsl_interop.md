# GLSL ⇔ shader-math 相互検証

同一の数式ソース（`tests/interop/glsl/math_cases.glsl`）を
(a) glslang で SPIR-V にして Vulkan（CI では lavapipe = llvmpipe）で実行、
(b) C++（shader-math の `glsl` 面）でビルド・実行し、数値を照合する。
HLSL 側の Slang 相互検証（[slang_compat.md](slang_compat.md)）の GLSL 版。

## 実行方法

```bash
# Ubuntu: glslang-tools libvulkan-dev mesa-vulkan-drivers python3
cmake -S . -B build-glsl -DCMAKE_BUILD_TYPE=Release -DSHADER_MATH_INTEROP_GLSL=ON
cmake --build build-glsl
ctest --test-dir build-glsl -R glsl_interop --output-on-failure
```

CI は `linux-x64-glsl-interop` ジョブで実行する。

## 照合している GLSL 固有の意味

| ケース | 内容 |
| --- | --- |
| `exact_ctor_layout` | 行列 ctor は列優先、`matCxR` は C 列 × R 行（`mat2x3` / `mat3x2`） |
| `exact_ctor_special` | `mat3(s)` は対角、`mat4(mat2)` は単位行列で拡大、`mat2(mat3)` は左上を切り出し、ベクトルの連結 ctor |
| `exact_componentwise` | `matrixCompMult`、非正方の `transpose`、`outerProduct`、成分ごとの関数 |
| `exact_rounding` | `roundEven`（.5 は偶数へ）、`trunc` `ceil` `min` `max` `clamp` `step` |
| `exact_compare` | ベクトル / 行列の `==` `!=` はスカラー bool、`lessThan` `equal` `greaterThanEqual` + `all` `any` |
| `near_matrix_product` | `*` は行列積、`v * m` は行ベクトル形、非正方の積（`mat3x2 * mat2x3`） |
| `near_inverse` | `inverse`、`determinant` |
| `near_mod` | `mod` は floor 基準で y の符号（HLSL の `fmod` と逆）、`mix`、`modf`、`fma` |
| `near_geometry` | `reflect` `refract` `faceforward` `distance` `inversesqrt` `smoothstep` |
| `approx_transcend` | `atan(y, x)`、逆三角・三角・指数・対数・`pow`・`radians` `degrees` |

## 許容誤差（名前の接頭辞で決まる）

- `exact_`: ビット一致。1 回丸めの演算だけを置く（GLSL コンパイラは `a * b + c` を融合してよく、
  除算や超越関数の精度も緩い）
- `near_`: 相対 1e-5（積和・除算）
- `approx_`: 相対 1e-4（超越関数。lavapipe の実装は libm と 1e-5 程度ずれる）

## 共有ファイルの書き方

- GLSL 4.50 と C++ の両方で通る書き方に限る（`f` 付きリテラル、`out` 引数を持つ関数を
  定義しない、組み込み関数はベクトル引数のみ）
- 入力は非正規化数を避ける（Vulkan 実装がフラッシュしうる）
