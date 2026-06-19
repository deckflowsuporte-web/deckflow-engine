/**
 * @file vector2.h
 * @brief 2D Vector math utilities
 */
#ifndef VECTOR2_H
#define VECTOR2_H

#include <math.h>
#include <stdbool.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

// Constructors
Vector2 vector2_new(float x, float y);
Vector2 vector2_zero(void);
Vector2 vector2_one(void);
Vector2 vector2_up(void);
Vector2 vector2_down(void);
Vector2 vector2_left(void);
Vector2 vector2_right(void);

// Operations
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_sub(Vector2 a, Vector2 b);
Vector2 vector2_mul(Vector2 v, float scalar);
Vector2 vector2_div(Vector2 v, float scalar);
Vector2 vector2_negate(Vector2 v);
Vector2 vector2_normalized(Vector2 v);
Vector2 vector2_rotated(Vector2 v, float angle);
Vector2 vector2_lerp(Vector2 a, Vector2 b, float t);
Vector2 vector2_clamp(Vector2 v, Vector2 min, Vector2 max);

// Scalar operations
float vector2_dot(Vector2 a, Vector2 b);
float vector2_cross(Vector2 a, Vector2 b);
float vector2_length(Vector2 v);
float vector2_length_squared(Vector2 v);
float vector2_distance(Vector2 a, Vector2 b);
float vector2_angle(Vector2 v);
float vector2_angle_to(Vector2 a, Vector2 b);
float vector2_angle_to_point(Vector2 a, Vector2 b);

// Utility
bool vector2_equals(Vector2 a, Vector2 b);
bool vector2_is_normalized(Vector2 v);
bool vector2_is_zero(Vector2 v);
Vector2 vector2_string_to(const char* str);

#endif // VECTOR2_H
