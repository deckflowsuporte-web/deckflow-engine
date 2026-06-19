/**
 * @file camera.h
 * @brief Camera System - Viewport, zoom, and camera following
 */

#ifndef DECKFLOW_CAMERA_H
#define DECKFLOW_CAMERA_H

#include "../math/vector2.h"
#include "../nodes/node.h"

// Camera modes
typedef enum {
    CAMERA_MODE_FIXED,      // Static camera
    CAMERA_MODE_FOLLOW,     // Follows a target node
    CAMERA_MODE_BOUNDS      // Follow with bounds limits
} CameraMode;

// Camera shake
typedef struct {
    float intensity;         // Shake strength
    float duration;         // Total duration
    float elapsed;          // Time elapsed
    Vector2 offset;         // Current offset
} CameraShake;

// Camera structure
typedef struct {
    Vector2 position;        // Camera center position
    float zoom;             // Zoom level (1.0 = normal)
    float rotation;          // Rotation in radians
    int viewport_width;     // Viewport dimensions
    int viewport_height;
    CameraMode mode;        // Camera mode
    
    // Follow mode
    Node* target;           // Node to follow
    Vector2 offset;         // Offset from target
    float smoothing;        // Lerp factor (0 = instant, 1 = max lag)
    
    // Bounds mode
    float bounds_min_x;
    float bounds_min_y;
    float bounds_max_x;
    float bounds_max_y;
    
    // Shake effect
    CameraShake shake;
    
    // Culling
    bool culling_enabled;
} Camera;

// Create camera
Camera* camera_create(const char* name);

// Free camera
void camera_free(Camera* cam);

// Set viewport size
void camera_set_viewport(Camera* cam, int width, int height);

// Set camera position
void camera_set_position(Camera* cam, Vector2 pos);
void camera_set_position_xy(Camera* cam, float x, float y);

// Get camera position
Vector2 camera_get_position(Camera* cam);

// Set zoom level
void camera_set_zoom(Camera* cam, float zoom);
void camera_zoom_in(Camera* cam, float delta);
void camera_zoom_out(Camera* cam, float delta);

// Set rotation
void camera_set_rotation(Camera* cam, float radians);
void camera_rotate(Camera* cam, float delta);

// Set camera mode
void camera_set_mode(Camera* cam, CameraMode mode);

// Follow mode
void camera_follow(Camera* cam, Node* target);
void camera_follow_with_offset(Camera* cam, Node* target, Vector2 offset);
void camera_set_smoothing(Camera* cam, float factor);

// Bounds mode
void camera_set_bounds(Camera* cam, float min_x, float min_y, float max_x, float max_y);

// Shake effects
void camera_shake(Camera* cam, float intensity, float duration);
void camera_update_shake(Camera* cam, float delta_time);

// Update camera (call once per frame)
void camera_update(Camera* cam, float delta_time);

// Convert world to screen coordinates
Vector2 camera_world_to_screen(Camera* cam, Vector2 world_pos);

// Convert screen to world coordinates
Vector2 camera_screen_to_world(Camera* cam, Vector2 screen_pos);

// Check if point is visible
bool camera_is_point_visible(Camera* cam, Vector2 world_pos);

// Check if rectangle is visible
bool camera_is_rect_visible(Camera* cam, Vector2 pos, float width, float height);

#endif // DECKFLOW_CAMERA_H
