/**
 * @file engine.h
 * @brief DeckFlow Engine - Main header
 */

#ifndef DECKFLOW_ENGINE_H
#define DECKFLOW_ENGINE_H

#include "scenes/scene.h"

// Version
#define DECKFLOW_VERSION_MAJOR 1
#define DECKFLOW_VERSION_MINOR 0
#define DECKFLOW_VERSION_PATCH 0

// Engine state
typedef enum EngineState {
    ENGINE_STATE_UNINITIALIZED = 0,
    ENGINE_STATE_CREATED,
    ENGINE_STATE_READY,
    ENGINE_STATE_RUNNING,
    ENGINE_STATE_PAUSED,
    ENGINE_STATE_STOPPED
} EngineState;

// Engine configuration
typedef struct EngineConfig {
    int window_width;
    int window_height;
    const char* window_title;
    bool fullscreen;
    bool resizable;
    int target_fps;
    float physics_fps;
} EngineConfig;

// Default configuration
EngineConfig engine_config_default(void);

// Engine lifecycle
bool engine_init(EngineConfig config);
void engine_shutdown(void);
EngineState engine_get_state(void);

// Main loop
void engine_start(void);
void engine_stop(void);
void engine_pause(void);
void engine_resume(void);

// Frame control
void engine_process_events(void);
void engine_update(float delta);
void engine_render(void);
void engine_set_fixed_update(float delta);

// Scene management
void engine_set_scene(Scene* scene);
Scene* engine_get_scene(void);
Scene* engine_load_scene(const char* path);

// Time
float engine_get_delta(void);
float engine_get_elapsed(void);
int engine_get_fps(void);

// Project paths
void engine_set_project_path(const char* path);
const char* engine_get_project_path(void);

// Debug
void engine_print_info(void);

#endif // DECKFLOW_ENGINE_H
