cmake_minimum_required(VERSION 3.16)
project(shader_math VERSION 0.1.0 LANGUAGES CXX)

# include/shader_math/version.hpp carries the same version for non-CMake
# users; keep the two in step.
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/include/shader_math/version.hpp _sm_version_lines
     REGEX "^#define SHADER_MATH_VERSION_(MAJOR|MINOR|PATCH) ")
set(_sm_header_version "")
foreach(_part MAJOR MINOR PATCH)
    foreach(_line ${_sm_version_lines})
        if(_line MATCHES "^#define SHADER_MATH_VERSION_${_part} ([0-9]+)")
            list(APPEND _sm_header_version ${CMAKE_MATCH_1})
        endif()
    endforeach()
endforeach()
list(JOIN _sm_header_version "." _sm_header_version)
if(NOT _sm_header_version STREQUAL PROJECT_VERSION)
    message(FATAL_ERROR "include/shader_math/version.hpp (${_sm_header_version}) does not match "
                        "project(VERSION ${PROJECT_VERSION})")
endif()

# PROJECT_IS_TOP_LEVEL needs CMake 3.21
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(SHADER_MATH_IS_TOP_LEVEL ON)
else()
    set(SHADER_MATH_IS_TOP_LEVEL OFF)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(GNUInstallDirs)

add_library(shader_math INTERFACE)
add_library(shader_math::shader_math ALIAS shader_math)
target_include_directories(shader_math INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(shader_math INTERFACE cxx_std_20)

# Boost is used header-only (Config / Predef for ISA detection). Prefer an
# installed copy; otherwise fetch just the two required header modules.
# Build-tree only: the installed package looks Boost up again on the
# consumer's machine (cmake/shader_mathConfig.cmake.in).
# Predef is located by boost/predef/hardware/simd.h (BOOST_HW_SIMD_*, Boost
# 1.60+), so an older system Boost is skipped in favor of the fetched copy.
find_path(SHADER_MATH_BOOST_INCLUDE boost/config.hpp)
find_path(SHADER_MATH_BOOST_PREDEF_INCLUDE boost/predef/hardware/simd.h
          HINTS ${SHADER_MATH_BOOST_INCLUDE})
if(SHADER_MATH_BOOST_PREDEF_INCLUDE AND
   NOT EXISTS "${SHADER_MATH_BOOST_PREDEF_INCLUDE}/boost/predef/hardware/simd.h")
    message(FATAL_ERROR "SHADER_MATH_BOOST_PREDEF_INCLUDE=${SHADER_MATH_BOOST_PREDEF_INCLUDE} "
                        "has no boost/predef/hardware/simd.h (Boost.Predef 1.60+ required)")
endif()
if(SHADER_MATH_BOOST_INCLUDE AND SHADER_MATH_BOOST_PREDEF_INCLUDE)
    target_include_directories(shader_math INTERFACE
        $<BUILD_INTERFACE:${SHADER_MATH_BOOST_INCLUDE}>
        $<BUILD_INTERFACE:${SHADER_MATH_BOOST_PREDEF_INCLUDE}>)
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
        $<BUILD_INTERFACE:${boost_config_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${boost_predef_SOURCE_DIR}/include>)
endif()

option(SHADER_MATH_FORCE_SCALAR
       "Force the scalar (non-SIMD) backend for shader_math and everything linking it" OFF)
# On the library target, so every translation unit of a consumer agrees (a mix
# of SIMD and scalar TUs would break the ODR). Build tree only: an installed
# package lets the consumer choose (cmake/shader_mathConfig.cmake.in).
if(SHADER_MATH_FORCE_SCALAR)
    target_compile_definitions(shader_math INTERFACE $<BUILD_INTERFACE:SHADER_MATH_FORCE_SCALAR>)
endif()
option(SHADER_MATH_GLSL_STPQ
       "Enable GLSL .stpq swizzles on shader_math and everything linking it" OFF)
# Same scheme as FORCE_SCALAR (the macro changes the vector class definition)
if(SHADER_MATH_GLSL_STPQ)
    target_compile_definitions(shader_math INTERFACE $<BUILD_INTERFACE:SHADER_MATH_GLSL_STPQ>)
endif()
option(SHADER_MATH_BUILD_TESTS "Build shader-math tests and examples" ${SHADER_MATH_IS_TOP_LEVEL})
option(SHADER_MATH_INSTALL "Generate the install / find_package rules" ${SHADER_MATH_IS_TOP_LEVEL})
option(SHADER_MATH_INTEROP_SLANG
       "Build the Slang cross-compilation check (needs slangc + python3)" OFF)
option(SHADER_MATH_INTEROP_GLSL
       "Build the GLSL cross-check (needs glslangValidator, Vulkan, python3)" OFF)
option(SHADER_MATH_WARNINGS_AS_ERRORS
       "Treat warnings as errors in this repo's own test and example targets" OFF)

# Warning set for targets built from this repo's sources (not the INTERFACE
# library, so consumers' flags are left alone).
function(shader_math_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
        if(SHADER_MATH_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if(SHADER_MATH_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

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
        tests/test_funcs.cpp
        tests/test_global_shadow.cpp
        tests/test_pad_lanes.cpp
        tests/test_random.cpp
        tests/test_proxy_alias.cpp)

    add_executable(shader_math_tests ${SHADER_MATH_TEST_SOURCES})
    target_link_libraries(shader_math_tests PRIVATE shader_math)
    shader_math_warnings(shader_math_tests)
    add_test(NAME shader_math_tests COMMAND shader_math_tests)

    # The scalar backend is never skipped: the same full suite runs against it
    # in every build (this is the RISC-V-like no-SIMD path).
    if(NOT SHADER_MATH_FORCE_SCALAR)
        add_executable(shader_math_tests_scalar ${SHADER_MATH_TEST_SOURCES})
        target_link_libraries(shader_math_tests_scalar PRIVATE shader_math)
        shader_math_warnings(shader_math_tests_scalar)
        target_compile_definitions(shader_math_tests_scalar PRIVATE SHADER_MATH_FORCE_SCALAR)
        add_test(NAME shader_math_tests_scalar COMMAND shader_math_tests_scalar)
    endif()

    # Disassembly-based codegen check: proves unroll<M> unrolls and that FMA
    # backends really emit fused instructions. Optimized single-config builds
    # only (Debug never unrolls); the script itself skips if objdump is absent.
    if(NOT MSVC AND NOT SHADER_MATH_FORCE_SCALAR AND CMAKE_BUILD_TYPE MATCHES "Rel")
        add_executable(shader_math_codegen tests/codegen.cpp)
        target_link_libraries(shader_math_codegen PRIVATE shader_math)
        shader_math_warnings(shader_math_codegen)
        add_test(NAME shader_math_codegen_check
                 COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/check_codegen.sh
                         $<TARGET_FILE:shader_math_codegen>)
        set_tests_properties(shader_math_codegen_check PROPERTIES SKIP_RETURN_CODE 77)
    endif()

    add_executable(sandbox examples/sandbox.cpp)
    target_link_libraries(sandbox PRIVATE shader_math)
    shader_math_warnings(sandbox)
    add_test(NAME sandbox COMMAND sandbox)

    # Micro-benchmarks (informational, not a ctest test): SIMD and the
    # scalar reference side by side, like the test suite.
    add_executable(shader_math_bench bench/bench.cpp)
    target_link_libraries(shader_math_bench PRIVATE shader_math)
    shader_math_warnings(shader_math_bench)
    add_executable(shader_math_bench_scalar bench/bench.cpp)
    target_link_libraries(shader_math_bench_scalar PRIVATE shader_math)
    target_compile_definitions(shader_math_bench_scalar PRIVATE SHADER_MATH_FORCE_SCALAR)
    shader_math_warnings(shader_math_bench_scalar)

    # Opt-in .stpq swizzles: its own executable (the macro changes the class
    # definition, so it cannot share a binary with the other tests)
    add_executable(shader_math_tests_stpq tests/test_stpq.cpp)
    target_link_libraries(shader_math_tests_stpq PRIVATE shader_math)
    target_compile_definitions(shader_math_tests_stpq PRIVATE SHADER_MATH_GLSL_STPQ)
    shader_math_warnings(shader_math_tests_stpq)
    add_test(NAME shader_math_tests_stpq COMMAND shader_math_tests_stpq)

    # Approximate-equivalence proof for the batch skinning backends (pure CPU,
    # no SDL): oracle bound + pairwise agreement + invariance properties.
    add_executable(skin_batch_check tests/test_skin_batch.cpp)
    target_link_libraries(skin_batch_check PRIVATE shader_math)
    shader_math_warnings(skin_batch_check)
    add_test(NAME skin_batch_check COMMAND skin_batch_check)

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
            shader_math_warnings(shader_math_interop_cpp)

            add_test(NAME slang_interop_check
                     COMMAND ${Python3_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:slang_interop_runner>
                             $<TARGET_FILE:shader_math_interop_cpp>)

            # half cases: the CPU target cannot compile half vector math, so
            # they go through SPIR-V on Vulkan (lavapipe) when it is available
            find_package(Vulkan)
            if(Vulkan_FOUND)
                set(SHADER_MATH_INTEROP_HALF_SPV ${CMAKE_CURRENT_BINARY_DIR}/kernels_half.spv)
                add_custom_command(
                    OUTPUT ${SHADER_MATH_INTEROP_HALF_SPV}
                    COMMAND ${SLANGC_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/kernels_half.slang
                            -target spirv -entry runCases -stage compute
                            -o ${SHADER_MATH_INTEROP_HALF_SPV}
                    DEPENDS ${SHADER_MATH_INTEROP_DIR}/kernels_half.slang
                            ${SHADER_MATH_INTEROP_DIR}/shared/math_cases.hlsli
                            ${SHADER_MATH_INTEROP_DIR}/shared/case_list.h
                    COMMENT "slangc: compiling the half cases to SPIR-V"
                    VERBATIM)
                add_custom_target(slang_interop_half_spv ALL DEPENDS ${SHADER_MATH_INTEROP_HALF_SPV})

                add_executable(slang_vk_runner_half ${SHADER_MATH_INTEROP_DIR}/vk_runner.cpp)
                target_include_directories(slang_vk_runner_half PRIVATE ${SHADER_MATH_INTEROP_DIR})
                target_compile_definitions(slang_vk_runner_half PRIVATE
                    "SM_VK_CASES_HEADER=\"shared/case_list.h\"" SM_VK_CASES=SM_INTEROP_HALF_CASES)
                target_link_libraries(slang_vk_runner_half PRIVATE Vulkan::Vulkan)
                shader_math_warnings(slang_vk_runner_half)

                add_executable(shader_math_interop_cpp_half ${SHADER_MATH_INTEROP_DIR}/cpp_harness.cpp)
                target_compile_definitions(shader_math_interop_cpp_half PRIVATE SM_INTEROP_ENABLE_HALF)
                target_link_libraries(shader_math_interop_cpp_half PRIVATE shader_math)
                shader_math_warnings(shader_math_interop_cpp_half)

                add_test(NAME slang_interop_half_check
                         COMMAND ${Python3_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/compare.py
                                 $<TARGET_FILE:slang_vk_runner_half>
                                 $<TARGET_FILE:shader_math_interop_cpp_half>
                                 slang-spirv ${SHADER_MATH_INTEROP_HALF_SPV} --f16)
                set_tests_properties(slang_interop_half_check PROPERTIES SKIP_RETURN_CODE 77)
            else()
                message(STATUS "Vulkan not found: skipping the Slang half cross-check")
            endif()
        else()
            message(WARNING "SHADER_MATH_INTEROP_SLANG=ON but slangc or python3 "
                            "was not found (set SHADER_MATH_SLANG_DIR); skipping "
                            "the interop check")
        endif()
    endif()

    # GLSL cross-check: tests/interop/glsl/math_cases.glsl is compiled to
    # SPIR-V with glslang and run on Vulkan (lavapipe in CI), and through
    # shader-math's glsl surface; compare.py checks the outputs slot by slot.
    if(SHADER_MATH_INTEROP_GLSL)
        find_program(GLSLANG_VALIDATOR glslangValidator)
        find_package(Vulkan)
        find_package(Python3 COMPONENTS Interpreter)
        if(GLSLANG_VALIDATOR AND Vulkan_FOUND AND Python3_FOUND)
            set(SM_GLSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/glsl)
            set(SM_GLSL_SPV ${CMAKE_CURRENT_BINARY_DIR}/glsl_kernels.spv)
            add_custom_command(
                OUTPUT ${SM_GLSL_SPV}
                COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.1
                        -o ${SM_GLSL_SPV} ${SM_GLSL_DIR}/kernels.comp
                DEPENDS ${SM_GLSL_DIR}/kernels.comp ${SM_GLSL_DIR}/math_cases.glsl
                        ${SM_GLSL_DIR}/case_list.h
                COMMENT "glslang: compiling kernels.comp to SPIR-V"
                VERBATIM)
            add_custom_target(glsl_interop_spv ALL DEPENDS ${SM_GLSL_SPV})

            add_executable(glsl_vk_runner ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/vk_runner.cpp)
            target_include_directories(glsl_vk_runner PRIVATE ${SM_GLSL_DIR})
            target_compile_definitions(glsl_vk_runner PRIVATE
                "SM_VK_CASES_HEADER=\"case_list.h\"" SM_VK_CASES=SM_GLSL_CASES)
            target_link_libraries(glsl_vk_runner PRIVATE Vulkan::Vulkan)
            shader_math_warnings(glsl_vk_runner)

            add_executable(shader_math_glsl_interop_cpp ${SM_GLSL_DIR}/cpp_harness.cpp)
            target_link_libraries(shader_math_glsl_interop_cpp PRIVATE shader_math)
            shader_math_warnings(shader_math_glsl_interop_cpp)

            add_test(NAME glsl_interop_check
                     COMMAND ${Python3_EXECUTABLE}
                             ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/compare.py
                             $<TARGET_FILE:glsl_vk_runner>
                             $<TARGET_FILE:shader_math_glsl_interop_cpp>
                             vulkan ${SM_GLSL_SPV})

            # f16vec / f16mat cases (shaderFloat16; skipped when the device lacks it)
            set(SM_GLSL_HALF_SPV ${CMAKE_CURRENT_BINARY_DIR}/glsl_kernels_half.spv)
            add_custom_command(
                OUTPUT ${SM_GLSL_HALF_SPV}
                COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.2
                        -o ${SM_GLSL_HALF_SPV} ${SM_GLSL_DIR}/kernels_half.comp
                DEPENDS ${SM_GLSL_DIR}/kernels_half.comp ${SM_GLSL_DIR}/half_cases.glsl
                        ${SM_GLSL_DIR}/half_case_list.h ${SM_GLSL_DIR}/math_cases.glsl
                COMMENT "glslang: compiling kernels_half.comp to SPIR-V"
                VERBATIM)
            add_custom_target(glsl_interop_half_spv ALL DEPENDS ${SM_GLSL_HALF_SPV})

            add_executable(glsl_vk_runner_half ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/vk_runner.cpp)
            target_include_directories(glsl_vk_runner_half PRIVATE ${SM_GLSL_DIR})
            target_compile_definitions(glsl_vk_runner_half PRIVATE
                "SM_VK_CASES_HEADER=\"half_case_list.h\"" SM_VK_CASES=SM_GLSL_HALF_CASES)
            target_link_libraries(glsl_vk_runner_half PRIVATE Vulkan::Vulkan)
            shader_math_warnings(glsl_vk_runner_half)

            add_executable(shader_math_glsl_interop_cpp_half ${SM_GLSL_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_glsl_interop_cpp_half PRIVATE SM_GLSL_HALF)
            target_link_libraries(shader_math_glsl_interop_cpp_half PRIVATE shader_math)
            shader_math_warnings(shader_math_glsl_interop_cpp_half)

            add_test(NAME glsl_interop_half_check
                     COMMAND ${Python3_EXECUTABLE}
                             ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/compare.py
                             $<TARGET_FILE:glsl_vk_runner_half>
                             $<TARGET_FILE:shader_math_glsl_interop_cpp_half>
                             vulkan ${SM_GLSL_HALF_SPV} --f16)
            set_tests_properties(glsl_interop_half_check PROPERTIES SKIP_RETURN_CODE 77)
        else()
            message(WARNING "SHADER_MATH_INTEROP_GLSL=ON but glslangValidator, Vulkan "
                            "or python3 was not found; skipping the GLSL cross-check")
        endif()
    endif()
endif()

if(SHADER_MATH_INSTALL)
    include(CMakePackageConfigHelpers)
    set(SHADER_MATH_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/shader_math)

    install(TARGETS shader_math EXPORT shader_mathTargets)
    install(DIRECTORY include/shader_math DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT shader_mathTargets
            NAMESPACE shader_math::
            DESTINATION ${SHADER_MATH_CMAKE_DIR})

    configure_package_config_file(cmake/shader_mathConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/shader_mathConfig.cmake
        INSTALL_DESTINATION ${SHADER_MATH_CMAKE_DIR})
    # 0.x: a minor bump may break the API, so only the same minor matches.
    # ARCH_INDEPENDENT: header-only, the package is valid for any target.
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/shader_mathConfigVersion.cmake
        COMPATIBILITY SameMinorVersion
        ARCH_INDEPENDENT)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/shader_mathConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/shader_mathConfigVersion.cmake
        DESTINATION ${SHADER_MATH_CMAKE_DIR})

    # pkg-config for non-CMake builds. Header-only, so it goes to
    # share/pkgconfig; the prefix is relative to the .pc file, which keeps
    # `cmake --install --prefix <dir>` working. Boost is listed only when a
    # system copy was used (a fetched copy is not installed).
    set(SHADER_MATH_PC_DIR ${CMAKE_INSTALL_DATADIR}/pkgconfig)
    # one ".." per component of the (relative) pkgconfig dir: share/pkgconfig
    # -> ../.. (file(RELATIVE_PATH) needs absolute paths, which differ by OS)
    string(REPLACE "/" ";" _pc_parts "${SHADER_MATH_PC_DIR}")
    set(SHADER_MATH_PC_TO_PREFIX "")
    foreach(_part ${_pc_parts})
        if(SHADER_MATH_PC_TO_PREFIX)
            string(APPEND SHADER_MATH_PC_TO_PREFIX "/")
        endif()
        string(APPEND SHADER_MATH_PC_TO_PREFIX "..")
    endforeach()
    set(SHADER_MATH_PC_BOOST_CFLAGS "")
    foreach(_dir ${SHADER_MATH_BOOST_INCLUDE} ${SHADER_MATH_BOOST_PREDEF_INCLUDE})
        if(_dir AND NOT _dir STREQUAL "/usr/include"
           AND NOT SHADER_MATH_PC_BOOST_CFLAGS MATCHES "-I${_dir}( |$)")
            string(APPEND SHADER_MATH_PC_BOOST_CFLAGS " -I${_dir}")
        endif()
    endforeach()
    configure_file(cmake/shader_math.pc.in ${CMAKE_CURRENT_BINARY_DIR}/shader_math.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/shader_math.pc DESTINATION ${SHADER_MATH_PC_DIR})
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
        GIT_TAG release-3.4.18
        GIT_SHALLOW TRUE)
    FetchContent_Declare(ufbx
        GIT_REPOSITORY https://github.com/ufbx/ufbx.git
        GIT_TAG v0.23.1
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(SDL3 ufbx)

    add_library(ufbx_lib STATIC ${ufbx_SOURCE_DIR}/ufbx.c)
    target_include_directories(ufbx_lib PUBLIC ${ufbx_SOURCE_DIR})

    add_executable(sdl_gpu_skinning examples/sdl_gpu_skinning/main.cpp)
    target_link_libraries(sdl_gpu_skinning PRIVATE shader_math SDL3::SDL3-static ufbx_lib)
    target_compile_definitions(sdl_gpu_skinning PRIVATE
        SM_DEMO_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/examples/sdl_gpu_skinning")
endif()
