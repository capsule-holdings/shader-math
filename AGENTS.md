# AGENTS.md

## Rules for every agent

### Git / PR

- **Every PR targets `main`** (base = main). When PRs stack, do not use an intermediate branch as the
  base: point every PR at main and manage them by merge order (merging the earlier PR shrinks the later
  diffs on its own). A PR merged into an intermediate branch never reaches main.
- **PRs are squash-merged.** Rewrite the squash commit's title and description to match the PR as it is
  at merge time (do not reuse an outdated description).
- **No `push -f` (force push) to `main`.**
- **The repository's primary language is English.** Code, comments, commit messages, PR descriptions and
  documentation (`docs/`, `CONTEXT.md`) are in English.
- **There are two READMEs: `README.md` (English, canonical) and `README.ja.md` (Japanese translation).**
  When user-facing content changes (types, functions, semantic differences, usage), update `README.md`
  and bring `README.ja.md` in line with it. The `docs` workflow fails when `README.ja.md` no longer has
  the same headings, code blocks and table rows as `README.md`, or when a relative link is broken
  (`python3 .github/scripts/check_docs.py` runs the same checks locally).
- **Never create a release tag (`vX.Y.Z`) without a human's confirmation.** The procedure and its
  automated checks are in the "Versions and releases" section of CONTEXT.md. When the version changes,
  change both `project(VERSION)` and `include/shader_math/version.hpp`.

## Development environment

`shader-math` is a header-only C++20 library (CMake; Boost is used header-only).
There is no server or web application; the closest things to an "application" are the
test suites (`shader_math_tests` / `shader_math_tests_scalar`), the playground executable
`sandbox`, and the benchmarks (`shader_math_bench` / `shader_math_bench_scalar`).

### Build, test, run

The standard steps are in `README.md` ("Build and test") and `CMakeLists.txt`. In short:

- Build and test: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build --output-on-failure`
- Run the playground: `./build/sandbox`
- Scalar-only build: `cmake -S . -B build-scalar -DSHADER_MATH_FORCE_SCALAR=ON`
- Slang cross-check (optional, needs slangc): unpack the Slang release pinned by `slang_tag` in `ci.yml`, then
  `cmake -S . -B build-interop -DCMAKE_BUILD_TYPE=Release -DSHADER_MATH_INTEROP_SLANG=ON -DSHADER_MATH_SLANG_DIR=<unpacked dir>`,
  build, and run `ctest --test-dir build-interop -R slang_interop --output-on-failure`.
  The subset is defined in `docs/slang_compat.md`; the cases are in `tests/interop/shared/math_cases.hlsli`.
- GLSL cross-check (optional, needs glslang-tools / libvulkan-dev / mesa-vulkan-drivers):
  `-DSHADER_MATH_INTEROP_GLSL=ON`, then `ctest -R glsl_interop`. The cases are in
  `tests/interop/glsl/math_cases.glsl`; details in `docs/glsl_interop.md`.
- DXC cross-check (optional): the SPIR-V variant on Linux with `-DSHADER_MATH_INTEROP_DXC=ON`
  (`ctest -R dxc_interop`), the DXIL variant on Windows with `-DSHADER_MATH_INTEROP_DXIL=ON`
  (`ctest -R dxil_interop`, run on D3D12 WARP). Both take `-DSHADER_MATH_DXC_DIR=<unpacked DXC release>`.
  Details in the DXC section of `docs/slang_compat.md`.
- Running a test binary directly prints the backend name (for example `SSE2+FMA`, `NEON+FP16`) and
  `N checks, 0 failures`. The count grows as tests are added, so do not compare it against a fixed number.
- `ctest` on a default build runs 8 tests: the SIMD suite and its scalar twin (`shader_math_tests_scalar`),
  the codegen check (Release and non-MSVC only; skipped without objdump), `sandbox`, `skin_batch_check`,
  and `shader_math_tests_nested` / `shader_math_tests_matrix_swizzle` / `shader_math_tests_matrix_swizzle3`,
  which are built with the opt-in macros.
- Every CI job builds with `-DSHADER_MATH_WARNINGS_AS_ERRORS=ON` (`-Wall -Wextra -Wpedantic -Werror` on
  gcc/clang, `/W4 /WX` on MSVC). Use it locally as well.
- Benchmarks: `./build/shader_math_bench` and `./build/shader_math_bench_scalar` (`--markdown` prints a
  table). The numbers are for reference; nothing is gated on them.
- SDL_GPU demo (optional): `-DSHADER_MATH_DEMO_SDLGPU=ON`. For a headless run, `-DSDL_UNIX_CONSOLE_BUILD=ON`
  removes the X11 dependency; run it on lavapipe (`mesa-vulkan-drivers`) with `--headless N --out <dir>`
  (create the output directory first).

### Non-obvious points

- **The default compiler `c++` is clang++ (clang-18).** CI builds with g++ and clang++ explicitly, so to
  check both, pass `-DCMAKE_CXX_COMPILER=g++` / `-DCMAKE_CXX_COMPILER=clang++`.
- clang uses the GCC toolchain (libstdc++), so **`libstdc++-14-dev` is required**. Without it, linking with
  the default compiler fails with `cannot find -lstdc++` (the update script installs it).
- **Boost is header-only.** CMake uses `libboost-dev` when it is installed (the update script installs it).
  Otherwise `FetchContent` in `CMakeLists.txt` clones boost config/predef with git, which needs network access.
- **There is no lint / format configuration** (no `.clang-tidy` / `.clang-format`). There is no separate
  lint step; a successful build and passing tests are the check.
- The build directories `build/` and `build-*/` are in `.gitignore`.
