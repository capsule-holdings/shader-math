# Consumer project for the packaging check: builds one TU against
# shader_math::shader_math obtained either way a downstream project would.
#   -DSHADER_MATH_CONSUME=find_package  (CMAKE_PREFIX_PATH = install prefix)
#   -DSHADER_MATH_CONSUME=subdirectory  (SHADER_MATH_SOURCE_DIR = repo root)
cmake_minimum_required(VERSION 3.16)
project(shader_math_consumer LANGUAGES CXX)

if(SHADER_MATH_CONSUME STREQUAL "find_package")
    find_package(shader_math 0.3 REQUIRED CONFIG)
elseif(SHADER_MATH_CONSUME STREQUAL "subdirectory")
    add_subdirectory(${SHADER_MATH_SOURCE_DIR} shader_math)
    # A consumed shader-math must not drag its own tests or install rules in.
    if(TARGET shader_math_tests OR TARGET sandbox)
        message(FATAL_ERROR "shader-math built its tests as a subproject")
    endif()
else()
    message(FATAL_ERROR "set SHADER_MATH_CONSUME to find_package or subdirectory")
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE shader_math::shader_math)
# -DSHADER_MATH_FORCE_SCALAR=ON must reach the consumer through the target
if(SHADER_MATH_FORCE_SCALAR)
    target_compile_definitions(consumer PRIVATE SM_EXPECT_SCALAR)
endif()

enable_testing()
add_test(NAME consumer COMMAND consumer)
