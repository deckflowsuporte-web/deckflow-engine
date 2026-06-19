/**
 * @file scene.h
 * @brief Scene - Container for all game content
 * 
 * A Scene is a tree of Nodes. Scenes can be saved/loaded
 * and instanced into other scenes.
 */

#ifndef SCENE_H
#define SCENE_H

#include <stdbool.h>
#include "nodes/node.h"

// Forward declarations
typedef struct SceneState SceneState;

// Scene structure
typedef struct Scene {
    // Scene info
    char name[64];
    char file_path[256];
    
    // Root node
    Node* root;
    
    // Current state
    SceneState* state;
    
    // Physics
    bool physics_enabled;
    float physics_fps;
    
    // Pause
    bool paused;
    int pause_depth;
    
    // Built-in nodes
    Node* camera;
    
    // Meta
    bool editable;
    bool unique_instance;
    
} Scene;

// Scene lifecycle
Scene* scene_create(const char* name);
void scene_destroy(Scene* scene);
void scene_free(Scene* scene);

// Scene tree
Node* scene_get_root(Scene* scene);
void scene_add_root_child(Scene* scene, Node* child);
void scene_remove_root_child(Scene* scene, Node* child);

// Scene management
void scene_set_paused(Scene* scene, bool paused);
bool scene_is_paused(Scene* scene);
void scene_push_pause(Scene* scene);
void scene_pop_pause(Scene* scene);

// Physics
void scene_set_physics_enabled(Scene* scene, bool enabled);
bool scene_is_physics_enabled(Scene* scene);
void scene_set_physics_fps(Scene* scene, float fps);

// Processing
void scene_process(Scene* scene, float delta);
void scene_physics_process(Scene* scene, float delta);
void scene_input(Scene* scene, int input_type, void* data);

// Node finding
Node* scene_find_node(Scene* scene, const char* name);
Node* scene_get_node_by_id(Scene* scene, uint32_t id);

// Camera
void scene_set_camera(Scene* scene, Node* camera);
Node* scene_get_camera(Scene* scene);

// File operations
bool scene_save(Scene* scene, const char* file_path);
bool scene_load(Scene* scene, const char* file_path);
Scene* scene_instance(const char* file_path);

// Debug
void scene_print_tree(Scene* scene);

#endif // SCENE_H
