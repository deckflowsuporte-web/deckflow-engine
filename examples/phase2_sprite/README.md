# Phase 2: SDL2 + Sprite Rendering

## 📋 Visão Geral

A Fase 2 demonstra a integração do motor de jogo com **SDL2** para renderização visual, incluindo:

- ✅ Criação de janela SDL2
- ✅ Renderização de sprites (texturas retangulares)
- ✅ Sistema de cena com hierarquia de nós
- ✅ Transformações globais (posição + rotação)
- ✅ Loop de jogo a 60 FPS

## 🎮 Demonstração

O exemplo cria uma cena de jogo simples com:
- **Player** (verde) - controlado pelo teclado
- **Inimigos** (vermelhos) - estáticos e em grupo
- **Moedas** (douradas) - coletáveis

## 🏗️ Arquitetura

```
Scene (GameScene)
└── Root Node
    ├── Player Sprite (64x64, verde)
    ├── Enemy1 Sprite (48x48, vermelho)
    ├── Enemy2 Sprite (48x48, vermelho, rotacionado)
    ├── Coin1 Sprite (24x24, dourado)
    ├── Coin2 Sprite (24x24, dourado)
    └── Enemies Node (grupo)
        └── Enemy3 Sprite (40x40, vermelho, posição relativa)
```

### Estrutura de Dados

```c
// Sprite attached to a Node via user_data
typedef struct {
    SDL_Texture* texture;
    int width;
    int height;
    SDL_Color color;
} SpriteData;
```

## 🔧 API de Sprite

### `create_sprite(name, width, height, color)`
Cria um nó do tipo sprite com textura colorido.

```c
Node* player = create_sprite("Player", 64, 64, COLOR_GREEN);
player->transform.position = vector2_new(320, 240);
node_add_child(root, player);
```

### `render_node(node)`
Renderiza recursivamente um nó e todos os seus filhos, aplicando transformações globais.

### `node_get_global_position(node)`
Retorna a posição global do nó (considerando todos os ancestrais).

### `node_get_global_rotation(node)`
Retorna a rotação global do nó (soma de todas as rotações).

## ⌨️ Controles

| Tecla | Ação |
|-------|------|
| ← → ↑ ↓ | Mover player |
| Q / E | Rotacionar player |
| ESC | Sair |

## 🛠️ Build

### Linux / macOS
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev

# Compilar
cd examples/phase2_sprite
gcc -o phase2 main.c ../../src/math/*.c ../../src/nodes/*.c ../../src/scenes/*.c \
    -I../../src -lSDL2 -lm -lpthread

# Executar
./phase2
```

### macOS (Homebrew)
```bash
brew install sdl2
gcc -o phase2 main.c ../../src/math/*.c ../../src/nodes/*.c ../../src/scenes/*.c \
    -I../../src -lSDL2 -lm
```

## 📐 Transformações

### Posição Global
A posição de um nó é calculada recursivamente:
```
global_pos = parent_global_pos + local_pos
```

Exemplo:
- Enemies Node: posição (50, 50)
- Enemy3 (filho): posição (100, 100)
- **Global Enemy3**: (150, 150)

### Rotação Global
A rotação é acumulada: `global_rotation = parent_rotation + local_rotation`

## 🔄 Loop de Jogo

```c
while (running) {
    // 1. Processar eventos (input)
    while (SDL_PollEvent(&event)) {
        handle_input(&event);
    }
    
    // 2. Atualizar lógica do jogo
    update_game();
    
    // 3. Renderizar
    SDL_RenderClear(renderer);
    render_node(root);
    SDL_RenderPresent(renderer);
    
    // 4. Frame rate
    SDL_Delay(16);  // 60 FPS
}
```

## 📊 Performance

- **FPS Target:** 60
- **Max Sprites:** Limitado por memória + GPU
- **Hierarquia:** Suporte a nós pais/filhos ilimitados

## 🔗 Integração com Engine

```
┌─────────────────────────────────────────────────────┐
│                    Scene                             │
│  ┌───────────────────────────────────────────────┐  │
│  │              Scene Tree                        │  │
│  │  ┌─────────────────────────────────────────┐  │  │
│  │  │              Node                       │  │  │
│  │  │  - Transform (position, rotation)       │  │  │
│  │  │  - Children (linked list)                │  │  │
│  │  │  - User Data (SpriteData*)              │  │  │
│  │  └─────────────────────────────────────────┘  │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────┐
│                  SDL2 Renderer                       │
│  - SDL_CreateWindow()                               │
│  - SDL_CreateRenderer()                             │
│  - SDL_RenderCopyEx() (com rotação)                │
└─────────────────────────────────────────────────────┘
```

## 🚀 Próximos Passos

1. **Phase 3:** Adicionar física com Box2D
2. **Texturas reais:** Carregar imagens PNG/JPG
3. **Animações:** Sistema de sprites animados
4. **Camera:** Sistema de câmera com viewport
5. **Tilemap:** Renderização de mapas baseados em tiles

## 📝 Notas Técnicas

- Sprites são textures SDL2 criados de `SDL_Surface`
- Rotação é aplicada via `SDL_RenderCopyEx()`
- Transformações globais são calculadas no momento da renderização
- `user_data_destructor` garante limpeza de memória
