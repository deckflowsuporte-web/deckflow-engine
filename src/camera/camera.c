/**
 * @file camera.c
 * @brief Camera System Implementation
 */

#include "camera.h"
#include <stdlib.h>
#include <math.h>

Camera* camera_create(const char* name) {
    (void)name;
    Camera* cam = (Camera*)calloc(1, sizeof(Camera));
    if (!cam) return NULL;
    
    cam->position = vector2_zero();
    cam->zoom = 1.0f;
    cam->rotation = 0.0f;
    cam->viewport_width = 640;
    cam->viewport_height = 480;
    cam->mode = CAMERA_MODE_FIXED;
    cam->target = NULL;
    cam->offset = vector2_zero();
    cam->smoothing = 0.1f;
    cam->shake.intensity = 0.0f;
    cam->shake.duration = 0.0f;
    cam->shake.elapsed = 0.0f;
    cam->culling_enabled = false;
    
    return cam;
}

void camera_free(Camera* cam) {
    if (cam) {
        free(cam);
    }
}

void camera_set_viewport(Camera* cam, int width, int height) {
    if (!cam) return;
    cam->viewport_width = width;
    cam->viewport_height = height;
}

void camera_set_position(Camera* cam, Vector2 pos) {
    if (!cam) return;
    cam->position = pos;
}

void camera_set_position_xy(Camera* cam, float x, float y) {
    if (!cam) return;
    cam->position.x = x;
    cam->position.y = y;
}

Vector2 camera_get_position(Camera* cam) {
    if (!cam) return vector2_zero();
    return cam->position;
}

void camera_set_zoom(Camera* cam, float zoom) {
    if (!cam) return;
    cam->zoom = zoom > 0.01f ? zoom : 0.01f;
}

void camera_zoom_in(Camera* cam, float delta) {
    if (!cam) return;
    camera_set_zoom(cam, cam->zoom + delta);
}

void camera_zoom_out(Camera* cam, float delta) {
    if (!cam) return;
    camera_set_zoom(cam, cam->zoom - delta);
}

void camera_set_rotation(Camera* cam, float radians) {
    if (!cam) return;
    cam->rotation = radians;
}

void camera_rotate(Camera* cam, float delta) {
    if (!cam) return;
    cam->rotation += delta;
}

void camera_set_mode(Camera* cam, CameraMode mode) {
    if (!cam) return;
    cam->mode = mode;
}

void camera_follow(Camera* cam, Node* target) {
    if (!cam) return;
    cam->target = target;
    cam->mode = CAMERA_MODE_FOLLOW;
}

void camera_follow_with_offset(Camera* cam, Node* target, Vector2 offset) {
    if (!cam) return;
    cam->target = target;
    cam->offset = offset;
    cam->mode = CAMERA_MODE_FOLLOW;
}

void camera_set_smoothing(Camera* cam, float factor) {
    if (!cam) return;
    cam->smoothing = factor < 0 ? 0 : (factor > 1 ? 1 : factor);
}

void camera_set_bounds(Camera* cam, float min_x, float min_y, float max_x, float max_y) {
    if (!cam) return;
    cam->bounds_min_x = min_x;
    cam->bounds_min_y = min_y;
    cam->bounds_max_x = max_x;
    cam->bounds_max_y = max_y;
    cam->mode = CAMERA_MODE_BOUNDS;
}

void camera_shake(Camera* cam, float intensity, float duration) {
    if (!cam) return;
    cam->shake.intensity = intensity;
    cam->shake.duration = duration;
    cam->shake.elapsed = 0.0f;
}

void camera_update_shake(Camera* cam, float delta_time) {
    if (!cam) return;
    if (cam->shake.elapsed >= cam->shake.duration) {
        cam->shake.offset = vector2_zero();
        return;
    }
    
    cam->shake.elapsed += delta_time;
    float progress = cam->shake.elapsed / cam->shake.duration;
    float current_intensity = cam->shake.intensity * (1.0f - progress);
    
    cam->shake.offset.x = (rand() % 100 - 50) / 50.0f * current_intensity;
    cam->shake.offset.y = (rand() % 100 - 50) / 50.0f * current_intensity;
}

void camera_update(Camera* cam, float delta_time) {
    if (!cam) return;
    
    // Update shake
    camera_update_shake(cam, delta_time);
    
    // Follow target
    if (cam->mode == CAMERA_MODE_FOLLOW && cam->target) {
        Vector2 target_pos = node_get_global_position(cam->target);
        target_pos = vector2_add(target_pos, cam->offset);
        
        // Smooth follow
        Vector2 diff = vector2_subtract(target_pos, cam->position);
        diff = vector2_scale(diff, cam->smoothing);
        cam->position = vector2_add(cam->position, diff);
    }
    
    // Bounds mode
    if (cam->mode == CAMERA_MODE_BOUNDS && cam->target) {
        // First follow
        Vector2 target_pos = node_get_global_position(cam->target);
        target_pos = vector2_add(target_pos, cam->offset);
        
        Vector2 diff = vector2_subtract(target_pos, cam->position);
        diff = vector2_scale(diff, cam->smoothing);
        cam->position = vector2_add(cam->position, diff);
        
        // Then clamp to bounds
        float half_w = cam->viewport_width / (2.0f * cam->zoom);
        float half_h = cam->viewport_height / (2.0f * cam->zoom);
        
        cam->position.x = cam->position.x < cam->bounds_min_x + half_w 
            ? cam->bounds_min_x + half_w : cam->position.x;
        cam->position.x = cam->position.x > cam->bounds_max_x - half_w 
            ? cam->bounds_max_x - half_w : cam->position.x;
        cam->position.y = cam->position.y < cam->bounds_min_y + half_h 
            ? cam->bounds_min_y + half_h : cam->position.y;
        cam->position.y = cam->position.y > cam->bounds_max_y - half_h 
            ? cam->bounds_max_y - half_h : cam->position.y;
    }
}

Vector2 camera_world_to_screen(Camera* cam, Vector2 world_pos) {
    if (!cam) return world_pos;
    
    Vector2 result;
    result.x = (world_pos.x - cam->position.x) * cam->zoom + cam->viewport_width / 2.0f;
    result.y = (world_pos.y - cam->position.y) * cam->zoom + cam->viewport_height / 2.0f;
    
    result = vector2_add(result, cam->shake.offset);
    
    return result;
}

Vector2 camera_screen_to_world(Camera* cam, Vector2 screen_pos) {
    if (!cam) return screen_pos;
    
    Vector2 result;
    result.x = (screen_pos.x - cam->viewport_width / 2.0f) / cam->zoom + cam->position.x;
    result.y = (screen_pos.y - cam->viewport_height / 2.0f) / cam->zoom + cam->position.y;
    
    return result;
}

bool camera_is_point_visible(Camera* cam, Vector2 world_pos) {
    if (!cam) return false;
    
    Vector2 screen = camera_world_to_screen(cam, world_pos);
    return screen.x >= 0 && screen.x <= cam->viewport_width &&
           screen.y >= 0 && screen.y <= cam->viewport_height;
}

bool camera_is_rect_visible(Camera* cam, Vector2 pos, float width, float height) {
    if (!cam) return false;
    
    Vector2 center = camera_world_to_screen(cam, pos);
    float half_w = (width / 2.0f) * cam->zoom;
    float half_h = (height / 2.0f) * cam->zoom;
    
    float screen_left = center.x - half_w;
    float screen_right = center.x + half_w;
    float screen_top = center.y - half_h;
    float screen_bottom = center.y + half_h;
    
    return !(screen_right < 0 || screen_left > cam->viewport_width ||
             screen_bottom < 0 || screen_top > cam->viewport_height);
}
