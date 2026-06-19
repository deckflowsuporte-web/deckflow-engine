/**
 * @file vector2.c
 * @brief 2D Vector math implementation
 */

#include "vector2.h"
#include <string.h>
#include <stdio.h>

Vector2 vector2_new(float x, float y) {
    Vector2 v = {x, y};
    return v;
}

Vector2 vector2_zero(void) {
    return vector2_new(0, 0);
}

Vector2 vector2_one(void) {
    return vector2_new(1, 1);
}

Vector2 vector2_up(void) {
    return vector2_new(0, -1);
}

Vector2 vector2_down(void) {
    return vector2_new(0, 1);
}

Vector2 vector2_left(void) {
    return vector2_new(-1, 0);
}

Vector2 vector2_right(void) {
    return vector2_new(1, 0);
}

Vector2 vector2_add(Vector2 a, Vector2 b) {
    return vector2_new(a.x + b.x, a.y + b.y);
}

Vector2 vector2_sub(Vector2 a, Vector2 b) {
    return vector2_new(a.x - b.x, a.y - b.y);
}

Vector2 vector2_mul(Vector2 v, float scalar) {
    return vector2_new(v.x * scalar, v.y * scalar);
}

Vector2 vector2_div(Vector2 v, float scalar) {
    if (scalar == 0) return vector2_zero();
    return vector2_new(v.x / scalar, v.y / scalar);
}

Vector2 vector2_negate(Vector2 v) {
    return vector2_new(-v.x, -v.y);
}

Vector2 vector2_normalized(Vector2 v) {
    float len = vector2_length(v);
    if (len > 0) {
        return vector2_div(v, len);
    }
    return vector2_zero();
}

Vector2 vector2_rotated(Vector2 v, float angle) {
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    return vector2_new(
        v.x * cos_a - v.y * sin_a,
        v.x * sin_a + v.y * cos_a
    );
}

Vector2 vector2_lerp(Vector2 a, Vector2 b, float t) {
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return vector2_new(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    );
}

Vector2 vector2_clamp(Vector2 v, Vector2 min, Vector2 max) {
    return vector2_new(
        v.x < min.x ? min.x : (v.x > max.x ? max.x : v.x),
        v.y < min.y ? min.y : (v.y > max.y ? max.y : v.y)
    );
}

float vector2_dot(Vector2 a, Vector2 b) {
    return a.x * b.x + a.y * b.y;
}

float vector2_cross(Vector2 a, Vector2 b) {
    return a.x * b.y - a.y * b.x;
}

float vector2_length(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

float vector2_length_squared(Vector2 v) {
    return v.x * v.x + v.y * v.y;
}

float vector2_distance(Vector2 a, Vector2 b) {
    return vector2_length(vector2_sub(b, a));
}

float vector2_angle(Vector2 v) {
    return atan2f(v.y, v.x);
}

float vector2_angle_to(Vector2 a, Vector2 b) {
    return atan2f(b.y - a.y, b.x - a.x);
}

float vector2_angle_to_point(Vector2 a, Vector2 b) {
    return vector2_angle_to(a, b);
}

bool vector2_equals(Vector2 a, Vector2 b) {
    const float EPSILON = 0.00001f;
    return fabsf(a.x - b.x) < EPSILON && fabsf(a.y - b.y) < EPSILON;
}

bool vector2_is_normalized(Vector2 v) {
    return fabsf(vector2_length_squared(v) - 1.0f) < 0.00001f;
}

bool vector2_is_zero(Vector2 v) {
    return vector2_equals(v, vector2_zero());
}
