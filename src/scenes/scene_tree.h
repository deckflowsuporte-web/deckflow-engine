/**
 * @file scene_tree.h
 * @brief Scene Tree utilities
 */

#ifndef SCENE_TREE_H
#define SCENE_TREE_H

#include "scene.h"
#include "nodes/node.h"

// Iterator callback
typedef void (*SceneTreeIteratorCallback)(Node* node, int depth, void* user_data);

// Iterate all nodes in a scene
void scene_tree_iterate(Scene* scene, SceneTreeIteratorCallback callback, void* user_data);
void scene_tree_iterate_node(Node* node, int depth, SceneTreeIteratorCallback callback, void* user_data);

// Get node at path (e.g., "Player/Sprite2D")
Node* scene_tree_get_node_at_path(Scene* scene, const char* path);
Node* node_get_node_at_path(Node* node, const char* path);

// Node path utilities
char* node_get_path(Node* node);
char* node_get_name_to_parent(Node* node);

// Count utilities
int scene_tree_count_nodes(Scene* scene);
int scene_tree_count_nodes_of_type(Scene* scene, NodeType type);

#endif // SCENE_TREE_H
