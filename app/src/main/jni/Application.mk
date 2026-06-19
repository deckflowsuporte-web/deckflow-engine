# Application.mk for DeckFlow Engine
# Configure NDK build settings

# Target Android API level
APP_PLATFORM := android-21

# Enable NEON for better performance on ARM
APP_ARM_NEON := true

# Support multiple ABIs
# Note: For production, build separate APKs per ABI
APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

# Use c++_shared for STL
APP_STL := c++_shared

# Optimization flags
APP_CPPFLAGS := -fno-rtti -fno-exceptions
APP_CFLAGS := -Wall -Wextra -Wno-unused-parameter

# Link-time optimization
APP_LDFLAGS := -Wl,--gc-sections
