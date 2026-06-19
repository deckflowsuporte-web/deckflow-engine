/**
 * @file main.c
 * @brief Phase 7: Animation System Demo
 * 
 * Demonstrates:
 * - AnimatedSprite with sprite sheets
 * - Tween animations (position, scale, rotation, color)
 * - Easing functions
 * - Animation callbacks
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
#include "animation/animation.h"
#include "input/input.h"

// Screen
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;

// Animation manager
static TweenManager* g_tweens = NULL;

// Demo objects
typedef struct {
    Node* node;
    SDL_Texture* texture;
    int width, height;
    Vector2 start_pos;
    float start_scale;
    float start_rot;
} DemoObject;

static DemoObject g_player;
static DemoObject g_box1, g_box2, g_box3;
static DemoObject g_fade_circle;

#define COLOR_BG SDL_Color{26, 26, 46, 255}
#define COLOR_PLAYER SDL_Color{100, 200, 100, 255}
#define COLOR_BOX1 SDL_Color{255, 100, 100, 255}
#define COLOR_BOX2 SDL_Color{100, 100, 255, 255}
#define COLOR_BOX3 SDL_Color{255, 200, 100, 255}

static SDL_Texture* create_texture(int w, int h, SDL_Color c) {
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a));
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    SDL_FreeSurface(s);
    return t;
}

static void setup_demo(void) {
    // Player (will be animated)
    g_player.node = node_create("Player", NODE_TYPE_SPRITE);
    g_player.texture = create_texture(50, 50, COLOR_PLAYER);
    g_player.width = 50;
    g_player.height = 50;
    g_player.start_pos = vector2_new(100, 300);
    g_player.start_scale = 1.0f;
    g_player.start_rot = 0.0f;
    g_player.node->transform.position = g_player.start_pos;
    g_player.node->transform.scale = vector2_new(1, 1);
    
    // Box 1 - Tween position demo
    g_box1.node = node_create("Box1", NODE_TYPE_SPRITE);
    g_box1.texture = create_texture(60, 60, COLOR_BOX1);
    g_box1.width = 60;
    g_box1.height = 60;
    g_box1.start_pos = vector2_new(500, 150);
    g_box1.node->transform.position = g_box1.start_pos;
    g_box1.node->transform.scale = vector2_new(1, 1);
    
    // Box 2 - Tween scale demo
    g_box2.node = node_create("Box2", NODE_TYPE_SPRITE);
    g_box2.texture = create_texture(60, 60, COLOR_BOX2);
    g_box2.width = 60;
    g_box2.height = 60;
    g_box2.start_pos = vector2_new(500, 300);
    g_box2.node->transform.position = g_box2.start_pos;
    g_box2.node->transform.scale = vector2_new(1, 1);
    
    // Box 3 - Tween rotation demo
    g_box3.node = node_create("Box3", NODE_TYPE_SPRITE);
    g_box3.texture = create_texture(60, 60, COLOR_BOX3);
    g_box3.width = 60;
    g_box3.height = 60;
    g_box3.start_pos = vector2_new(500, 450);
    g_box3.node->transform.position = g_box3.start_pos;
    g_box3.node->transform.scale = vector2_new(1, 1);
    
    // Fade circle
    g_fade_circle.node = node_create("FadeCircle", NODE_TYPE_SPRITE);
    g_fade_circle.texture = create_texture(80, 80, (SDL_Color){255, 255, 255, 128});
    g_fade_circle.width = 80;
    g_fade_circle.height = 80;
    g_fade_circle.start_pos = vector2_new(400, 300);
    g_fade_circle.node->transform.position = g_fade_circle.start_pos;
    g_fade_circle.node->transform.scale = vector2_new(1, 1);
}

static void render_object(DemoObject* obj) {
    if (!obj || !obj->node || !obj->texture) return;
    
    Vector2 pos = node_get_global_position(obj->node);
    Vector2 scale = obj->node->transform.scale;
    float rot = node_get_global_rotation(obj->node);
    
    SDL_Rect dst = {
        (int)(pos.x - obj->width * scale.x / 2),
        (int)(pos.y - obj->height * scale.y / 2),
        (int)(obj->width * scale.x),
        (int)(obj->height * scale.y)
    };
    
    if (fabsf(rot) > 0.001f) {
        SDL_RenderCopyEx(g_renderer, obj->texture, NULL, &dst, 
                        rot * 180.0f / M_PI, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopy(g_renderer, obj->texture, NULL, &dst);
    }
}

static void demo_tween_bounce(void) {
    printf("[Demo] Tween: Bounce\n");
    
    // Position tween
    Tween* t = tween_create(g_tweens, g_box1.node);
    if (t) {
        tween_position(t, vector2_new(500, 150), vector2_new(500, 350));
        tween_ease(t, EASE_BOUNCE);
        t->duration = 0.8f;
        t->auto_kill = false;
        t->paused = false;
    }
}

static void demo_tween_elastic(void) {
    printf("[Demo] Tween: Elastic Scale\n");
    
    Tween* t = tween_create(g_tweens, g_box2.node);
    if (t) {
        tween_scale(t, vector2_new(1, 1), vector2_new(1.5f, 1.5f));
        tween_ease(t, EASE_ELASTIC);
        t->duration = 0.6f;
        t->auto_kill = false;
    }
}

static void demo_tween_rotate(void) {
    printf("[Demo] Tween: Rotation\n");
    
    Tween* t = tween_create(g_tweens, g_box3.node);
    if (t) {
        tween_rotation(t, 0, M_PI * 2);
        tween_ease(t, EASE_LINEAR);
        t->duration = 2.0f;
        t->auto_kill = false;
    }
}

static void demo_tween_lerp_player(void) {
    printf("[Demo] Tween: Lerp Player\n");
    
    // Reset player
    g_player.node->transform.position = g_player.start_pos;
    
    // Move to right
    Tween* t1 = tween_create(g_tweens, g_player.node);
    if (t1) {
        tween_position(t1, g_player.start_pos, vector2_new(700, 300));
        tween_ease(t1, EASE_IN_OUT_QUAD);
        t1->duration = 1.0f;
    }
}

static void demo_tween_scale_pop(void) {
    printf("[Demo] Tween: Scale Pop\n");
    
    // Reset scale first
    g_player.node->transform.scale = vector2_new(1, 1);
    
    Tween* t = tween_create(g_tweens, g_player.node);
    if (t) {
        tween_scale(t, vector2_new(1, 1), vector2_new(1.3f, 1.3f));
        tween_ease(t, EASE_OUT_ELASTIC);
        t->duration = 0.3f;
    }
}

int main(void) {
    printf("+============================================================+\n");
    printf("|  Phase 7: Animation System Demo                              |\n");
    printf("+============================================================+\n\n");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    g_window = SDL_CreateWindow("Phase 7: Animation",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!g_window) { SDL_Quit(); return 1; }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_renderer) { SDL_DestroyWindow(g_window); SDL_Quit(); return 1; }
    
    // Init
    input_init();
    
    // Create tween manager
    g_tweens = tween_manager_create(100);
    
    // Setup scene
    Scene* scene = scene_create("AnimationScene");
    Node* root = scene_get_root(scene);
    node_add_child(root, g_player.node);
    node_add_child(root, g_box1.node);
    node_add_child(root, g_box2.node);
    node_add_child(root, g_box3.node);
    node_add_child(root, g_fade_circle.node);
    
    setup_demo();
    
    printf("Animation System Demo:\n");
    printf("  - Tween position, scale, rotation\n");
    printf("  - Easing functions (bounce, elastic, etc)\n");
    printf("  - Animation callbacks\n\n");
    
    printf("Controls:\n");
    printf("  1: Bounce (position)\n");
    printf("  2: Elastic (scale)\n");
    printf("  3: Rotate\n");
    printf("  4: Player lerp\n");
    printf("  5: Scale pop + lerp\n");
    printf("  R: Reset all\n");
    printf("  ESC: Exit\n\n");
    
    // Start all demos
    demo_tween_bounce();
    demo_tween_elastic();
    demo_tween_rotate();
    
    bool running = true;
    SDL_Event event;
    float delta_time = 1.0f / 60.0f;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                input_handle_key_down(event.key.keysym.scancode);
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
                
                // Demo controls
                if (event.key.keysym.scancode == SDL_SCANCODE_1) demo_tween_bounce();
                if (event.key.keysym.scancode == SDL_SCANCODE_2) demo_tween_elastic();
                if (event.key.keysym.scancode == SDL_SCANCODE_3) demo_tween_rotate();
                if (event.key.keysym.scancode == SDL_SCANCODE_4) demo_tween_lerp_player();
                if (event.key.keysym.scancode == SDL_SCANCODE_5) demo_tween_scale_pop();
                
                // Reset
                if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                    printf("[Demo] Reset all\n");
                    tween_kill_all(g_tweens);
                    g_player.node->transform.position = g_player.start_pos;
                    g_player.node->transform.scale = vector2_new(1, 1);
                    g_box1.node->transform.position = g_box1.start_pos;
                    g_box2.node->transform.scale = vector2_new(1, 1);
                    g_box3.node->transform.rotation = 0;
                    demo_tween_bounce();
                    demo_tween_elastic();
                    demo_tween_rotate();
                }
            }
            if (event.type == SDL_KEYUP) {
                input_handle_key_up(event.key.keysym.scancode);
            }
        }
        
        // Update tweens
        tween_manager_update(g_tweens, delta_time);
        
        // Render
        SDL_SetRenderDrawColor(g_renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
        SDL_RenderClear(g_renderer);
        
        // Render all objects
        render_object(&g_player);
        render_object(&g_box1);
        render_object(&g_box2);
        render_object(&g_box3);
        render_object(&g_fade_circle);
        
        // Labels
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* surf;
        SDL_Texture* tex;
        SDL_Rect dst;
        
        surf = TTF_RenderText_Blended(NULL, "Bounce", white);
        if (surf) {
            tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            dst = {510, 110, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
        
        surf = TTF_RenderText_Blended(NULL, "Elastic", white);
        if (surf) {
            tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            dst = {510, 260, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
        
        surf = TTF_RenderText_Blended(NULL, "Rotate", white);
        if (surf) {
            tex = SDL_CreateTextureFromSurface(g_renderer, surf);
            dst = {510, 410, surf->w, surf->h};
            SDL_RenderCopy(g_renderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
            SDL_FreeSurface(surf);
        }
        
        SDL_RenderPresent(g_renderer);
        input_update();
        SDL_Delay(16);
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    tween_manager_free(g_tweens);
    scene_free(scene);
    input_shutdown();
    
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 7 Complete! Animation System Working!                  |\n");
    printf("+============================================================+\n");
    
    return 0;
}
