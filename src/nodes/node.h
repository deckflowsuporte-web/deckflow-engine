/**
 * @file node.h
 * @brief Base Node class - Godot-style entity system
 * 
 * Everything in the engine is a Node.
 * Nodes are organized in a tree structure (Scene Tree).
 */

#ifndef NODE_H
#define NODE_H

#include <stdbool.h>
#include <stdint.h>
#include "math/transform2d.h"

// Forward declarations
typedef struct Scene Scene;
typedef struct Node Node;

// Node type identifiers
typedef enum NodeType {
    NODE_TYPE_GENERIC = 0,
    // 2D Nodes
    NODE_TYPE_SPATIAL_2D,
    NODE_TYPE_SPRITE,
    NODE_TYPE_CAMERA_2D,
    NODE_TYPE_RIGID_BODY_2D,
    NODE_TYPE_COLLISION_SHAPE_2D,
    NODE_TYPE_TILE_MAP,
    // UI Nodes
    NODE_TYPE_CONTROL,
    NODE_TYPE_BUTTON,
    NODE_TYPE_LABEL,
    NODE_TYPE_TEXTURE_RECT,
    NODE_TYPE_PANEL,
    // Effects
    NODE_TYPE_ANIMATION_PLAYER,
    NODE_TYPE_AUDIO_STREAM_PLAYER,
    NODE_TYPE_PARTICLE_EMITTER,
    // Script
    NODE_TYPE_SCRIPT,
    // Max types
    NODE_TYPE_COUNT
} NodeType;

// Node flags
typedef enum NodeFlags {
    NODE_FLAG_NONE = 0,
    NODE_FLAG_PAUSED = 1 << 0,
    NODE_FLAG_PROCESS_PAUSED = 1 << 1,
    NODE_FLAG_PHYSICS_PAUSED = 1 << 2,
    NODE_FLAG_EDITOR_ONLY = 1 << 3,
    NODE_FLAG_REPLICATE = 1 << 4,
} NodeFlags;

// Process callbacks
typedef enum ProcessMode {
    PROCESS_MODE_INHERIT,      // Inherit from parent
    PROCESS_MODE_DISABLED,     // Never process
    PROCESS_MODE_ALWAYS,       // Process always
    PROCESS_MODE_WHEN_PAUSED,  // Process even when paused
} ProcessMode;

// Node lifecycle callbacks
typedef void (*NodeCallback)(Node* node);
typedef void (*NodeProcessCallback)(Node* node, float delta);
typedef void (*NodePhysicsCallback)(Node* node, float delta);
typedef void (*NodeInputCallback)(Node* node, int input_type, void* data);

// Node structure
struct Node {
    // Identity
    char name[64];
    uint32_t id;
    NodeType type;
    NodeFlags flags;
    
    // Tree structure
    Node* parent;
    Node* first_child;
    Node* last_child;
    Node* next_sibling;
    Node* prev_sibling;
    int child_count;
    
    // Transform
    Transform2D transform;
    int z_index;
    bool visible;
    
    // Processing
    ProcessMode process_mode;
    bool process_enabled;
    bool physics_process_enabled;
    bool input_enabled;
    
    // Callbacks
    NodeCallback on_ready;
    NodeCallback on_enter_tree;
    NodeCallback on_exit_tree;
    NodeCallback on_destroy;
    NodeProcessCallback on_process;
    NodePhysicsCallback on_physics_process;
    NodeInputCallback on_input;
    
    // User data
    void* user_data;
    void (*user_data_destructor)(void*);
};

// Node lifecycle
Node* node_create(const char* name, NodeType type);
void node_destroy(Node* node);
void node_free(Node* node);

// Tree operations
void node_add_child(Node* parent, Node* child);
void node_add_child_below(Node* parent, Node* child, Node* below);
void node_remove_child(Node* parent, Node* child);
void node_remove_and_free_child(Node* parent, Node* child);
void node_remove_all_children(Node* node);
void node_replace_by(Node* node, Node* replacement);

Node* node_get_child(Node* node, int index);
Node* node_find_child(Node* node, const char* name, bool recursive);
int node_get_child_count(Node* node);
bool node_is_ancestor_of(Node* ancestor, Node* node);

Node* node_get_next_sibling(Node* node);
Node* node_get_prev_sibling(Node* node);
Node* node_get_parent(Node* node);
Node* node_get_root(Node* node);

// Scene operations
void node_attach_to_scene(Node* node, Scene* scene);
Scene* node_get_scene(Node* node);

// Transform
Vector2 node_get_global_position(Node* node);
float node_get_global_rotation(Node* node);
Vector2 node_get_global_scale(Node* node);
Transform2D node_get_global_transform(Node* node);
void node_set_global_position(Node* node, Vector2 pos);
void node_set_global_rotation(Node* node, float rotation);

// Processing
void node_set_process(Node* node, bool enabled);
void node_set_physics_process(Node* node, bool enabled);
void node_set_process_priority(Node* node, int priority);
void node_set_input_as_handled(Node* node);

bool node_can_process(Node* node);

// Utilities
const char* node_get_type_name(NodeType type);
const char* node_get_name(Node* node);
void node_set_name(Node* node, const char* name);
NodeType node_get_type(Node* node);
bool node_has_method(Node* node);

// Signal system (basic)
void node_emit_signal(Node* node, const char* signal_name);

#endif // NODE_H
