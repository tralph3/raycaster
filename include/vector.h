#ifndef VECTOR_H
#define VECTOR_H

#include <raylib.h>
#include <math.h>

Vector2 vec2_add(Vector2 a, Vector2 b) {
    Vector2 result = {
        .x = a.x + b.x,
        .y = a.y + b.y,
    };
    return result;
}

Vector2 vec2_sub(Vector2 a, Vector2 b) {
    Vector2 result = {
        .x = a.x - b.x,
        .y = a.y - b.y,
    };
    return result;
}

Vector2 vec2_mul(Vector2 vec, float value) {
    Vector2 result = {
        .x = vec.x * value,
        .y = vec.y * value,
    };
    return result;
}

Vector2 vec2_div(Vector2 vec, float divisor) {
    Vector2 result = {
        .x = vec.x / divisor,
        .y = vec.y / divisor,
    };
    return result;
}

float vec2_length(Vector2 vec) { return sqrt(pow(vec.x, 2) + pow(vec.y, 2)); }

Vector2 vec2_normalize(Vector2 vec) {
    return vec2_div(vec, vec2_length(vec));
}

Vector2 vec2_rotate(Vector2 vec, float angle) {
    Vector2 result = {
        .x = cos(angle) * vec.x - sin(angle) * vec.y,
        .y = sin(angle) * vec.x + cos(angle) * vec.y,
    };
    return result;
}

#endif
