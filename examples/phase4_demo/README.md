# Phase 4: Complete Game Systems

## 📋 Visão Geral

A Phase 4 integra todos os subsistemas do motor de jogo:

| Sistema | Descrição | Status |
|---------|-----------|--------|
| **Input** | Teclado, mouse, touch | ✅ |
| **Camera** | Viewport, zoom, seguimento | ✅ |
| **Audio** | Sons e música | ✅ |
| **Tilemap** | Mapas baseados em tiles | ✅ |
| **Particle** | Efeitos visuais | ✅ |

## 🎮 Demonstração

O Phase 4 Demo demonstra:
- Movimento do player com WASD/Setas
- Câmera que segue o player com zoom
- Sistema de partículas (fogo, fumaça)
- Tilemap procedural
- Efeitos de câmera (shake)

## 🏗️ Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                     Game Engine                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │  Input   │  │  Camera  │  │  Audio   │  │ Tilemap  │   │
│  │          │  │          │  │          │  │          │   │
│  │ KeyState │  │ Follow   │  │ SDL_mixer│  │ TileData │   │
│  │ MousePos │  │ Zoom     │  │ Music    │  │ Layers   │   │
│  │ Touch    │  │ Shake    │  │ SFX      │  │ Tileset  │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
│                                                             │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              Particle System                          │   │
│  │  ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐     │   │
│  │  │  Fire  │  │ Smoke  │  │Explode │  │Sparkle │     │   │
│  │  └────────┘  └────────┘  └────────┘  └────────┘     │   │
│  └──────────────────────────────────────────────────────┘   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 📁 Estrutura de Código

```
src/
├── input/
│   ├── input.h      # Header com tipos e funções
│   └── input.c      # Implementação do input
├── camera/
│   ├── camera.h     # Câmera com seguimento e zoom
│   └── camera.c     # Transformações e culling
├── audio/
│   ├── audio.h      # Áudio com SDL_mixer
│   └── audio.c      # Música e efeitos sonoros
├── tilemap/
│   ├── tilemap.h    # Tilemap com layers
│   └── tilemap.c    # Renderização e colisão
└── particle/
    ├── particle.h   # Sistema de partículas
    └── particle.c   # Emissores e presets
```

## 🔧 API do Input System

### Inicialização
```c
input_init();        // Iniciar sistema
input_update();      // Chamar a cada frame
input_shutdown();    // Finalizar
```

### Teclado
```c
// Estado contínuo
if (input_key_pressed(KEY_SPACE)) { ... }

// Eventos (um frame)
if (input_key_just_pressed(KEY_SPACE)) { ... }
if (input_key_just_released(KEY_SPACE)) { ... }
```

### Mouse
```c
Vector2 pos = input_mouse_position();
Vector2 delta = input_mouse_delta();
Vector2 wheel = input_mouse_wheel();

if (input_mouse_button_just_pressed(MOUSE_LEFT)) { ... }
```

## 📷 API da Câmera

### Criação e Configuração
```c
Camera* cam = camera_create("MainCamera");
camera_set_viewport(cam, 800, 600);
camera_set_position(cam, vector2_new(400, 300));
camera_set_zoom(cam, 2.0f);  // 2x zoom
```

### Modo Follow
```c
camera_follow(cam, player_node);
camera_set_smoothing(cam, 0.1f);  // 0 = instant, 1 = máximo lag
```

### Shake
```c
camera_shake(cam, 10.0f, 0.5f);  // intensidade, duração
```

### Coordenadas
```c
Vector2 screen = camera_world_to_screen(cam, world_pos);
Vector2 world = camera_screen_to_world(cam, screen_pos);

// Culling
if (camera_is_point_visible(cam, pos)) { ... }
```

## 🔊 API de Áudio

### Inicialização
```c
audio_init();
audio_set_master_volume(0.8f);
audio_set_music_volume(0.5f);
audio_set_sfx_volume(1.0f);
```

### Carregar e Tocar
```c
AudioChannel* music = audio_load_music("music.mp3");
audio_play_music(music);

AudioChannel* sfx = audio_load_sfx("explosion.wav");
audio_play_sfx(sfx);
```

### Controle
```c
audio_pause_music();
audio_resume_music();
audio_stop_music();
audio_crossfade_music(new_music, 1000);  // 1 segundo
```

## 🗺️ API do Tilemap

### Criação
```c
Tilemap* map = tilemap_create(20, 15, 32, 32);  // largura, altura, tile_w, tile_h
TilemapLayer* layer = tilemap_add_layer(map, 20, 15);
```

### Operações
```c
// Definir tile
tilemap_set_tile(map, 0, x, y, TILE_WALL);

// Obter tile
int tile = tilemap_get_tile(map, 0, x, y);

// Colisão
if (tilemap_is_solid_at(map, 0, world_x, world_y)) { ... }

// Conversão
Vector2 world_pos = tilemap_tile_to_world(map, tile_x, tile_y);
```

### Geração
```c
tilemap_generate_level(map, 0, 12345);  // Gera caverna aleatória
```

## ✨ API de Partículas

### Sistema
```c
ParticleSystem* particles = particles_create(1000);
particles_update(particles, delta_time);
particles_render(particles, renderer, camera);
```

### Emissores
```c
ParticleEmitter* emitter = particles_add_emitter(particles, "fire");
emitter_configure_fire(emitter);  // Preset
emitter_start(emitter);
```

### Configuração Manual
```c
emitter_set_position(emitter, vector2_new(100, 100));
emitter_set_rate(emitter, 30.0f);  // partículas/segundo
emitter_set_life(emitter, 0.5f, 1.5f);
emitter_set_size(emitter, 10.0f, 20.0f, 0.0f, 5.0f);
emitter_set_velocity(emitter, vector2_new(-50, 50), vector2_new(50, 150));
emitter_set_color(emitter, 1.0f, 0.5f, 0.0f, 1.0f,   // start RGBA
                              0.3f, 0.0f, 0.0f, 0.0f); // end RGBA
emitter_set_gravity(emitter, -50.0f, -30.0f);
```

### Presets
```c
emitter_configure_fire(emitter);      // Chamas subindo
emitter_configure_smoke(emitter);     // Fumaça
emitter_configure_explosion(emitter); // Explosão radial
emitter_configure_sparkle(emitter);   // Brilhos
emitter_configure_rain(emitter);      // Gotas de chuva
```

### Burst
```c
// Explosão única
emitter_configure_explosion(emitter);
particles_emit(particles, emitter, position, 50);
```

## ⌨️ Controles do Demo

| Tecla | Ação |
|-------|------|
| ← → ↑ ↓ / WASD | Mover player |
| Q / E | Zoom in/out |
| R | Resetar câmera |
| SPACE | Camera shake + explosão |
| F | Toggle partículas |
| ESC | Sair |

## 🛠️ Build

```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev

# Compilar
cd examples/phase4_demo
gcc -o phase4 main.c \
    ../../src/math/*.c \
    ../../src/nodes/*.c \
    ../../src/scenes/*.c \
    ../../src/input/*.c \
    ../../src/camera/*.c \
    ../../src/audio/*.c \
    ../../src/tilemap/*.c \
    ../../src/particle/*.c \
    -I../../src -lSDL2 -lm -lpthread

# Executar
./phase4
```

## 🚀 Próximos Passos

1. **Phase 5:** Integração com Box2D physics
2. **UI System:** Menus e HUDs
3. **Animation:** Sprites animados
4. **Networking:** Multiplayer
5. **Scripting:** Lua/JavaScript integration

## 📝 Notas Técnicas

- **Input:** Polling por frame para `just_pressed`
- **Camera:** Smooth follow com interpolação linear
- **Audio:** Stub com SDL_mixer (implementar para produção)
- **Tilemap:** Suporte a múltiplas layers com parallax
- **Particle:** Sistema baseado em emissores com presets
