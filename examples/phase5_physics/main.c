/**
 * @file main.c
 * @brief Phase 5: Physics + Scene Integration
 * 
 * Demonstrates:
 * - Physics components attached to Nodes
 * - Rigid body as Node component
 * - Automatic sync between physics and transform
 * - Collision detection
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
#include "physics/physics.h"
#include "input/input.h"

// Screen
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Colors
#define COLOR_BG (SDL_Color){26, 26, 46, 255}
#define COLOR_GROUND (SDL_Color){60, 60, 80, 255}
#define COLOR_PLAYER (SDL_Color){100, 200, 100, 255}
#define COLOR_BALL (SDL_Color){100, 150, 255, 255}
#define COLOR_BOX (SDL_Color){255, 100, 100, 255}

typedef struct {
    SDL_Texture* texture;
    int width, height;
    SDL_Color color;
} SpriteData;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;

// Create sprite texture
static SDL_Texture* create_texture(int w, int h, SDL_Color c) {
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a));
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    SDL_FreeSurface(s);
    return t;
}

// Create sprite node
static Node* create_sprite(const char* name, int w, int h, SDL_Color c) {
    Node* node = node_create(name, NODE_TYPE_SPRITE);
    SpriteData* sp = (SpriteData*)malloc(sizeof(SpriteData));
    sp->texture = create_texture(w, h, c);
    sp->width = w;
    sp->height = h;
    sp->color = c;
    node->user_data = sp;
    node->user_data_destructor = free;
    return node;
}

// Render node with physics sync
static void render_node(Node* node, PhysicsWorld* world) {
    if (!node || !node->visible) return;
    
    // Sync physics to node transform
    if (node->physics_component) {
        physics_sync_to_node(node->physics_component, node);
    }
    
    SpriteData* sp = node->user_data;
    if (sp && sp->texture) {
        Vector2 pos = node_get_global_position(node);
        float rot = node_get_global_rotation(node);
        
        SDL_Rect dest = {(int)pos.x - sp->width/2, (int)pos.y - sp->height/2, sp->width, sp->height};
        
        if (fabsf(rot) > 0.001f) {
            SDL_RenderCopyEx(g_renderer, sp->texture, NULL, &dest, rot * 180.0f / M_PI, NULL, SDL_FLIP_NONE);
        } else {
            SDL_RenderCopy(g_renderer, sp->texture, NULL, &dest);
        }
    }
    
    for (Node* c = node->first_child; c; c = c->next_sibling) {
        render_node(c, world);
    }
}

int main(void) {
    printf("+============================================================+\n");
    printf("|  Phase 5: Physics + Scene Integration                        |\n");
    printf("+============================================================+\n\n");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    g_window = SDL_CreateWindow("Phase 5: Physics Integration",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!g_window) { SDL_Quit(); return 1; }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_renderer) { SDL_DestroyWindow(g_window); SDL_Quit(); return 1; }
    
    // Init
    input_init();
    
    // Create physics world
    printf("[1] Creating physics world...\n");
    PhysicsWorld* world = physics_world_create(vector2_new(0, -500));  // Gravity down
    physics_world_set_bounds(world, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Create scene
    printf("[2] Creating scene...\n");
    Scene* scene = scene_create("PhysicsScene");
    Node* root = scene_get_root(scene);
    
    // Create ground (STATIC body)
    printf("[3] Creating ground...\n");
    Node* ground = create_sprite("Ground", SCREEN_WIDTH, 40, COLOR_GROUND);
    ground->transform.position = vector2_new(SCREEN_WIDTH/2, SCREEN_HEIGHT - 20);
    node_add_child(root, ground);
    
    PhysicsComponent* ground_ph = physics_component_create(world, ground, PHYSICS_BODY_STATIC);
    physics_set_box(ground_ph, SCREEN_WIDTH, 40);
    physics_set_material(ground_ph, 0, 0.8f, 0.1f);
    physics_node_attach(ground, ground_ph);
    
    // Create walls
    Node* wall_left = create_sprite("WallL", 20, SCREEN_HEIGHT, COLOR_GROUND);
    wall_left->transform.position = vector2_new(10, SCREEN_HEIGHT/2);
    node_add_child(root, wall_left);
    
    PhysicsComponent* wall_l_ph = physics_component_create(world, wall_left, PHYSICS_BODY_STATIC);
    physics_set_box(wall_l_ph, 20, SCREEN_HEIGHT);
    physics_node_attach(wall_left, wall_l_ph);
    
    Node* wall_right = create_sprite("WallR", 20, SCREEN_HEIGHT, COLOR_GROUND);
    wall_right->transform.position = vector2_new(SCREEN_WIDTH - 10, SCREEN_HEIGHT/2);
    node_add_child(root, wall_right);
    
    PhysicsComponent* wall_r_ph = physics_component_create(world, wall_right, PHYSICS_BODY_STATIC);
    physics_set_box(wall_r_ph, 20, SCREEN_HEIGHT);
    physics_node_attach(wall_right, wall_r_ph);
    
    // Create platforms
    printf("[4] Creating platforms...\n");
    for (int i = 0; i < 3; i++) {
        Node* plat = create_sprite("Platform", 150, 20, COLOR_GROUND);
        float x = 150 + i * 220;
        float y = 200 + i * 120;
        plat->transform.position = vector2_new(x, y);
        node_add_child(root, plat);
        
        PhysicsComponent* plat_ph = physics_component_create(world, plat, PHYSICS_BODY_STATIC);
        physics_set_box(plat_ph, 150, 20);
        physics_node_attach(plat, plat_ph);
    }
    
    // Create player (DYNAMIC body)
    printf("[5] Creating player...\n");
    Node* player = create_sprite("Player", 50, 50, COLOR_PLAYER);
    player->transform.position = vector2_new(100, 400);
    node_add_child(root, player);
    
    PhysicsComponent* player_ph = physics_component_create(world, player, PHYSICS_BODY_DYNAMIC);
    physics_set_box(player_ph, 50, 50);
    physics_set_material(player_ph, 1.0f, 0.3f, 0.2f);
    physics_node_attach(player, player_ph);
    
    // Create dynamic objects
    printf("[6] Creating dynamic objects...\n");
    Node* balls[5];
    PhysicsComponent* ball_ph[5];
    
    for (int i = 0; i < 5; i++) {
        balls[i] = create_sprite("Ball", 30, 30, COLOR_BALL);
        float x = 300 + (i % 3) * 60;
        float y = 100 + i * 40;
        balls[i]->transform.position = vector2_new(x, y);
        node_add_child(root, balls[i]);
        
        ball_ph[i] = physics_component_create(world, balls[i], PHYSICS_BODY_DYNAMIC);
        physics_set_circle(ball_ph[i], 15);
        physics_set_material(ball_ph[i], 0.8f, 0.2f, 0.8f);
        physics_node_attach(balls[i], ball_ph[i]);
    }
    
    printf("\nPhysics + Scene Integration Demo:\n");
    printf("  - %d bodies in physics world\n", world->component_count);
    printf("  - Physics components attached to Nodes\n");
    printf("  - Auto-sync between physics and transforms\n\n");
    
    printf("Controls:\n");
    printf("  WASD/Arrows: Apply force to player\n");
    printf("  SPACE: Jump (apply upward force)\n");
    printf("  R: Reset player position\n");
    printf("  B: Drop new ball\n");
    printf("  ESC: Exit\n\n");
    
    bool running = true;
    SDL_Event event;
    int ball_count = 5;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                input_handle_key_down(event.key.keysym.scancode);
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
            }
            if (event.type == SDL_KEYUP) {
                input_handle_key_up(event.key.keysym.scancode);
            }
        }
        
        // Player input - apply forces
        if (input_key_pressed(KEY_LEFT) || input_key_pressed(KEY_A)) {
            physics_apply_force(player_ph, vector2_new(-500, 0));
        }
        if (input_key_pressed(KEY_RIGHT) || input_key_pressed(KEY_D)) {
            physics_apply_force(player_ph, vector2_new(500, 0));
        }
        if (input_key_pressed(KEY_UP) || input_key_pressed(KEY_W)) {
            physics_apply_force(player_ph, vector2_new(0, 300));
        }
        if (input_key_pressed(KEY_DOWN) || input_key_pressed(KEY_S)) {
            physics_apply_force(player_ph, vector2_new(0, -200));
        }
        
        // Jump
        if (input_key_just_pressed(KEY_SPACE)) {
            physics_apply_impulse(player_ph, vector2_new(0, 400));
        }
        
        // Reset
        if (input_key_just_pressed(KEY_R)) {
            physics_set_position(player_ph, vector2_new(100, 400));
            physics_set_linear_velocity(player_ph, vector2_zero());
            physics_set_angular_velocity(player_ph, 0);
        }
        
        // Drop new ball
        if (input_key_just_pressed(KEY_B)) {
            Node* new_ball = create_sprite("Ball", 30, 30, COLOR_BALL);
            float x = 100 + (rand() % 600);
            new_ball->transform.position = vector2_new(x, 50);
            node_add_child(root, new_ball);
            
            PhysicsComponent* new_ph = physics_component_create(world, new_ball, PHYSICS_BODY_DYNAMIC);
            physics_set_circle(new_ph, 15);
            physics_set_material(new_ph, 0.8f, 0.2f, 0.8f);
            physics_node_attach(new_ball, new_ph);
            
            ball_count++;
            printf("Dropped ball #%d\n", ball_count);
        }
        
        // Physics step
        physics_world_update(world, 1.0f / 60.0f);
        
        // Render
        SDL_SetRenderDrawColor(g_renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
        SDL_RenderClear(g_renderer);
        
        render_node(root, world);
        
        SDL_RenderPresent(g_renderer);
        input_update();
        
        SDL_Delay(16);
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    physics_world_free(world);
    scene_free(scene);
    input_shutdown();
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 5 Complete! Physics + Scene Integration!              |\n");
    printf("+============================================================+\n");
    
    return 0;
}
