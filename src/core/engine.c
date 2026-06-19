/**
 * @file engine.c
 * @brief DeckFlow Engine implementation
 */

#include "engine.h"
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

// Engine state
static struct {
    EngineState state;
    EngineConfig config;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    Scene* current_scene;
    Scene* next_scene;
    
    bool running;
    bool should_quit;
    
    float delta;
    float elapsed;
    int fps;
    int frame_count;
    Uint32 last_time;
    Uint32 fps_timer;
    
    char project_path[256];
} g_engine;

EngineConfig engine_config_default(void) {
    EngineConfig config = {
        .window_width = 480,
        .window_height = 320,
        .window_title = "DeckFlow Engine",
        .fullscreen = false,
        .resizable = false,
        .target_fps = 60,
        .physics_fps = 60.0f
    };
    return config;
}

bool engine_init(EngineConfig config) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL init failed: %s", SDL_GetError());
        return false;
    }
    
    g_engine.config = config;
    g_engine.state = ENGINE_STATE_CREATED;
    g_engine.running = false;
    g_engine.should_quit = false;
    g_engine.delta = 0;
    g_engine.elapsed = 0;
    g_engine.fps = 0;
    g_engine.frame_count = 0;
    g_engine.current_scene = NULL;
    g_engine.next_scene = NULL;
    
    // Create window
    Uint32 flags = SDL_WINDOW_SHOWN;
    if (config.fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (config.resizable) flags |= SDL_WINDOW_RESIZABLE;
    
    g_engine.window = SDL_CreateWindow(
        config.window_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.window_width,
        config.window_height,
        flags
    );
    
    if (!g_engine.window) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    // Create renderer
    g_engine.renderer = SDL_CreateRenderer(g_engine.window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!g_engine.renderer) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(g_engine.window);
        SDL_Quit();
        return false;
    }
    
    g_engine.state = ENGINE_STATE_READY;
    return true;
}

void engine_shutdown(void) {
    if (g_engine.current_scene) {
        scene_free(g_engine.current_scene);
        g_engine.current_scene = NULL;
    }
    
    if (g_engine.renderer) {
        SDL_DestroyRenderer(g_engine.renderer);
        g_engine.renderer = NULL;
    }
    
    if (g_engine.window) {
        SDL_DestroyWindow(g_engine.window);
        g_engine.window = NULL;
    }
    
    SDL_Quit();
    g_engine.state = ENGINE_STATE_STOPPED;
}

EngineState engine_get_state(void) {
    return g_engine.state;
}

void engine_start(void) {
    if (g_engine.state != ENGINE_STATE_READY) return;
    
    g_engine.running = true;
    g_engine.state = ENGINE_STATE_RUNNING;
    g_engine.last_time = SDL_GetTicks();
    g_engine.fps_timer = g_engine.last_time;
}

void engine_stop(void) {
    g_engine.running = false;
    g_engine.should_quit = true;
    g_engine.state = ENGINE_STATE_STOPPED;
}

void engine_pause(void) {
    if (g_engine.state == ENGINE_STATE_RUNNING) {
        g_engine.state = ENGINE_STATE_PAUSED;
    }
}

void engine_resume(void) {
    if (g_engine.state == ENGINE_STATE_PAUSED) {
        g_engine.state = ENGINE_STATE_RUNNING;
        g_engine.last_time = SDL_GetTicks();
    }
}

void engine_process_events(void) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                g_engine.should_quit = true;
                break;
                
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    g_engine.should_quit = true;
                }
                break;
                
            default:
                break;
        }
    }
}

void engine_update(float delta) {
    // Update FPS counter
    g_engine.elapsed += delta;
    
    // Process scene change
    if (g_engine.next_scene) {
        if (g_engine.current_scene) {
            scene_free(g_engine.current_scene);
        }
        g_engine.current_scene = g_engine.next_scene;
        g_engine.next_scene = NULL;
    }
    
    // Update current scene
    if (g_engine.current_scene) {
        scene_process(g_engine.current_scene, delta);
        scene_physics_process(g_engine.current_scene, delta);
    }
}

void engine_render(void) {
    if (!g_engine.renderer) return;
    
    // Clear screen
    SDL_SetRenderDrawColor(g_engine.renderer, 26, 26, 46, 255);
    SDL_RenderClear(g_engine.renderer);
    
    // TODO: Render scene
    
    // Present
    SDL_RenderPresent(g_engine.renderer);
}

void engine_set_fixed_update(float delta) {
    // Fixed timestep logic would go here
    (void)delta;
}

void engine_set_scene(Scene* scene) {
    g_engine.next_scene = scene;
}

Scene* engine_get_scene(void) {
    return g_engine.current_scene;
}

Scene* engine_load_scene(const char* path) {
    Scene* scene = scene_instance(path);
    if (scene) {
        engine_set_scene(scene);
    }
    return scene;
}

float engine_get_delta(void) {
    return g_engine.delta;
}

float engine_get_elapsed(void) {
    return g_engine.elapsed;
}

int engine_get_fps(void) {
    return g_engine.fps;
}

void engine_set_project_path(const char* path) {
    if (path) {
        strncpy(g_engine.project_path, path, sizeof(g_engine.project_path) - 1);
    }
}

const char* engine_get_project_path(void) {
    return g_engine.project_path;
}

void engine_print_info(void) {
    SDL_Log("=== DeckFlow Engine ===");
    SDL_Log("Version: %d.%d.%d", 
            DECKFLOW_VERSION_MAJOR,
            DECKFLOW_VERSION_MINOR,
            DECKFLOW_VERSION_PATCH);
    SDL_Log("State: %d", g_engine.state);
    SDL_Log("FPS: %d", g_engine.fps);
}

// Main loop (for testing)
void engine_main_loop(void) {
    engine_start();
    
    while (!g_engine.should_quit) {
        Uint32 current_time = SDL_GetTicks();
        g_engine.delta = (current_time - g_engine.last_time) / 1000.0f;
        g_engine.last_time = current_time;
        
        // Cap delta to prevent spiral of death
        if (g_engine.delta > 0.25f) {
            g_engine.delta = 0.25f;
        }
        
        // FPS calculation
        g_engine.frame_count++;
        if (current_time - g_engine.fps_timer >= 1000) {
            g_engine.fps = g_engine.frame_count;
            g_engine.frame_count = 0;
            g_engine.fps_timer = current_time;
        }
        
        engine_process_events();
        engine_update(g_engine.delta);
        engine_render();
        
        // Frame limiting
        Uint32 frame_time = SDL_GetTicks() - current_time;
        if (frame_time < 16) { // ~60 FPS
            SDL_Delay(16 - frame_time);
        }
    }
    
    engine_shutdown();
}
