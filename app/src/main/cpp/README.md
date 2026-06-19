# Android NDK Integration - Phase 9

## 📋 Visão Geral

Esta fase adiciona suporte para build nativo no Android usando o **NDK (Native Development Kit)**.

## 🏗️ Estrutura

```
app/src/main/
├── cpp/
│   ├── CMakeLists.txt     # CMake build (moderno)
│   ├── NativeMain.c       # Main nativo
│   └── (generated .so)   # Bibliotecas compiladas
├── jni/
│   ├── Android.mk        # ndk-build makefile
│   └── Application.mk    # Configurações NDK
└── java/
    └── com/deckflow/engine/
        ├── NativeLib.java     # Bridge Java-C
        └── NativeMain.c      # C entry point
```

## 🔧 Build Systems Suportados

### 1. ndk-build (tradicional)
```bash
cd app/src/main
ndk-build
```

### 2. CMake (moderno - recomendado)
```bash
cd app/src/main/cpp
mkdir build && cd build
cmake ..
make
```

## 📦 Bibliotecas Geradas

| Biblioteca | Descrição |
|-----------|-----------|
| `libdeckflow_engine.a` | Engine core (estática) |
| `libdeckflow_main.so` | Main nativo (compartilhada) |

## 🔌 Interface JNI

### Java → Native
```java
public class NativeLib extends Activity {
    public static native int nativeInit(Surface surface);
    public static native void nativeResize(int width, int height);
    public static native void nativeStep();
    public static native void nativeTouch(int action, float x, float y);
    public static native void nativeShutdown();
}
```

### Native → Java
```c
JNIEXPORT jint JNICALL
Java_com_deckflow_engine_NativeLib_nativeInit(JNIEnv* env, jobject thiz, jobject surface) {
    // Inicializar EGL e engine
    return 0; // Sucesso
}
```

## 📱 Renderização Android

```
┌─────────────────────────────────────────────────┐
│              Android Activity                      │
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌─────────────────────────────────────────┐   │
│  │            SurfaceView                    │   │
│  │  ┌─────────────────────────────────┐   │   │
│  │  │          EGL Context            │   │   │
│  │  │  ┌───────────────────────────┐   │   │   │
│  │  │  │      OpenGL ES 2.0        │   │   │   │
│  │  │  │                           │   │   │   │
│  │  │  │    DeckFlow Engine       │   │   │   │
│  │  │  │    (C Native Code)       │   │   │   │
│  │  │  │                           │   │   │   │
│  │  │  └───────────────────────────┘   │   │   │
│  │  └─────────────────────────────────┘   │   │
│  └─────────────────────────────────────────┘   │
│                                                 │
└─────────────────────────────────────────────────┘
```

## 🎮 Game Loop Nativo

```c
static void game_loop(AndroidState* state) {
    // 1. Calcular delta time
    float dt = get_delta_time();
    
    // 2. Processar input (touch)
    process_touch_input(state);
    
    // 3. Update física
    physics_world_update(state->physics, dt);
    
    // 4. Renderizar (OpenGL ES)
    glClear(GL_COLOR_BUFFER_BIT);
    render_scene(state->scene);
    
    // 5. Swap buffers
    eglSwapBuffers(state->display, state->surface);
}
```

## 📊 ABI Suportadas

| ABI | Descrição | Uso |
|-----|-----------|-----|
| `armeabi-v7a` | ARM 32-bit | Dispositivos antigos |
| `arm64-v8a` | ARM 64-bit | Dispositivos modernos |
| `x86` | Intel 32-bit | Emuladores |
| `x86_64` | Intel 64-bit | Emuladores modernos |

## 🔧 Build Manual

```bash
# Instalar NDK (uma vez)
# Baixe do Android Developer site

# Exportar path
export ANDROID_NDK_HOME=/path/to/ndk

# Build com ndk-build
cd app/src/main
$ANDROID_NDK_HOME/ndk-build

# Ou com CMake
cd app/src/main/cpp
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake ..
make
```

## 🚀 Fluxo de Execução

```
1. Java Activity inicia
   ↓
2. SurfaceView criado
   ↓
3. nativeInit(surface) chamado
   ↓
4. EGL inicializado
   ↓
5. Engine C criado
   ↓
6. Game loop começa (60 FPS)
   ↓
7. nativeStep() chamado por frame
   ↓
8. Touch events dispatchados
   ↓
9. nativeShutdown() na destruição
```

## 📝 Arquivos Criados

- `jni/Android.mk` - ndk-build makefile
- `jni/Application.mk` - Configurações NDK
- `jni/NativeMain.c` - Código nativo principal
- `cpp/CMakeLists.txt` - CMake build
- `NativeLib.java` - Bridge Java

## ⚠️ Requisitos

- Android SDK
- Android NDK
- CMake (para build CMake)
- Android Studio 3.5+ (para IDE)

## 🔄 Próximos Passos

1. **Testar no emulador/device**
2. **Adicionar mais módulos do engine** (sprites, tilemap)
3. **OpenGL shader para sprites**
4. **Audio nativo** (OpenSL ES ou AAudio)
5. **Input nativo** (AInput)
