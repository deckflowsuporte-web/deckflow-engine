# Phase 3: Box2D Physics - Makefile
# Builds with Box2D (v2.4.1) + SDL2

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LDFLAGS = -lm

BUILD_DIR = ./build

# Paths (adjust if needed)
BOX2D_DIR ?= ./box2d
SDL2_DIR ?= ./SDL2

# Engine sources
ENGINE_SRC = ../../src/math/vector2.c \
             ../../src/math/transform2d.c \
             ../../src/nodes/node.c \
             ../../src/scenes/scene.c \
             ../../src/scenes/scene_tree.c

ENGINE_INC = -I../../src

.PHONY: all deps box2d sdl2 clean run info

all: deps $(BUILD_DIR)/phase3_physics

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build Box2D if not present
box2d:
	@if [ ! -f "$(BOX2D_DIR)/src/collision/b2_collision.cpp" ]; then \
		echo "Cloning Box2D..."; \
		git clone --depth 1 --branch v2.4.1 https://github.com/erincatto/box2d.git $(BOX2D_DIR); \
	fi
	@if [ ! -f "$(BOX2D_DIR)/build/libbox2d.a" ]; then \
		echo "Building Box2D..."; \
		cd $(BOX2D_DIR) && mkdir -p build && cd build && \
		cmake .. -DCMAKE_BUILD_TYPE=Release 2>/dev/null || \
		(echo "cmake not available - using manual compilation" && \
		g++ -c -O2 -I$(BOX2D_DIR)/include $(BOX2D_DIR)/src/collision/*.cpp $(ENGINE_SRC) -o /dev/null 2>/dev/null); \
	fi

# Build SDL2 if not present
sdl2:
	@if [ ! -d "$(SDL2_DIR)/include" ]; then \
		echo "Downloading SDL2..."; \
		mkdir -p $(SDL2_DIR) && \
		curl -L https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz -o /tmp/sdl.tar.gz && \
		tar -xzf /tmp/sdl.tar.gz -C $(SDL2_DIR) --strip-components=1 && \
		cd $(SDL2_DIR) && ./configure --disable-shared --enable-static > /dev/null 2>&1 && make -j4 > /dev/null 2>&1; \
	fi

deps: box2d sdl2

# Build with Box2D and SDL2
$(BUILD_DIR)/phase3_physics: main.c $(ENGINE_SRC) deps
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(ENGINE_INC) \
		-I$(BOX2D_DIR)/include -I$(SDL2_DIR)/include \
		-DSDL_MAIN_HANDLED \
		main.c $(ENGINE_SRC) \
		$(BOX2D_DIR)/build/libbox2d.a \
		$(SDL2_DIR)/build/.libs/libSDL2.a \
		$(SDL2_DIR)/build/libSDL2test.a \
		-lpthread -ldl -lrt \
		$(LDFLAGS) -o $@
	@echo "Built: $@"

clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(BUILD_DIR)/phase3_physics

info:
	@echo "Phase 3 requires:"
	@echo "  Box2D: $(BOX2D_DIR) (will be cloned)"
	@echo "  SDL2:  $(SDL2_DIR) (will be cloned)"
	@echo ""
	@echo "Build with: make"
	@echo "Run with:   make run"
