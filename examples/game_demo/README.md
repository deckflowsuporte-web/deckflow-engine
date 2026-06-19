# DeckFlow Platformer - Complete Game Demo

## 📋 Visão Geral

Este é um **platformer 2D completo** demonstrando todos os sistemas do DeckFlow Engine:

- ✅ Scene/Node hierarchy
- ✅ Physics (gravity, collisions)
- ✅ Sprite rendering
- ✅ Camera follow
- ✅ Tilemap system
- ✅ Particle effects
- ✅ UI overlay

## 🎮 Gameplay

**Objetivo:** Coletar todas as moedas!

**Controles:**
- **← → / A D**: Mover
- **SPACE**: Pular
- **ESC**: Sair

## 🏗️ Arquitetura

```
┌─────────────────────────────────────────────────────────────┐
│                        Game Loop                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐               │
│  │  Input   │ → │ Physics  │ → │  Render  │               │
│  │          │   │  Update  │   │          │               │
│  └──────────┘   └──────────┘   └──────────┘               │
│       ↓             ↓              ↓                         │
│  ┌──────────────────────────────────────────┐               │
│  │              Game Logic                     │               │
│  │  - Collision detection                    │               │
│  │  - Coin collection                        │               │
│  │  - Lives management                       │               │
│  └──────────────────────────────────────────┘               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 🎯 Sistemas Demonstrados

### 1. Scene Graph
```c
Scene* scene = scene_create("GameScene");
Node* root = scene_get_root(scene);
node_add_child(root, player_node);
```

### 2. Physics
```c
PhysicsWorld* world = physics_world_create(vector2_new(0, -800));

PhysicsComponent* body = physics_component_create(world, node, DYNAMIC);
physics_set_box(body, 32, 40);
physics_set_material(body, 1.0f, 0.1f, 0.0f);

// Apply forces
physics_apply_force(body, vector2_new(force_x, GRAVITY));
```

### 3. Camera
```c
Camera* cam = camera_create("Main");
camera_follow(cam, player_node);
camera_set_smoothing(cam, 0.1f);
camera_update(cam, delta_time);
```

### 4. Particles
```c
ParticleSystem* particles = particles_create(500);
ParticleEmitter* emitter = particles_add_emitter(particles, "sparkle");
emitter_configure_sparkle(emitter);
particles_emit(particles, emitter, position, 10);
```

## 🗺️ Level Design

```
Legend:
  █ = Ground
  ▓ = Platform  
  ● = Coin
  ░ = Lava (death)
```

```
        ▓▓▓▓▓             ▓▓▓▓▓▓           ▓▓▓▓
                 ●                    ●
    ▓▓▓▓         ▓▓▓▓▓         ▓▓▓▓▓         ▓▓▓▓▓
         ●              ●                   ●
██████████████████████████████████░░░████████████
```

## 🏃 Player Physics

| Property | Value |
|----------|-------|
| Width | 32px |
| Height | 40px |
| Speed | 250 |
| Jump | 450 |
| Gravity | -800 |

## 🔧 Build

```bash
cd examples/game_demo
gcc -o game main.c \
    ../../src/math/*.c \
    ../../src/nodes/*.c \
    ../../src/scenes/*.c \
    ../../src/physics/*.c \
    ../../src/input/*.c \
    ../../src/camera/*.c \
    ../../src/particle/*.c \
    -I../../src -lSDL2 -lSDL2_ttf -lm -lpthread

./game
```

## 📝 Game States

```c
typedef enum {
    STATE_MENU,      // Title screen
    STATE_PLAYING,  // Gameplay
    STATE_PAUSED,   // Paused
    STATE_GAMEOVER, // Player died
    STATE_WIN       // All coins collected
} GameState;
```

## 🎨 Visual Style

| Element | Color |
|---------|-------|
| Sky | #3C64B4 |
| Ground | #503C28 |
| Platform | #785032 |
| Player | #50C878 |
| Coin | #FFD700 |
| Lava | #FF3C00 |

## 🚀 Próximos Passos

1. **Inimigos:** AdicionarIA de movimento
2. **Armas:** Sistema de combate
3. **Power-ups:** Invencibilidade, velocidade
4. **Sons:** Música e efeitos
5. **Animations:** Sprites animados para o player
6. **Levels:** Mais fases
7. **Save:** Sistema de salvar/carregar

## 📄 Arquivos

```
game_demo/
├── main.c    # Complete game (~800 lines)
└── README.md
```

Este demo é a integração completa de todos os sistemas desenvolvidos nas Phases 1-7 do DeckFlow Engine!
