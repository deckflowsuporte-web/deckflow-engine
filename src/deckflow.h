/**
 * @file deckflow.h
 * @brief DeckFlow Engine - Main Header
 * 
 * All-in-one header for the DeckFlow 2D Game Engine.
 * Include this file to access all engine systems.
 */

#ifndef DECKFLOW_H
#define DECKFLOW_H

// Core types
#include <stdbool.h>
#include <stdint.h>

// Math
#include "math/vector2.h"
#include "math/transform2d.h"

// Scene Graph
#include "nodes/node.h"
#include "scenes/scene.h"

// Phase 4 Systems
#include "input/input.h"
#include "camera/camera.h"
#include "audio/audio.h"
#include "tilemap/tilemap.h"
#include "particle/particle.h"

// Version
#define DECKFLOW_VERSION_MAJOR 1
#define DECKFLOW_VERSION_MINOR 0
#define DECKFLOW_VERSION_PATCH 0

// Engine info
#define DECKFLOW_NAME "DeckFlow Engine"
#define DECKFLOW_DESCRIPTION "Lightweight 2D Game Engine"

#endif // DECKFLOW_H
