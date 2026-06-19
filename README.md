# DeckFlow Engine

**Mobile-First 2D Game Engine for Android**

A professional-grade 2D game engine inspired by Godot, designed for game development directly on Android devices.

## Philosophy

**Visual First, Code Optional**

Create games using visual tools without writing code. When needed, use Lua for advanced logic.

## Features

- [x] **Node System** - Godot-style entity system
- [x] **Transform2D** - Position, rotation, scale
- [x] **Scene Tree** - Hierarchical scene management
- [x] **Vector2 Math** - Complete 2D vector operations
- [ ] SDL2 Rendering (Phase 2)
- [ ] Lua Scripting (Phase 3)
- [ ] UI Components (Phase 4)
- [ ] Event System (Phase 5)
- [ ] Box2D Physics (Phase 6)
- [ ] Mobile Editor (Phase 7)
- [ ] APK Export (Phase 8)

## Architecture

```
Scene
├── Node (Base)
│   ├── Sprite
│   ├── Camera2D
│   ├── RigidBody2D
│   ├── CollisionShape2D
│   └── ... (more components)
└── Node
```

## Tech Stack

| Component | Technology |
|-----------|------------|
| Core | C11 |
| Scripting | Lua |
| Physics | Box2D |
| Rendering | SDL2 |
| Build | CMake |
| Target | Android |

## Project Structure

```
deckflow-engine/
├── src/
│   ├── core/           # Engine core
│   ├── math/           # Vector2, Transform2D
│   ├── nodes/          # Node system
│   ├── scenes/         # Scene management
│   └── utils/          # Utilities
├── app/
│   ├── jni/            # Android NDK
│   └── src/main/       # Java wrapper
└── CMakeLists.txt
```

## Build Status

- **Phase 1**: Node System & Transform ✅
- **Phase 2**: SDL2 Rendering 🚧
- **Phase 3-8**: In Progress

## Roadmap

### Phase 1: Foundation ✅
- [x] Project structure
- [x] Node system
- [x] Transform component
- [x] Scene management

### Phase 2: Rendering
- [ ] SDL2 integration
- [ ] Sprite rendering
- [ ] Camera2D

### Phase 3: Scripting
- [ ] Lua binding
- [ ] Script component

### Phase 4: UI
- [ ] Button
- [ ] Label
- [ ] Image
- [ ] Panel

### Phase 5: Events
- [ ] Signal system
- [ ] Event callbacks

### Phase 6: Physics
- [ ] Box2D integration
- [ ] Collision detection

### Phase 7: Editor
- [ ] Mobile editor UI
- [ ] Scene editing
- [ ] Property inspector

### Phase 8: Export
- [ ] APK builder
- [ ] Project export

## Contributing

This project follows the SPEC.md specification. All changes must align with the defined phases and technology stack.

## License

MIT License
