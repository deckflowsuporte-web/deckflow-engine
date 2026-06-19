# Phase 7: Animation System

## 📋 Visão Geral

A Phase 7 implementa um sistema completo de animações:

| Sistema | Descrição |
|---------|-----------|
| **SpriteSheet** | Textura com múltiplos frames |
| **AnimatedSprite** | Sprite que alterna frames automaticamente |
| **Tween** | Animações de propriedades (posición, escala, rotação) |
| **Easing** | Funções de interpolação (bounce, elastic, etc) |

## 🎬 Animated Sprite

### Sprite Sheet
```c
// Carregar spritesheet
SpriteSheet* sheet = spritesheet_create(renderer, "player.png", 32, 32);

// Criar animação
AnimationClip* walk = anim_clip_create("walk", 4);
anim_clip_add_frame(walk, 0);
anim_clip_add_frame(walk, 1);
anim_clip_add_frame(walk, 2);
anim_clip_add_frame(walk, 3);
anim_clip_set_fps(walk, 12);

// Criar sprite animado
AnimatedSprite* sprite = animsprite_create(parent, sheet);
animsprite_play(sprite, walk);

// No game loop
animsprite_update(sprite, delta_time);
```

## 🎯 Tween System

Tween anima propriedades de objetos ao longo do tempo.

### Posição
```c
Tween* t = tween_create(manager, node);
tween_position(t, start_pos, end_pos);
tween_ease(t, EASE_BOUNCE);
tween_duration(t, 1.0f);
tween_start(t);
```

### Escala
```c
tween_scale(t, vector2_new(1, 1), vector2_new(2, 2));
tween_ease(t, EASE_ELASTIC);
```

### Rotação
```c
tween_rotation(t, 0, M_PI * 2);  // 360 graus
tween_ease(t, EASE_LINEAR);
```

### Encadeamento
```c
Tween* t1 = tween_create(manager, node);
tween_position(t1, pos1, pos2);

Tween* t2 = tween_create(manager, node);
tween_position(t2, pos2, pos1);

tween_chain(t1, t2);  // Executa t2 depois de t1
```

## 📐 Easing Functions

```
EASE_LINEAR      - Velocidade constante
EASE_IN          - Começa devagar, termina rápido
EASE_OUT         - Começa rápido, termina devagar
EASE_IN_OUT      - Devagar no início e fim
EASE_BOUNCE      - Efeito de pulo
EASE_ELASTIC     - Efeito elástico
EASE_SINE        - Suave como seno
```

### Visualização

```
Linear:     ══════════════
EaseIn:     ═╗
EaseOut:    ╔═
EaseInOut:  ═╦═
Bounce:     ═╱╲╱╲
Elastic:    ═≈≈≈≈
```

## 🎮 Demo

**Controles:**
- `1`: Tween bounce (posición)
- `2`: Tween elastic (escala)
- `3`: Tween rotation
- `4`: Player lerp
- `5`: Scale pop
- `R`: Reset all
- `ESC`: Exit

## 🔄 Callback Pattern

```c
void on_complete(void* data) {
    printf("Animação terminou!\n");
}

Tween* t = tween_create(manager, node);
tween_position(t, start, end);
tween_on_complete(t, on_complete, NULL);
```

## 📊 API Rápida

| Função | Descrição |
|--------|-----------|
| `tween_create()` | Criar tween |
| `tween_position()` | Animar posición |
| `tween_scale()` | Animar escala |
| `tween_rotation()` | Animar rotação |
| `tween_color()` | Animar cor |
| `tween_ease()` | Definir easing |
| `tween_delay()` | Atrasar início |
| `tween_on_complete()` | Callback ao terminar |
| `tween_chain()` | Encadear tweens |
| `tween_kill()` | Cancelar tween |

## 🚀 Próximos Passos

1. **Skeletal Animation**: Animações de skeleton
2. **Spine Runtime**: Integração com Spine
3. **DragonBones**: Suporte a DragonBones
4. **Animation Tree**: Mistura de animações
5. **Inverse Kinematics**: IK para personagens
