/**
 * @file transform2d.c
 * @brief 2D Transform implementation
 */

#include "transform2d.h"
#include <math.h>

#define M_PI 3.14159265358979323846

Transform2D transform2d_new(void) {
    Transform2D t = {
        .position = {0, 0},
        .rotation = 0,
        .scale = {1, 1},
        .origin = {0, 0},
        .dirty = false
    };
    return t;
}

Transform2D transform2d_identity(void) {
    return transform2d_new();
}

Transform2D transform2d_from_position(Vector2 pos) {
    Transform2D t = transform2d_new();
    t.position = pos;
    return t;
}

Transform2D transform2d_new_full(Vector2 pos, float rot, Vector2 scale) {
    Transform2D t = transform2d_new();
    t.position = pos;
    t.rotation = rot;
    t.scale = scale;
    return t;
}

Transform2D transform2d_clone(Transform2D t) {
    return t;
}

Vector2 transform2d_xform_vector2(Transform2D t, Vector2 v) {
    float cos_r = cosf(t.rotation);
    float sin_r = sinf(t.rotation);
    
    // Apply rotation, then scale, then translate
    float x = (v.x * t.scale.x * cos_r - v.y * t.scale.y * sin_r) + t.position.x;
    float y = (v.x * t.scale.x * sin_r + v.y * t.scale.y * cos_r) + t.position.y;
    
    return vector2_new(x, y);
}

Vector2 transform2d_xform_inv_vector2(Transform2D t, Vector2 v) {
    // Inverse transform (to local space)
    Vector2 p = vector2_sub(v, t.position);
    
    float cos_r = cosf(-t.rotation);
    float sin_r = sinf(-t.rotation);
    
    float x = (p.x * cos_r - p.y * sin_r) / t.scale.x;
    float y = (p.x * sin_r + p.y * cos_r) / t.scale.y;
    
    return vector2_new(x, y);
}

Vector2 transform2d_get_axis_x(Transform2D t) {
    return vector2_new(cosf(t.rotation), sinf(t.rotation));
}

Vector2 transform2d_get_axis_y(Transform2D t) {
    return vector2_new(-sinf(t.rotation), cosf(t.rotation));
}

Vector2 transform2d_get_translation(Transform2D t) {
    return t.position;
}

void transform2d_set_position(Transform2D* t, Vector2 pos) {
    t->position = pos;
    t->dirty = true;
}

void transform2d_set_rotation(Transform2D* t, float rot) {
    t->rotation = rot;
    t->dirty = true;
}

void transform2d_set_scale(Transform2D* t, Vector2 scale) {
    t->scale = scale;
    t->dirty = true;
}

void transform2d_set_origin(Transform2D* t, Vector2 origin) {
    t->origin = origin;
    t->dirty = true;
}

void transform2d_translate(Transform2D* t, Vector2 offset) {
    t->position = vector2_add(t->position, offset);
    t->dirty = true;
}

void transform2d_rotate(Transform2D* t, float angle) {
    t->rotation += angle;
    t->dirty = true;
}

void transform2d_scale_by(Transform2D* t, Vector2 scale) {
    t->scale.x *= scale.x;
    t->scale.y *= scale.y;
    t->dirty = true;
}

float transform2d_get_rotation_degrees(Transform2D t) {
    return t.rotation * (180.0f / M_PI);
}

void transform2d_rotate_degrees(Transform2D* t, float degrees) {
    t->rotation += degrees * (M_PI / 180.0f);
    t->dirty = true;
}

void transform2d_set_rotation_degrees(Transform2D* t, float degrees) {
    t->rotation = degrees * (M_PI / 180.0f);
    t->dirty = true;
}

Transform2D transform2d_inverted(Transform2D t) {
    // Create inverted transform
    float cos_r = cosf(-t.rotation);
    float sin_r = sinf(-t.rotation);
    
    Transform2D inv = t;
    inv.rotation = -t.rotation;
    inv.scale = vector2_new(1.0f / t.scale.x, 1.0f / t.scale.y);
    inv.position = vector2_new(
        -(t.position.x * cos_r - t.position.y * sin_r) / t.scale.x,
        -(t.position.x * sin_r + t.position.y * cos_r) / t.scale.y
    );
    
    return inv;
}

Transform2D transform2d_linear_interpolate(Transform2D a, Transform2D b, float t) {
    Transform2D result;
    result.position = vector2_lerp(a.position, b.position, t);
    result.rotation = a.rotation + (b.rotation - a.rotation) * t;
    result.scale = vector2_lerp(a.scale, b.scale, t);
    result.origin = vector2_lerp(a.origin, b.origin, t);
    result.dirty = true;
    return result;
}
