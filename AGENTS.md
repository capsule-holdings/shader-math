# AGENTS.md

## Cursor Cloud specific instructions

`shader-math` はヘッダオンリーの C++20 ライブラリ（CMake、Boost はヘッダのみ利用）。
常駐するサーバ・Web アプリは無く、「アプリケーション」に相当するのは
テストスイート（`shader_math_tests` / `shader_math_tests_scalar`）と
遊び場実行ファイル `sandbox`。

### ビルド・テスト・実行

標準手順は `README.md`（「ビルドとテスト」節）と `CMakeLists.txt` を参照。要点のみ:

- 通常ビルド + テスト: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build --output-on-failure`
- アプリ実行: `./build/sandbox`
- scalar 強制ビルド: `cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON`
- テストバイナリを直接実行するとバックエンドと `N checks, 0 failures` を出力する（例: `SSE, 397 checks`）。
  既定ビルドでは `ctest` が SIMD 版と scalar twin（`shader_math_tests_scalar`）の 2 つを実行する。

### 非自明な注意点

- **既定コンパイラ `c++` は clang++（clang-18）**。CI は g++ / clang++ の両方を明示指定してビルドするので、
  両対応を確認するときは `-DCMAKE_CXX_COMPILER=g++` / `-DCMAKE_CXX_COMPILER=clang++` を明示すること。
- clang が GCC ツールチェイン（libstdc++）を参照するため **`libstdc++-14-dev` が必要**。
  無いと既定コンパイラでのリンクが `cannot find -lstdc++` で失敗する（update script で導入済み）。
- **Boost はヘッダのみ**。`libboost-dev` が入っていれば CMake はそれを使う（update script で導入済み）。
  未インストール時は `CMakeLists.txt` の `FetchContent` が boost config/predef を git 取得するためネットワークが必要。
- **lint / format 用の設定は無い**（`.clang-tidy` / `.clang-format` なし）。独立した lint ステップは存在せず、
  検証はビルド成功とテスト通過で行う。
- 生成物ディレクトリ `build/` `build-*/` は `.gitignore` 済み。
