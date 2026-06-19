#include <jni.h>
#include <android/log.h>

#define LOG_TAG "DeckFlow"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" {
    JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
        LOGI("DeckFlow JNI loaded");
        return JNI_VERSION_1_6;
    }
    
    JNIEXPORT void JNICALL Java_com_deckflow_engine_DeckFlowLib_nativeInit(JNIEnv*, jobject) {
        LOGI("============================================");
        LOGI("   DECKFLOW ENGINE v1.0.0");
        LOGI("   Mobile-First 2D Game Engine");
        LOGI("============================================");
        LOGI("[Phase 1] COMPLETE: Nodes, Transform, Scene");
    }
    
    JNIEXPORT void JNICALL Java_com_deckflow_engine_DeckFlowLib_nativeDestroy(JNIEnv*, jobject) {
        LOGI("DeckFlow shutdown");
    }
}
