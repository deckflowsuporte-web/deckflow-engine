/**
 * @file particle.h
 * @brief Particle System - Visual effects like fire, smoke, explosions
 */

#ifndef DECKFLOW_PARTICLE_H
#define DECKFLOW_PARTICLE_H

#include <stdbool.h>
#include "../math/vector2.h"

// Particle blend modes
typedef enum {
    PARTICLE_BLEND_ALPHA,     // Standard alpha blending
    PARTICLE_BLEND_ADDITIVE    // Additive blending (glow effect)
} ParticleBlendMode;

// Particle shape
typedef enum {
    PARTICLE_SHAPE_CIRCLE,
    PARTICLE_SHAPE_SQUARE,
    PARTICLE_SHAPE_LINE,
    PARTICLE_SHAPE_STAR
} ParticleShape;

// Single particle
typedef struct {
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float life;           // Current life remaining
    float max_life;       // Initial life
    float size;           // Current size
    float start_size;     // Initial size
    float end_size;       // Final size
    float rotation;        // Current rotation
    float rotation_speed;  // Rotation velocity
    float color[4];       // RGBA (0-1 range)
    float start_color[4];
    float end_color[4];
    float gravity;
    float friction;
    ParticleShape shape;
} Particle;

// Emitter types
typedef enum {
    EMITTER_TYPE_BURST,   // Emit all at once
    EMITTER_TYPE_STREAM   // Continuous emission
} EmitterType;

// Particle emitter
typedef struct ParticleEmitter {
    // Configuration
    Vector2 position;
    EmitterType type;
    int max_particles;
    
    // Emission
    int emit_count;       // Particles per emission
    float emit_rate;      // Particles per second (stream)
    float emit_timer;
    
    // Particle properties
    float particle_life_min;
    float particle_life_max;
    
    float start_size_min;
    float start_size_max;
    float end_size_min;
    float end_size_max;
    
    float start_color[4];
    float end_color[4];
    
    Vector2 velocity_min;
    Vector2 velocity_max;
    
    float gravity_min;
    float gravity_max;
    float friction_min;
    float friction_max;
    
    float rotation_speed_min;
    float rotation_speed_max;
    
    ParticleShape shape;
    ParticleBlendMode blend_mode;
    
    // Runtime
    Particle* particles;
    int particle_count;
    bool active;
    
    struct ParticleEmitter* next;
} ParticleEmitter;

// Particle System
typedef struct {
    ParticleEmitter* emitters;
    int emitter_count;
    int max_particles_total;
} ParticleSystem;

// Create particle system
ParticleSystem* particles_create(int max_particles);

// Free particle system
void particles_free(ParticleSystem* system);

// Create emitter
ParticleEmitter* particles_add_emitter(ParticleSystem* system, const char* name);

// Remove emitter
void particles_remove_emitter(ParticleSystem* system, ParticleEmitter* emitter);

// Emit burst of particles
void particles_emit(ParticleSystem* system, ParticleEmitter* emitter, Vector2 position, int count);

// Update all particles
void particles_update(ParticleSystem* system, float delta_time);

// Render all particles
void particles_render(ParticleSystem* system, void* renderer, void* camera);

// Activate/deactivate emitter
void emitter_start(ParticleEmitter* emitter);
void emitter_stop(ParticleEmitter* emitter);

// Configure emitter properties
void emitter_set_position(ParticleEmitter* emitter, Vector2 pos);
void emitter_set_rate(ParticleEmitter* emitter, float rate);
void emitter_set_life(ParticleEmitter* emitter, float min, float max);
void emitter_set_size(ParticleEmitter* emitter, float start_min, float start_max, float end_min, float end_max);
void emitter_set_velocity(ParticleEmitter* emitter, Vector2 min, Vector2 max);
void emitter_set_color(ParticleEmitter* emitter, float sr, float sg, float sb, float sa, float er, float eg, float eb, float ea);
void emitter_set_gravity(ParticleEmitter* emitter, float min, float max);
void emitter_set_shape(ParticleEmitter* emitter, ParticleShape shape);
void emitter_set_blend(ParticleEmitter* emitter, ParticleBlendMode mode);

// Preset: Fire effect
void emitter_configure_fire(ParticleEmitter* emitter);

// Preset: Smoke effect
void emitter_configure_smoke(ParticleEmitter* emitter);

// Preset: Explosion effect
void emitter_configure_explosion(ParticleEmitter* emitter);

// Preset: Sparkle effect
void emitter_configure_sparkle(ParticleEmitter* emitter);

// Preset: Rain effect
void emitter_configure_rain(ParticleEmitter* emitter);

#endif // DECKFLOW_PARTICLE_H
