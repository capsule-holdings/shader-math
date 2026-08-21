cmake_minimum_required(VERSION 3.16)
project(shader_math LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(shader_math INTERFACE)
target_include_directories(shader_math INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(shader_math INTERFACE cxx_std_20)

# Boost is used header-only (Config / Predef for ISA detection). Prefer an
# installed copy; otherwise fetch just the two required header modules.
find_path(SHADER_MATH_BOOST_INCLUDE boost/config.hpp)
if(SHADER_MATH_BOOST_INCLUDE)
    target_include_directories(shader_math INTERFACE ${SHADER_MATH_BOOST_INCLUDE})
else()
    include(FetchContent)
    FetchContent_Declare(boost_config
        GIT_REPOSITORY https://github.com/boostorg/config.git
        GIT_TAG boost-1.92.0
        GIT_SHALLOW TRUE)
    FetchContent_Declare(boost_predef
        GIT_REPOSITORY https://github.com/boostorg/predef.git
        GIT_TAG boost-1.92.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(boost_config boost_predef)
    target_include_directories(shader_math INTERFACE
        ${boost_config_SOURCE_DIR}/include
        ${boost_predef_SOURCE_DIR}/include)
endif()

option(SHADER_MATH_FORCE_SCALAR "Force the scalar (non-SIMD) backend" OFF)
option(SHADER_MATH_BUILD_TESTS "Build shader-math tests and examples" ON)
option(SHADER_MATH_INTEROP_SLANG
       "Build the Slang cross-compilation check (needs slangc + python3)" OFF)

if(SHADER_MATH_BUILD_TESTS)
    enable_testing()

    set(SHADER_MATH_TEST_SOURCES
        src/shader_math.cpp        # proves the library compiles as a plain TU
        tests/main.cpp
        tests/test_vector.cpp
        tests/test_matrix_hlsl.cpp
        tests/test_matrix_glsl.cpp
        tests/test_cross_lang.cpp
        tests/test_restrict.cpp
        tests/test_simd.cpp
        tests/test_half.cpp
        tests/test_funcs.cpp)

    add_executable(shader_math_tests ${SHADER_MATH_TEST_SOURCES})
    target_link_libraries(shader_math_tests PRIVATE shader_math)
    if(SHADER_MATH_FORCE_SCALAR)
        target_compile_definitions(shader_math_tests PRIVATE SHADER_MATH_FORCE_SCALAR)
    endif()
    add_test(NAME shader_math_tests COMMAND shader_math_tests)

    # The scalar backend is never skipped: the same full suite runs against it
    # in every build (this is the RISC-V-like no-SIMD path).
    if(NOT SHADER_MATH_FORCE_SCALAR)
        add_executable(shader_math_tests_scalar ${SHADER_MATH_TEST_SOURCES})
        target_link_libraries(shader_math_tests_scalar PRIVATE shader_math)
        target_compile_definitions(shader_math_tests_scalar PRIVATE SHADER_MATH_FORCE_SCALAR)
        add_test(NAME shader_math_tests_scalar COMMAND shader_math_tests_scalar)
    endif()

    # Disassembly-based codegen check: proves unroll<M> unrolls and that FMA
    # backends really emit fused instructions. Optimized single-config builds
    # only (Debug never unrolls); the script itself skips if objdump is absent.
    if(NOT MSVC AND NOT SHADER_MATH_FORCE_SCALAR AND CMAKE_BUILD_TYPE MATCHES "Rel")
        add_executable(shader_math_codegen tests/codegen.cpp)
        target_link_libraries(shader_math_codegen PRIVATE shader_math)
        add_test(NAME shader_math_codegen_check
                 COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/check_codegen.sh
                         $<TARGET_FILE:shader_math_codegen>)
        set_tests_properties(shader_math_codegen_check PROPERTIES SKIP_RETURN_CODE 77)
    endif()

    add_executable(sandbox examples/sandbox.cpp)
    target_link_libraries(sandbox PRIVATE shader_math)

    # Slang cross-compilation check: the same math_cases.hlsli is built with
    # slangc (-target cpp, CPU execution) and with shader-math, and compare.py
    # checks the outputs slot by slot (see docs/slang_compat.md).
    if(SHADER_MATH_INTEROP_SLANG)
        find_program(SLANGC_EXECUTABLE slangc HINTS ${SHADER_MATH_SLANG_DIR}/bin)
        find_package(Python3 COMPONENTS Interpreter)
        if(SLANGC_EXECUTABLE AND Python3_FOUND)
            get_filename_component(SHADER_MATH_SLANG_ROOT
                                   ${SLANGC_EXECUTABLE} DIRECTORY)          # <slang>/bin
            get_filename_component(SHADER_MATH_SLANG_ROOT
                                   ${SHADER_MATH_SLANG_ROOT} DIRECTORY)    # <slang>
            set(SHADER_MATH_INTEROP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop)
            set(SHADER_MATH_INTEROP_GEN ${CMAKE_CURRENT_BINARY_DIR}/kernels.gen.cpp)

            add_custom_command(
                OUTPUT ${SHADER_MATH_INTEROP_GEN}
                COMMAND ${SLANGC_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/kernels.slang
                        -target cpp -entry runCases -o ${SHADER_MATH_INTEROP_GEN}
                DEPENDS ${SHADER_MATH_INTEROP_DIR}/kernels.slang
                        ${SHADER_MATH_INTEROP_DIR}/shared/math_cases.hlsli
                        ${SHADER_MATH_INTEROP_DIR}/shared/case_list.h
                COMMENT "slangc: generating kernels.gen.cpp (CPU target)"
                VERBATIM)

            add_executable(slang_interop_runner
                ${SHADER_MATH_INTEROP_GEN}
                ${SHADER_MATH_INTEROP_DIR}/slang_driver.cpp)
            target_include_directories(slang_interop_runner PRIVATE
                ${SHADER_MATH_SLANG_ROOT}/include ${SHADER_MATH_INTEROP_DIR})
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                # -ffp-contract=off: keep Slang-side rounding fmadd-free;
                # -w: the generated code is not this repo's to keep warning-clean.
                target_compile_options(slang_interop_runner PRIVATE -ffp-contract=off -w)
            endif()

            add_executable(shader_math_interop_cpp
                ${SHADER_MATH_INTEROP_DIR}/cpp_harness.cpp)
            target_link_libraries(shader_math_interop_cpp PRIVATE shader_math)

            add_test(NAME slang_interop_check
                     COMMAND ${Python3_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:slang_interop_runner>
                             $<TARGET_FILE:shader_math_interop_cpp>)
        else()
            message(WARNING "SHADER_MATH_INTEROP_SLANG=ON but slangc or python3 "
                            "was not found (set SHADER_MATH_SLANG_DIR); skipping "
                            "the interop check")
        endif()
    endif()
endif()

# SDL_GPU skinned-mesh demo (FBX via ufbx, CPU skinning via shader-math).
# Off by default: fetches and builds SDL3, so it is opt-in.
option(SHADER_MATH_DEMO_SDLGPU "Build the SDL_GPU skinning demo" OFF)
if(SHADER_MATH_DEMO_SDLGPU)
    enable_language(C)  # SDL3 and ufbx are C
    include(FetchContent)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.4.14
        GIT_SHALLOW TRUE)
    FetchContent_Declare(ufbx
        GIT_REPOSITORY https://github.com/ufbx/ufbx.git
        GIT_TAG v0.23.0
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(SDL3 ufbx)

    add_library(ufbx_lib STATIC ${ufbx_SOURCE_DIR}/ufbx.c)
    target_include_directories(ufbx_lib PUBLIC ${ufbx_SOURCE_DIR})

    add_executable(sdl_gpu_skinning examples/sdl_gpu_skinning/main.cpp)
    target_link_libraries(sdl_gpu_skinning PRIVATE shader_math SDL3::SDL3-static ufbx_lib)
    target_compile_definitions(sdl_gpu_skinning PRIVATE
        SM_DEMO_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/examples/sdl_gpu_skinning")
endif()
