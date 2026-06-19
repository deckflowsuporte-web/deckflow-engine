/**
 * @file animation.h
 * @brief Animation System - Spritesheets, Keyframes, Tweening
 */

#ifndef DECKFLOW_ANIMATION_H
#define DECKFLOW_ANIMATION_H

#include <stdbool.h>
#include "../math/vector2.h"
#include "../nodes/node.h"

// Animation playback direction
typedef enum {
    ANIM_FORWARD = 1,
    ANIM_BACKWARD = -1,
    ANIM_PING_PONG = 0  // Special: handled in update
} AnimDirection;

// Animation play mode
typedef enum {
    ANIM_PLAY_ONCE,      // Play once and stop
    ANIM_PLAY_LOOP,       // Loop forever
    ANIM_PLAY_PING_PONG,  // Forward then backward
    ANIM_PLAY_HOLD        // Hold on last frame
} AnimPlayMode;

// Tween easing types
typedef enum {
    EASE_LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    EASE_BOUNCE,
    EASE_ELASTIC,
    EASE_SINE
} EasingType;

// Interpolation type for keyframes
typedef enum {
    INTERP_CONSTANT,   // No interpolation
    INTERP_LINEAR,     // Linear interpolation
    INTERP_SMOOTH,     // Cubic smoothstep
    INTERP_EASE,       // Ease in/out
    INTERP_CUBIC       // Cubic bezier
} InterpType;

// ============================================
// SPRITE SHEET
// ============================================

typedef struct SpriteFrame {
    int x, y;           // Position in spritesheet
    int width, height;   // Frame size
    float duration;      // Duration in seconds (0 = use default)
} SpriteFrame;

typedef struct {
    void* texture;           // SDL_Texture*
    int sheet_width;
    int sheet_height;
    int frame_width;
    int frame_height;
    int columns;
    int rows;
    int frame_count;
    SpriteFrame* frames;
} SpriteSheet;

// Create sprite sheet from image
SpriteSheet* spritesheet_create(void* renderer, const char* filepath, 
                                int frame_width, int frame_height);
void spritesheet_free(SpriteSheet* sheet);

// Get frame by index
SpriteFrame* spritesheet_get_frame(SpriteSheet* sheet, int index);

// ============================================
// ANIMATION CLIP
// ============================================

typedef struct AnimationClip {
    char* name;
    
    // Frames
    int* frame_indices;     // Array of frame indices
    int frame_count;
    float fps;              // Frames per second
    
    // Playback
    AnimPlayMode play_mode;
    
    // Events
    char** event_names;     // Event triggers
    int* event_frames;      // Frame at which event triggers
    int event_count;
    
    struct AnimationClip* next;
} AnimationClip;

// Create animation clip
AnimationClip* anim_clip_create(const char* name, int frame_count);
void anim_clip_add_frame(AnimationClip* clip, int frame_index);
void anim_clip_set_fps(AnimationClip* clip, float fps);
void anim_clip_set_play_mode(AnimationClip* clip, AnimPlayMode mode);
void anim_clip_add_event(AnimationClip* clip, const char* event_name, int frame);

// ============================================
// ANIMATED SPRITE
// ============================================

typedef struct AnimatedSprite {
    Node* node;
    
    // Sprite sheet
    SpriteSheet* sheet;
    void* texture;  // Current texture (from sheet frame)
    
    // Current frame
    int current_frame;
    float frame_time;
    float frame_duration;
    
    // Clip
    AnimationClip* current_clip;
    AnimationClip* default_clip;
    
    // Playback
    bool playing;
    AnimDirection direction;
    bool flip_h;
    bool flip_v;
    
    // Callbacks
    void (*on_frame_change)(struct AnimatedSprite* sprite, int frame);
    void (*on_animation_end)(struct AnimatedSprite* sprite);
    void (*on_event)(struct AnimatedSprite* sprite, const char* event);
    
    // Callbacks user data
    void* callback_data;
    
    // Precomputed
    SDL_Rect src_rect;  // Current frame rect in sheet
} AnimatedSprite;

// Create animated sprite
AnimatedSprite* animsprite_create(Node* parent, SpriteSheet* sheet);
void animsprite_free(AnimatedSprite* sprite);

// Set sprite sheet
void animsprite_set_sheet(AnimatedSprite* sprite, SpriteSheet* sheet);

// Play animation
void animsprite_play(AnimatedSprite* sprite, AnimationClip* clip);
void animsprite_play_default(AnimatedSprite* sprite);
void animsprite_stop(AnimatedSprite* sprite);
void animsprite_pause(AnimatedSprite* sprite);
void animsprite_resume(AnimatedSprite* sprite);

// Set frame directly
void animsprite_set_frame(AnimatedSprite* sprite, int frame);

// Playback control
void animsprite_set_flip(AnimatedSprite* sprite, bool h, bool v);
void animsprite_set_speed(AnimatedSprite* sprite, float speed);
float animsprite_get_speed(AnimatedSprite* sprite);

// Update (call every frame)
void animsprite_update(AnimatedSprite* sprite, float delta_time);

// ============================================
// TWEEN SYSTEM
// ============================================

typedef void (*TweenCallback)(void* user_data);
typedef float (*EasingFunction)(float t);

// Tweenable properties
typedef enum {
    TWEEN_POS_X = 1 << 0,
    TWEEN_POS_Y = 1 << 1,
    TWEEN_POS = TWEEN_POS_X | TWEEN_POS_Y,
    TWEEN_SCALE_X = 1 << 2,
    TWEEN_SCALE_Y = 1 << 3,
    TWEEN_SCALE = TWEEN_SCALE_X | TWEEN_SCALE_Y,
    TWEEN_ROTATION = 1 << 4,
    TWEEN_COLOR_R = 1 << 5,
    TWEEN_COLOR_G = 1 << 6,
    TWEEN_COLOR_B = 1 << 7,
    TWEEN_COLOR_A = 1 << 8,
    TWEEN_COLOR = TWEEN_COLOR_R | TWEEN_COLOR_G | TWEEN_COLOR_B | TWEEN_COLOR_A,
    TWEEN_ALL = 0xFFFF
} TweenProperty;

#define TWEEN_DELAY_MAX 100

typedef struct Tween {
    // Target
    void* target;          // Node or custom target
    TweenProperty properties;
    
    // From/To values
    Vector2 from_pos;
    Vector2 to_pos;
    Vector2 from_scale;
    Vector2 to_scale;
    float from_rotation;
    float to_rotation;
    float from_color[4];
    float to_color[4];
    
    // Timing
    float duration;
    float elapsed;
    float delay;
    float speed;  // Playback speed multiplier
    
    // Easing
    EasingFunction easing_func;
    
    // Callbacks
    TweenCallback on_start;
    TweenCallback on_update;
    TweenCallback on_complete;
    void* user_data;
    
    // State
    bool playing;
    bool paused;
    bool killed;
    bool auto_kill;  // Kill when complete
    
    // For sequences
    struct Tween* next;
    struct Tween* chain_next;  // Next tween in chain
    
    struct Tween* _next;  // Internal list
} Tween;

// Tween manager
typedef struct {
    Tween* tweens;
    int tween_count;
    int max_tweens;
} TweenManager;

// Create/destroy manager
TweenManager* tween_manager_create(int max_tweens);
void tween_manager_free(TweenManager* mgr);

// Create tweens
Tween* tween_create(TweenManager* mgr, void* target);
Tween* tween_to(TweenManager* mgr, void* target, float duration, ...);
Tween* tween_from(TweenManager* mgr, void* target, float duration, ...);

// Tween properties
Tween* tween_position(Tween* t, Vector2 from, Vector2 to);
Tween* tween_scale(Tween* t, Vector2 from, Vector2 to);
Tween* tween_rotation(Tween* t, float from, float to);
Tween* tween_color(Tween* t, float from_r, float from_g, float from_b, float from_a,
                             float to_r, float to_g, float to_b, float to_a);

// Tween timing
Tween* tween_delay(Tween* t, float delay);
Tween* tween_speed(Tween* t, float speed);
Tween* tween_ease(Tween* t, EasingType ease);

// Tween callbacks
Tween* tween_on_start(Tween* t, TweenCallback cb, void* data);
Tween* tween_on_update(Tween* t, TweenCallback cb, void* data);
Tween* tween_on_complete(Tween* t, TweenCallback cb, void* data);

// Tween control
void tween_start(Tween* t);
void tween_stop(Tween* t);
void tween_pause(Tween* t);
void tween_resume(Tween* t);
void tween_kill(Tween* t);
void tween_kill_all(TweenManager* mgr);

// Chain tweens
Tween* tween_chain(Tween* first, Tween* second);
Tween* tween_then(Tween* first, Tween* second);

// Update all tweens
void tween_manager_update(TweenManager* mgr, float delta_time);

// ============================================
// EASING FUNCTIONS
// ============================================

// Built-in easing functions
float ease_linear(float t);
float ease_in_quad(float t);
float ease_out_quad(float t);
float ease_in_out_quad(float t);
float ease_in_cubic(float t);
float ease_out_cubic(float t);
float ease_in_out_cubic(float t);
float ease_in_sine(float t);
float ease_out_sine(float t);
float ease_in_out_sine(float t);
float ease_bounce(float t);
float ease_elastic(float t);

// Get easing function by type
EasingFunction easing_get_func(EasingType type);

// ============================================
// SKELETAL ANIMATION (Basic)
// ============================================

typedef struct {
    char* name;
    Vector2 position;
    float rotation;
    float scale_x;
    float scale_y;
} BoneKeyframe;

typedef struct {
    char* name;
    BoneKeyframe* keyframes;
    int keyframe_count;
    float duration;
} BoneAnimation;

typedef struct Bone {
    char* name;
    int parent_index;  // -1 for root
    Vector2 local_offset;
    float local_rotation;
    Vector2 bind_position;
    float bind_rotation;
} Bone;

typedef struct Skeleton {
    Bone* bones;
    int bone_count;
} Skeleton;

#endif // DECKFLOW_ANIMATION_H
