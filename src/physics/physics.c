/**
 * @file physics.c
 * @brief Physics Integration Implementation
 * 
 * This is a standalone implementation that provides the physics API.
 * In production, this integrates with Box2D library.
 */

#include "physics.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Scale factor (pixels per meter)
#define PIXELS_PER_METER 50.0f

// Standalone physics simulation (simplified)
typedef struct {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float angular_velocity;
    Vector2 acceleration;
    float mass;
    float inertia;
    PhysicsBodyType type;
    PhysicsComponent* comp;
} SimpleBody;

static SimpleBody* g_bodies = NULL;
static int g_body_capacity = 0;
static int g_body_count = 0;

PhysicsWorld* physics_world_create(Vector2 gravity) {
    PhysicsWorld* world = (PhysicsWorld*)calloc(1, sizeof(PhysicsWorld));
    if (!world) return NULL;
    
    world->gravity = gravity;
    world->component_count = 0;
    world->components = NULL;
    world->has_bounds = false;
    
    // Initialize simple physics
    g_body_capacity = 100;
    g_bodies = (SimpleBody*)calloc(g_body_capacity, sizeof(SimpleBody));
    g_body_count = 0;
    
    printf("[Physics] World created (gravity: %.1f, %.1f)\n", gravity.x, gravity.y);
    return world;
}

void physics_world_free(PhysicsWorld* world) {
    if (!world) return;
    
    // Free components
    PhysicsComponent* comp = world->components;
    while (comp) {
        PhysicsComponent* next = comp->next;
        free(comp);
        comp = next;
    }
    
    free(world);
    
    // Free simple bodies
    if (g_bodies) free(g_bodies);
    g_bodies = NULL;
    g_body_count = 0;
    
    printf("[Physics] World freed\n");
}

void physics_world_set_bounds(PhysicsWorld* world, float min_x, float min_y, float max_x, float max_y) {
    if (!world) return;
    world->has_bounds = true;
    world->bounds_min_x = min_x;
    world->bounds_min_y = min_y;
    world->bounds_max_x = max_x;
    world->bounds_max_y = max_y;
}

void physics_world_update(PhysicsWorld* world, float delta_time) {
    if (!world) return;
    
    // Simple physics simulation
    for (int i = 0; i < g_body_count; i++) {
        SimpleBody* body = &g_bodies[i];
        if (body->type != PHYSICS_BODY_DYNAMIC) continue;
        if (!body->comp || !body->comp->enabled) continue;
        
        // Apply gravity
        body->acceleration = world->gravity;
        
        // Integrate velocity
        body->velocity.x += body->acceleration.x * delta_time;
        body->velocity.y += body->acceleration.y * delta_time;
        
        // Integrate position
        body->position.x += body->velocity.x * delta_time;
        body->position.y += body->velocity.y * delta_time;
        
        // Integrate rotation
        body->rotation += body->angular_velocity * delta_time;
        
        // Simple ground collision
        if (world->has_bounds) {
            float radius = body->comp->shape_type == PHYSICS_SHAPE_CIRCLE ? 
                          body->comp->shape_data.circle_radius : 0.5f;
            
            if (body->position.y - radius < world->bounds_min_y) {
                body->position.y = world->bounds_min_y + radius;
                body->velocity.y = -body->velocity.y * body->comp->material.restitution;
                body->angular_velocity *= 0.8f;
            }
            
            if (body->position.x - radius < world->bounds_min_x) {
                body->position.x = world->bounds_min_x + radius;
                body->velocity.x = -body->velocity.x * body->comp->material.restitution;
            }
            
            if (body->position.x + radius > world->bounds_max_x) {
                body->position.x = world->bounds_max_x - radius;
                body->velocity.x = -body->velocity.x * body->comp->material.restitution;
            }
        }
        
        // Friction
        body->velocity.x *= 0.99f;
        body->velocity.y *= 0.995f;
        body->angular_velocity *= 0.98f;
    }
}

PhysicsComponent* physics_component_create(PhysicsWorld* world, Node* node, PhysicsBodyType type) {
    if (!world) return NULL;
    
    PhysicsComponent* comp = (PhysicsComponent*)calloc(1, sizeof(PhysicsComponent));
    if (!comp) return NULL;
    
    comp->body = NULL;  // Would be Box2D body
    comp->shape_type = PHYSICS_SHAPE_BOX;
    comp->shape_data.box.w = 1.0f;
    comp->shape_data.box.h = 1.0f;
    comp->material.density = type == PHYSICS_BODY_STATIC ? 0.0f : 1.0f;
    comp->material.friction = 0.3f;
    comp->material.restitution = 0.2f;
    comp->material.is_sensor = false;
    comp->collision.category = PHYSICS_LAYER_DEFAULT;
    comp->collision.mask = 0xFFFF;
    comp->collision.group = 0;
    comp->enabled = true;
    comp->auto_sync = true;
    comp->user_data = NULL;
    comp->next = world->components;
    world->components = comp;
    world->component_count++;
    
    // Create simple body
    if (g_body_count >= g_body_capacity) {
        g_body_capacity *= 2;
        g_bodies = (SimpleBody*)realloc(g_bodies, sizeof(SimpleBody) * g_body_capacity);
    }
    
    SimpleBody* body = &g_bodies[g_body_count];
    body->position = node ? node->transform.position : vector2_zero();
    body->velocity = vector2_zero();
    body->rotation = node ? node->transform.rotation : 0.0f;
    body->angular_velocity = 0.0f;
    body->acceleration = vector2_zero();
    body->mass = comp->material.density > 0 ? comp->material.density : 1.0f;
    body->inertia = 1.0f;
    body->type = type;
    body->comp = comp;
    comp->body = (void*)((intptr_t)g_body_count);  // Store index
    g_body_count++;
    
    printf("[Physics] Component created (type: %d)\n", type);
    return comp;
}

void physics_component_free(PhysicsWorld* world, PhysicsComponent* comp) {
    if (!world || !comp) return;
    
    // Remove from list
    if (world->components == comp) {
        world->components = comp->next;
    } else {
        PhysicsComponent* prev = world->components;
        while (prev && prev->next != comp) {
            prev = prev->next;
        }
        if (prev) prev->next = comp->next;
    }
    
    world->component_count--;
    free(comp);
    printf("[Physics] Component freed\n");
}

void physics_node_attach(Node* node, PhysicsComponent* comp) {
    if (!node || !comp) return;
    node->physics_component = comp;
}

void physics_node_detach(Node* node) {
    if (!node) return;
    node->physics_component = NULL;
}

void physics_set_circle(PhysicsComponent* comp, float radius) {
    if (!comp) return;
    comp->shape_type = PHYSICS_SHAPE_CIRCLE;
    comp->shape_data.circle_radius = radius;
}

void physics_set_box(PhysicsComponent* comp, float width, float height) {
    if (!comp) return;
    comp->shape_type = PHYSICS_SHAPE_BOX;
    comp->shape_data.box.w = width;
    comp->shape_data.box.h = height;
}

void physics_set_material(PhysicsComponent* comp, float density, float friction, float restitution) {
    if (!comp) return;
    comp->material.density = density;
    comp->material.friction = friction;
    comp->material.restitution = restitution;
}

void physics_set_collision_layer(PhysicsComponent* comp, PhysicsLayer category, PhysicsLayer mask) {
    if (!comp) return;
    comp->collision.category = category;
    comp->collision.mask = mask;
}

void physics_apply_force(PhysicsComponent* comp, Vector2 force) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    SimpleBody* body = &g_bodies[idx];
    body->acceleration.x += force.x / body->mass;
    body->acceleration.y += force.y / body->mass;
}

void physics_apply_force_at(PhysicsComponent* comp, Vector2 force, Vector2 point) {
    physics_apply_force(comp, force);
    (void)point;
}

void physics_apply_impulse(PhysicsComponent* comp, Vector2 impulse) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    SimpleBody* body = &g_bodies[idx];
    body->velocity.x += impulse.x / body->mass;
    body->velocity.y += impulse.y / body->mass;
}

void physics_apply_torque(PhysicsComponent* comp, float torque) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    SimpleBody* body = &g_bodies[idx];
    body->angular_velocity += torque / body->inertia;
}

void physics_set_linear_velocity(PhysicsComponent* comp, Vector2 velocity) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    g_bodies[idx].velocity = velocity;
}

void physics_set_angular_velocity(PhysicsComponent* comp, float omega) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    g_bodies[idx].angular_velocity = omega;
}

Vector2 physics_get_linear_velocity(PhysicsComponent* comp) {
    if (!comp || comp->body == NULL) return vector2_zero();
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return vector2_zero();
    
    return g_bodies[idx].velocity;
}

float physics_get_angular_velocity(PhysicsComponent* comp) {
    if (!comp || comp->body == NULL) return 0.0f;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return 0.0f;
    
    return g_bodies[idx].angular_velocity;
}

Vector2 physics_get_position(PhysicsComponent* comp) {
    if (!comp || comp->body == NULL) return vector2_zero();
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return vector2_zero();
    
    return g_bodies[idx].position;
}

float physics_get_rotation(PhysicsComponent* comp) {
    if (!comp || comp->body == NULL) return 0.0f;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return 0.0f;
    
    return g_bodies[idx].rotation;
}

void physics_set_position(PhysicsComponent* comp, Vector2 pos) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    g_bodies[idx].position = pos;
}

void physics_set_rotation(PhysicsComponent* comp, float angle) {
    if (!comp || comp->body == NULL) return;
    int idx = (int)((intptr_t)comp->body);
    if (idx < 0 || idx >= g_body_count) return;
    
    g_bodies[idx].rotation = angle;
}

void physics_sync_to_node(PhysicsComponent* comp, Node* node) {
    if (!comp || !node) return;
    node->transform.position = physics_get_position(comp);
    node->transform.rotation = physics_get_rotation(comp);
}

void physics_sync_from_node(PhysicsComponent* comp, Node* node) {
    if (!comp || !node) return;
    physics_set_position(comp, node->transform.position);
    physics_set_rotation(comp, node->transform.rotation);
}

bool physics_raycast(PhysicsWorld* world, Vector2 origin, Vector2 direction, float max_dist, PhysicsRayHit* hit) {
    (void)world; (void)origin; (void)direction; (void)max_dist;
    if (hit) {
        hit->point = origin;
        hit->normal = direction;
        hit->fraction = max_dist;
        hit->component = NULL;
    }
    return false;
}

bool physics_raycast_layer(PhysicsWorld* world, Vector2 origin, Vector2 direction, float max_dist, PhysicsLayer layer, PhysicsRayHit* hit) {
    (void)layer;
    return physics_raycast(world, origin, direction, max_dist, hit);
}

PhysicsComponent* physics_query_point(PhysicsWorld* world, Vector2 point) {
    (void)world; (void)point;
    return NULL;
}

PhysicsComponent* physics_query_point_layer(PhysicsWorld* world, Vector2 point, PhysicsLayer layer) {
    (void)layer;
    return physics_query_point(world, point);
}

void physics_query_aabb(PhysicsWorld* world, Vector2 min, Vector2 max, PhysicsQueryCallback callback, void* data) {
    (void)world; (void)min; (void)max; (void)callback; (void)data;
}

void physics_world_lock(PhysicsWorld* world) {
    (void)world;
}

void physics_world_unlock(PhysicsWorld* world) {
    (void)world;
}

float physics_meters_to_pixels(float meters) { return meters * PIXELS_PER_METER; }
float physics_pixels_to_meters(float pixels) { return pixels / PIXELS_PER_METER; }
Vector2 physics_vec_to_world(Vector2 pixels) { return vector2_scale(pixels, 1.0f / PIXELS_PER_METER); }
Vector2 physics_vec_to_screen(Vector2 world) { return vector2_scale(world, PIXELS_PER_METER); }
