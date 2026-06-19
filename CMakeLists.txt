cmake_minimum_required(VERSION 3.18.1)

project("DeckFlowEngine" VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(ENGINE_SOURCES
    src/core/engine.c
    src/math/vector2.c
    src/math/transform2d.c
    src/nodes/node.c
    src/scenes/scene.c
    src/scenes/scene_tree.c
)

# Create shared library
add_library(deckflow SHARED ${ENGINE_SOURCES})

# Find SDL2 (will be provided by Android)
find_package(SDL2 REQUIRED CONFIG)

target_include_directories(deckflow PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(deckflow
    android
    log
    SDL2
)
