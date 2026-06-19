# Phase 1 Test - CMakeLists.txt
# Builds the Phase 1 functional test (no SDL2 dependency)

cmake_minimum_required(VERSION 3.18.1)

project("Phase1Test" LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Engine source files (exclude engine.c which requires SDL2)
set(ENGINE_SOURCES
    ../../src/math/vector2.c
    ../../src/math/transform2d.c
    ../../src/nodes/node.c
    ../../src/scenes/scene.c
    ../../src/scenes/scene_tree.c
)

# Include directories
set(ENGINE_INCLUDE_DIRS
    ../../src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create test executable
add_executable(phase1_test
    main.c
    ${ENGINE_SOURCES}
)

target_include_directories(phase1_test PRIVATE
    ${ENGINE_INCLUDE_DIRS}
)

# Enable all warnings
target_compile_options(phase1_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Print configuration
message(STATUS "Building Phase 1 Test (no SDL2)")
message(STATUS "Engine sources: ${ENGINE_SOURCES}")
