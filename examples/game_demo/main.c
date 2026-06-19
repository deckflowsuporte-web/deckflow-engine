/**
 * @file main.c
 * @brief DeckFlow Engine - Complete Platformer Demo
 * 
 * A complete platformer game demonstrating all engine systems:
 * - Scene/Node hierarchy
 * - Physics integration
 * - Sprite rendering
 * - Camera system
 * - UI overlay
 * - Particle effects
 * - Tilemap
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
#include "camera/camera.h"
#include "particle/particle.h"

// ============================================
// CONSTANTS
// ============================================
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WORLD_WIDTH 1600
#define WORLD_HEIGHT 1200
#define TILE_SIZE 32

#define GRAVITY -800
#define PLAYER_SPEED 250
#define PLAYER_JUMP 450
#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 40

// ============================================
// COLORS
// ============================================
#define C_BG        SDL_Color{30, 30, 60, 255}
#define C_SKY       SDL_Color{60, 100, 180, 255}
#define C_GROUND    SDL_Color{80, 60, 40, 255}
#define C_PLATFORM  SDL_Color{120, 80, 50, 255}
#define C_PLAYER    SDL_Color{80, 200, 120, 255}
#define C_COIN      SDL_Color{255, 215, 0, 255}
#define C_ENEMY     SDL_Color{200, 80, 80, 255}
#define C_LAVA      SDL_Color{255, 60, 0, 255}
#define C_UI_BG     SDL_Color{20, 20, 40, 200}
#define C_UI_TEXT   SDL_Color{255, 255, 255, 255}

// ============================================
// GAME STATE
// ============================================
typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_GAMEOVER,
    STATE_WIN
} GameState;

static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;
static GameState g_state = STATE_MENU;

// Systems
static PhysicsWorld* g_physics = NULL;
static Camera* g_camera = NULL;
static ParticleSystem* g_particles = NULL;

// Player
typedef struct {
    Node* node;
    PhysicsComponent* body;
    int coins;
    int lives;
    bool grounded;
    bool facing_right;
    float anim_time;
} Player;

static Player g_player;
static bool g_key_left, g_key_right, g_key_jump;

// World
static int g_map[50][40];  // 1600x1280 at 32px tiles
static Node* g_tiles[50][40];
static int g_total_coins = 0;
static int g_collected_coins = 0;

// ============================================
// TEXTURE HELPERS
// ============================================
static SDL_Texture* create_tex(int w, int h, SDL_Color c) {
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a));
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    SDL_FreeSurface(s);
    return t;
}

static SDL_Texture* create_tex_circle(int r, SDL_Color c) {
    SDL_Surface* s = SDL_CreateRGBSurfaceWithFormat(0, r*2, r*2, 32, SDL_PIXELFORMAT_RGBA32);
    SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, 0, 0, 0, 0));
    
    for (int y = 0; y < r*2; y++) {
        for (int x = 0; x < r*2; x++) {
            float dx = x - r, dy = y - r;
            if (dx*dx + dy*dy <= r*r) {
                SDL_Rect px = {x, y, 1, 1};
                SDL_FillRect(s, &px, SDL_MapRGBA(s->format, c.r, c.g, c.b, c.a));
            }
        }
    }
    
    SDL_Texture* t = SDL_CreateTextureFromSurface(g_renderer, s);
    SDL_FreeSurface(s);
    return t;
}

// ============================================
// LEVEL GENERATION
// ============================================
static void generate_level(void) {
    // Clear
    for (int y = 0; y < 40; y++) {
        for (int x = 0; x < 50; x++) {
            g_map[x][y] = 0;
            g_tiles[x][y] = NULL;
        }
    }
    
    // Ground
    for (int x = 0; x < 50; x++) {
        g_map[x][35] = 1;  // Ground
        g_map[x][36] = 1;
        g_map[x][37] = 1;
    }
    
    // Platforms - scattered
    int platforms[][4] = {
        {3, 30, 6, 1}, {12, 28, 5, 1}, {20, 25, 4, 1},
        {28, 22, 6, 1}, {38, 28, 5, 1}, {8, 20, 4, 1},
        {18, 18, 5, 1}, {30, 15, 6, 1}, {42, 20, 4, 1},
        {15, 12, 5, 1}, {25, 10, 4, 1}, {35, 12, 5, 1},
    };
    
    for (int i = 0; i < 12; i++) {
        int px = platforms[i][0];
        int py = platforms[i][1];
        int pw = platforms[i][2];
        for (int x = px; x < px + pw && x < 50; x++) {
            if (py < 40) g_map[x][py] = 2;  // Platform
        }
    }
    
    // Coins on platforms
    int coin_positions[][2] = {
        {5, 28}, {14, 26}, {22, 23}, {30, 20},
        {10, 18}, {20, 16}, {32, 13}, {16, 10},
        {27, 8}, {37, 10}, {8, 32}, {25, 32}
    };
    
    g_total_coins = 12;
    for (int i = 0; i < g_total_coins; i++) {
        int cx = coin_positions[i][0];
        int cy = coin_positions[i][1];
        g_map[cx][cy] = 3;  // Coin
    }
    
    // Lava pits
    g_map[17][36] = 4;
    g_map[18][36] = 4;
    g_map[19][36] = 4;
    g_map[33][36] = 4;
    g_map[34][36] = 4;
    
    printf("[Game] Level generated: %d coins\n", g_total_coins);
}

// ============================================
// GAME OBJECTS
// ============================================
static void create_tiles(Scene* scene, Node* root) {
    SDL_Texture* tex_ground = create_tex(TILE_SIZE, TILE_SIZE, C_GROUND);
    SDL_Texture* tex_platform = create_tex(TILE_SIZE, TILE_SIZE, C_PLATFORM);
    SDL_Texture* tex_coin = create_tex_circle(12, C_COIN);
    
    for (int y = 0; y < 40; y++) {
        for (int x = 0; x < 50; x++) {
            int tile = g_map[x][y];
            if (tile == 0) continue;
            
            Node* tile_node = node_create("Tile", NODE_TYPE_SPRITE);
            tile_node->transform.position = vector2_new(
                (float)x * TILE_SIZE + TILE_SIZE / 2,
                (float)y * TILE_SIZE + TILE_SIZE / 2
            );
            
            // Set texture
            void** ud = (void**)&tile_node->user_data;
            switch (tile) {
                case 1: *ud = tex_ground; break;
                case 2: *ud = tex_platform; break;
                case 3: *ud = tex_coin; break;
                case 4: *ud = tex_ground; break;
            }
            
            node_add_child(root, tile_node);
            g_tiles[x][y] = tile_node;
            
            // Create physics for solid tiles
            if (tile == 1 || tile == 2) {
                PhysicsComponent* body = physics_component_create(g_physics, tile_node, PHYSICS_BODY_STATIC);
                physics_set_box(body, TILE_SIZE, TILE_SIZE);
                physics_set_material(body, 0, 0.8f, 0.1f);
                physics_node_attach(tile_node, body);
            }
        }
    }
}

static void create_player(Scene* scene, Node* root) {
    g_player.node = node_create("Player", NODE_TYPE_SPRITE);
    g_player.node->transform.position = vector2_new(100, 300);
    g_player.texture = create_tex(PLAYER_WIDTH, PLAYER_HEIGHT, C_PLAYER);
    void** ud = (void**)&g_player.node->user_data;
    *ud = g_player.texture;
    
    node_add_child(root, g_player.node);
    
    // Physics body
    g_player.body = physics_component_create(g_physics, g_player.node, PHYSICS_BODY_DYNAMIC);
    physics_set_box(g_player.body, PLAYER_WIDTH, PLAYER_HEIGHT);
    physics_set_material(g_player.body, 1.0f, 0.1f, 0.0f);
    physics_node_attach(g_player.node, g_player.body);
    
    // Start position
    physics_set_position(g_player.body, vector2_new(100, 300));
    
    g_player.coins = 0;
    g_player.lives = 3;
    g_player.grounded = false;
    g_player.facing_right = true;
    g_player.anim_time = 0;
    
    printf("[Game] Player created\n");
}

// ============================================
// PARTICLE EFFECTS
// ============================================
static void spawn_coin_effect(float x, float y) {
    ParticleEmitter* emitter = particles_add_emitter(g_particles, "sparkle");
    emitter_configure_sparkle(emitter);
    emitter_set_position(emitter, vector2_new(x, y));
    emitter_start(emitter);
    
    particles_emit(g_particles, emitter, vector2_new(x, y), 10);
    emitter_stop(emitter);
}

static void spawn_death_effect(float x, float y) {
    ParticleEmitter* emitter = particles_add_emitter(g_particles, "explode");
    emitter_configure_explosion(emitter);
    emitter_set_position(emitter, vector2_new(x, y));
    emitter_start(emitter);
    
    particles_emit(g_particles, emitter, vector2_new(x, y), 30);
    emitter_stop(emitter);
}

// ============================================
// COLLISION & GAME LOGIC
// ============================================
static void check_collisions(void) {
    Vector2 pos = physics_get_position(g_player.body);
    
    // Coin collection
    int tile_x = (int)(pos.x / TILE_SIZE);
    int tile_y = (int)(pos.y / TILE_SIZE);
    
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int tx = tile_x + dx;
            int ty = tile_y + dy;
            
            if (tx >= 0 && tx < 50 && ty >= 0 && ty < 40) {
                if (g_map[tx][ty] == 3) {  // Coin
                    float cx = tx * TILE_SIZE + TILE_SIZE / 2;
                    float cy = ty * TILE_SIZE + TILE_SIZE / 2;
                    float dist = sqrtf((pos.x - cx) * (pos.x - cx) + (pos.y - cy) * (pos.y - cy));
                    
                    if (dist < 30) {
                        g_map[tx][ty] = 0;
                        if (g_tiles[tx][ty]) {
                            node_free(g_tiles[tx][ty]);
                            g_tiles[tx][ty] = NULL;
                        }
                        
                        g_player.coins++;
                        g_collected_coins++;
                        spawn_coin_effect(cx, cy);
                        
                        printf("[Game] Coin collected! %d/%d\n", g_player.coins, g_total_coins);
                        
                        if (g_player.coins >= g_total_coins) {
                            g_state = STATE_WIN;
                            printf("[Game] YOU WIN!\n");
                        }
                    }
                }
                
                // Lava death
                if (g_map[tx][ty] == 4) {
                    float lx = tx * TILE_SIZE + TILE_SIZE / 2;
                    float ly = ty * TILE_SIZE + TILE_SIZE / 2;
                    float dist = sqrtf((pos.x - lx) * (pos.x - lx) + (pos.y - ly) * (pos.y - ly));
                    
                    if (dist < 20) {
                        g_player.lives--;
                        spawn_death_effect(pos.x, pos.y);
                        
                        if (g_player.lives <= 0) {
                            g_state = STATE_GAMEOVER;
                            printf("[Game] GAME OVER!\n");
                        } else {
                            // Respawn
                            physics_set_position(g_player.body, vector2_new(100, 300));
                            physics_set_linear_velocity(g_player.body, vector2_zero());
                            printf("[Game] Respawning! Lives: %d\n", g_player.lives);
                        }
                    }
                }
            }
        }
    }
    
    // World bounds
    if (pos.y > WORLD_HEIGHT) {
        g_player.lives--;
        if (g_player.lives <= 0) {
            g_state = STATE_GAMEOVER;
        } else {
            physics_set_position(g_player.body, vector2_new(100, 300));
            physics_set_linear_velocity(g_player.body, vector2_zero());
        }
    }
}

// ============================================
// RENDERING
// ============================================
static void render_tile(Node* node) {
    if (!node || !node->visible) return;
    
    void** ud = (void**)node->user_data;
    SDL_Texture* tex = (SDL_Texture*)*ud;
    if (!tex) return;
    
    Vector2 pos = node_get_global_position(node);
    
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    
    SDL_Rect dst = {(int)(pos.x - w/2), (int)(pos.y - h/2), w, h};
    SDL_RenderCopy(g_renderer, tex, NULL, &dst);
}

static void render_all_tiles(Node* root) {
    if (!root) return;
    render_tile(root);
    
    for (Node* c = root->first_child; c; c = c->next_sibling) {
        render_all_tiles(c);
    }
}

static void render_player(void) {
    if (!g_player.node || !g_player.texture) return;
    
    Vector2 pos = node_get_global_position(g_player.node);
    float rot = node_get_global_rotation(g_player.node);
    
    SDL_RendererFlip flip = g_player.facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    
    SDL_Rect dst = {
        (int)(pos.x - PLAYER_WIDTH / 2),
        (int)(pos.y - PLAYER_HEIGHT / 2),
        PLAYER_WIDTH, PLAYER_HEIGHT
    };
    
    SDL_RenderCopyEx(g_renderer, g_player.texture, NULL, &dst, 
                    rot * 180.0f / M_PI, NULL, flip);
}

static void render_particles(void) {
    ParticleEmitter* emitter = g_particles->emitters;
    while (emitter) {
        for (int i = 0; i < emitter->max_particles; i++) {
            Particle* p = &emitter->particles[i];
            if (p->life > 0) {
                Vector2 screen = camera_world_to_screen(g_camera, p->position);
                
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
}

static void render_ui(void) {
    // Background bar
    SDL_SetRenderDrawColor(g_renderer, C_UI_BG.r, C_UI_BG.g, C_UI_BG.b, C_UI_BG.a);
    SDL_RenderFillRect(g_renderer, &(SDL_Rect){0, 0, SCREEN_WIDTH, 50});
    
    // Coins
    char buf[64];
    snprintf(buf, sizeof(buf), "COINS: %d/%d", g_player.coins, g_total_coins);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surf = TTF_RenderText_Blended(NULL, buf, white);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        SDL_Rect dst = {20, 15, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
    
    // Lives
    snprintf(buf, sizeof(buf), "LIVES: %d", g_player.lives);
    surf = TTF_RenderText_Blended(NULL, buf, white);
    if (surf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        SDL_Rect dst = {200, 15, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
}

static void render_menu(void) {
    SDL_SetRenderDrawColor(g_renderer, C_BG.r, C_BG.g, C_BG.b, C_BG.a);
    SDL_RenderClear(g_renderer);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 150, 255};
    
    SDL_Surface* surf;
    SDL_Texture* tex;
    SDL_Rect dst;
    
    // Title
    surf = TTF_RenderText_Blended(NULL, "DECKFLOW PLATFORMER", green);
    if (surf) {
        tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        dst = {250, 150, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
    
    // Instructions
    surf = TTF_RenderText_Blended(NULL, "Collect all coins!", white);
    if (surf) {
        tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        dst = {300, 250, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
    
    surf = TTF_RenderText_Blended(NULL, "Arrow keys / WASD to move", white);
    if (surf) {
        tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        dst = {280, 290, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
    
    surf = TTF_RenderText_Blended(NULL, "SPACE to jump", white);
    if (surf) {
        tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        dst = {330, 320, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
    
    // Start prompt
    surf = TTF_RenderText_Blended(NULL, "Press ENTER to start", green);
    if (surf) {
        tex = SDL_CreateTextureFromSurface(g_renderer, surf);
        dst = {290, 420, surf->w, surf->h};
        SDL_RenderCopy(g_renderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
        SDL_FreeSurface(surf);
    }
}

// ============================================
// GAME LOOP
// ============================================
static void update_player(float dt) {
    // Input forces
    Vector2 vel = physics_get_linear_velocity(g_player.body);
    Vector2 force = vector2_zero();
    
    if (g_key_left) {
        force.x = -500;
        g_player.facing_right = false;
    }
    if (g_key_right) {
        force.x = 500;
        g_player.facing_right = true;
    }
    
    // Ground check (simple)
    Vector2 pos = physics_get_position(g_player.body);
    int tile_y = (int)(pos.y / TILE_SIZE) + 1;
    int tile_x = (int)(pos.x / TILE_SIZE);
    
    g_player.grounded = false;
    if (tile_y < 40 && tile_x >= 0 && tile_x < 50) {
        if (g_map[tile_x][tile_y] == 1 || g_map[tile_x][tile_y] == 2) {
            g_player.grounded = true;
        }
    }
    
    // Jump
    if (g_key_jump && g_player.grounded) {
        vel.y = PLAYER_JUMP;
        physics_set_linear_velocity(g_player.body, vel);
        g_key_jump = false;
    }
    
    // Apply gravity
    force.y = GRAVITY;
    
    // Apply forces
    physics_apply_force(g_player.body, vector2_scale(force, dt));
    
    // Clamp horizontal speed
    vel = physics_get_linear_velocity(g_player.body);
    vel.x = vel.x > PLAYER_SPEED ? PLAYER_SPEED : vel.x;
    vel.x = vel.x < -PLAYER_SPEED ? -PLAYER_SPEED : vel.x;
    physics_set_linear_velocity(g_player.body, vel);
    
    // Sync to node
    physics_sync_to_node(g_player.body, g_player.node);
    
    // Camera follow
    camera_follow(g_camera, g_player.node);
}

// ============================================
// MAIN
// ============================================
int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    
    printf("+============================================================+\n");
    printf("|  DeckFlow Engine - Platformer Demo                            |\n");
    printf("+============================================================+\n\n");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    
    if (TTF_Init() < 0) {
        printf("TTF init failed: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    }
    
    g_window = SDL_CreateWindow("DeckFlow Platformer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!g_window) { TTF_Quit(); SDL_Quit(); return 1; }
    
    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
    if (!g_renderer) { SDL_DestroyWindow(g_window); TTF_Quit(); SDL_Quit(); return 1; }
    
    // Init systems
    printf("[Init] Starting systems...\n");
    input_init();
    
    g_physics = physics_world_create(vector2_new(0, GRAVITY));
    physics_world_set_bounds(g_physics, 0, 0, WORLD_WIDTH, WORLD_HEIGHT);
    
    g_camera = camera_create("MainCamera");
    camera_set_viewport(g_camera, SCREEN_WIDTH, SCREEN_HEIGHT);
    camera_set_mode(g_camera, CAMERA_MODE_FOLLOW);
    camera_set_smoothing(g_camera, 0.1f);
    
    g_particles = particles_create(500);
    
    // Create scene
    Scene* scene = scene_create("GameScene");
    Node* root = scene_get_root(scene);
    
    // Generate level
    generate_level();
    create_tiles(scene, root);
    create_player(scene, root);
    
    printf("\n");
    printf("Controls:\n");
    printf("  Arrow keys / WASD: Move\n");
    printf("  SPACE: Jump\n");
    printf("  ESC: Exit\n\n");
    
    bool running = true;
    SDL_Event event;
    float dt = 1.0f / 60.0f;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            
            if (event.type == SDL_KEYDOWN) {
                input_handle_key_down(event.key.keysym.scancode);
                
                if (g_state == STATE_MENU && event.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    g_state = STATE_PLAYING;
                    printf("[Game] Starting!\n");
                }
                
                if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) running = false;
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) g_key_jump = true;
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT || 
                    event.key.keysym.scancode == SDL_SCANCODE_A) g_key_left = true;
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT ||
                    event.key.keysym.scancode == SDL_SCANCODE_D) g_key_right = true;
            }
            
            if (event.type == SDL_KEYUP) {
                input_handle_key_up(event.key.keysym.scancode);
                if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) g_key_jump = false;
                if (event.key.keysym.scancode == SDL_SCANCODE_LEFT ||
                    event.key.keysym.scancode == SDL_SCANCODE_A) g_key_left = false;
                if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT ||
                    event.key.keysym.scancode == SDL_SCANCODE_D) g_key_right = false;
            }
        }
        
        // Update
        if (g_state == STATE_PLAYING) {
            physics_world_update(g_physics, dt);
            update_player(dt);
            camera_update(g_camera, dt);
            check_collisions();
            particles_update(g_particles, dt);
        }
        
        // Render
        if (g_state == STATE_MENU) {
            render_menu();
        } else {
            // Background
            SDL_SetRenderDrawColor(g_renderer, C_SKY.r, C_SKY.g, C_SKY.b, C_SKY.a);
            SDL_RenderClear(g_renderer);
            
            // Tiles (with camera)
            render_all_tiles(root);
            
            // Particles
            render_particles();
            
            // Player
            render_player();
            
            // UI
            render_ui();
        }
        
        SDL_RenderPresent(g_renderer);
        input_update();
        SDL_Delay(16);
    }
    
    // Cleanup
    printf("\nCleaning up...\n");
    particles_free(g_particles);
    camera_free(g_camera);
    physics_world_free(g_physics);
    scene_free(scene);
    input_shutdown();
    
    TTF_Quit();
    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("\n+============================================================+\n");
    printf("|  Thanks for playing!                                          |\n");
    printf("+============================================================+\n");
    
    return 0;
}
