/**
 * @file tilemap.c
 * @brief Tilemap System Implementation
 */

#include "tilemap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Tilemap* tilemap_create(int width, int height, int tile_width, int tile_height) {
    Tilemap* map = (Tilemap*)calloc(1, sizeof(Tilemap));
    if (!map) return NULL;
    
    map->map_width = width;
    map->map_height = height;
    map->tile_width = (float)tile_width;
    map->tile_height = (float)tile_height;
    map->layer_count = 0;
    map->layers = NULL;
    
    // Default tileset
    map->tileset.tile_width = tile_width;
    map->tileset.tile_height = tile_height;
    map->tileset.tile_count = 6;
    
    // Default tile definitions
    map->tileset.tiles[0].id = TILE_EMPTY;  map->tileset.tiles[0].solid = false;
    map->tileset.tiles[1].id = TILE_GROUND; map->tileset.tiles[1].solid = true;
    map->tileset.tiles[2].id = TILE_WALL;  map->tileset.tiles[2].solid = true;
    map->tileset.tiles[3].id = TILE_WATER; map->tileset.tiles[3].solid = false;
    map->tileset.tiles[4].id = TILE_LAVA;  map->tileset.tiles[4].solid = false;
    map->tileset.tiles[5].id = TILE_GRASS; map->tileset.tiles[5].solid = false;
    
    printf("[Tilemap] Created %dx%d map\n", width, height);
    return map;
}

void tilemap_free(Tilemap* map) {
    if (!map) return;
    
    for (int i = 0; i < map->layer_count; i++) {
        if (map->layers[i].data) {
            free(map->layers[i].data);
        }
    }
    
    if (map->layers) free(map->layers);
    free(map);
}

TilemapLayer* tilemap_add_layer(Tilemap* map, int width, int height) {
    if (!map) return NULL;
    
    map->layer_count++;
    map->layers = (TilemapLayer*)realloc(map->layers, sizeof(TilemapLayer) * map->layer_count);
    
    TilemapLayer* layer = &map->layers[map->layer_count - 1];
    layer->data = (int*)calloc(width * height, sizeof(int));
    layer->width = width;
    layer->height = height;
    layer->visible = true;
    layer->parallax_x = 1.0f;
    layer->parallax_y = 1.0f;
    
    printf("[Tilemap] Added layer %d (%dx%d)\n", map->layer_count - 1, width, height);
    return layer;
}

void tilemap_set_tile(Tilemap* map, int layer, int x, int y, int tile_id) {
    if (!map || layer < 0 || layer >= map->layer_count) return;
    TilemapLayer* l = &map->layers[layer];
    if (x < 0 || x >= l->width || y < 0 || y >= l->height) return;
    
    l->data[y * l->width + x] = tile_id;
}

int tilemap_get_tile(Tilemap* map, int layer, int x, int y) {
    if (!map || layer < 0 || layer >= map->layer_count) return TILE_EMPTY;
    TilemapLayer* l = &map->layers[layer];
    if (x < 0 || x >= l->width || y < 0 || y >= l->height) return TILE_EMPTY;
    
    return l->data[y * l->width + x];
}

int tilemap_get_tile_at(Tilemap* map, int layer, float world_x, float world_y) {
    int tile_x = (int)(world_x / map->tile_width);
    int tile_y = (int)(world_y / map->tile_height);
    return tilemap_get_tile(map, layer, tile_x, tile_y);
}

bool tilemap_is_solid(Tilemap* map, int layer, int tile_x, int tile_y) {
    int tile_id = tilemap_get_tile(map, layer, tile_x, tile_y);
    if (tile_id <= 0 || tile_id >= map->tileset.tile_count) return false;
    return map->tileset.tiles[tile_id].solid;
}

bool tilemap_is_solid_at(Tilemap* map, int layer, float world_x, float world_y) {
    int tile_x = (int)(world_x / map->tile_width);
    int tile_y = (int)(world_y / map->tile_height);
    return tilemap_is_solid(map, layer, tile_x, tile_y);
}

void tilemap_load_layer_data(Tilemap* map, int layer_index, const int* data, int width, int height) {
    if (!map || layer_index < 0 || layer_index >= map->layer_count) return;
    
    TilemapLayer* layer = &map->layers[layer_index];
    if (layer->data) free(layer->data);
    
    layer->data = (int*)malloc(sizeof(int) * width * height);
    memcpy(layer->data, data, sizeof(int) * width * height);
    layer->width = width;
    layer->height = height;
}

void tilemap_generate_level(Tilemap* map, int layer, int seed) {
    if (!map || layer < 0 || layer >= map->layer_count) return;
    
    TilemapLayer* l = &map->layers[layer];
    
    // Simple cave generation
    srand(seed);
    
    for (int y = 0; y < l->height; y++) {
        for (int x = 0; x < l->width; x++) {
            // Border walls
            if (x == 0 || y == 0 || x == l->width - 1 || y == l->height - 1) {
                l->data[y * l->width + x] = TILE_WALL;
            }
            // Random ground/wall
            else if ((rand() % 10) < 4) {
                l->data[y * l->width + x] = TILE_WALL;
            }
            else {
                l->data[y * l->width + x] = TILE_GROUND;
            }
        }
    }
    
    printf("[Tilemap] Generated level (seed=%d)\n", seed);
}

void tilemap_render(Tilemap* map, void* renderer, void* camera) {
    (void)renderer;
    (void)camera;
    // Stub: actual rendering would use SDL_RenderCopy with camera transforms
    // This shows the basic structure
}

Vector2 tilemap_tile_to_world(Tilemap* map, int tile_x, int tile_y) {
    return vector2_new((float)tile_x * map->tile_width, (float)tile_y * map->tile_height);
}

void tilemap_world_to_tile(Tilemap* map, float world_x, float world_y, int* tile_x, int* tile_y) {
    if (tile_x) *tile_x = (int)(world_x / map->tile_width);
    if (tile_y) *tile_y = (int)(world_y / map->tile_height);
}

bool tilemap_resize(Tilemap* map, int new_width, int new_height) {
    if (!map) return false;
    
    for (int i = 0; i < map->layer_count; i++) {
        int* new_data = (int*)calloc(new_width * new_height, sizeof(int));
        TilemapLayer* l = &map->layers[i];
        
        // Copy old data
        int copy_w = new_width < l->width ? new_width : l->width;
        int copy_h = new_height < l->height ? new_height : l->height;
        
        for (int y = 0; y < copy_h; y++) {
            for (int x = 0; x < copy_w; x++) {
                new_data[y * new_width + x] = l->data[y * l->width + x];
            }
        }
        
        free(l->data);
        l->data = new_data;
        l->width = new_width;
        l->height = new_height;
    }
    
    map->map_width = new_width;
    map->map_height = new_height;
    
    return true;
}

void tilemap_clear(Tilemap* map, int layer) {
    if (!map || layer < 0 || layer >= map->layer_count) return;
    TilemapLayer* l = &map->layers[layer];
    memset(l->data, 0, sizeof(int) * l->width * l->height);
}
