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
        tests/test_simd.cpp)

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

    add_executable(sandbox examples/sandbox.cpp)
    target_link_libraries(sandbox PRIVATE shader_math)
endif()
