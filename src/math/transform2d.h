/**
 * @file transform2d.h
 * @brief 2D Transform component (Godot-style)
 */

#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include "vector2.h"

// Transform2D represents position, rotation, scale and origin
typedef struct Transform2D {
    Vector2 position;      // Position in world
    float rotation;        // Rotation in radians
    Vector2 scale;         // Scale (1 = normal)
    Vector2 origin;        // Pivot/origin point
    bool dirty;            // Cache invalidation flag
} Transform2D;

// Constructors
Transform2D transform2d_new(void);
Transform2D transform2d_identity(void);
Transform2D transform2d_from_position(Vector2 pos);
Transform2D transform2d_new_full(Vector2 pos, float rot, Vector2 scale);

// Operations
Transform2D transform2d_clone(Transform2D t);
Transform2D transform2d_inverted(Transform2D t);
Transform2D transform2d_linear_interpolate(Transform2D a, Transform2D b, float t);

// Affine matrix operations
Vector2 transform2d_xform_vector2(Transform2D t, Vector2 v);
Vector2 transform2d_xform_inv_vector2(Transform2D t, Vector2 v);
Vector2 transform2d_get_axis_x(Transform2D t);
Vector2 transform2d_get_axis_y(Transform2D t);
Vector2 transform2d_get_translation(Transform2D t);

// Setters
void transform2d_set_position(Transform2D* t, Vector2 pos);
void transform2d_set_rotation(Transform2D* t, float rot);
void transform2d_set_scale(Transform2D* t, Vector2 scale);
void transform2d_set_origin(Transform2D* t, Vector2 origin);
void transform2d_translate(Transform2D* t, Vector2 offset);
void transform2d_rotate(Transform2D* t, float angle);
void transform2d_scale_by(Transform2D* t, Vector2 scale);

// Utility
float transform2d_get_rotation_degrees(Transform2D t);
void transform2d_rotate_degrees(Transform2D* t, float degrees);
void transform2d_set_rotation_degrees(Transform2D* t, float degrees);
Vector2 transform2d_get_global_position(Transform2D t);
float transform2d_get_global_rotation(Transform2D t);

#endif // TRANSFORM2D_H
