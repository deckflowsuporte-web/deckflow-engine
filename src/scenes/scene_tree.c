/**
 * @file scene_tree.c
 * @brief Scene Tree utilities implementation
 */

#include "scene_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void scene_tree_iterate(Scene* scene, SceneTreeIteratorCallback callback, void* user_data) {
    if (!scene || !scene->root || !callback) return;
    
    Node* child = scene->root->first_child;
    while (child) {
        scene_tree_iterate_node(child, 0, callback, user_data);
        child = child->next_sibling;
    }
}

void scene_tree_iterate_node(Node* node, int depth, SceneTreeIteratorCallback callback, void* user_data) {
    if (!node || !callback) return;
    
    callback(node, depth, user_data);
    
    Node* child = node->first_child;
    while (child) {
        scene_tree_iterate_node(child, depth + 1, callback, user_data);
        child = child->next_sibling;
    }
}

Node* scene_tree_get_node_at_path(Scene* scene, const char* path) {
    if (!scene || !scene->root || !path) return NULL;
    return node_get_node_at_path(scene->root, path);
}

Node* node_get_node_at_path(Node* node, const char* path) {
    if (!node || !path) return NULL;
    
    // Find first component
    const char* sep = strchr(path, '/');
    size_t len = sep ? (size_t)(sep - path) : strlen(path);
    
    // Find matching child
    Node* child = node->first_child;
    while (child) {
        if (strncmp(child->name, path, len) == 0 && strlen(child->name) == len) {
            if (!sep) return child;
            return node_get_node_at_path(child, sep + 1);
        }
        child = child->next_sibling;
    }
    
    return NULL;
}

char* node_get_path(Node* node) {
    if (!node) return NULL;
    
    // Calculate required size
    int size = 256;
    char* path = (char*)malloc(size);
    path[0] = '\0';
    
    Node* current = node;
    Node* parent = node->parent;
    
    while (parent) {
        // Prepend node name
        char temp[128];
        snprintf(temp, sizeof(temp), "/%s%s", current->name, path);
        strncpy(path, temp, size - 1);
        
        current = parent;
        parent = parent->parent;
    }
    
    return path;
}

char* node_get_name_to_parent(Node* node) {
    return node ? node->name : NULL;
}

// Counter structure
typedef struct {
    int count;
    NodeType type;
    bool match_type;
} CountData;

static void count_callback(Node* node, int depth, void* user_data) {
    CountData* data = (CountData*)user_data;
    if (!data->match_type || node->type == data->type) {
        data->count++;
    }
}

int scene_tree_count_nodes(Scene* scene) {
    CountData data = {0, NODE_TYPE_GENERIC, false};
    scene_tree_iterate(scene, count_callback, &data);
    return data.count;
}

int scene_tree_count_nodes_of_type(Scene* scene, NodeType type) {
    CountData data = {0, type, true};
    scene_tree_iterate(scene, count_callback, &data);
    return data.count;
}
