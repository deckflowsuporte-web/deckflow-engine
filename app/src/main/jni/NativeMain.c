/**
 * @file NativeMain.c
 * @brief Android Native Main - Bridge between Java and C Engine
 * 
 * This file provides the JNI interface for the DeckFlow Engine
 * running on Android via the NDK.
 */

#include <jni.h>
#include <android/log.h>
#include <android/input.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_TAG "DeckFlow"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/*
 * DeckFlow Engine includes - only core modules for Android
 */
#include "math/vector2.h"
#include "math/transform2d.h"
#include "nodes/node.h"
#include "scenes/scene.h"
#include "physics/physics.h"

// ============================================
// ANDROID NATIVE STATE
// ============================================
typedef struct {
    ANativeWindow* window;
    EGLDisplay display;
    EGLContext context;
    EGLSurface surface;
    
    int width;
    int height;
    bool initialized;
    bool running;
    
    // Game state
    Scene* scene;
    PhysicsWorld* physics;
    
    // Touch input
    float touch_x;
    float touch_y;
    bool touch_down;
    bool touch_pressed;
    
    // Time
    long last_time;
    float delta_time;
} AndroidState;

static AndroidState g_state = {0};

// ============================================
// EGL INIT
// ============================================
static bool init_egl(AndroidState* state) {
    // Get EGL display
    state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (state->display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return false;
    }
    
    // Initialize EGL
    EGLint major, minor;
    if (!eglInitialize(state->display, &major, &minor)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    LOGI("EGL Version: %d.%d", major, minor);
    
    // Configure
    EGLConfig config;
    EGLint num_configs;
    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_ALPHA_SIZE, 0,
        EGL_DEPTH_SIZE, 0,
        EGL_NONE
    };
    
    if (!eglChooseConfig(state->display, config_attribs, &config, 1, &num_configs)) {
        LOGE("Failed to choose EGL config");
        return false;
    }
    
    // Create EGL context
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attribs);
    if (state->context == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        return false;
    }
    
    // Create surface
    state->surface = eglCreateWindowSurface(state->display, config, state->window, NULL);
    if (state->surface == EGL_NO_SURFACE) {
        LOGE("Failed to create EGL surface");
        return false;
    }
    
    // Make current
    if (!eglMakeCurrent(state->display, state->surface, state->surface, state->context)) {
        LOGE("Failed to make EGL current");
        return false;
    }
    
    LOGI("EGL initialized successfully");
    return true;
}

static void shutdown_egl(AndroidState* state) {
    if (state->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroySurface(state->display, state->surface);
        eglDestroyContext(state->display, state->context);
        eglTerminate(state->display);
        state->display = EGL_NO_DISPLAY;
    }
}

// ============================================
// GAME LOGIC
// ============================================
static void init_game(AndroidState* state) {
    LOGI("Initializing DeckFlow Engine on Android...");
    
    // Create physics world
    state->physics = physics_world_create(vector2_new(0, -500));
    LOGI("Physics world created");
    
    // Create scene
    state->scene = scene_create("AndroidScene");
    LOGI("Scene created");
    
    // Get window size
    if (state->window) {
        state->width = ANativeWindow_getWidth(state->window);
        state->height = ANativeWindow_getHeight(state->window);
    } else {
        state->width = 800;
        state->height = 600;
    }
    
    LOGI("Screen size: %dx%d", state->width, state->height);
    
    // Create a simple test scene
    Node* root = scene_get_root(state->scene);
    
    // Create ground
    Node* ground = node_create("Ground", NODE_TYPE_SPATIAL_2D);
    ground->transform.position = vector2_new(state->width / 2, state->height - 50);
    ground->transform.scale = vector2_new(state->width, 100);
    node_add_child(root, ground);
    
    PhysicsComponent* ground_body = physics_component_create(state->physics, ground, PHYSICS_BODY_STATIC);
    physics_set_box(ground_body, state->width, 100);
    physics_node_attach(ground, ground_body);
    
    // Create player
    Node* player = node_create("Player", NODE_TYPE_SPATIAL_2D);
    player->transform.position = vector2_new(state->width / 2, state->height / 2);
    player->transform.scale = vector2_new(50, 50);
    node_add_child(root, player);
    
    PhysicsComponent* player_body = physics_component_create(state->physics, player, PHYSICS_BODY_DYNAMIC);
    physics_set_box(player_body, 50, 50);
    physics_set_material(player_body, 1.0f, 0.1f, 0.3f);
    physics_node_attach(player, player_body);
    
    LOGI("Game initialized");
}

static void update_game(AndroidState* state, float dt) {
    if (!state->physics) return;
    
    // Simple touch controls
    // In a real game, this would be more sophisticated
    Vector2 force = vector2_zero();
    float speed = 300.0f;
    
    if (state->touch_down) {
        // Move towards touch
        Vector2 player_pos = vector2_new(state->width / 2, state->height / 2); // Would get from player
        
        if (state->touch_x > state->width / 2 + 50) {
            force.x = speed;
        } else if (state->touch_x < state->width / 2 - 50) {
            force.x = -speed;
        }
        
        // Jump if touch is in lower third
        if (state->touch_y > state->height * 2 / 3) {
            force.y = 400; // Jump
        }
    }
    
    // Update physics
    physics_world_update(state->physics, dt);
}

static void render_game(AndroidState* state) {
    // Clear screen
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Simple rectangle rendering for demo
    // In production, this would use proper OpenGL shaders and sprite batching
    
    glEnableClientState(GL_VERTEX_ARRAY);
    
    // Draw background color
    // (OpenGL ES 2.0 requires shaders, simplified for demo)
    
    glDisableClientState(GL_VERTEX_ARRAY);
}

static void game_loop(AndroidState* state) {
    if (!state->initialized || !state->running) return;
    
    // Calculate delta time
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    
    long now_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    if (state->last_time > 0) {
        state->delta_time = (now_ms - state->last_time) / 1000.0f;
        if (state->delta_time > 0.1f) state->delta_time = 0.1f; // Cap
    }
    state->last_time = now_ms;
    
    // Update
    update_game(state, state->delta_time);
    
    // Render
    if (!eglMakeCurrent(state->display, state->surface, state->surface, state->context)) {
        LOGE("Failed to make EGL current in loop");
        return;
    }
    
    render_game(state);
    
    // Swap buffers
    eglSwapBuffers(state->display, state->surface);
}

// ============================================
// JNI INTERFACE
// ============================================

JNIEXPORT jint JNICALL
Java_com_deckflow_engine_NativeLib_nativeInit(JNIEnv* env, jobject thiz, jobject surface) {
    (void)env; (void)thiz;
    
    LOGI("Native init called");
    
    // Get native window
    g_state.window = ANativeWindow_fromSurface(env, surface);
    if (!g_state.window) {
        LOGE("Failed to get native window");
        return -1;
    }
    
    // Initialize EGL
    if (!init_egl(&g_state)) {
        ANativeWindow_release(g_state.window);
        g_state.window = NULL;
        return -1;
    }
    
    // Initialize game
    init_game(&g_state);
    
    g_state.initialized = true;
    g_state.running = true;
    
    LOGI("Native init complete");
    return 0;
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_NativeLib_nativeResize(JNIEnv* env, jobject thiz, jint width, jint height) {
    (void)env; (void)thiz;
    
    LOGI("Native resize: %dx%d", width, height);
    g_state.width = width;
    g_state.height = height;
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_NativeLib_nativeStep(JNIEnv* env, jobject thiz) {
    (void)env; (void)thiz;
    
    game_loop(&g_state);
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_NativeLib_nativeTouch(JNIEnv* env, jobject thiz, 
                                               jint action, jfloat x, jfloat y) {
    (void)env; (void)thiz;
    
    g_state.touch_x = x;
    g_state.touch_y = y;
    
    if (action == 0) { // ACTION_DOWN
        g_state.touch_down = true;
        g_state.touch_pressed = true;
    } else if (action == 1) { // ACTION_UP
        g_state.touch_down = false;
    } else if (action == 2) { // ACTION_MOVE
        // Already updated x,y
    }
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_NativeLib_nativeShutdown(JNIEnv* env, jobject thiz) {
    (void)env; (void)thiz;
    
    LOGI("Native shutdown");
    
    g_state.running = false;
    
    // Cleanup game
    if (g_state.scene) {
        scene_free(g_state.scene);
        g_state.scene = NULL;
    }
    
    if (g_state.physics) {
        physics_world_free(g_state.physics);
        g_state.physics = NULL;
    }
    
    // Shutdown EGL
    shutdown_egl(&g_state);
    
    // Release window
    if (g_state.window) {
        ANativeWindow_release(g_state.window);
        g_state.window = NULL;
    }
    
    g_state.initialized = false;
    
    LOGI("Shutdown complete");
}
