# Android NDK Makefile for DeckFlow Engine
# Builds the native C library for Android

LOCAL_PATH := $(call my-dir)

# DeckFlow Engine source path
DECKFLOW_SRC := $(LOCAL_PATH)/../../../../src

# ========================
# DECKFLOW ENGINE LIBRARY
# ========================
include $(CLEAR_VARS)
LOCAL_MODULE := deckflow_engine

LOCAL_C_INCLUDES := \
    $(DECKFLOW_SRC) \
    $(DECKFLOW_SRC)/math \
    $(DECKFLOW_SRC)/nodes \
    $(DECKFLOW_SRC)/scenes \
    $(DECKFLOW_SRC)/physics \
    $(DECKFLOW_SRC)/input \
    $(DECKFLOW_SRC)/camera \
    $(DECKFLOW_SRC)/audio \
    $(DECKFLOW_SRC)/tilemap \
    $(DECKFLOW_SRC)/particle \
    $(DECKFLOW_SRC)/ui \
    $(DECKFLOW_SRC)/animation

# Core source files
LOCAL_SRC_FILES := \
    $(DECKFLOW_SRC)/math/vector2.c \
    $(DECKFLOW_SRC)/math/transform2d.c \
    $(DECKFLOW_SRC)/nodes/node.c \
    $(DECKFLOW_SRC)/scenes/scene.c \
    $(DECKFLOW_SRC)/physics/physics.c

# Additional modules (can be enabled)
# LOCAL_SRC_FILES += \
#     $(DECKFLOW_SRC)/input/input.c \
#     $(DECKFLOW_SRC)/camera/camera.c \
#     $(DECKFLOW_SRC)/tilemap/tilemap.c \
#     $(DECKFLOW_SRC)/particle/particle.c

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_LDLIBS := -lGLESv2 -lEGL -llog
LOCAL_CFLAGS := -Wall -Wextra -O2 -DNDEBUG

include $(BUILD_SHARED_LIBRARY)

# ========================
# MAIN NATIVE LIBRARY
# ========================
include $(CLEAR_VARS)
LOCAL_MODULE := deckflow_main

LOCAL_SRC_FILES := NativeMain.c

LOCAL_SHARED_LIBRARIES := deckflow_engine

LOCAL_LDLIBS := -lGLESv2 -lEGL -llog -landroid

LOCAL_CFLAGS := -Wall -Wextra -O2

include $(BUILD_SHARED_LIBRARY)
