# CMakeLists.txt for DeckFlow Engine - Android Native Build
# 
# This file enables building the native C library using CMake instead of ndk-build.
# Use this for modern Android Studio projects.

cmake_minimum_required(VERSION 3.18.1)

project(deckflow_engine VERSION 1.0.0 LANGUAGES C)

# ========================
# ENGINE SOURCE PATH
# ========================
set(ENGINE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../src")

# ========================
# ENGINE MODULES
# ========================
set(ENGINE_CORE_SOURCES
    ${ENGINE_SRC_DIR}/math/vector2.c
    ${ENGINE_SRC_DIR}/math/transform2d.c
    ${ENGINE_SRC_DIR}/nodes/node.c
    ${ENGINE_SRC_DIR}/scenes/scene.c
)

set(ENGINE_PHYSICS_SOURCES
    ${ENGINE_SRC_DIR}/physics/physics.c
)

set(ENGINE_RENDER_SOURCES
    # Add sprite and rendering sources here
)

# ========================
# LIBRARY: deckflow_engine
# ========================
add_library(deckflow_engine STATIC
    ${ENGINE_CORE_SOURCES}
    ${ENGINE_PHYSICS_SOURCES}
    ${ENGINE_RENDER_SOURCES}
)

target_include_directories(deckflow_engine PUBLIC
    ${ENGINE_SRC_DIR}
    ${ENGINE_SRC_DIR}/math
    ${ENGINE_SRC_DIR}/nodes
    ${ENGINE_SRC_DIR}/scenes
    ${ENGINE_SRC_DIR}/physics
)

target_compile_options(deckflow_engine PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
    -O2
    -DNDEBUG
)

# ========================
# LIBRARY: deckflow_main
# ========================
add_library(deckflow_main SHARED
    NativeMain.cpp
)

target_link_libraries(deckflow_main
    deckflow_engine
    android
    EGL
    GLESv2
    log
)

target_include_directories(deckflow_main PRIVATE
    ${ENGINE_SRC_DIR}
    ${ENGINE_SRC_DIR}/math
    ${ENGINE_SRC_DIR}/nodes
    ${ENGINE_SRC_DIR}/scenes
    ${ENGINE_SRC_DIR}/physics
)

target_compile_options(deckflow_main PRIVATE
    -Wall
    -Wextra
    -O2
    -std=c11
)

# ========================
# EXPORT
# ========================
add_library(deckflow::deckflow ALIAS deckflow_engine)
