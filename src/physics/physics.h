/**
 * @file physics.h
 * @brief Physics Integration - Box2D as Node Components
 * 
 * This module integrates Box2D physics with the Scene Graph system.
 * Physics bodies become components attached to Nodes.
 */

#ifndef DECKFLOW_PHYSICS_H
#define DECKFLOW_PHYSICS_H

#include <stdbool.h>
#include "../math/vector2.h"
#include "../nodes/node.h"

// Forward declare Box2D types (user provides the library)
typedef struct b2World b2World;
typedef struct b2Body b2Body;
typedef struct b2Shape b2Shape;
typedef struct b2Fixture b2Fixture;
typedef struct b2Vec2 b2Vec2;

// Physics body types
typedef enum {
    PHYSICS_BODY_STATIC = 0,    // Immovable
    PHYSICS_BODY_KINEMATIC,    // Moved by code
    PHYSICS_BODY_DYNAMIC       // Affected by physics
} PhysicsBodyType;

// Physics shape types
typedef enum {
    PHYSICS_SHAPE_CIRCLE = 0,
    PHYSICS_SHAPE_BOX,
    PHYSICS_SHAPE_POLYGON,
    PHYSICS_SHAPE_CHAIN
} PhysicsShapeType;

// Physics material (for fixtures)
typedef struct {
    float density;         // kg/m² (0 for static)
    float friction;       // 0-1
    float restitution;    // Bounciness 0-1
    bool is_sensor;       // Trigger zone (no collision response)
} PhysicsMaterial;

// Physics layer (for collision filtering)
typedef unsigned int PhysicsLayer;
#define PHYSICS_LAYER_DEFAULT 0x01
#define PHYSICS_LAYER_PLAYER  0x02
#define PHYSICS_LAYER_ENEMY   0x04
#define PHYSICS_LAYER_ITEM    0x08
#define PHYSICS_LAYER_WORLD   0x10

// Collision categories and masks
typedef struct {
    PhysicsLayer category;   // What this body IS
    PhysicsLayer mask;      // What this body COLLIDES WITH
    PhysicsLayer group;     // Collision group (positive = always, negative = never)
} PhysicsCollisionFilter;

// Physics Component (attaches to a Node)
typedef struct PhysicsComponent {
    // Box2D handle
    b2Body* body;
    
    // Shape
    PhysicsShapeType shape_type;
    union {
        float circle_radius;
        struct { float w, h; } box;
    } shape_data;
    
    // Material
    PhysicsMaterial material;
    
    // Collision
    PhysicsCollisionFilter collision;
    
    // State
    bool enabled;
    bool auto_sync;  // Auto-sync position with Node transform
    
    // User data
    void* user_data;
    
    struct PhysicsComponent* next;
} PhysicsComponent;

// Physics World
typedef struct {
    b2World* world;
    Vector2 gravity;
    
    // Components list
    PhysicsComponent* components;
    int component_count;
    
    // Collision callbacks
    void (*on_collision_start)(PhysicsComponent* a, PhysicsComponent* b);
    void (*on_collision_end)(PhysicsComponent* a, PhysicsComponent* b);
    
    // World bounds
    bool has_bounds;
    float bounds_min_x, bounds_min_y;
    float bounds_max_x, bounds_max_y;
} PhysicsWorld;

// Create/destroy physics world
PhysicsWorld* physics_world_create(Vector2 gravity);
void physics_world_free(PhysicsWorld* world);

// Set world bounds (for camera limits)
void physics_world_set_bounds(PhysicsWorld* world, float min_x, float min_y, float max_x, float max_y);

// Step simulation
void physics_world_update(PhysicsWorld* world, float delta_time);

// Create component
PhysicsComponent* physics_component_create(PhysicsWorld* world, Node* node, PhysicsBodyType type);
void physics_component_free(PhysicsWorld* world, PhysicsComponent* comp);

// Attach component to node
void physics_node_attach(Node* node, PhysicsComponent* comp);
void physics_node_detach(Node* node);

// Set shape
void physics_set_circle(PhysicsComponent* comp, float radius);
void physics_set_box(PhysicsComponent* comp, float width, float height);

// Set material
void physics_set_material(PhysicsComponent* comp, float density, float friction, float restitution);

// Collision filtering
void physics_set_collision_layer(PhysicsComponent* comp, PhysicsLayer category, PhysicsLayer mask);

// Apply forces (dynamic bodies only)
void physics_apply_force(PhysicsComponent* comp, Vector2 force);
void physics_apply_force_at(PhysicsComponent* comp, Vector2 force, Vector2 point);
void physics_apply_impulse(PhysicsComponent* comp, Vector2 impulse);
void physics_apply_torque(PhysicsComponent* comp, float torque);

// Set velocity directly
void physics_set_linear_velocity(PhysicsComponent* comp, Vector2 velocity);
void physics_set_angular_velocity(PhysicsComponent* comp, float omega);
Vector2 physics_get_linear_velocity(PhysicsComponent* comp);
float physics_get_angular_velocity(PhysicsComponent* comp);

// Get transform
Vector2 physics_get_position(PhysicsComponent* comp);
float physics_get_rotation(PhysicsComponent* comp);

// Set transform (kinematic bodies)
void physics_set_position(PhysicsComponent* comp, Vector2 pos);
void physics_set_rotation(PhysicsComponent* comp, float angle);

// Sync Node transform with physics body
void physics_sync_to_node(PhysicsComponent* comp, Node* node);
void physics_sync_from_node(PhysicsComponent* comp, Node* node);

// Ray casting
typedef struct {
    Vector2 point;
    Vector2 normal;
    float fraction;
    PhysicsComponent* component;
} PhysicsRayHit;

bool physics_raycast(PhysicsWorld* world, Vector2 origin, Vector2 direction, float max_dist, PhysicsRayHit* hit);
bool physics_raycast_layer(PhysicsWorld* world, Vector2 origin, Vector2 direction, float max_dist, PhysicsLayer layer, PhysicsRayHit* hit);

// Query point
PhysicsComponent* physics_query_point(PhysicsWorld* world, Vector2 point);
PhysicsComponent* physics_query_point_layer(PhysicsWorld* world, Vector2 point, PhysicsLayer layer);

// Query AABB
typedef bool (*PhysicsQueryCallback)(PhysicsComponent* comp, void* data);
void physics_query_aabb(PhysicsWorld* world, Vector2 min, Vector2 max, PhysicsQueryCallback callback, void* data);

// Lock/unlock world (for multi-threaded physics)
void physics_world_lock(PhysicsWorld* world);
void physics_world_unlock(PhysicsWorld* world);

// Utility
float physics_meters_to_pixels(float meters);
float physics_pixels_to_meters(float pixels);
Vector2 physics_vec_to_world(Vector2 pixels);
Vector2 physics_vec_to_screen(Vector2 world);

#endif // DECKFLOW_PHYSICS_H
