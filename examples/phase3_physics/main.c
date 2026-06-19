/**
 * @file main.c
 * @brief Phase 3: Box2D Physics Integration
 * 
 * Demonstrates:
 * - Box2D world setup with gravity
 * - Rigid body creation (static and dynamic)
 * - Collision detection
 * - Physics visualization
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <box2d.h>

// Screen dimensions (in pixels)
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Physics scale (pixels per meter)
#define PIXELS_PER_METER 50.0f

// Physics world bounds
#define WORLD_WIDTH (SCREEN_WIDTH / PIXELS_PER_METER)
#define WORLD_HEIGHT (SCREEN_HEIGHT / PIXELS_PER_METER)

// Colors
#define COLOR_BG (SDL_Color){20, 20, 35, 255}
#define COLOR_GROUND (SDL_Color){80, 80, 100, 255}
#define COLOR_BALL (SDL_Color){100, 200, 100, 255}
#define COLOR_BOX (SDL_Color){200, 100, 100, 255}
#define COLOR_WALL (SDL_Color){100, 100, 120, 255}

// Global SDL
static SDL_Window* g_window = NULL;
static SDL_Renderer* g_renderer = NULL;
static b2World* g_world = NULL;

// Convert physics to screen coordinates (Y is inverted)
static SDL_Point phys_to_screen(b2Vec2 pos) {
    return (SDL_Point){
        (int)(pos.x * PIXELS_PER_METER),
        SCREEN_HEIGHT - (int)(pos.y * PIXELS_PER_METER)
    };
}

// Initialize SDL
static bool init_sdl(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    g_window = SDL_CreateWindow(
        "DeckFlow Engine - Phase 3: Box2D Physics",
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
    
    g_renderer = SDL_CreateRenderer(g_window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!g_renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(g_window);
        SDL_Quit();
        return false;
    }
    
    printf("SDL initialized!\n");
    return true;
}

// Initialize Box2D world
static bool init_physics(void) {
    b2Vec2 gravity(0.0f, -10.0f);
    g_world = new b2World(gravity);
    
    if (!g_world) {
        printf("Failed to create Box2D world!\n");
        return false;
    }
    
    printf("Box2D world created (gravity: 0, -10 m/s2)\n");
    return true;
}

// Create static ground body
static b2Body* create_ground(float x, float y, float width, float height, SDL_Color color) {
    b2BodyDef groundDef;
    groundDef.position.Set(x, y);
    groundDef.type = b2_staticBody;
    
    b2Body* ground = g_world->CreateBody(&groundDef);
    
    b2PolygonShape groundShape;
    groundShape.SetAsBox(width / 2.0f, height / 2.0f);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &groundShape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.8f;
    fixtureDef.userData = malloc(sizeof(SDL_Color));
    *(SDL_Color*)fixtureDef.userData = color;
    
    ground->CreateFixture(&fixtureDef);
    
    return ground;
}

// Create dynamic ball body
static b2Body* create_ball(float x, float y, float radius, SDL_Color color) {
    b2BodyDef ballDef;
    ballDef.position.Set(x, y);
    ballDef.type = b2_dynamicBody;
    ballDef.linearDamping = 0.1f;
    ballDef.angularDamping = 0.3f;
    
    b2Body* ball = g_world->CreateBody(&ballDef);
    
    b2CircleShape circleShape;
    circleShape.m_radius = radius;
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.6f;
    
    SDL_Color* userData = (SDL_Color*)malloc(sizeof(SDL_Color));
    *userData = color;
    fixtureDef.userData = userData;
    
    ball->CreateFixture(&fixtureDef);
    
    return ball;
}

// Create dynamic box body
static b2Body* create_box(float x, float y, float width, float height, SDL_Color color) {
    b2BodyDef boxDef;
    boxDef.position.Set(x, y);
    boxDef.type = b2_dynamicBody;
    boxDef.linearDamping = 0.1f;
    boxDef.angularDamping = 0.5f;
    
    b2Body* box = g_world->CreateBody(&boxDef);
    
    b2PolygonShape boxShape;
    boxShape.SetAsBox(width / 2.0f, height / 2.0f);
    
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    fixtureDef.density = 2.0f;
    fixtureDef.friction = 0.5f;
    fixtureDef.restitution = 0.3f;
    
    SDL_Color* userData = (SDL_Color*)malloc(sizeof(SDL_Color));
    *userData = color;
    fixtureDef.userData = userData;
    
    box->CreateFixture(&fixtureDef);
    
    return box;
}

// Render a single body
static void render_body(b2Body* body) {
    SDL_Color* color = (SDL_Color*)body->GetFixtureList()->GetUserData();
    if (!color) return;
    
    b2Vec2 position = body->GetPosition();
    float angle = body->GetAngle();
    
    SDL_SetRenderDrawColor(g_renderer, color->r, color->g, color->b, color->a);
    
    for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        b2Shape* shape = fixture->GetShape();
        
        if (shape->GetType() == b2Shape::e_circle) {
            b2CircleShape* circle = (b2CircleShape*)shape;
            float radius = circle->m_radius;
            
            SDL_Point center = phys_to_screen(position);
            int r = (int)(radius * PIXELS_PER_METER);
            SDL_Rect rect = {center.x - r, center.y - r, r * 2, r * 2};
            SDL_RenderDrawRect(g_renderer, &rect);
            
            float dir_x = cosf(angle);
            float dir_y = sinf(angle);
            SDL_Point edge = {
                center.x + (int)(dir_x * radius * PIXELS_PER_METER),
                center.y - (int)(dir_y * radius * PIXELS_PER_METER)
            };
            SDL_RenderDrawLine(g_renderer, center.x, center.y, edge.x, edge.y);
            
        } else if (shape->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* polygon = (b2PolygonShape*)shape;
            int count = polygon->GetVertexCount();
            
            SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * (count + 1));
            for (int i = 0; i < count; i++) {
                b2Vec2 worldVert = b2Mul(b2Transform(position, b2Rot(angle)), polygon->GetVertex(i));
                points[i] = phys_to_screen(worldVert);
            }
            points[count] = points[0];
            
            SDL_RenderDrawLines(g_renderer, points, count + 1);
            free(points);
        }
    }
}

static void render_world(void) {
    for (b2Body* body = g_world->GetBodyList(); body; body = body->GetNext()) {
        render_body(body);
    }
}

static void cleanup(void) {
    for (b2Body* body = g_world->GetBodyList(); body; body = body->GetNext()) {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
            void* userData = fixture->GetUserData();
            if (userData) free(userData);
        }
    }
    
    delete g_world;
    
    if (g_renderer) SDL_DestroyRenderer(g_renderer);
    if (g_window) SDL_DestroyWindow(g_window);
    SDL_Quit();
    
    printf("Cleanup complete.\n");
}

int main(void) {
    printf("+============================================================+\n");
    printf("|  DeckFlow Engine - Phase 3: Box2D Physics                   |\n");
    printf("+============================================================+\n\n");
    
    if (!init_sdl()) return 1;
    if (!init_physics()) return 1;
    
    printf("\nCreating physics scene...\n");
    
    // Main ground
    b2Body* ground1 = create_ground(WORLD_WIDTH / 2, 0.5f, WORLD_WIDTH, 1.0f, COLOR_GROUND);
    (void)ground1;
    printf("  Ground platform created\n");
    
    // Elevated platforms
    create_ground(3.0f, 3.0f, 3.0f, 0.3f, COLOR_WALL);
    create_ground(13.0f, 5.0f, 4.0f, 0.3f, COLOR_WALL);
    printf("  Platforms created\n");
    
    // Walls
    create_ground(0.2f, WORLD_HEIGHT / 2, 0.3f, WORLD_HEIGHT, COLOR_WALL);
    create_ground(WORLD_WIDTH - 0.2f, WORLD_HEIGHT / 2, 0.3f, WORLD_HEIGHT, COLOR_WALL);
    printf("  Walls created\n");
    
    // Dynamic bodies
    b2Body* ball1 = create_ball(3.0f, 8.0f, 0.5f, COLOR_BALL);
    printf("  Ball 1 created (bouncy)\n");
    
    b2Body* ball2 = create_ball(4.5f, 7.0f, 0.4f, COLOR_BALL);
    printf("  Ball 2 created\n");
    
    b2Body* box1 = create_box(10.0f, 8.0f, 1.0f, 1.0f, COLOR_BOX);
    printf("  Box 1 created (heavy)\n");
    
    b2Body* box2 = create_box(12.0f, 7.0f, 0.8f, 0.8f, COLOR_BOX);
    printf("  Box 2 created\n");
    
    printf("\nScene ready!\n");
    printf("Controls:\n");
    printf("  SPACE: Drop new ball\n");
    printf("  R: Reset positions\n");
    printf("  ESC: Exit\n\n");
    
    b2Vec2 ball1_init(3.0f, 8.0f);
    b2Vec2 ball2_init(4.5f, 7.0f);
    b2Vec2 box1_init(10.0f, 8.0f);
    b2Vec2 box2_init(12.0f, 7.0f);
    
    bool running = true;
    SDL_Event event;
    int ball_count = 2;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
                if (event.key.keysym.sym == SDLK_SPACE) {
                    float x = 3.0f + (rand() % 10) * 1.0f;
                    create_ball(x, 10.0f, 0.3f + (rand() % 5) * 0.1f, COLOR_BALL);
                    ball_count++;
                    printf("Dropped ball #%d\n", ball_count);
                }
                if (event.key.keysym.sym == SDLK_r) {
                    ball1->SetTransform(ball1_init, 0);
                    ball1->SetLinearVelocity(b2Vec2(0, 0));
                    ball1->SetAngularVelocity(0);
                    
                    ball2->SetTransform(ball2_init, 0);
                    ball2->SetLinearVelocity(b2Vec2(0, 0));
                    ball2->SetAngularVelocity(0);
                    
                    box1->SetTransform(box1_init, 0);
                    box1->SetLinearVelocity(b2Vec2(0, 0));
                    box1->SetAngularVelocity(0);
                    
                    box2->SetTransform(box2_init, 0);
                    box2->SetLinearVelocity(b2Vec2(0, 0));
                    box2->SetAngularVelocity(0);
                    
                    printf("Positions reset!\n");
                }
            }
        }
        
        float time_step = 1.0f / 60.0f;
        int velocity_iterations = 8;
        int position_iterations = 3;
        g_world->Step(time_step, velocity_iterations, position_iterations);
        
        SDL_SetRenderDrawColor(g_renderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
        SDL_RenderClear(g_renderer);
        
        render_world();
        
        SDL_RenderPresent(g_renderer);
        
        static int frame_count = 0;
        if (frame_count++ % 360 == 0) {
            printf("\n--- Physics State ---\n");
            printf("Bodies in world: %d\n", ball_count + 5);
        }
    }
    
    printf("\nCleaning up...\n");
    cleanup();
    
    printf("\n+============================================================+\n");
    printf("|  Phase 3 Complete! Box2D Physics working!                  |\n");
    printf("+============================================================+\n");
    
    return 0;
}
