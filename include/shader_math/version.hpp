// shader-math version. Must match project(VERSION) in CMakeLists.txt; the
// CMake configure step fails if they differ, and the release workflow
// checks both against the pushed tag.
#pragma once

#define SHADER_MATH_VERSION_MAJOR 0
#define SHADER_MATH_VERSION_MINOR 4
#define SHADER_MATH_VERSION_PATCH 1

// MAJOR * 10000 + MINOR * 100 + PATCH, for #if comparisons
#define SHADER_MATH_VERSION \
    (SHADER_MATH_VERSION_MAJOR * 10000 + SHADER_MATH_VERSION_MINOR * 100 + SHADER_MATH_VERSION_PATCH)
