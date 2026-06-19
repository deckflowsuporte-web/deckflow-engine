/**
 * @file node.c
 * @brief Node implementation
 */

#include "node.h"
#include "scenes/scene.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global node ID counter
static uint32_t g_node_id_counter = 1;

Node* node_create(const char* name, NodeType type) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (!node) return NULL;
    
    memset(node, 0, sizeof(Node));
    
    // Set name
    if (name && name[0]) {
        strncpy(node->name, name, sizeof(node->name) - 1);
    } else {
        snprintf(node->name, sizeof(node->name), "Node_%u", g_node_id_counter);
    }
    
    node->id = g_node_id_counter++;
    node->type = type;
    
    // Default values
    node->transform = transform2d_new();
    node->z_index = 0;
    node->visible = true;
    node->process_enabled = true;
    node->physics_process_enabled = false;
    node->input_enabled = false;
    node->process_mode = PROCESS_MODE_INHERIT;
    
    return node;
}

void node_destroy(Node* node) {
    if (!node) return;
    
    // Call destroy callback
    if (node->on_destroy) {
        node->on_destroy(node);
    }
    
    // Free user data
    if (node->user_data && node->user_data_destructor) {
        node->user_data_destructor(node->user_data);
    }
    
    // Note: doesn't free children - use node_free() for that
}

void node_free(Node* node) {
    if (!node) return;
    
    // Free all children first
    Node* child = node->first_child;
    while (child) {
        Node* next = child->next_sibling;
        node_free(child);
        child = next;
    }
    
    node_destroy(node);
    free(node);
}

// Tree operations
void node_add_child(Node* parent, Node* child) {
    if (!parent || !child) return;
    
    // Remove from previous parent
    if (child->parent) {
        node_remove_child(child->parent, child);
    }
    
    child->parent = parent;
    
    // Add to end of children list
    if (parent->last_child) {
        parent->last_child->next_sibling = child;
        child->prev_sibling = parent->last_child;
        parent->last_child = child;
    } else {
        parent->first_child = child;
        parent->last_child = child;
    }
    
    parent->child_count++;
}

void node_add_child_below(Node* parent, Node* child, Node* below) {
    if (!parent || !child) return;
    
    if (child->parent) {
        node_remove_child(child->parent, child);
    }
    
    child->parent = parent;
    
    if (below == NULL) {
        // Add at beginning
        if (parent->first_child) {
            parent->first_child->prev_sibling = child;
            child->next_sibling = parent->first_child;
            parent->first_child = child;
        } else {
            parent->first_child = child;
            parent->last_child = child;
        }
    } else {
        // Add after 'below'
        child->prev_sibling = below;
        child->next_sibling = below->next_sibling;
        
        if (below->next_sibling) {
            below->next_sibling->prev_sibling = child;
        } else {
            parent->last_child = child;
        }
        below->next_sibling = child;
    }
    
    parent->child_count++;
}

void node_remove_child(Node* parent, Node* child) {
    if (!parent || !child) return;
    
    if (child->parent != parent) return;
    
    // Update siblings
    if (child->prev_sibling) {
        child->prev_sibling->next_sibling = child->next_sibling;
    } else {
        parent->first_child = child->next_sibling;
    }
    
    if (child->next_sibling) {
        child->next_sibling->prev_sibling = child->prev_sibling;
    } else {
        parent->last_child = child->prev_sibling;
    }
    
    parent->child_count--;
    child->parent = NULL;
    child->prev_sibling = NULL;
    child->next_sibling = NULL;
}

void node_remove_and_free_child(Node* parent, Node* child) {
    if (!parent || !child) return;
    node_remove_child(parent, child);
    node_free(child);
}

void node_remove_all_children(Node* node) {
    if (!node) return;
    
    Node* child = node->first_child;
    while (child) {
        Node* next = child->next_sibling;
        node_free(child);
        child = next;
    }
    
    node->first_child = NULL;
    node->last_child = NULL;
    node->child_count = 0;
}

Node* node_get_child(Node* node, int index) {
    if (!node || index < 0) return NULL;
    
    Node* child = node->first_child;
    int i = 0;
    
    while (child && i < index) {
        child = child->next_sibling;
        i++;
    }
    
    return child;
}

Node* node_find_child(Node* node, const char* name, bool recursive) {
    if (!node || !name) return NULL;
    
    Node* child = node->first_child;
    
    while (child) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        
        if (recursive) {
            Node* found = node_find_child(child, name, true);
            if (found) return found;
        }
        
        child = child->next_sibling;
    }
    
    return NULL;
}

int node_get_child_count(Node* node) {
    return node ? node->child_count : 0;
}

bool node_is_ancestor_of(Node* ancestor, Node* node) {
    if (!ancestor || !node) return false;
    
    Node* parent = node->parent;
    while (parent) {
        if (parent == ancestor) return true;
        parent = parent->parent;
    }
    
    return false;
}

Node* node_get_next_sibling(Node* node) {
    return node ? node->next_sibling : NULL;
}

Node* node_get_prev_sibling(Node* node) {
    return node ? node->prev_sibling : NULL;
}

Node* node_get_parent(Node* node) {
    return node ? node->parent : NULL;
}

Node* node_get_root(Node* node) {
    if (!node) return NULL;
    
    Node* root = node;
    while (root->parent) {
        root = root->parent;
    }
    
    return root;
}

// Transform helpers
Vector2 node_get_global_position(Node* node) {
    if (!node) return vector2_zero();
    
    if (node->parent) {
        Transform2D parent_xform = node_get_global_transform(node->parent);
        return transform2d_xform_vector2(parent_xform, node->transform.position);
    }
    
    return node->transform.position;
}

float node_get_global_rotation(Node* node) {
    if (!node) return 0;
    
    float rotation = node->transform.rotation;
    Node* parent = node->parent;
    
    while (parent) {
        rotation += parent->transform.rotation;
        parent = parent->parent;
    }
    
    return rotation;
}

Vector2 node_get_global_scale(Node* node) {
    if (!node) return vector2_one();
    
    Vector2 scale = node->transform.scale;
    Node* parent = node->parent;
    
    while (parent) {
        scale.x *= parent->transform.scale.x;
        scale.y *= parent->transform.scale.y;
        parent = parent->parent;
    }
    
    return scale;
}

Transform2D node_get_global_transform(Node* node) {
    if (!node) return transform2d_identity();
    
    if (node->parent) {
        Transform2D parent_xform = node_get_global_transform(node->parent);
        Transform2D local = node->transform;
        
        Transform2D result;
        result.position = transform2d_xform_vector2(parent_xform, local.position);
        result.rotation = parent_xform.rotation + local.rotation;
        result.scale.x = parent_xform.scale.x * local.scale.x;
        result.scale.y = parent_xform.scale.y * local.scale.y;
        result.origin = vector2_zero();
        result.dirty = true;
        
        return result;
    }
    
    return node->transform;
}

void node_set_global_position(Node* node, Vector2 pos) {
    if (!node) return;
    
    if (node->parent) {
        Transform2D parent_inv = transform2d_inverted(node_get_global_transform(node->parent));
        node->transform.position = transform2d_xform_vector2(parent_inv, pos);
    } else {
        node->transform.position = pos;
    }
}

void node_set_global_rotation(Node* node, float rotation) {
    if (!node) return;
    
    if (node->parent) {
        float parent_rot = node_get_global_rotation(node->parent);
        node->transform.rotation = rotation - parent_rot;
    } else {
        node->transform.rotation = rotation;
    }
}

// Processing
void node_set_process(Node* node, bool enabled) {
    if (node) node->process_enabled = enabled;
}

void node_set_physics_process(Node* node, bool enabled) {
    if (node) node->physics_process_enabled = enabled;
}

bool node_can_process(Node* node) {
    if (!node) return false;
    
    if (node->process_mode == PROCESS_MODE_DISABLED) return false;
    if (node->process_mode == PROCESS_MODE_WHEN_PAUSED) return true;
    
    // Check flags
    if (node->flags & NODE_FLAG_PROCESS_PAUSED) return false;
    
    return node->process_enabled;
}

// Utilities
const char* node_get_type_name(NodeType type) {
    switch (type) {
        case NODE_TYPE_GENERIC: return "Node";
        case NODE_TYPE_SPATIAL_2D: return "Node2D";
        case NODE_TYPE_SPRITE: return "Sprite";
        case NODE_TYPE_CAMERA_2D: return "Camera2D";
        case NODE_TYPE_RIGID_BODY_2D: return "RigidBody2D";
        case NODE_TYPE_COLLISION_SHAPE_2D: return "CollisionShape2D";
        case NODE_TYPE_TILE_MAP: return "TileMap";
        case NODE_TYPE_CONTROL: return "Control";
        case NODE_TYPE_BUTTON: return "Button";
        case NODE_TYPE_LABEL: return "Label";
        case NODE_TYPE_TEXTURE_RECT: return "TextureRect";
        case NODE_TYPE_PANEL: return "Panel";
        case NODE_TYPE_ANIMATION_PLAYER: return "AnimationPlayer";
        case NODE_TYPE_AUDIO_STREAM_PLAYER: return "AudioStreamPlayer";
        case NODE_TYPE_PARTICLE_EMITTER: return "ParticleEmitter";
        case NODE_TYPE_SCRIPT: return "Script";
        default: return "Unknown";
    }
}

const char* node_get_name(Node* node) {
    return node ? node->name : "";
}

void node_set_name(Node* node, const char* name) {
    if (node && name) {
        strncpy(node->name, name, sizeof(node->name) - 1);
    }
}

NodeType node_get_type(Node* node) {
    return node ? node->type : NODE_TYPE_GENERIC;
}

void node_emit_signal(Node* node, const char* signal_name) {
    // Signal system - to be implemented
    (void)node;
    (void)signal_name;
}
