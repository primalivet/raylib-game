#ifndef VECTOR2_H
#define VECTOR2_H
#include <raylib.h>

float vector2_magnitude(Vector2 v);
Vector2 vector2_sub(Vector2 a, Vector2 b);
Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_mult_by_scalar(Vector2 v, float x);
Vector2 vector2_div_by_scalar(Vector2 v, float x);
Vector2 vector2_add_by_scalar(Vector2 v, float x);
float vector2_euclidean_distance(Vector2 v1, Vector2 v2);
float vector2_manhattan_distance(Vector2 v1, Vector2 v2);
Vector2 vector2_normalize(Vector2 v);

#endif
