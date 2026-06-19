# Phase 2: SDL2 + Sprite Rendering

## Build Instructions (Linux/macOS)

This example requires SDL2 to be installed on your system.

### Install SDL2

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**macOS (Homebrew):**
```bash
brew install sdl2
```

### Build and Run

```bash
cd examples/phase2_sprite
mkdir build && cd build
cmake ..
make
./phase2_sprite
```

### Controls
- **Arrow keys**: Move player
- **Q/E**: Rotate player
- **ESC**: Exit

## Features Demonstrated
- SDL2 window and renderer setup
- Sprite creation and rendering
- Scene tree with visual output
- Node hierarchy with transforms
