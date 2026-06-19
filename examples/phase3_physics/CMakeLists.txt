# Phase 3: Box2D Physics - CMakeLists.txt

cmake_minimum_required(VERSION 3.18.1)

project("Phase3Physics" LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Box2D (install with: vcpkg install box2d)
find_package(box2d CONFIG)

# Engine source files
set(ENGINE_SOURCES
    ../../src/math/vector2.c
    ../../src/math/transform2d.c
    ../../src/nodes/node.c
    ../../src/scenes/scene.c
    ../../src/scenes/scene_tree.c
)

set(ENGINE_INCLUDE_DIRS ../../src)

# Create executable
add_executable(phase3_physics
    main.c
    ${ENGINE_SOURCES}
)

target_include_directories(phase3_physics PRIVATE
    ${ENGINE_INCLUDE_DIRS}
    ${box2d_INCLUDE_DIRS}
)

target_link_libraries(phase3_physics
    SDL2::SDL2main
    SDL2::SDL2
    box2d::box2d
    m
)

message(STATUS "Building Phase 3: Box2D Physics")
