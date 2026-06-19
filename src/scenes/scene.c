/**
 * @file scene.c
 * @brief Scene implementation
 */

#include "scene.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Scene* scene_create(const char* name) {
    Scene* scene = (Scene*)malloc(sizeof(Scene));
    if (!scene) return NULL;
    
    memset(scene, 0, sizeof(Scene));
    
    // Set name
    if (name && name[0]) {
        strncpy(scene->name, name, sizeof(scene->name) - 1);
    } else {
        strcpy(scene->name, "Scene");
    }
    
    // Create root node
    scene->root = node_create(".", NODE_TYPE_GENERIC);
    
    // Defaults
    scene->physics_enabled = true;
    scene->physics_fps = 60.0f;
    scene->editable = true;
    
    return scene;
}

void scene_destroy(Scene* scene) {
    if (!scene) return;
    
    // Note: doesn't free root - use scene_free() for that
    (void)scene;
}

void scene_free(Scene* scene) {
    if (!scene) return;
    
    if (scene->root) {
        node_free(scene->root);
    }
    
    free(scene);
}

Node* scene_get_root(Scene* scene) {
    return scene ? scene->root : NULL;
}

void scene_add_root_child(Scene* scene, Node* child) {
    if (!scene || !scene->root || !child) return;
    node_add_child(scene->root, child);
}

void scene_remove_root_child(Scene* scene, Node* child) {
    if (!scene || !scene->root || !child) return;
    node_remove_child(scene->root, child);
}

void scene_set_paused(Scene* scene, bool paused) {
    if (scene) scene->paused = paused;
}

bool scene_is_paused(Scene* scene) {
    return scene ? (scene->paused || scene->pause_depth > 0) : true;
}

void scene_push_pause(Scene* scene) {
    if (scene) scene->pause_depth++;
}

void scene_pop_pause(Scene* scene) {
    if (scene && scene->pause_depth > 0) {
        scene->pause_depth--;
    }
}

void scene_set_physics_enabled(Scene* scene, bool enabled) {
    if (scene) scene->physics_enabled = enabled;
}

bool scene_is_physics_enabled(Scene* scene) {
    return scene ? scene->physics_enabled : false;
}

void scene_set_physics_fps(Scene* scene, float fps) {
    if (scene) scene->physics_fps = fps;
}

// Recursive process function
static void process_node(Node* node, float delta) {
    if (!node) return;
    
    // Call process callback if enabled
    if (node_can_process(node) && node->on_process) {
        node->on_process(node, delta);
    }
    
    // Process children
    Node* child = node->first_child;
    while (child) {
        process_node(child, delta);
        child = child->next_sibling;
    }
}

void scene_process(Scene* scene, float delta) {
    if (!scene) return;
    if (scene_is_paused(scene)) return;
    
    process_node(scene->root, delta);
}

// Recursive physics process
static void physics_process_node(Node* node, float delta) {
    if (!node) return;
    
    if (node_can_process(node) && node->physics_process_enabled && node->on_physics_process) {
        node->on_physics_process(node, delta);
    }
    
    Node* child = node->first_child;
    while (child) {
        physics_process_node(child, delta);
        child = child->next_sibling;
    }
}

void scene_physics_process(Scene* scene, float delta) {
    if (!scene) return;
    if (!scene_is_physics_enabled(scene)) return;
    if (scene_is_paused(scene)) return;
    
    physics_process_node(scene->root, delta);
}

void scene_input(Scene* scene, int input_type, void* data) {
    if (!scene) return;
    
    // Propagate input to all nodes
    Node* child = scene->root ? scene->root->first_child : NULL;
    while (child) {
        if (child->input_enabled && child->on_input) {
            child->on_input(child, input_type, data);
        }
        child = child->next_sibling;
    }
}

Node* scene_find_node(Scene* scene, const char* name) {
    if (!scene || !scene->root || !name) return NULL;
    return node_find_child(scene->root, name, true);
}

Node* scene_get_node_by_id(Scene* scene, uint32_t id) {
    if (!scene || !scene->root) return NULL;
    
    // Simple recursive search
    static Node* (*find_by_id)(Node*, uint32_t) = NULL;
    
    Node* root = scene->root;
    Node* child = root->first_child;
    
    while (child) {
        if (child->id == id) return child;
        
        // Search children
        Node* found = node_find_child(child, "", false); // Will implement properly later
        if (found && found->id == id) return found;
        
        child = child->next_sibling;
    }
    
    return NULL;
}

void scene_set_camera(Scene* scene, Node* camera) {
    if (scene) scene->camera = camera;
}

Node* scene_get_camera(Scene* scene) {
    return scene ? scene->camera : NULL;
}

bool scene_save(Scene* scene, const char* file_path) {
    if (!scene || !file_path) return false;
    
    // TODO: JSON serialization
    strncpy(scene->file_path, file_path, sizeof(scene->file_path) - 1);
    return true;
}

bool scene_load(Scene* scene, const char* file_path) {
    if (!scene || !file_path) return false;
    
    // TODO: JSON deserialization
    return true;
}

Scene* scene_instance(const char* file_path) {
    // Create new scene from file
    Scene* scene = scene_create("instance");
    if (scene) {
        scene_load(scene, file_path);
    }
    return scene;
}

void scene_print_tree(Scene* scene) {
    if (!scene || !scene->root) return;
    
    printf("Scene: %s\n", scene->name);
    printf("{\n");
    
    Node* child = scene->root->first_child;
    while (child) {
        printf("  - %s [%s]\n", child->name, node_get_type_name(child->type));
        child = child->next_sibling;
    }
    
    printf("}\n");
}
