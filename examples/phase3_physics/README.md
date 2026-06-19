# Phase 3: Box2D Physics Integration

## Build Instructions (Linux/macOS)

This example requires Box2D and SDL2.

### Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
# Box2D needs to be built from source (see below)
```

**macOS (Homebrew):**
```bash
brew install sdl2 box2d
```

### Build Box2D (if not installed)

```bash
git clone --depth 1 --branch v2.4.1 https://github.com/erincatto/box2d.git
cd box2d
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
sudo make install
```

### Build and Run

```bash
cd examples/phase3_physics
# Edit Makefile to point to your SDL2 and Box2D paths
mkdir build && cd build
cmake ..
make
./phase3_physics
```

### Controls
- **Arrow keys**: Move player
- **Q/E**: Rotate player  
- **SPACE**: Drop new ball
- **R**: Reset positions
- **Click**: Apply force to nearby bodies
- **ESC**: Exit

## Features Demonstrated
- Box2D world with gravity (0, -10 m/s²)
- Static bodies (ground, platforms, walls)
- Dynamic bodies (balls, boxes)
- Circle and polygon collision shapes
- Ray casting for mouse interaction
- Body manipulation (apply force, reset position)

## Physics Scene
```
     [Platform 2]           [Platform 3]
        █████               ████████
        
    ●      ●            ■       ■
    ●      ●         ■       ■
    
═══════════════════════════════════════
         [Ground Platform]
```
