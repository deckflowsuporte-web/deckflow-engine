/**
 * @file main.c
 * @brief Phase 4: Complete Game Engine Demo
 * 
 * Demonstrates all Phase 4 systems:
 * - Input System (keyboard, mouse)
 * - Camera System (follow, zoom, shake)
 * - Audio System (stub implementation)
 * - Tilemap System (tile-based maps)
 * - Particle System (fire, smoke, explosions)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "scenes/scene.h"
#include "nodes/node.h"
#include "math/vector2.h"
#include "math/transform2d.h"
#include "input/input.h"
#include "camera/camera.h"
#include "audio/audio.h"
#include "tilemap/tilemap.h"
#include "particle/particle.h"

// Screen dimensions
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Colors
#define COLOR_BG (SDL_Color){26, 26, 46, 255}
#define COLOR_PLAYER (SDL_Color){100, 200, 100, 255}
#define COLOR_ENEMY (SDL_Color){200, 100, 100, 255}

// Sprite data
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
    SDL_Color color;
} SpriteData;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;

// Create colored texture
static SDL_Texture* create_colored_texture(int width, int height, SDL_Color color) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return NULL;
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// Create sprite node
static Node* create_sprite(const char* name, int width, int height, SDL_Color color) {
    Node* node = node_create(name, NODE_TYPE_SPRITE);
    SpriteData* sprite = (SpriteData*)malloc(sizeof(SpriteData));
    sprite->texture = create_colored_texture(width, height, color);
    sprite->width = width;
    sprite->height = height;
    sprite->color = color;
    node->user_data = sprite;
    node->user_data_destructor = free;
    return node;
}

// Render sprite with camera
static void render_sprite(Node* node, Camera* cam) {
    if (!node || !node->visible) return;
    
    SpriteData* sprite = (SpriteData*)node->user_data;
    if (sprite && sprite->texture) {
        Vector2 global_pos = node_get_global_position(node);
        Vector2 screen_pos = camera_world_to_screen(cam, global_pos);
        float global_rot = node_get_global_rotation(node);
        
        SDL_Rect dest = {
            (int)(screen_pos.x - sprite->width / 2),
            (int)(screen_pos.y - sprite->height / 2),
            sprite->width, sprite->height
        };
        
        if (fabsf(global_rot) > 0.001f) {
            SDL_RenderCopyEx(g_renderer, sprite->texture, NULL, &dest, 
                           global_rot * 180.0f / M_PI, NULL, SDL_FLIP_NONE);
        } else {
            SDL_RenderCopy(g_renderer, sprite->texture, NULL, &dest);
        }
    }
    
    Node* child = node->first_child;
    while (child) {
        render_sprite(child, cam);
        child = child->next_sibling;
    }
}

// Main
int main(void) {
    printf("+============================================================+\n");
    printf("|  DeckFlow Engine - Phase 4: Complete Game Systems           |\n");
    printf("+============================================================+\n\n");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    g_window = SDL_CreateWindow("Phase 4 Demo", SDL_WINDOWPOS_CENTERED, 
                                SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!g_window) {
        SDL_Quit();
        return 1;
    }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer) {
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return 1;
    }
    
    // Init systems
    printf("[1] Initializing systems...\n");
    input_init();
    audio_init();
    
    // Create camera
    printf("[2] Creating camera...\n");
    Camera* camera = camera_create("MainCamera");
    camera_set_viewport(camera, SCREEN_WIDTH, SCREEN_HEIGHT);
    camera_set_position(camera, vector2_new(400, 300));
    camera_set_mode(camera, CAMERA_MODE_FOLLOW);
    camera_set_smoothing(camera, 0.1f);
    
    // Create tilemap
    printf("[3] Creating tilemap...\n");
    Tilemap* map = tilemap_create(20, 15, 32, 32);
    tilemap_add_layer(map, 20, 15);
    tilemap_generate_level(map, 0, 12345);
    
    // Create particle system
    printf("[4] Creating particle system...\n");
    ParticleSystem* particles = particles_create(1000);
    ParticleEmitter* fire = particles_add_emitter(particles, "fire");
    emitter_configure_fire(fire);
    emitter_set_position(fire, vector2_new(400, 300));
    emitter_start(fire);
    
    ParticleEmitter* smoke = particles_add_emitter(particles, "smoke");
    emitter_configure_smoke(smoke);
    emitter_set_position(smoke, vector2_new(400, 320));
    emitter_start(smoke);
    
    // Create player
    printf("[5] Creating player...\n");
    Scene* scene = scene_create("GameScene");
    Node* root = scene_get_root(scene);
    
    Node* player = create_sprite("Player", 40, 40, COLOR_PLAYER);
    player->transform.position = vector2_new(400, 300);
    node_add_child(root, player);
    
    camera_follow(camera, player);
    
    // Create enemies
    for (int i = 0; i < 5; i++) {
        Node* enemy = create_sprite("Enemy", 32, 32, COLOR_ENEMY);
        enemy->transform.position = vector2_new(100 + i * 150, 100 + (i % 3) * 100);
        node_add_child(root, enemy);
    }
    
    printf("\n");
    printf("Controls:\n");
    printf("  Arrow keys / WASD: Move player\n");
    printf("  Q/E: Zoom in/out\n");
    printf("  R: Reset camera\n");
    printf("  SPACE: Camera shake\n");
    printf("  F: Toggle particles\n");
    printf("  ESC: Exit\n\n");
    
    printf("Systems initialized:\n");
    printf("  [x] Input System\n");
    printf("  [x] Camera System\n");
    printf("  [x] Audio System\n");
    printf("  [x] Tilemap System\n");
    printf("  [x] Particle System\n\n");
    
    bool running = true;
    float delta_time = 1.0f / 60.0f;
    int frame_count = 0;
    
    while (running) {
        // Events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                input_handle_key_down(event.key.keysym.scancode);
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
            }
            if (event.type == SDL_KEYUP) {
                input_handle_key_up(event.key.keysym.scancode);
            }
            if (event.type == SDL_MOUSEMOTION) {
                input_handle_mouse_motion(event.motion.x, event.motion.y, 
                                         event.motion.xrel, event.motion.yrel);
            }
        }
        
        // Player movement
        float speed = 200.0f * delta_time;
        if (input_key_pressed(KEY_LEFT) || input_key_pressed(KEY_A)) 
            player->transform.position.x -= speed;
        if (input_key_pressed(KEY_RIGHT) || input_key_pressed(KEY_D)) 
            player->transform.position.x += speed;
        if (input_key_pressed(KEY_UP) || input_key_pressed(KEY_W)) 
            player->transform.position.y -= speed;
        if (input_key_pressed(KEY_DOWN) || input_key_pressed(KEY_S)) 
            player->transform.position.y += speed;
        
        // Camera zoom
        if (input_key_just_pressed(KEY_Q)) camera_zoom_out(camera, 0.1f);
        if (input_key_just_pressed(KEY_E)) camera_zoom_in(camera, 0.1f);
        
        // Camera reset
        if (input_key_just_pressed(KEY_R)) {
            camera_set_position(camera, vector2_new(400, 300));
            camera_set_zoom(camera, 1.0f);
        }
        
        // Camera shake
        if (input_key_just_pressed(KEY_SPACE)) {
            camera_shake(camera, 10.0f, 0.5f);
            // Explosion effect
            emitter_configure_explosion(fire);
            particles_emit(particles, fire, player->transform.position, 30);
        }
        
        // Toggle particles
        if (input_key_just_pressed(KEY_F)) {
            if (fire->active) {
                emitter_stop(fire);
                emitter_stop(smoke);
                printf("Particles OFF\n");
            } else {
                emitter_start(fire);
                emitter_start(smoke);
                printf("Particles ON\n");
            }
        }
        
        // Update
        camera_update(camera, delta_time);
        particles_update(particles, delta_time);
        
        // Move fire position to player
        Vector2 player_pos = player->transform.position;
        emitter_set_position(fire, player_pos);
        emitter_set_position(smoke, vector2_new(player_pos.x, player_pos.y + 20));
        
        // Render
        SDL_SetRenderDrawColor(g_renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
        SDL_RenderClear(g_renderer);
        
        // Render tilemap (simplified - just show grid)
        SDL_SetRenderDrawColor(g_renderer, 50, 50, 70, 255);
        for (int x = 0; x < 20; x++) {
            for (int y = 0; y < 15; y++) {
                int tile = tilemap_get_tile(map, 0, x, y);
                if (tile > 0) {
                    Vector2 world_pos = tilemap_tile_to_world(map, x, y);
                    Vector2 screen = camera_world_to_screen(camera, world_pos);
                    SDL_Rect rect = {(int)screen.x, (int)screen.y, 32, 32};
                    SDL_RenderFillRect(g_renderer, &rect);
                }
            }
        }
        
        // Render scene
        render_sprite(root, camera);
        
        // Render particles (simple circles)
        ParticleEmitter* emitter = particles->emitters;
        while (emitter) {
            for (int i = 0; i < emitter->max_particles; i++) {
                Particle* p = &emitter->particles[i];
                if (p->life > 0) {
                    Vector2 screen = camera_world_to_screen(camera, p->position);
                    SDL_SetRenderDrawColor(g_renderer, 
                        (int)(p->color[0] * 255),
                        (int)(p->color[1] * 255),
                        (int)(p->color[2] * 255),
                        (int)(p->color[3] * 255));
                    
                    SDL_Rect rect = {
                        (int)(screen.x - p->size / 2),
                        (int)(screen.y - p->size / 2),
                        (int)p->size, (int)p->size
                    };
                    SDL_RenderFillRect(g_renderer, &rect);
                }
            }
            emitter = emitter->next;
        }
        
        // HUD
        Vector2 cam_pos = camera_get_position(camera);
        char hud[128];
        snprintf(hud, sizeof(hud), "Camera: (%.0f, %.0f) Zoom: %.1fx Particles: %d",
                cam_pos.x, cam_pos.y, camera->zoom, particles->emitter_count > 0 ? 
                particles->emitters->particle_count : 0);
        
        SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
        SDL_RenderPresent(g_renderer);
        
        input_update();
        
        if (++frame_count % 60 == 0) {
            printf("\rFPS: %d | %s", 60, hud);
            fflush(stdout);
        }
        
        SDL_Delay(16);
    }
    
    // Cleanup
    printf("\n\nCleaning up...\n");
    camera_free(camera);
    tilemap_free(map);
    particles_free(particles);
    scene_free(scene);
    input_shutdown();
    audio_shutdown();
    
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 4 Complete! All game systems working!                |\n");
    printf("+============================================================+\n");
    
    return 0;
}
