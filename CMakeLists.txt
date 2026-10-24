cmake_minimum_required(VERSION 3.16)
project(shader_math VERSION 0.5.0 LANGUAGES CXX)

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
option(SHADER_MATH_NESTED_SWIZZLE
       "Enable swizzles of swizzles (v.xy.yx) on shader_math and everything linking it" OFF)
if(SHADER_MATH_NESTED_SWIZZLE)
    target_compile_definitions(shader_math INTERFACE $<BUILD_INTERFACE:SHADER_MATH_NESTED_SWIZZLE>)
endif()
option(SHADER_MATH_MATRIX_SWIZZLE
       "Enable HLSL 2-element matrix accessors (m._m00_m11) on shader_math and everything linking it" OFF)
option(SHADER_MATH_MATRIX_SWIZZLE3
       "Also enable the 3-element ones (m._m30_m31_m32); much slower to compile" OFF)
if(SHADER_MATH_MATRIX_SWIZZLE3)
    target_compile_definitions(shader_math INTERFACE $<BUILD_INTERFACE:SHADER_MATH_MATRIX_SWIZZLE3>)
elseif(SHADER_MATH_MATRIX_SWIZZLE)
    target_compile_definitions(shader_math INTERFACE $<BUILD_INTERFACE:SHADER_MATH_MATRIX_SWIZZLE>)
endif()
option(SHADER_MATH_BUILD_TESTS "Build shader-math tests and examples" ${SHADER_MATH_IS_TOP_LEVEL})
option(SHADER_MATH_INSTALL "Generate the install / find_package rules" ${SHADER_MATH_IS_TOP_LEVEL})
option(SHADER_MATH_INTEROP_SLANG
       "Build the Slang cross-compilation check (needs slangc + python3)" OFF)
option(SHADER_MATH_INTEROP_DXC
       "Build the DXC cross-check (needs dxc, Vulkan, python3; set SHADER_MATH_DXC_DIR)" OFF)
option(SHADER_MATH_INTEROP_DXIL
       "Build the DXIL cross-check on D3D12 WARP (Windows; needs dxc, python3; set SHADER_MATH_DXC_DIR)" OFF)
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
        tests/test_proxy_alias.cpp
        tests/test_std.cpp)

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

    # Opt-in swizzles of swizzles, alone and together with .stpq.
    add_executable(shader_math_tests_nested tests/test_nested_swizzle.cpp)
    target_link_libraries(shader_math_tests_nested PRIVATE shader_math)
    target_compile_definitions(shader_math_tests_nested PRIVATE SHADER_MATH_NESTED_SWIZZLE)
    shader_math_warnings(shader_math_tests_nested)
    add_test(NAME shader_math_tests_nested COMMAND shader_math_tests_nested)
    add_executable(shader_math_tests_nested_stpq tests/test_nested_swizzle.cpp)
    target_link_libraries(shader_math_tests_nested_stpq PRIVATE shader_math)
    target_compile_definitions(shader_math_tests_nested_stpq PRIVATE
        SHADER_MATH_NESTED_SWIZZLE SHADER_MATH_GLSL_STPQ)
    shader_math_warnings(shader_math_tests_nested_stpq)
    add_test(NAME shader_math_tests_nested_stpq COMMAND shader_math_tests_nested_stpq)

    # Opt-in multi-element matrix accessors: 2-element, then also 3-element.
    add_executable(shader_math_tests_matrix_swizzle tests/test_matrix_swizzle.cpp)
    target_link_libraries(shader_math_tests_matrix_swizzle PRIVATE shader_math)
    target_compile_definitions(shader_math_tests_matrix_swizzle PRIVATE SHADER_MATH_MATRIX_SWIZZLE)
    shader_math_warnings(shader_math_tests_matrix_swizzle)
    add_test(NAME shader_math_tests_matrix_swizzle COMMAND shader_math_tests_matrix_swizzle)
    add_executable(shader_math_tests_matrix_swizzle3 tests/test_matrix_swizzle.cpp)
    target_link_libraries(shader_math_tests_matrix_swizzle3 PRIVATE shader_math)
    target_compile_definitions(shader_math_tests_matrix_swizzle3 PRIVATE SHADER_MATH_MATRIX_SWIZZLE3)
    shader_math_warnings(shader_math_tests_matrix_swizzle3)
    add_test(NAME shader_math_tests_matrix_swizzle3 COMMAND shader_math_tests_matrix_swizzle3)

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

            # opt-in swizzles of swizzles: kernels_nested.slang on the CPU
            # target against a twin built with SHADER_MATH_NESTED_SWIZZLE
            set(SHADER_MATH_INTEROP_NESTED_GEN ${CMAKE_CURRENT_BINARY_DIR}/kernels_nested.gen.cpp)
            add_custom_command(
                OUTPUT ${SHADER_MATH_INTEROP_NESTED_GEN}
                COMMAND ${SLANGC_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/kernels_nested.slang
                        -target cpp -entry runCases -o ${SHADER_MATH_INTEROP_NESTED_GEN}
                DEPENDS ${SHADER_MATH_INTEROP_DIR}/kernels_nested.slang
                        ${SHADER_MATH_INTEROP_DIR}/shared/math_cases.hlsli
                        ${SHADER_MATH_INTEROP_DIR}/shared/nested_cases.hlsli
                        ${SHADER_MATH_INTEROP_DIR}/shared/case_list.h
                COMMENT "slangc: generating kernels_nested.gen.cpp (CPU target)"
                VERBATIM)

            add_executable(slang_interop_runner_nested
                ${SHADER_MATH_INTEROP_NESTED_GEN}
                ${SHADER_MATH_INTEROP_DIR}/slang_driver.cpp)
            target_include_directories(slang_interop_runner_nested PRIVATE
                ${SHADER_MATH_SLANG_ROOT}/include ${SHADER_MATH_INTEROP_DIR})
            target_compile_definitions(slang_interop_runner_nested PRIVATE
                SM_DRIVER_CASES=SM_INTEROP_NESTED_CASES)
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(slang_interop_runner_nested PRIVATE -ffp-contract=off -w)
            endif()

            add_executable(shader_math_interop_cpp_nested
                ${SHADER_MATH_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_interop_cpp_nested PRIVATE
                SM_INTEROP_NESTED SHADER_MATH_NESTED_SWIZZLE)
            target_link_libraries(shader_math_interop_cpp_nested PRIVATE shader_math)
            shader_math_warnings(shader_math_interop_cpp_nested)

            add_test(NAME slang_interop_nested_check
                     COMMAND ${Python3_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:slang_interop_runner_nested>
                             $<TARGET_FILE:shader_math_interop_cpp_nested>)

            # opt-in multi-element matrix accessors: kernels_matrix_swizzle.slang
            # on the CPU target against a twin built with SHADER_MATH_MATRIX_SWIZZLE3
            set(SHADER_MATH_INTEROP_MATRIX_SWIZZLE_GEN ${CMAKE_CURRENT_BINARY_DIR}/kernels_matrix_swizzle.gen.cpp)
            add_custom_command(
                OUTPUT ${SHADER_MATH_INTEROP_MATRIX_SWIZZLE_GEN}
                COMMAND ${SLANGC_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/kernels_matrix_swizzle.slang
                        -target cpp -entry runCases -o ${SHADER_MATH_INTEROP_MATRIX_SWIZZLE_GEN}
                DEPENDS ${SHADER_MATH_INTEROP_DIR}/kernels_matrix_swizzle.slang
                        ${SHADER_MATH_INTEROP_DIR}/shared/math_cases.hlsli
                        ${SHADER_MATH_INTEROP_DIR}/shared/matrix_swizzle_cases.hlsli
                        ${SHADER_MATH_INTEROP_DIR}/shared/case_list.h
                COMMENT "slangc: generating kernels_matrix_swizzle.gen.cpp (CPU target)"
                VERBATIM)

            add_executable(slang_interop_runner_matrix_swizzle
                ${SHADER_MATH_INTEROP_MATRIX_SWIZZLE_GEN}
                ${SHADER_MATH_INTEROP_DIR}/slang_driver.cpp)
            target_include_directories(slang_interop_runner_matrix_swizzle PRIVATE
                ${SHADER_MATH_SLANG_ROOT}/include ${SHADER_MATH_INTEROP_DIR})
            target_compile_definitions(slang_interop_runner_matrix_swizzle PRIVATE
                SM_DRIVER_CASES=SM_INTEROP_MATRIX_SWIZZLE_CASES)
            if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
                target_compile_options(slang_interop_runner_matrix_swizzle PRIVATE -ffp-contract=off -w)
            endif()

            add_executable(shader_math_interop_cpp_matrix_swizzle
                ${SHADER_MATH_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_interop_cpp_matrix_swizzle PRIVATE
                SM_INTEROP_MATRIX_SWIZZLE SHADER_MATH_MATRIX_SWIZZLE3)
            target_link_libraries(shader_math_interop_cpp_matrix_swizzle PRIVATE shader_math)
            shader_math_warnings(shader_math_interop_cpp_matrix_swizzle)

            add_test(NAME slang_interop_matrix_swizzle_check
                     COMMAND ${Python3_EXECUTABLE} ${SHADER_MATH_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:slang_interop_runner_matrix_swizzle>
                             $<TARGET_FILE:shader_math_interop_cpp_matrix_swizzle>)

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

    # DXC cross-check: the shared HLSL cases (plus DXC-only ones) compiled by
    # the official HLSL compiler to SPIR-V and run on Vulkan (lavapipe in CI);
    # half with -enable-16bit-types (otherwise DXC treats half as float).
    if(SHADER_MATH_INTEROP_DXC)
        find_program(DXC_EXECUTABLE dxc HINTS ${SHADER_MATH_DXC_DIR}/bin NO_DEFAULT_PATH)
        find_program(DXC_EXECUTABLE dxc)
        find_package(Vulkan)
        find_package(Python3 COMPONENTS Interpreter)
        if(DXC_EXECUTABLE AND Vulkan_FOUND AND Python3_FOUND)
            get_filename_component(SM_DXC_ROOT ${DXC_EXECUTABLE} DIRECTORY)
            get_filename_component(SM_DXC_ROOT ${SM_DXC_ROOT} DIRECTORY)
            set(SM_INTEROP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop)
            set(SM_DXC_DEPS ${SM_INTEROP_DIR}/dxc/kernels.hlsl ${SM_INTEROP_DIR}/dxc/dxc_cases.hlsli
                            ${SM_INTEROP_DIR}/dxc/dxc_case_list.h
                            ${SM_INTEROP_DIR}/shared/math_cases.hlsli ${SM_INTEROP_DIR}/shared/case_list.h
                            ${SM_INTEROP_DIR}/shared/matrix_swizzle_cases.hlsli)
            set(SM_DXC_RUN ${CMAKE_COMMAND} -E env LD_LIBRARY_PATH=${SM_DXC_ROOT}/lib ${DXC_EXECUTABLE}
                           -spirv -T cs_6_2 -E main -HV 2021 -fspv-target-env=vulkan1.1
                           -I ${SM_INTEROP_DIR})
            set(SM_DXC_SPV ${CMAKE_CURRENT_BINARY_DIR}/dxc_kernels.spv)
            set(SM_DXC_HALF_SPV ${CMAKE_CURRENT_BINARY_DIR}/dxc_kernels_half.spv)
            add_custom_command(
                OUTPUT ${SM_DXC_SPV}
                COMMAND ${SM_DXC_RUN} -Fo ${SM_DXC_SPV} ${SM_INTEROP_DIR}/dxc/kernels.hlsl
                DEPENDS ${SM_DXC_DEPS}
                COMMENT "dxc: compiling the HLSL cases to SPIR-V"
                VERBATIM)
            add_custom_command(
                OUTPUT ${SM_DXC_HALF_SPV}
                COMMAND ${SM_DXC_RUN} -enable-16bit-types -D SM_INTEROP_ENABLE_HALF=1
                        -Fo ${SM_DXC_HALF_SPV} ${SM_INTEROP_DIR}/dxc/kernels.hlsl
                DEPENDS ${SM_DXC_DEPS}
                COMMENT "dxc: compiling the half cases to SPIR-V (-enable-16bit-types)"
                VERBATIM)
            set(SM_DXC_MATRIX_SWIZZLE_SPV ${CMAKE_CURRENT_BINARY_DIR}/dxc_kernels_matrix_swizzle.spv)
            add_custom_command(
                OUTPUT ${SM_DXC_MATRIX_SWIZZLE_SPV}
                COMMAND ${SM_DXC_RUN} -D SM_INTEROP_MATRIX_SWIZZLE=1
                        -Fo ${SM_DXC_MATRIX_SWIZZLE_SPV} ${SM_INTEROP_DIR}/dxc/kernels.hlsl
                DEPENDS ${SM_DXC_DEPS}
                COMMENT "dxc: compiling the matrix accessor cases to SPIR-V"
                VERBATIM)
            add_custom_target(dxc_interop_spv ALL
                DEPENDS ${SM_DXC_SPV} ${SM_DXC_HALF_SPV} ${SM_DXC_MATRIX_SWIZZLE_SPV})

            add_executable(dxc_vk_runner ${SM_INTEROP_DIR}/vk_runner.cpp)
            target_include_directories(dxc_vk_runner PRIVATE ${SM_INTEROP_DIR})
            target_compile_definitions(dxc_vk_runner PRIVATE
                "SM_VK_CASES_HEADER=\"dxc/dxc_case_list.h\"" SM_VK_CASES=SM_DXC_CASES)
            target_link_libraries(dxc_vk_runner PRIVATE Vulkan::Vulkan)
            shader_math_warnings(dxc_vk_runner)
            add_executable(dxc_vk_runner_half ${SM_INTEROP_DIR}/vk_runner.cpp)
            target_include_directories(dxc_vk_runner_half PRIVATE ${SM_INTEROP_DIR})
            target_compile_definitions(dxc_vk_runner_half PRIVATE
                "SM_VK_CASES_HEADER=\"shared/case_list.h\"" SM_VK_CASES=SM_INTEROP_HALF_CASES)
            target_link_libraries(dxc_vk_runner_half PRIVATE Vulkan::Vulkan)
            shader_math_warnings(dxc_vk_runner_half)
            add_executable(dxc_vk_runner_matrix_swizzle ${SM_INTEROP_DIR}/vk_runner.cpp)
            target_include_directories(dxc_vk_runner_matrix_swizzle PRIVATE ${SM_INTEROP_DIR})
            target_compile_definitions(dxc_vk_runner_matrix_swizzle PRIVATE
                "SM_VK_CASES_HEADER=\"shared/case_list.h\"" SM_VK_CASES=SM_INTEROP_MATRIX_SWIZZLE_CASES)
            target_link_libraries(dxc_vk_runner_matrix_swizzle PRIVATE Vulkan::Vulkan)
            shader_math_warnings(dxc_vk_runner_matrix_swizzle)

            add_executable(shader_math_dxc_cpp ${SM_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_dxc_cpp PRIVATE SM_INTEROP_DXC)
            target_include_directories(shader_math_dxc_cpp PRIVATE ${SM_INTEROP_DIR})
            target_link_libraries(shader_math_dxc_cpp PRIVATE shader_math)
            shader_math_warnings(shader_math_dxc_cpp)
            add_executable(shader_math_dxc_cpp_half ${SM_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_dxc_cpp_half PRIVATE SM_INTEROP_ENABLE_HALF)
            target_link_libraries(shader_math_dxc_cpp_half PRIVATE shader_math)
            shader_math_warnings(shader_math_dxc_cpp_half)
            add_executable(shader_math_dxc_cpp_matrix_swizzle ${SM_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_dxc_cpp_matrix_swizzle PRIVATE
                SM_INTEROP_MATRIX_SWIZZLE SHADER_MATH_MATRIX_SWIZZLE3)
            target_link_libraries(shader_math_dxc_cpp_matrix_swizzle PRIVATE shader_math)
            shader_math_warnings(shader_math_dxc_cpp_matrix_swizzle)

            add_test(NAME dxc_interop_check
                     COMMAND ${Python3_EXECUTABLE} ${SM_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:dxc_vk_runner> $<TARGET_FILE:shader_math_dxc_cpp>
                             dxc ${SM_DXC_SPV})
            add_test(NAME dxc_interop_half_check
                     COMMAND ${Python3_EXECUTABLE} ${SM_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:dxc_vk_runner_half> $<TARGET_FILE:shader_math_dxc_cpp_half>
                             dxc ${SM_DXC_HALF_SPV} --f16)
            add_test(NAME dxc_interop_matrix_swizzle_check
                     COMMAND ${Python3_EXECUTABLE} ${SM_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:dxc_vk_runner_matrix_swizzle>
                             $<TARGET_FILE:shader_math_dxc_cpp_matrix_swizzle>
                             dxc ${SM_DXC_MATRIX_SWIZZLE_SPV})
            set_tests_properties(dxc_interop_check dxc_interop_half_check
                                 dxc_interop_matrix_swizzle_check PROPERTIES
                                 SKIP_RETURN_CODE 77)
        else()
            message(WARNING "SHADER_MATH_INTEROP_DXC=ON but dxc (set SHADER_MATH_DXC_DIR), "
                            "Vulkan or python3 was not found; skipping the DXC cross-check")
        endif()
    endif()

    # DXIL cross-check (Windows): the same HLSL compiled by DXC to DXIL and run
    # on WARP through D3D12. dxc signs the DXIL with the dxil.dll next to it,
    # which D3D12 requires.
    if(SHADER_MATH_INTEROP_DXIL)
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "^(ARM64|arm64|aarch64)$")
            set(SM_DXIL_HOST_ARCH arm64)
        else()
            set(SM_DXIL_HOST_ARCH x64)
        endif()
        find_program(DXIL_DXC_EXECUTABLE dxc HINTS ${SHADER_MATH_DXC_DIR}/bin/${SM_DXIL_HOST_ARCH}
                     NO_DEFAULT_PATH)
        find_program(DXIL_DXC_EXECUTABLE dxc)
        find_package(Python3 COMPONENTS Interpreter)
        if(WIN32 AND DXIL_DXC_EXECUTABLE AND Python3_FOUND)
            set(SM_INTEROP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop)
            set(SM_DXIL_DEPS ${SM_INTEROP_DIR}/dxc/kernels.hlsl ${SM_INTEROP_DIR}/dxc/dxc_cases.hlsli
                             ${SM_INTEROP_DIR}/dxc/dxc_case_list.h
                             ${SM_INTEROP_DIR}/shared/math_cases.hlsli ${SM_INTEROP_DIR}/shared/case_list.h)
            set(SM_DXIL_RUN ${DXIL_DXC_EXECUTABLE} -T cs_6_2 -E main -HV 2021 -I ${SM_INTEROP_DIR}
                            -D SM_INTEROP_OPAQUE_INPUTS=1)
            set(SM_DXIL_OUT ${CMAKE_CURRENT_BINARY_DIR}/dxil_kernels.dxil)
            set(SM_DXIL_HALF_OUT ${CMAKE_CURRENT_BINARY_DIR}/dxil_kernels_half.dxil)
            add_custom_command(
                OUTPUT ${SM_DXIL_OUT}
                COMMAND ${SM_DXIL_RUN} -D SM_INTEROP_DXIL=1
                        -Fo ${SM_DXIL_OUT} ${SM_INTEROP_DIR}/dxc/kernels.hlsl
                DEPENDS ${SM_DXIL_DEPS}
                COMMENT "dxc: compiling the HLSL cases to DXIL"
                VERBATIM)
            add_custom_command(
                OUTPUT ${SM_DXIL_HALF_OUT}
                COMMAND ${SM_DXIL_RUN} -enable-16bit-types -D SM_INTEROP_ENABLE_HALF=1
                        -Fo ${SM_DXIL_HALF_OUT} ${SM_INTEROP_DIR}/dxc/kernels.hlsl
                DEPENDS ${SM_DXIL_DEPS}
                COMMENT "dxc: compiling the half cases to DXIL (-enable-16bit-types)"
                VERBATIM)
            add_custom_target(dxil_interop_kernels ALL DEPENDS ${SM_DXIL_OUT} ${SM_DXIL_HALF_OUT})

            add_executable(dxil_d3d12_runner ${SM_INTEROP_DIR}/d3d12_runner.cpp)
            target_include_directories(dxil_d3d12_runner PRIVATE ${SM_INTEROP_DIR})
            target_compile_definitions(dxil_d3d12_runner PRIVATE
                "SM_D3D12_CASES_HEADER=\"dxc/dxc_case_list.h\"" SM_D3D12_CASES=SM_DXIL_CASES)
            target_link_libraries(dxil_d3d12_runner PRIVATE d3d12 dxgi)
            shader_math_warnings(dxil_d3d12_runner)
            add_executable(dxil_d3d12_runner_half ${SM_INTEROP_DIR}/d3d12_runner.cpp)
            target_include_directories(dxil_d3d12_runner_half PRIVATE ${SM_INTEROP_DIR})
            target_compile_definitions(dxil_d3d12_runner_half PRIVATE
                "SM_D3D12_CASES_HEADER=\"shared/case_list.h\"" SM_D3D12_CASES=SM_INTEROP_HALF_CASES)
            target_link_libraries(dxil_d3d12_runner_half PRIVATE d3d12 dxgi)
            shader_math_warnings(dxil_d3d12_runner_half)

            add_executable(shader_math_dxil_cpp ${SM_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_dxil_cpp PRIVATE SM_INTEROP_DXIL)
            target_include_directories(shader_math_dxil_cpp PRIVATE ${SM_INTEROP_DIR})
            target_link_libraries(shader_math_dxil_cpp PRIVATE shader_math)
            shader_math_warnings(shader_math_dxil_cpp)
            add_executable(shader_math_dxil_cpp_half ${SM_INTEROP_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_dxil_cpp_half PRIVATE SM_INTEROP_ENABLE_HALF)
            target_link_libraries(shader_math_dxil_cpp_half PRIVATE shader_math)
            shader_math_warnings(shader_math_dxil_cpp_half)

            # WARP's asin / acos / atan are about 1e-3 off libm in the cases
            # using them (its other transcendentals stay within near_)
            add_test(NAME dxil_interop_check
                     COMMAND ${Python3_EXECUTABLE} ${SM_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:dxil_d3d12_runner> $<TARGET_FILE:shader_math_dxil_cpp>
                             dxil ${SM_DXIL_OUT}
                             --tol=near_inverse_trig:2e-3 --tol=near_matrix_intrinsics:2e-3)
            add_test(NAME dxil_interop_half_check
                     COMMAND ${Python3_EXECUTABLE} ${SM_INTEROP_DIR}/compare.py
                             $<TARGET_FILE:dxil_d3d12_runner_half> $<TARGET_FILE:shader_math_dxil_cpp_half>
                             dxil ${SM_DXIL_HALF_OUT} --f16)
            set_tests_properties(dxil_interop_check dxil_interop_half_check PROPERTIES
                                 SKIP_RETURN_CODE 77)
        else()
            message(WARNING "SHADER_MATH_INTEROP_DXIL=ON but this is not Windows, or dxc "
                            "(set SHADER_MATH_DXC_DIR) or python3 was not found; "
                            "skipping the DXIL cross-check")
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

            # opt-in .stpq swizzles: a harness built with SHADER_MATH_GLSL_STPQ
            # (its own executable: the macro changes the vector class)
            set(SM_GLSL_STPQ_SPV ${CMAKE_CURRENT_BINARY_DIR}/glsl_kernels_stpq.spv)
            add_custom_command(
                OUTPUT ${SM_GLSL_STPQ_SPV}
                COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.1
                        -o ${SM_GLSL_STPQ_SPV} ${SM_GLSL_DIR}/kernels_stpq.comp
                DEPENDS ${SM_GLSL_DIR}/kernels_stpq.comp ${SM_GLSL_DIR}/stpq_cases.glsl
                        ${SM_GLSL_DIR}/stpq_case_list.h ${SM_GLSL_DIR}/math_cases.glsl
                COMMENT "glslang: compiling kernels_stpq.comp to SPIR-V"
                VERBATIM)
            add_custom_target(glsl_interop_stpq_spv ALL DEPENDS ${SM_GLSL_STPQ_SPV})

            add_executable(glsl_vk_runner_stpq ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/vk_runner.cpp)
            target_include_directories(glsl_vk_runner_stpq PRIVATE ${SM_GLSL_DIR})
            target_compile_definitions(glsl_vk_runner_stpq PRIVATE
                "SM_VK_CASES_HEADER=\"stpq_case_list.h\"" SM_VK_CASES=SM_GLSL_STPQ_CASES)
            target_link_libraries(glsl_vk_runner_stpq PRIVATE Vulkan::Vulkan)
            shader_math_warnings(glsl_vk_runner_stpq)

            add_executable(shader_math_glsl_interop_cpp_stpq ${SM_GLSL_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_glsl_interop_cpp_stpq PRIVATE
                SM_GLSL_STPQ SHADER_MATH_GLSL_STPQ)
            target_link_libraries(shader_math_glsl_interop_cpp_stpq PRIVATE shader_math)
            shader_math_warnings(shader_math_glsl_interop_cpp_stpq)

            add_test(NAME glsl_interop_stpq_check
                     COMMAND ${Python3_EXECUTABLE}
                             ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/compare.py
                             $<TARGET_FILE:glsl_vk_runner_stpq>
                             $<TARGET_FILE:shader_math_glsl_interop_cpp_stpq>
                             vulkan ${SM_GLSL_STPQ_SPV})

            # opt-in swizzles of swizzles: a twin built with
            # SHADER_MATH_NESTED_SWIZZLE (and .stpq for those letters)
            set(SM_GLSL_NESTED_SPV ${CMAKE_CURRENT_BINARY_DIR}/glsl_kernels_nested.spv)
            add_custom_command(
                OUTPUT ${SM_GLSL_NESTED_SPV}
                COMMAND ${GLSLANG_VALIDATOR} -V --target-env vulkan1.1
                        -o ${SM_GLSL_NESTED_SPV} ${SM_GLSL_DIR}/kernels_nested.comp
                DEPENDS ${SM_GLSL_DIR}/kernels_nested.comp ${SM_GLSL_DIR}/nested_cases.glsl
                        ${SM_GLSL_DIR}/nested_case_list.h ${SM_GLSL_DIR}/math_cases.glsl
                COMMENT "glslang: compiling kernels_nested.comp to SPIR-V"
                VERBATIM)
            add_custom_target(glsl_interop_nested_spv ALL DEPENDS ${SM_GLSL_NESTED_SPV})

            add_executable(glsl_vk_runner_nested ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/vk_runner.cpp)
            target_include_directories(glsl_vk_runner_nested PRIVATE ${SM_GLSL_DIR})
            target_compile_definitions(glsl_vk_runner_nested PRIVATE
                "SM_VK_CASES_HEADER=\"nested_case_list.h\"" SM_VK_CASES=SM_GLSL_NESTED_CASES)
            target_link_libraries(glsl_vk_runner_nested PRIVATE Vulkan::Vulkan)
            shader_math_warnings(glsl_vk_runner_nested)

            add_executable(shader_math_glsl_interop_cpp_nested ${SM_GLSL_DIR}/cpp_harness.cpp)
            target_compile_definitions(shader_math_glsl_interop_cpp_nested PRIVATE
                SM_GLSL_NESTED SHADER_MATH_NESTED_SWIZZLE SHADER_MATH_GLSL_STPQ)
            target_link_libraries(shader_math_glsl_interop_cpp_nested PRIVATE shader_math)
            shader_math_warnings(shader_math_glsl_interop_cpp_nested)

            add_test(NAME glsl_interop_nested_check
                     COMMAND ${Python3_EXECUTABLE}
                             ${CMAKE_CURRENT_SOURCE_DIR}/tests/interop/compare.py
                             $<TARGET_FILE:glsl_vk_runner_nested>
                             $<TARGET_FILE:shader_math_glsl_interop_cpp_nested>
                             vulkan ${SM_GLSL_NESTED_SPV})
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
    install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})
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
