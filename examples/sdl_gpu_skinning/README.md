# SDL_GPU スキニングデモ

FBX（ufbx）からスキンメッシュとアニメーションを読み込み、**CPU 側のリニアブレンド
スキニングを shader-math（hlsl 面、SIMD の `mul` / `normalize`）で計算**して、
SDL_GPU（Vulkan / SPIR-V）でフラットシェーディングまたはワイヤーフレーム描画する
最小デモ。

```
FBX --ufbx--> rest pose + weights + anim
              各フレーム: palette[j] = node_to_world(t) * geometry_to_bone   (float4x4 mul)
                          pos' = Σ w_j * mul(palette[j], float4(pos, 1))     (shader-math SIMD)
              SDL_GPU: 頂点バッファ更新 -> viewProj は頂点シェーダ（Slang -> SPIR-V）
```

## ビルド

```bash
cmake -S . -B build-demo -DCMAKE_BUILD_TYPE=Release -DSHADER_MATH_DEMO_SDLGPU=ON
cmake --build build-demo -j
```

SDL3（release-3.4.14）と ufbx（v0.23.0）は FetchContent で取得する。Linux では X11
開発パッケージ（`libx11-dev` 等）が SDL のビルドに必要。

## 実行

```bash
./build-demo/sdl_gpu_skinning                 # ウィンドウ表示（要ディスプレイ）
./build-demo/sdl_gpu_skinning --wireframe     # ワイヤーフレーム
# ヘッドレス（ソフトウェア Vulkan / lavapipe で動作確認済み）:
SDL_VIDEO_DRIVER=offscreen ./build-demo/sdl_gpu_skinning --headless 24 --out /tmp/frames
./build-demo/sdl_gpu_skinning --fbx path/to/your.fbx   # 任意の FBX（スキン付き最初のメッシュ）
```

## ファイル

- `main.cpp` — 全実装（FBX ロード、CPU スキニング、SDL_GPU 描画、ヘッドレス BMP 出力）
- `shader.slang` — 頂点/フラグメントシェーダ（Slang）。`build_shaders.sh` で SPIR-V を再生成
  （コミット済み `.spv` を使うため通常は不要）。SPIR-V のエントリ名は Slang により
  `main` へリネームされる点に注意
- `gen_asset.py` — 同梱アセット `tentacle.fbx`（3 ボーンの触手 + スウェイアニメ）を
  Blender で生成するスクリプト。アセットは本リポジトリ生成物でライセンスはリポジトリに従う

## 外部 FBX でのスモークテスト

デモは同梱アセット以外も読める（スキンなしメッシュは剛体としてノード変換で描画、
複数メッシュ対応、カメラは初期ポーズの AABB から自動フレーミング）。エクスポータ差の
スモークテストとして、ufbx テストスイート（MIT）から Blender / Maya / 3ds Max 産の
3 ファイルを取得して描画するスクリプトを同梱:

```bash
SDL_VIDEO_DRIVER=offscreen ./examples/sdl_gpu_skinning/test_external_assets.sh \
    ./build-demo/sdl_gpu_skinning /tmp/ext_assets
```

（ファイルはスクラッチディレクトリへダウンロードするのみで、リポジトリには含めない）

## 制約（意図的）

- シェーダは SPIR-V のみコミット（Vulkan バックエンド前提）。D3D12 / Metal で動かす場合は
  Slang から DXIL / MSL を生成して `load_shader` の format を切り替える
- スキニングは毎フレーム全頂点を CPU で変換する素朴な実装（shader-math のデモが目的。
  実運用では GPU スキニングかパレットの頂点シェーダ渡しにする）
