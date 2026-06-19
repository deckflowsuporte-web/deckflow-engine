LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := deckflow
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../src
LOCAL_SRC_FILES := ../../src/core/engine.c \
                   ../../src/math/vector2.c \
                   ../../src/math/transform2d.c \
                   ../../src/nodes/node.c \
                   ../../src/scenes/scene.c \
                   ../../src/scenes/scene_tree.c \
                   android_wrapper.c
LOCAL_LDLIBS := -llog -lSDL2 -lSDL2main
LOCAL_CFLAGS := -Wall -Wextra -std=c11
include $(BUILD_SHARED_LIBRARY)
