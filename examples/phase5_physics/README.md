# Phase 5: Physics + Scene Integration

## 📋 Visão Geral

A Phase 5 integra o sistema de física **Box2D** com o **Scene Graph**, permitindo que corpos rígidos sejam componentes anexados a Nodes.

## 🎯 Conceito Central

```
┌─────────────────────────────────────────────────────────────┐
│                        Scene                                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │                    Node Tree                          │   │
│  │                                                      │   │
│  │   ┌─────────────┐      ┌─────────────┐              │   │
│  │   │  Node       │      │  Node       │              │   │
│  │   │  ┌───────┐  │      │  ┌───────┐  │              │   │
│  │   │  │Sprite │  │      │  │Sprite │  │              │   │
│  │   │  └───────┘  │      │  └───────┘  │              │   │
│  │   │  ┌───────┐  │      │  ┌───────┐  │              │   │
│  │   │  │Physics│──┼──────┼──│Physics│  │              │   │
│  │   │  │Comp   │  │      │  │Comp   │  │              │   │
│  │   │  └───────┘  │      │  └───────┘  │              │   │
│  │   └─────────────┘      └─────────────┘              │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
```

## 🔗 Integração Node ↔ Physics

### Anexar Physics a um Node
```c
// Criar physics component
PhysicsComponent* body = physics_component_create(world, node, PHYSICS_BODY_DYNAMIC);

// Anexar ao node
physics_node_attach(node, body);

// Configurar
physics_set_box(body, 50, 50);
physics_set_material(body, 1.0f, 0.3f, 0.2f);
```

### Sincronização Automática
```c
// Física atualiza → Node transform
physics_sync_to_node(body, node);

// Node transform atualiza → Física
physics_sync_from_node(body, node);
```

## 📦 API do Physics Component

### Criação
```c
PhysicsComponent* physics_component_create(
    PhysicsWorld* world,   // Mundo físico
    Node* node,           // Node associado
    PhysicsBodyType type  // STATIC, KINEMATIC, DYNAMIC
);
```

### Formas
```c
physics_set_circle(comp, radius);
physics_set_box(comp, width, height);
```

### Material
```c
physics_set_material(comp, density, friction, restitution);
```

### Colisão
```c
physics_set_collision_layer(comp, PHYSICS_LAYER_PLAYER, PHYSICS_LAYER_WORLD);
```

## 🚀 Aplicar Forces

```c
// Força contínua (para movimento)
physics_apply_force(comp, vector2_new(500, 0));

// Impulso instantâneo (para pulo)
physics_apply_impulse(comp, vector2_new(0, 400));

// Torque (rotação)
physics_apply_torque(comp, 10.0f);
```

## 🎮 Demo

**Controles:**
- **WASD/Arrows**: Aplicar força ao player
- **SPACE**: Pulo (impulso para cima)
- **R**: Resetar posição
- **B**: Criar nova bola
- **ESC**: Sair

## 🏗️ Hierarquia da Cena

```
Scene
└── Root
    ├── Ground (STATIC)
    ├── WallLeft (STATIC)
    ├── WallRight (STATIC)
    ├── Platform0 (STATIC)
    ├── Platform1 (STATIC)
    ├── Platform2 (STATIC)
    ├── Player (DYNAMIC) ← PhysicsComponent
    └── Ball0-4 (DYNAMIC) ← PhysicsComponent
```

## 📝 Fluxo de Update

```c
// 1. Input
handle_input();

// 2. Aplicar forças baseado no input
if (left_pressed) physics_apply_force(player, left_force);

// 3. Step da física
physics_world_update(world, delta_time);

// 4. Sincronizar transforms
physics_sync_to_node(player_body, player_node);

// 5. Renderizar
render_scene(scene);
```

## 🔧 API do Physics World

```c
// Criar mundo
PhysicsWorld* world = physics_world_create(gravity);

// Configurar limites
physics_world_set_bounds(world, 0, 0, width, height);

// Update
physics_world_update(world, delta_time);

// Raycast
PhysicsRayHit hit;
if (physics_raycast(world, origin, dir, max_dist, &hit)) {
    // Colidiu com algo
}
```

## 📊 Body Types

| Type | gravity | Forces | Collision |
|------|---------|--------|-----------|
| **STATIC** | ❌ | ❌ | ✅ |
| **KINEMATIC** | ❌ | ❌ | ✅ |
| **DYNAMIC** | ✅ | ✅ | ✅ |

## 🚀 Próximos Passos

1. **Collision callbacks**: `on_collision_start/end`
2. **Compound shapes**: Múltiplas formas em um body
3. **Joints**: Hinges, springs, sliders
4. **Triggers**: Sensors para detecção sem física
