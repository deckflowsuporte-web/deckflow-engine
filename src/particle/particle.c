/**
 * @file particle.c
 * @brief Particle System Implementation
 */

#include "particle.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DEG_TO_RAD 0.0174533f

ParticleSystem* particles_create(int max_particles) {
    ParticleSystem* system = (ParticleSystem*)calloc(1, sizeof(ParticleSystem));
    if (!system) return NULL;
    
    system->max_particles_total = max_particles;
    system->emitters = NULL;
    system->emitter_count = 0;
    
    printf("[Particles] System created (max: %d)\n", max_particles);
    return system;
}

void particles_free(ParticleSystem* system) {
    if (!system) return;
    
    ParticleEmitter* emitter = system->emitters;
    while (emitter) {
        ParticleEmitter* next = emitter->next;
        particles_remove_emitter(system, emitter);
        emitter = next;
    }
    
    free(system);
    printf("[Particles] System freed\n");
}

ParticleEmitter* particles_add_emitter(ParticleSystem* system, const char* name) {
    (void)name;
    if (!system) return NULL;
    
    ParticleEmitter* emitter = (ParticleEmitter*)calloc(1, sizeof(ParticleEmitter));
    if (!emitter) return NULL;
    
    emitter->max_particles = 100;
    emitter->particles = (Particle*)calloc(emitter->max_particles, sizeof(Particle));
    emitter->active = false;
    emitter->type = EMITTER_TYPE_STREAM;
    emitter->emit_rate = 10.0f;
    emitter->emit_timer = 0.0f;
    emitter->particle_life_min = 1.0f;
    emitter->particle_life_max = 2.0f;
    emitter->start_size_min = 5.0f;
    emitter->start_size_max = 10.0f;
    emitter->end_size_min = 0.0f;
    emitter->end_size_max = 0.0f;
    emitter->velocity_min = vector2_new(-50, -50);
    emitter->velocity_max = vector2_new(50, 50);
    emitter->gravity_min = 0.0f;
    emitter->gravity_max = 0.0f;
    emitter->shape = PARTICLE_SHAPE_CIRCLE;
    emitter->blend_mode = PARTICLE_BLEND_ALPHA;
    
    // Add to list
    emitter->next = system->emitters;
    system->emitters = emitter;
    system->emitter_count++;
    
    printf("[Particles] Added emitter\n");
    return emitter;
}

void particles_remove_emitter(ParticleSystem* system, ParticleEmitter* emitter) {
    if (!system || !emitter) return;
    
    if (emitter->particles) free(emitter->particles);
    
    // Remove from list
    if (system->emitters == emitter) {
        system->emitters = emitter->next;
    } else {
        ParticleEmitter* prev = system->emitters;
        while (prev && prev->next != emitter) {
            prev = prev->next;
        }
        if (prev) prev->next = emitter->next;
    }
    
    free(emitter);
    system->emitter_count--;
}

void particles_emit(ParticleSystem* system, ParticleEmitter* emitter, Vector2 position, int count) {
    (void)system;
    if (!emitter || !emitter->active) return;
    
    for (int i = 0; i < count; i++) {
        // Find dead particle
        Particle* p = NULL;
        for (int j = 0; j < emitter->max_particles; j++) {
            if (emitter->particles[j].life <= 0) {
                p = &emitter->particles[j];
                break;
            }
        }
        if (!p) continue;
        
        // Initialize particle
        p->position = position;
        
        float angle = ((float)rand() / RAND_MAX) * 360.0f * DEG_TO_RAD;
        float speed = emitter->velocity_min.x + ((float)rand() / RAND_MAX) * (emitter->velocity_max.x - emitter->velocity_min.x);
        p->velocity.x = cosf(angle) * speed;
        p->velocity.y = sinf(angle) * speed;
        
        p->acceleration = vector2_zero();
        
        p->max_life = emitter->particle_life_min + ((float)rand() / RAND_MAX) * (emitter->particle_life_max - emitter->particle_life_min);
        p->life = p->max_life;
        
        p->start_size = emitter->start_size_min + ((float)rand() / RAND_MAX) * (emitter->start_size_max - emitter->start_size_min);
        p->end_size = emitter->end_size_min + ((float)rand() / RAND_MAX) * (emitter->end_size_max - emitter->end_size_min);
        p->size = p->start_size;
        
        p->rotation = 0.0f;
        p->rotation_speed = emitter->rotation_speed_min + ((float)rand() / RAND_MAX) * (emitter->rotation_speed_max - emitter->rotation_speed_min);
        
        memcpy(p->color, emitter->start_color, sizeof(float) * 4);
        memcpy(p->start_color, emitter->start_color, sizeof(float) * 4);
        memcpy(p->end_color, emitter->end_color, sizeof(float) * 4);
        
        p->gravity = emitter->gravity_min + ((float)rand() / RAND_MAX) * (emitter->gravity_max - emitter->gravity_min);
        p->friction = emitter->friction_min + ((float)rand() / RAND_MAX) * (emitter->friction_max - emitter->friction_min);
        p->shape = emitter->shape;
        
        emitter->particle_count++;
    }
}

void particles_update(ParticleSystem* system, float delta_time) {
    if (!system) return;
    
    ParticleEmitter* emitter = system->emitters;
    while (emitter) {
        if (emitter->active) {
            // Stream emission
            if (emitter->type == EMITTER_TYPE_STREAM) {
                emitter->emit_timer += delta_time;
                float emit_interval = 1.0f / emitter->emit_rate;
                
                while (emitter->emit_timer >= emit_interval) {
                    particles_emit(system, emitter, emitter->position, emitter->emit_count);
                    emitter->emit_timer -= emit_interval;
                }
            }
        }
        
        // Update particles
        for (int i = 0; i < emitter->max_particles; i++) {
            Particle* p = &emitter->particles[i];
            if (p->life <= 0) continue;
            
            // Physics
            p->velocity.y -= p->gravity * delta_time;
            p->velocity.x *= (1.0f - p->friction * delta_time);
            p->velocity.y *= (1.0f - p->friction * delta_time);
            
            p->position.x += p->velocity.x * delta_time;
            p->position.y += p->velocity.y * delta_time;
            
            p->rotation += p->rotation_speed * delta_time;
            
            // Life
            p->life -= delta_time;
            float life_ratio = p->life / p->max_life;
            
            // Size interpolation
            p->size = p->start_size * life_ratio + p->end_size * (1.0f - life_ratio);
            
            // Color interpolation
            for (int c = 0; c < 4; c++) {
                p->color[c] = p->start_color[c] * life_ratio + p->end_color[c] * (1.0f - life_ratio);
            }
            
            if (p->life <= 0) {
                emitter->particle_count--;
            }
        }
        
        emitter = emitter->next;
    }
}

void particles_render(ParticleSystem* system, void* renderer, void* camera) {
    (void)system;
    (void)renderer;
    (void)camera;
    // Stub: actual rendering would use SDL_RenderFillRect or custom primitives
}

void emitter_start(ParticleEmitter* emitter) {
    if (emitter) emitter->active = true;
}

void emitter_stop(ParticleEmitter* emitter) {
    if (emitter) emitter->active = false;
}

void emitter_set_position(ParticleEmitter* emitter, Vector2 pos) {
    if (emitter) emitter->position = pos;
}

void emitter_set_rate(ParticleEmitter* emitter, float rate) {
    if (emitter) emitter->emit_rate = rate;
}

void emitter_set_life(ParticleEmitter* emitter, float min, float max) {
    if (emitter) {
        emitter->particle_life_min = min;
        emitter->particle_life_max = max;
    }
}

void emitter_set_size(ParticleEmitter* emitter, float start_min, float start_max, float end_min, float end_max) {
    if (emitter) {
        emitter->start_size_min = start_min;
        emitter->start_size_max = start_max;
        emitter->end_size_min = end_min;
        emitter->end_size_max = end_max;
    }
}

void emitter_set_velocity(ParticleEmitter* emitter, Vector2 min, Vector2 max) {
    if (emitter) {
        emitter->velocity_min = min;
        emitter->velocity_max = max;
    }
}

void emitter_set_color(ParticleEmitter* emitter, float sr, float sg, float sb, float sa, float er, float eg, float eb, float ea) {
    if (emitter) {
        emitter->start_color[0] = sr;
        emitter->start_color[1] = sg;
        emitter->start_color[2] = sb;
        emitter->start_color[3] = sa;
        emitter->end_color[0] = er;
        emitter->end_color[1] = eg;
        emitter->end_color[2] = eb;
        emitter->end_color[3] = ea;
    }
}

void emitter_set_gravity(ParticleEmitter* emitter, float min, float max) {
    if (emitter) {
        emitter->gravity_min = min;
        emitter->gravity_max = max;
    }
}

void emitter_set_shape(ParticleEmitter* emitter, ParticleShape shape) {
    if (emitter) emitter->shape = shape;
}

void emitter_set_blend(ParticleEmitter* emitter, ParticleBlendMode mode) {
    if (emitter) emitter->blend_mode = mode;
}

void emitter_configure_fire(ParticleEmitter* emitter) {
    if (!emitter) return;
    emitter->type = EMITTER_TYPE_STREAM;
    emitter->emit_rate = 30.0f;
    emitter->emit_count = 1;
    emitter_set_life(emitter, 0.5f, 1.0f);
    emitter_set_size(emitter, 10.0f, 20.0f, 0.0f, 0.0f);
    emitter_set_velocity(emitter, vector2_new(-20, 50), vector2_new(20, 150));
    emitter_set_color(emitter, 1.0f, 0.5f, 0.0f, 1.0f, 0.3f, 0.0f, 0.0f, 0.0f);
    emitter_set_gravity(emitter, -50.0f, -30.0f);
    emitter_set_shape(emitter, PARTICLE_SHAPE_CIRCLE);
}

void emitter_configure_smoke(ParticleEmitter* emitter) {
    if (!emitter) return;
    emitter->type = EMITTER_TYPE_STREAM;
    emitter->emit_rate = 15.0f;
    emitter->emit_count = 1;
    emitter_set_life(emitter, 1.0f, 2.0f);
    emitter_set_size(emitter, 15.0f, 25.0f, 30.0f, 40.0f);
    emitter_set_velocity(emitter, vector2_new(-10, 20), vector2_new(10, 40));
    emitter_set_color(emitter, 0.5f, 0.5f, 0.5f, 0.3f, 0.2f, 0.2f, 0.2f, 0.0f);
    emitter_set_gravity(emitter, 10.0f, 20.0f);
    emitter_set_shape(emitter, PARTICLE_SHAPE_CIRCLE);
    emitter->blend_mode = PARTICLE_BLEND_ALPHA;
}

void emitter_configure_explosion(ParticleEmitter* emitter) {
    if (!emitter) return;
    emitter->type = EMITTER_TYPE_BURST;
    emitter->emit_count = 50;
    emitter_set_life(emitter, 0.3f, 0.8f);
    emitter_set_size(emitter, 10.0f, 20.0f, 0.0f, 5.0f);
    emitter_set_velocity(emitter, vector2_new(100, 100), vector2_new(300, 300));
    emitter_set_color(emitter, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
    emitter_set_gravity(emitter, -100.0f, -50.0f);
    emitter_set_shape(emitter, PARTICLE_SHAPE_CIRCLE);
}

void emitter_configure_sparkle(ParticleEmitter* emitter) {
    if (!emitter) return;
    emitter->type = EMITTER_TYPE_STREAM;
    emitter->emit_rate = 20.0f;
    emitter->emit_count = 1;
    emitter_set_life(emitter, 0.5f, 1.5f);
    emitter_set_size(emitter, 2.0f, 4.0f, 0.0f, 0.0f);
    emitter_set_velocity(emitter, vector2_new(-30, -30), vector2_new(30, 30));
    emitter_set_color(emitter, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.8f, 0.2f, 0.0f);
    emitter_set_gravity(emitter, 0.0f, 0.0f);
    emitter_set_shape(emitter, PARTICLE_SHAPE_STAR);
}

void emitter_configure_rain(ParticleEmitter* emitter) {
    if (!emitter) return;
    emitter->type = EMITTER_TYPE_STREAM;
    emitter->emit_rate = 100.0f;
    emitter->emit_count = 1;
    emitter_set_life(emitter, 0.5f, 1.0f);
    emitter_set_size(emitter, 2.0f, 3.0f, 2.0f, 3.0f);
    emitter_set_velocity(emitter, vector2_new(-10, -200), vector2_new(10, -400));
    emitter_set_color(emitter, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f);
    emitter_set_gravity(emitter, 0.0f, 0.0f);
    emitter_set_shape(emitter, PARTICLE_SHAPE_LINE);
}
