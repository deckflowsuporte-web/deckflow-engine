# DeckFlow Engine - FASE 1: Fundamentos

**Versão:** 1.0.0  
**Data:** 2024-06-19  
**Status:** ✅ COMPLETA

---

## Resumo Executivo

A FASE 1 estabeleceu a fundação do DeckFlow Engine - um motor de jogos 2D mobile-first inspirado na Godot. Foi implementado o sistema de nodes, transformações 2D, e gerenciamento de cenas em C11 puro.

---

## Estrutura do Projeto

```
deckflow-engine/
├── src/                          # Código fonte C do motor
│   ├── core/                    # Núcleo do motor
│   │   ├── engine.h             # Header principal
│   │   └── engine.c             # Loop principal, SDL2
│   ├── math/                     # Biblioteca matemática
│   │   ├── vector2.h            # Header Vector2
│   │   ├── vector2.c            # Implementação Vector2
│   │   ├── transform2d.h        # Header Transform2D
│   │   └── transform2d.c        # Implementação Transform2D
│   ├── nodes/                    # Sistema de nodes
│   │   ├── node.h               # Header Node
│   │   └── node.c               # Implementação Node
│   └── scenes/                   # Gerenciamento de cenas
│       ├── scene.h               # Header Scene
│       ├── scene.c               # Implementação Scene
│       ├── scene_tree.h          # Header SceneTree
│       └── scene_tree.c          # Implementação SceneTree
├── app/                          # Wrapper Android
│   ├── jni/                      # Código nativo Android
│   │   ├── Android.mk            # NDK build
│   │   └── android_wrapper.c     # Bridge JNI
│   └── src/main/
│       ├── AndroidManifest.xml
│       └── java/com/deckflow/engine/
│           └── DeckFlowLib.java  # Interface Java
├── CMakeLists.txt                # Build CMake
├── README.md                     # Documentação principal
└── .github/workflows/
    └── build.yml                # CI/CD
```

---

## Componentes Implementados

### 1. Vector2 (`src/math/vector2.h/.c`)

**Propósito:** Operações matemáticas em vetores 2D.

**Estrutura:**
```c
typedef struct Vector2 {
    float x;
    float y;
} Vector2;
```

**Funções Implementadas:**

| Categoria | Função | Descrição |
|-----------|--------|-----------|
| Construtores | `vector2_new(x, y)` | Cria vetor com coordenadas |
| | `vector2_zero()` | Vetor (0, 0) |
| | `vector2_one()` | Vetor (1, 1) |
| | `vector2_up/down/left/right()` | Vetores unitários |
| Aritmética | `vector2_add(a, b)` | Soma |
| | `vector2_sub(a, b)` | Subtração |
| | `vector2_mul(v, s)` | Multiplicação por escalar |
| | `vector2_div(v, s)` | Divisão por escalar |
| | `vector2_negate(v)` | Negação |
| Normalização | `vector2_normalized(v)` | Vetor unitário |
| | `vector2_length(v)` | Magnitude |
| | `vector2_length_squared(v)` | Magnitude² (otimizado) |
| Geometria | `vector2_dot(a, b)` | Produto escalar |
| | `vector2_cross(a, b)` | Produto vetorial |
| | `vector2_distance(a, b)` | Distância entre pontos |
| | `vector2_angle(v)` | Ângulo do vetor |
| | `vector2_rotated(v, angle)` | Rotação |
| Interpolação | `vector2_lerp(a, b, t)` | Interpolação linear |
| | `vector2_clamp(v, min, max)` | Limita valores |

**Impacto:** Usado em TODO o motor - positions, scales, velocities, etc.

---

### 2. Transform2D (`src/math/transform2d.h/.c`)

**Propósito:** Representar posição, rotação e escala de nodes.

**Estrutura:**
```c
typedef struct Transform2D {
    Vector2 position;      // Posição no mundo
    float rotation;        // Rotação em radianos
    Vector2 scale;         // Escala (1 = normal)
    Vector2 origin;        // Ponto de pivô
    bool dirty;            // Flag de cache inválido
} Transform2D;
```

**Construtores:**
| Função | Descrição |
|--------|-----------|
| `transform2d_new()` | Cria transform identidade |
| `transform2d_identity()` | Alias para new() |
| `transform2d_from_position(pos)` | Cria com posição |
| `transform2d_new_full(pos, rot, scale)` | Cria completo |

**Operações:**
| Função | Descrição |
|--------|-----------|
| `transform2d_xform_vector2(t, v)` | Transforma vetor (local→mundo) |
| `transform2d_xform_inv_vector2(t, v)` | Transforma vetor inverso (mundo→local) |
| `transform2d_inverted(t)` | Retorna transform inversa |
| `transform2d_linear_interpolate(a, b, t)` | Interpolação entre transforms |

**Modificadores:**
| Função | Descrição |
|--------|-----------|
| `transform2d_set_position(t, pos)` | Define posição |
| `transform2d_set_rotation(t, rot)` | Define rotação |
| `transform2d_set_scale(t, scale)` | Define escala |
| `transform2d_translate(t, offset)` | Move relativamente |
| `transform2d_rotate(t, angle)` | Rotaciona relativamente |

**Impacto:** TODO node tem um Transform2D. Sistema de parent/child usa transforms globais.

---

### 3. Node (`src/nodes/node.h/.c`)

**Propósito:** Base para TODO elemento do jogo - inspirado no sistema de nodes da Godot.

**Filosofia:**
> "Tudo é um Node. Nodes são organizados em árvore (Scene Tree)."

**Estrutura:**
```c
struct Node {
    // Identidade
    char name[64];
    uint32_t id;
    NodeType type;
    NodeFlags flags;
    
    // Estrutura de árvore
    Node* parent;
    Node* first_child;
    Node* last_child;
    Node* next_sibling;
    Node* prev_sibling;
    int child_count;
    
    // Transform
    Transform2D transform;
    int z_index;
    bool visible;
    
    // Processamento
    ProcessMode process_mode;
    bool process_enabled;
    bool physics_process_enabled;
    bool input_enabled;
    
    // Callbacks
    NodeCallback on_ready;
    NodeCallback on_enter_tree;
    NodeCallback on_exit_tree;
    NodeCallback on_destroy;
    NodeProcessCallback on_process;
    NodePhysicsCallback on_physics_process;
    NodeInputCallback on_input;
    
    // User data
    void* user_data;
    void (*user_data_destructor)(void*);
};
```

**Tipos de Node (enum NodeType):**
```c
typedef enum NodeType {
    NODE_TYPE_GENERIC = 0,
    NODE_TYPE_SPATIAL_2D,
    NODE_TYPE_SPRITE,
    NODE_TYPE_CAMERA_2D,
    NODE_TYPE_RIGID_BODY_2D,
    NODE_TYPE_COLLISION_SHAPE_2D,
    NODE_TYPE_TILE_MAP,
    NODE_TYPE_CONTROL,
    NODE_TYPE_BUTTON,
    NODE_TYPE_LABEL,
    NODE_TYPE_TEXTURE_RECT,
    NODE_TYPE_PANEL,
    NODE_TYPE_ANIMATION_PLAYER,
    NODE_TYPE_AUDIO_STREAM_PLAYER,
    NODE_TYPE_PARTICLE_EMITTER,
    NODE_TYPE_SCRIPT,
    NODE_TYPE_COUNT
} NodeType;
```

**Operações de Árvore:**
| Função | Descrição |
|--------|-----------|
| `node_create(name, type)` | Cria novo node |
| `node_destroy(node)` | Libera node (não filhos) |
| `node_free(node)` | Libera node e TODOS filhos |
| `node_add_child(parent, child)` | Adiciona filho |
| `node_remove_child(parent, child)` | Remove filho |
| `node_find_child(node, name, recursive)` | Encontra filho por nome |
| `node_get_child(node, index)` | Get filho por índice |
| `node_get_child_count(node)` | Conta filhos |

**Transform Global:**
| Função | Descrição |
|--------|-----------|
| `node_get_global_position(node)` | Posição no mundo |
| `node_get_global_rotation(node)` | Rotação acumulada |
| `node_get_global_scale(node)` | Escala acumulada |
| `node_get_global_transform(node)` | Transform completa no mundo |
| `node_set_global_position(node, pos)` | Seta posição mundo |
| `node_set_global_rotation(node, rot)` | Seta rotação mundo |

**Impacto:** Sistema central. TODO objeto do jogo (sprites, players,etc) será um Node.

---

### 4. Scene (`src/scenes/scene.h/.c`)

**Propósito:** Container para todo conteúdo do jogo. Uma Scene é uma árvore de Nodes.

**Estrutura:**
```c
typedef struct Scene {
    char name[64];
    char file_path[256];
    Node* root;
    SceneState* state;
    bool physics_enabled;
    float physics_fps;
    bool paused;
    int pause_depth;
    Node* camera;
    bool editable;
    bool unique_instance;
} Scene;
```

**Ciclo de Vida:**
| Função | Descrição |
|--------|-----------|
| `scene_create(name)` | Cria nova cena |
| `scene_destroy(scene)` | Destroi cena |
| `scene_free(scene)` | Libera memória completa |

**Gerenciamento de Pause:**
| Função | Descrição |
|--------|-----------|
| `scene_set_paused(scene, paused)` | Pausa/resume |
| `scene_push_pause(scene)` | Pausa aninhada |
| `scene_pop_pause(scene)` | Resume aninhado |
| `scene_is_paused(scene)` | Verifica pausa |

**Processamento:**
| Função | Descrição |
|--------|-----------|
| `scene_process(scene, delta)` | Atualiza lógica (60fps) |
| `scene_physics_process(scene, delta)` | Atualiza física |
| `scene_input(scene, type, data)` | Processa input |

**Busca:**
| Função | Descrição |
|--------|-----------|
| `scene_find_node(scene, name)` | Encontra node por nome |
| `scene_get_node_by_id(scene, id)` | Encontra por ID |

**Serialização:**
| Função | Descrição |
|--------|-----------|
| `scene_save(scene, path)` | Salva para JSON |
| `scene_load(scene, path)` | Carrega de JSON |
| `scene_instance(path)` | Instancia cena de arquivo |

**Impacto:** Cada "nível" ou "tela" do jogo será uma Scene.

---

### 5. SceneTree (`src/scenes/scene_tree.h/.c`)

**Propósito:** Utilitários para iterar e manipular a árvore de cenas.

**Funções:**
| Função | Descrição |
|--------|-----------|
| `scene_tree_iterate(scene, callback, data)` | Itera todos nodes |
| `scene_tree_get_node_at_path(scene, path)` | Get node por caminho (ex: "Player/Sprite") |
| `node_get_path(node)` | Get caminho completo do node |
| `scene_tree_count_nodes(scene)` | Conta total de nodes |
| `scene_tree_count_nodes_of_type(scene, type)` | Conta nodes de tipo |

---

### 6. Engine (`src/core/engine.h/.c`)

**Propósito:** Orquestra todo o motor - loop principal, janelas, scenes.

**Estado do Motor:**
```c
typedef enum EngineState {
    ENGINE_STATE_UNINITIALIZED = 0,
    ENGINE_STATE_CREATED,
    ENGINE_STATE_READY,
    ENGINE_STATE_RUNNING,
    ENGINE_STATE_PAUSED,
    ENGINE_STATE_STOPPED
} EngineState;
```

**Configuração:**
```c
typedef struct EngineConfig {
    int window_width;
    int window_height;
    const char* window_title;
    bool fullscreen;
    bool resizable;
    int target_fps;
    float physics_fps;
} EngineConfig;
```

**Loop Principal:**
1. `engine_process_events()` - Processa SDL events
2. `engine_update(delta)` - Atualiza lógica
3. `engine_render()` - Renderiza frame

**Gestão de Cena:**
| Função | Descrição |
|--------|-----------|
| `engine_set_scene(scene)` | Define cena ativa |
| `engine_get_scene()` | Get cena atual |
| `engine_load_scene(path)` | Carrega e define cena |

**Tempo:**
| Função | Descrição |
|--------|-----------|
| `engine_get_delta()` | Delta time do frame |
| `engine_get_elapsed()` | Tempo total |
| `engine_get_fps()` | FPS atual |

---

## Decisões de Arquitetura

### 1. Por que C ao invés de C++?

- **Controle total** de memória
- **Portabilidade** - compila em qualquer lugar
- **Simplicidade** - sem templates, exceções, RAII
- **Performance** - sem overhead de runtime C++

### 2. Por que não orientação a objetos tradicional?

- **Composição sobre herança** - nodes são componentes
- **Scripting** mais simples com C
- **FFI** (Foreign Function Interface) mais fácil para Lua

### 3. Por que doubly-linked list para children?

- **Inserção O(1)** no final
- **Remoção O(1)**
- **Iteração bidirecional**
- **Simples de serializar**

### 4. Por que transform local vs global?

- **Performance** - local é o que designers/editores manipulam
- **Reutilização** - mesmo sprite pode ser instanciado em posições diferentes
- **Parent/Child** - transforms se acumulam naturalmente

---

## Dependências

### Externas
| Biblioteca | Versão | Propósito |
|-----------|--------|-----------|
| SDL2 | 2.0+ | Rendering, input, janela |

### Internas (orden de dependência)
| Módulo | Dependências |
|--------|-------------|
| vector2 | nenhuma |
| transform2d | vector2 |
| node | transform2d |
| scene | node |
| engine | scene, SDL2 |

---

## Limitações da FASE 1

1. **Sem rendering real** - SDL2 inicializado mas sem draw calls
2. **Sem Lua** - FASE 3
3. **Sem física** - FASE 6 (Box2D)
4. **Sem UI** - FASE 4
5. **Sem editor** - FASE 7
6. **Serialização básica** - placeholders para JSON

---

## Como Testar

### Compilação Standalone (Linux)
```bash
mkdir build && cd build
cmake .. && make
./deckflow
```

### Android (NDK)
```bash
cd app
ndk-build
```

---

## Próximos Passos

### FASE 2: SDL2 Rendering
- [ ] Sprite rendering
- [ ] Camera2D
- [ ] Texture loading
- [ ] Batch rendering

### FASE 3: Lua Scripting
- [ ] Lua binding
- [ ] Script component
- [ ] API Lua

### FASE 4: UI Components
- [ ] Button
- [ ] Label
- [ ] Image
- [ ] Panel

### FASE 5: Events
- [ ] Signal system
- [ ] Event callbacks

### FASE 6: Physics
- [ ] Box2D integration
- [ ] Collision detection
- [ ] RigidBody2D

### FASE 7: Mobile Editor
- [ ] Touch-friendly UI
- [ ] Scene editor
- [ ] Property inspector

### FASE 8: APK Export
- [ ] Build APK
- [ ] Sign APK
- [ ] Export wizard

---

## Glossário

| Termo | Definição |
|-------|-----------|
| Node | Objeto base do motor |
| Scene | Container de nodes |
| Transform | Posição, rotação, escala |
| Callback | Função chamada em eventos |
| Delta | Tempo entre frames |
| Z-Index | Ordem de renderização |

---

## Changelog

### v1.0.0 (2024-06-19)
- ✅ Implementação inicial
- ✅ Sistema de nodes
- ✅ Transform2D
- ✅ Vector2 math
- ✅ Scene management
- ✅ SceneTree utilities
