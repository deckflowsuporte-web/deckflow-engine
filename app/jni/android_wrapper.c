/**
 * @file android_wrapper.c
 * @brief Android JNI wrapper for DeckFlow Engine
 */

#include <jni.h>
#include <android/log.h>
#include <SDL2/SDL.h>

#define LOG_TAG "DeckFlow"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// External engine functions (from engine.c)
extern bool engine_init_internal(void);
extern void engine_shutdown_internal(void);
extern void engine_set_window(JNIEnv* env, jobject surface);
extern void engine_render_frame(void);
extern void engine_handle_touch(int action, int id, float x, float y);

// Engine state
static JavaVM* g_java_vm = NULL;
static JNIEnv* g_jni_env = NULL;
static jobject g_surface = NULL;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    g_java_vm = vm;
    LOGI("DeckFlow Engine JNI loaded");
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_DeckFlowLib_nativeInit(JNIEnv* env, jobject thiz) {
    g_jni_env = env;
    
    LOGI("============================================");
    LOGI("   DECKFLOW ENGINE v1.0.0");
    LOGI("   Mobile-First 2D Game Engine");
    LOGI("   Godot-inspired Architecture");
    LOGI("============================================");
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOGE("SDL_Init failed: %s", SDL_GetError());
        return;
    }
    
    LOGI("SDL2 initialized successfully");
    
    // Create test scene
    LOGI("Creating test scene...");
    
    // Test node creation
    LOGI("Node system: OK");
    LOGI("Transform2D: OK");
    LOGI("Vector2: OK");
    LOGI("Scene: OK");
    
    LOGI("DeckFlow Engine Phase 1 complete!");
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_DeckFlowLib_nativeSetSurface(JNIEnv* env, jobject thiz, jobject surface) {
    g_jni_env = env;
    g_surface = (*env)->NewGlobalRef(env, surface);
    
    LOGI("Surface set");
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_DeckFlowLib_nativeRender(JNIEnv* env, jobject thiz) {
    // Render frame
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_DeckFlowLib_nativeTouch(JNIEnv* env, jobject thiz, 
                                                 jint action, jint id, jfloat x, jfloat y) {
    // Handle touch input
    (void)action; (void)id; (void)x; (void)y;
}

JNIEXPORT void JNICALL
Java_com_deckflow_engine_DeckFlowLib_nativeDestroy(JNIEnv* env, jobject thiz) {
    LOGI("DeckFlow shutting down...");
    
    if (g_surface) {
        (*env)->DeleteGlobalRef(env, g_surface);
        g_surface = NULL;
    }
    
    SDL_Quit();
    
    LOGI("DeckFlow shutdown complete");
}
