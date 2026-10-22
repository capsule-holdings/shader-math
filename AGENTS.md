# AGENTS.md

## 共通ルール（Cursor / Claude ほか全エージェント）

### Git / PR

- **PR は必ず `main` に向ける**（base = main）。スタックする場合も中間ブランチを base にせず、
  全 PR を main 向きにしてマージ順で管理する（先行マージで後続の差分は自動的に縮小する）。
  中間ブランチへマージすると内容が main に届かず迷子になる。
- **PR は squash マージする**。squash 時のコミットメッセージ（タイトル・説明）は
  その時点の PR 内容に合わせて都度書き直すこと（古い説明の使い回し禁止）。
- **`main` への `push -f`（force push）は禁止**。

## Cursor Cloud specific instructions

`shader-math` はヘッダオンリーの C++20 ライブラリ（CMake、Boost はヘッダのみ利用）。
常駐するサーバ・Web アプリは無く、「アプリケーション」に相当するのは
テストスイート（`shader_math_tests` / `shader_math_tests_scalar`）、
遊び場実行ファイル `sandbox`、ベンチマーク（`shader_math_bench` / `shader_math_bench_scalar`）。

### ビルド・テスト・実行

標準手順は `README.md`（「ビルドとテスト」節）と `CMakeLists.txt` を参照。要点のみ:

- 通常ビルド + テスト: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build --output-on-failure`
- アプリ実行: `./build/sandbox`
- scalar 強制ビルド: `cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON`
- Slang 相互検証（任意、slangc が必要）: Slang リリース（v2026.16 で検証済み）を展開して
  `cmake -S . -B build-interop -DCMAKE_BUILD_TYPE=Release -DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<展開先>`
  → build 後 `ctest --test-dir build-interop -R slang_interop --output-on-failure`。
  サブセット定義は `docs/slang_compat.md`、ケース本体は `tests/interop/shared/math_cases.hlsli`。
- テストバイナリを直接実行するとバックエンド名（例: `SSE2+FMA`、`NEON+FP16`）と
  `N checks, 0 failures` を出力する。件数はテスト追加で増えるので固定値で比べないこと。
- 既定ビルドの `ctest` は 5 件: SIMD 版と scalar twin（`shader_math_tests_scalar`）、
  codegen チェック（Release かつ非 MSVC のみ。objdump が無ければ skip）、`sandbox`、`skin_batch_check`。
- CI は全ジョブ `-DSHADER_MATH_WARNINGS_AS_ERRORS=ON`（gcc/clang は `-Wall -Wextra -Wpedantic -Werror`、
  MSVC は `/W4 /WX`）。手元でも付けて確認すること。
- ベンチマーク: `./build/shader_math_bench` と `./build/shader_math_bench_scalar`（`--markdown` で表）。
  参考値であり合否判定はしない。
- SDL_GPU デモ（任意）: `-DSHADER_MATH_DEMO_SDLGPU=ON`。ヘッドレスなら `-DSDL_UNIX_CONSOLE_BUILD=ON` で
  X11 不要、lavapipe（`mesa-vulkan-drivers`）上で `--headless N --out <dir>`（出力先は事前に作成）。

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
