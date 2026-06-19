/**
 * @file main.c
 * @brief Phase 2: SDL2 + Sprite Rendering
 * 
 * Demonstrates:
 * - SDL2 window and renderer setup
 * - Sprite creation and rendering
 * - Scene tree with visual output
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

// Screen dimensions
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Colors
#define COLOR_BG (SDL_Color){26, 26, 46, 255}
#define COLOR_PLAYER (SDL_Color){100, 200, 100, 255}
#define COLOR_ENEMY (SDL_Color){200, 100, 100, 255}
#define COLOR_COIN (SDL_Color){255, 215, 0, 255}

// Sprite data structure
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
    SDL_Color color;
} SpriteData;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;

// Create a simple colored rectangle texture
static SDL_Texture* create_colored_texture(int width, int height, SDL_Color color) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
    if (!surface) return NULL;
    
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);
    SDL_FreeSurface(surface);
    
    return texture;
}

// Initialize SDL
static bool init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    g_window = SDL_CreateWindow(
        "DeckFlow Engine - Phase 2: Sprite Rendering",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!g_window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!g_renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }
    
    printf("SDL initialized successfully!\n");
    return true;
}

// Cleanup SDL
static void cleanup_sdl(void) {
    if (g_renderer) SDL_DestroyRenderer(g_renderer);
    if (g_window) SDL_DestroyWindow(g_window);
    SDL_Quit();
    printf("SDL cleaned up.\n");
}

// Render a node and all its children
static void render_node(Node* node) {
    if (!node || !node->visible) return;
    
    // Check if this is a sprite node (user_data contains sprite info)
    if (node->user_data) {
        SpriteData* sprite = (SpriteData*)node->user_data;
        if (sprite && sprite->texture) {
            Vector2 global_pos = node_get_global_position(node);
            float global_rot = node_get_global_rotation(node);
            
            SDL_Rect dest = {
                (int)global_pos.x - sprite->width / 2,
                (int)global_pos.y - sprite->height / 2,
                sprite->width,
                sprite->height
            };
            
            // Apply rotation
            if (fabsf(global_rot) > 0.001f) {
                SDL_RenderCopyEx(g_renderer, sprite->texture, NULL, &dest, 
                                 global_rot * 180.0f / M_PI, NULL, SDL_FLIP_NONE);
            } else {
                SDL_RenderCopy(g_renderer, sprite->texture, NULL, &dest);
            }
        }
    }
    
    // Render children
    Node* child = node->first_child;
    while (child) {
        render_node(child);
        child = child->next_sibling;
    }
}

// Create a sprite node
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

// Print scene tree to console
static void print_scene_tree(Node* node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    const char* type_name = node_get_type_name(node->type);
    Vector2 pos = node_get_global_position(node);
    
    if (node->user_data) {
        printf("|- %s [%s] at (%.0f, %.0f)\n", node->name, type_name, pos.x, pos.y);
    } else {
        printf("|- %s [%s]\n", node->name, type_name);
    }
    
    Node* child = node->first_child;
    while (child) {
        print_scene_tree(child, depth + 1);
        child = child->next_sibling;
    }
}

int main(void) {
    printf("+============================================================+\n");
    printf("|  DeckFlow Engine - Phase 2: SDL2 + Sprite Rendering        |\n");
    printf("+============================================================+\n\n");
    
    // Initialize SDL
    if (!init_sdl()) {
        return 1;
    }
    
    // Create scene
    Scene* scene = scene_create("GameScene");
    Node* root = scene_get_root(scene);
    
    printf("Created scene: %s\n\n", scene->name);
    
    // Create sprites
    printf("Creating sprites...\n");
    
    // Player sprite (green square)
    Node* player = create_sprite("Player", 64, 64, COLOR_PLAYER);
    player->transform.position = vector2_new(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    node_add_child(root, player);
    
    // Enemy sprites (red squares)
    Node* enemy1 = create_sprite("Enemy1", 48, 48, COLOR_ENEMY);
    enemy1->transform.position = vector2_new(200, 150);
    node_add_child(root, enemy1);
    
    Node* enemy2 = create_sprite("Enemy2", 48, 48, COLOR_ENEMY);
    enemy2->transform.position = vector2_new(450, 300);
    enemy2->transform.rotation = 0.5f;  // Rotate 30 degrees
    node_add_child(root, enemy2);
    
    // Coin sprites (gold squares)
    Node* coin1 = create_sprite("Coin1", 24, 24, COLOR_COIN);
    coin1->transform.position = vector2_new(100, 400);
    node_add_child(root, coin1);
    
    Node* coin2 = create_sprite("Coin2", 24, 24, COLOR_COIN);
    coin2->transform.position = vector2_new(540, 100);
    node_add_child(root, coin2);
    
    // Create a parent node with children (demonstrates hierarchy)
    Node* enemies = node_create("Enemies", NODE_TYPE_SPATIAL_2D);
    enemies->transform.position = vector2_new(50, 50);  // Offset all enemies
    node_add_child(root, enemies);
    
    // This enemy is child of Enemies node (demonstrates parent/child transform)
    Node* enemy3 = create_sprite("Enemy3", 40, 40, COLOR_ENEMY);
    enemy3->transform.position = vector2_new(100, 100);  // Will be at (150, 150) globally
    node_add_child(enemies, enemy3);
    
    printf("\nScene tree:\n");
    print_scene_tree(root, 0);
    
    // Main loop
    printf("\nStarting render loop...\n");
    printf("Controls: Arrow keys to move, Q/E to rotate, ESC to exit\n\n");
    
    bool running = true;
    SDL_Event event;
    
    while (running) {
        // Process events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                // Arrow keys to move player
                if (event.key.keysym.sym == SDLK_LEFT) {
                    player->transform.position.x -= 10;
                }
                if (event.key.keysym.sym == SDLK_RIGHT) {
                    player->transform.position.x += 10;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    player->transform.position.y -= 10;
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    player->transform.position.y += 10;
                }
                // Rotate player with Q/E
                if (event.key.keysym.sym == SDLK_q) {
                    player->transform.rotation -= 0.1f;
                }
                if (event.key.keysym.sym == SDLK_e) {
                    player->transform.rotation += 0.1f;
                }
            }
        }
        
        // Clear screen
        SDL_SetRenderDrawColor(g_renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
        SDL_RenderClear(g_renderer);
        
        // Render scene
        render_node(root);
        
        // Present
        SDL_RenderPresent(g_renderer);
        
        // Frame rate limiting
        SDL_Delay(16);  // ~60 FPS
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    scene_free(scene);
    cleanup_sdl();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 2 Complete! Sprite rendering working!               |\n");
    printf("+============================================================+\n");
    
    return 0;
}
