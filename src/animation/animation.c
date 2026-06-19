/**
 * @file animation.c
 * @brief Animation System Implementation
 */

#include "animation.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// ============================================
// SPRITE SHEET
// ============================================

SpriteSheet* spritesheet_create(void* renderer, const char* filepath, 
                                int frame_width, int frame_height) {
    (void)renderer;
    SpriteSheet* sheet = (SpriteSheet*)calloc(1, sizeof(SpriteSheet));
    if (!sheet) return NULL;
    
    sheet->frame_width = frame_width;
    sheet->frame_height = frame_height;
    // In production, load texture here:
    // sheet->texture = IMG_LoadTexture(renderer, filepath);
    
    printf("[Animation] Created spritesheet (%dx%d frames)\n", frame_width, frame_height);
    return sheet;
}

void spritesheet_free(SpriteSheet* sheet) {
    if (!sheet) return;
    // if (sheet->texture) SDL_DestroyTexture(sheet->texture);
    if (sheet->frames) free(sheet->frames);
    free(sheet);
}

SpriteFrame* spritesheet_get_frame(SpriteSheet* sheet, int index) {
    if (!sheet || index < 0 || index >= sheet->frame_count) return NULL;
    return &sheet->frames[index];
}

// ============================================
// ANIMATION CLIP
// ============================================

AnimationClip* anim_clip_create(const char* name, int frame_count) {
    AnimationClip* clip = (AnimationClip*)calloc(1, sizeof(AnimationClip));
    if (!clip) return NULL;
    
    clip->name = strdup(name);
    clip->frame_indices = (int*)malloc(sizeof(int) * frame_count);
    clip->frame_count = 0;
    clip->fps = 12.0f;
    clip->play_mode = ANIM_PLAY_LOOP;
    
    printf("[Animation] Created clip: %s\n", name);
    return clip;
}

void anim_clip_add_frame(AnimationClip* clip, int frame_index) {
    if (!clip) return;
    if (clip->frame_count > 0) {
        clip->frame_indices = (int*)realloc(clip->frame_indices, 
            sizeof(int) * (clip->frame_count + 1));
    }
    clip->frame_indices[clip->frame_count++] = frame_index;
}

void anim_clip_set_fps(AnimationClip* clip, float fps) {
    if (!clip) return;
    clip->fps = fps;
}

void anim_clip_set_play_mode(AnimationClip* clip, AnimPlayMode mode) {
    if (!clip) return;
    clip->play_mode = mode;
}

void anim_clip_add_event(AnimationClip* clip, const char* event_name, int frame) {
    if (!clip) return;
    // Add event system
    (void)event_name;
    (void)frame;
}

// ============================================
// ANIMATED SPRITE
// ============================================

AnimatedSprite* animsprite_create(Node* parent, SpriteSheet* sheet) {
    AnimatedSprite* sprite = (AnimatedSprite*)calloc(1, sizeof(AnimatedSprite));
    if (!sprite) return NULL;
    
    sprite->node = node_create("AnimatedSprite", NODE_TYPE_SPRITE);
    if (parent) {
        node_add_child(parent, sprite->node);
    }
    
    sprite->sheet = sheet;
    sprite->texture = NULL;
    sprite->current_frame = 0;
    sprite->frame_time = 0;
    sprite->frame_duration = 1.0f / 12.0f;
    sprite->playing = false;
    sprite->direction = ANIM_FORWARD;
    sprite->flip_h = false;
    sprite->flip_v = false;
    
    printf("[Animation] Created animated sprite\n");
    return sprite;
}

void animsprite_free(AnimatedSprite* sprite) {
    if (!sprite) return;
    if (sprite->node) node_free(sprite->node);
    free(sprite);
}

void animsprite_set_sheet(AnimatedSprite* sprite, SpriteSheet* sheet) {
    if (!sprite) return;
    sprite->sheet = sheet;
}

void animsprite_play(AnimatedSprite* sprite, AnimationClip* clip) {
    if (!sprite || !clip) return;
    sprite->current_clip = clip;
    sprite->current_frame = 0;
    sprite->frame_time = 0;
    sprite->frame_duration = 1.0f / clip->fps;
    sprite->playing = true;
    sprite->direction = ANIM_FORWARD;
}

void animsprite_play_default(AnimatedSprite* sprite) {
    if (!sprite || !sprite->default_clip) return;
    animsprite_play(sprite, sprite->default_clip);
}

void animsprite_stop(AnimatedSprite* sprite) {
    if (!sprite) return;
    sprite->playing = false;
    sprite->current_frame = 0;
}

void animsprite_pause(AnimatedSprite* sprite) {
    if (!sprite) return;
    sprite->playing = false;
}

void animsprite_resume(AnimatedSprite* sprite) {
    if (!sprite) return;
    sprite->playing = true;
}

void animsprite_set_frame(AnimatedSprite* sprite, int frame) {
    if (!sprite) return;
    if (sprite->current_clip) {
        if (frame < 0) frame = 0;
        if (frame >= sprite->current_clip->frame_count) {
            frame = sprite->current_clip->frame_count - 1;
        }
    }
    sprite->current_frame = frame;
    sprite->frame_time = 0;
}

void animsprite_set_flip(AnimatedSprite* sprite, bool h, bool v) {
    if (!sprite) return;
    sprite->flip_h = h;
    sprite->flip_v = v;
}

void animsprite_set_speed(AnimatedSprite* sprite, float speed) {
    if (!sprite || !sprite->current_clip) return;
    sprite->frame_duration = 1.0f / (sprite->current_clip->fps * speed);
}

float animsprite_get_speed(AnimatedSprite* sprite) {
    if (!sprite || !sprite->current_clip) return 1.0f;
    return 1.0f / (sprite->frame_duration * sprite->current_clip->fps);
}

void animsprite_update(AnimatedSprite* sprite, float delta_time) {
    if (!sprite || !sprite->playing || !sprite->current_clip) return;
    
    sprite->frame_time += delta_time;
    
    while (sprite->frame_time >= sprite->frame_duration) {
        sprite->frame_time -= sprite->frame_duration;
        
        // Frame change callback
        if (sprite->on_frame_change) {
            sprite->on_frame_change(sprite, sprite->current_frame);
        }
        
        // Advance frame
        sprite->current_frame += sprite->direction;
        
        // Handle animation end
        if (sprite->current_frame >= sprite->current_clip->frame_count) {
            switch (sprite->current_clip->play_mode) {
                case ANIM_PLAY_ONCE:
                    sprite->current_frame = sprite->current_clip->frame_count - 1;
                    sprite->playing = false;
                    if (sprite->on_animation_end) {
                        sprite->on_animation_end(sprite);
                    }
                    break;
                    
                case ANIM_PLAY_LOOP:
                    sprite->current_frame = 0;
                    break;
                    
                case ANIM_PLAY_PING_PONG:
                    sprite->current_frame = sprite->current_clip->frame_count - 2;
                    sprite->direction = ANIM_BACKWARD;
                    break;
                    
                case ANIM_PLAY_HOLD:
                    sprite->current_frame = sprite->current_clip->frame_count - 1;
                    sprite->playing = false;
                    break;
            }
        }
        else if (sprite->current_frame < 0) {
            sprite->current_frame = 1;
            sprite->direction = ANIM_FORWARD;
        }
    }
}

// ============================================
// EASING FUNCTIONS
// ============================================

float ease_linear(float t) {
    return t;
}

float ease_in_quad(float t) {
    return t * t;
}

float ease_out_quad(float t) {
    return t * (2 - t);
}

float ease_in_out_quad(float t) {
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

float ease_in_cubic(float t) {
    return t * t * t;
}

float ease_out_cubic(float t) {
    return (t - 1) * (t - 1) * (t - 1) + 1;
}

float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

float ease_in_sine(float t) {
    return 1 - cosf(t * M_PI / 2);
}

float ease_out_sine(float t) {
    return sinf(t * M_PI / 2);
}

float ease_in_out_sine(float t) {
    return -(cosf(M_PI * t) - 1) / 2;
}

float ease_bounce(float t) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    float x;
    
    if (t < 1 / d1) {
        x = n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5f / d1;
        x = n1 * t * t + 0.75f;
    } else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        x = n1 * t * t + 0.9375f;
    } else {
        t -= 2.625f / d1;
        x = n1 * t * t + 0.984375f;
    }
    return x;
}

float ease_elastic(float t) {
    if (t == 0 || t == 1) return t;
    float p = 0.3f;
    float s = p / 4;
    return powf(2, -10 * t) * sinf((t - s) * (2 * M_PI) / p) + 1;
}

EasingFunction easing_get_func(EasingType type) {
    switch (type) {
        case EASE_IN: return ease_in_quad;
        case EASE_OUT: return ease_out_quad;
        case EASE_IN_OUT: return ease_in_out_quad;
        case EASE_BOUNCE: return ease_bounce;
        case EASE_ELASTIC: return ease_elastic;
        case EASE_SINE: return ease_in_out_sine;
        default: return ease_linear;
    }
}

// ============================================
// TWEEN SYSTEM
// ============================================

TweenManager* tween_manager_create(int max_tweens) {
    TweenManager* mgr = (TweenManager*)calloc(1, sizeof(TweenManager));
    if (!mgr) return NULL;
    
    mgr->max_tweens = max_tweens;
    mgr->tweens = (Tween*)calloc(max_tweens, sizeof(Tween));
    
    printf("[Animation] Tween manager created (max: %d)\n", max_tweens);
    return mgr;
}

void tween_manager_free(TweenManager* mgr) {
    if (!mgr) return;
    if (mgr->tweens) free(mgr->tweens);
    free(mgr);
}

Tween* tween_create(TweenManager* mgr, void* target) {
    if (!mgr || !target) return NULL;
    
    // Find free tween slot
    for (int i = 0; i < mgr->max_tweens; i++) {
        if (!mgr->tweens[i].playing && !mgr->tweens[i].paused) {
            Tween* t = &mgr->tweens[i];
            memset(t, 0, sizeof(Tween));
            t->target = target;
            t->duration = 1.0f;
            t->speed = 1.0f;
            t->easing_func = ease_linear;
            t->auto_kill = true;
            tween_start(t);
            return t;
        }
    }
    
    return NULL;  // No free slots
}

Tween* tween_to(TweenManager* mgr, void* target, float duration, ...) {
    (void)mgr;
    // Simplified: just create and return
    return tween_create(NULL, target);
}

Tween* tween_from(TweenManager* mgr, void* target, float duration, ...) {
    (void)mgr;
    return tween_create(NULL, target);
}

Tween* tween_position(Tween* t, Vector2 from, Vector2 to) {
    if (!t) return NULL;
    t->properties |= TWEEN_POS;
    t->from_pos = from;
    t->to_pos = to;
    return t;
}

Tween* tween_scale(Tween* t, Vector2 from, Vector2 to) {
    if (!t) return NULL;
    t->properties |= TWEEN_SCALE;
    t->from_scale = from;
    t->to_scale = to;
    return t;
}

Tween* tween_rotation(Tween* t, float from, float to) {
    if (!t) return NULL;
    t->properties |= TWEEN_ROTATION;
    t->from_rotation = from;
    t->to_rotation = to;
    return t;
}

Tween* tween_color(Tween* t, float from_r, float from_g, float from_b, float from_a,
                             float to_r, float to_g, float to_b, float to_a) {
    if (!t) return NULL;
    t->properties |= TWEEN_COLOR;
    t->from_color[0] = from_r; t->from_color[1] = from_g;
    t->from_color[2] = from_b; t->from_color[3] = from_a;
    t->to_color[0] = to_r; t->to_color[1] = to_g;
    t->to_color[2] = to_b; t->to_color[3] = to_a;
    return t;
}

Tween* tween_delay(Tween* t, float delay) {
    if (!t) return NULL;
    t->delay = delay;
    return t;
}

Tween* tween_speed(Tween* t, float speed) {
    if (!t) return NULL;
    t->speed = speed;
    return t;
}

Tween* tween_ease(Tween* t, EasingType ease) {
    if (!t) return NULL;
    t->easing_func = easing_get_func(ease);
    return t;
}

Tween* tween_on_start(Tween* t, TweenCallback cb, void* data) {
    if (!t) return NULL;
    t->on_start = cb;
    t->user_data = data;
    return t;
}

Tween* tween_on_update(Tween* t, TweenCallback cb, void* data) {
    if (!t) return NULL;
    t->on_update = cb;
    return t;
}

Tween* tween_on_complete(Tween* t, TweenCallback cb, void* data) {
    if (!t) return NULL;
    t->on_complete = cb;
    t->user_data = data;
    return t;
}

void tween_start(Tween* t) {
    if (!t) return;
    t->playing = true;
    t->paused = false;
    t->elapsed = 0;
}

void tween_stop(Tween* t) {
    if (!t) return;
    t->playing = false;
}

void tween_pause(Tween* t) {
    if (!t) return;
    t->paused = true;
}

void tween_resume(Tween* t) {
    if (!t) return;
    t->paused = false;
}

void tween_kill(Tween* t) {
    if (!t) return;
    t->killed = true;
    t->playing = false;
}

void tween_kill_all(TweenManager* mgr) {
    if (!mgr) return;
    for (int i = 0; i < mgr->max_tweens; i++) {
        tween_kill(&mgr->tweens[i]);
    }
}

Tween* tween_chain(Tween* first, Tween* second) {
    if (!first) return second;
    first->chain_next = second;
    return first;
}

Tween* tween_then(Tween* first, Tween* second) {
    return tween_chain(first, second);
}

void tween_manager_update(TweenManager* mgr, float delta_time) {
    if (!mgr) return;
    
    for (int i = 0; i < mgr->max_tweens; i++) {
        Tween* t = &mgr->tweens[i];
        
        if (!t->playing || t->paused || t->killed) continue;
        
        // Handle delay
        if (t->elapsed < t->delay) {
            t->elapsed += delta_time;
            continue;
        }
        
        float elapsed = t->elapsed - t->delay;
        float duration = t->duration / t->speed;
        float progress = elapsed / duration;
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            t->playing = false;
            
            if (t->on_complete) {
                t->on_complete(t->user_data);
            }
            
            if (t->chain_next && !t->chain_next->playing) {
                tween_start(t->chain_next);
            }
            
            if (t->auto_kill) {
                t->killed = true;
            }
        }
        
        // Apply easing
        float t_value = t->easing_func(progress);
        
        // Update target (simplified - would need proper node access)
        // Node* node = (Node*)t->target;
        // if (t->properties & TWEEN_POS_X) node->transform.position.x = lerp(t->from_pos.x, t->to_pos.x, t_value);
        
        if (t->on_update) {
            t->on_update(t->user_data);
        }
        
        t->elapsed += delta_time;
    }
}
