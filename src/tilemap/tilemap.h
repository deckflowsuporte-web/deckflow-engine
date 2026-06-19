/**
 * @file tilemap.h
 * @brief Tilemap System - Tile-based map rendering
 */

#ifndef DECKFLOW_TILEMAP_H
#define DECKFLOW_TILEMAP_H

#include <stdbool.h>
#include "../math/vector2.h"

// Tile types
#define TILE_EMPTY 0
#define TILE_GROUND 1
#define TILE_WALL 2
#define TILE_WATER 3
#define TILE_LAVA 4
#define TILE_GRASS 5

// Maximum tileset size
#define MAX_TILESET_TILES 256

// Tile definition
typedef struct {
    int id;              // Tile ID
    int x, y;           // Position in tileset (if using spritesheet)
    bool solid;         // Collision enabled
    bool animated;      // Is animated tile
    float animation_speed;
    int frame_count;
} TileDef;

// Tileset
typedef struct {
    TileDef tiles[MAX_TILESET_TILES];
    int tile_count;
    int tile_width;
    int tile_height;
    void* texture;      // SDL_Texture* or similar
} Tileset;

// Tilemap layer
typedef struct {
    int* data;          // Tile IDs
    int width;          // Map width in tiles
    int height;         // Map height in tiles
    bool visible;
    float parallax_x;   // Parallax scroll factor
    float parallax_y;
} TilemapLayer;

// Tilemap
typedef struct {
    TilemapLayer* layers;
    int layer_count;
    Tileset tileset;
    int map_width;      // In tiles
    int map_height;     // In tiles
    float tile_width;   // In pixels
    float tile_height;  // In pixels
} Tilemap;

// Create tilemap
Tilemap* tilemap_create(int width, int height, int tile_width, int tile_height);

// Free tilemap
void tilemap_free(Tilemap* map);

// Add layer
TilemapLayer* tilemap_add_layer(Tilemap* map, int width, int height);

// Set tile
void tilemap_set_tile(Tilemap* map, int layer, int x, int y, int tile_id);

// Get tile
int tilemap_get_tile(Tilemap* map, int layer, int x, int y);

// Get tile at world position
int tilemap_get_tile_at(Tilemap* map, int layer, float world_x, float world_y);

// Check if position is solid
bool tilemap_is_solid(Tilemap* map, int layer, int tile_x, int tile_y);
bool tilemap_is_solid_at(Tilemap* map, int layer, float world_x, float world_y);

// Load tilemap from array
void tilemap_load_layer_data(Tilemap* map, int layer, const int* data, int width, int height);

// Generate simple level (for testing)
void tilemap_generate_level(Tilemap* map, int layer, int seed);

// Render tilemap
void tilemap_render(Tilemap* map, void* renderer, void* camera);

// Get tile world position
Vector2 tilemap_tile_to_world(Tilemap* map, int tile_x, int tile_y);

// Get world position to tile
void tilemap_world_to_tile(Tilemap* map, float world_x, float world_y, int* tile_x, int* tile_y);

// Resize tilemap
bool tilemap_resize(Tilemap* map, int new_width, int new_height);

// Clear tilemap
void tilemap_clear(Tilemap* map, int layer);

#endif // DECKFLOW_TILEMAP_H
