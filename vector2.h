#ifndef VECTOR2_H
#define VECTOR2_H

#include <raylib.h>

typedef struct vector2_t {
  float x;
  float y;
} vector2_t;

#define TO_RL_VEC2(v) (*(Vector2*)&(v))
#define TO_MY_VEC2(v) (*(vector2_t*)&(v))

float vector2_magnitude(vector2_t v);
vector2_t vector2_sub(vector2_t a, vector2_t b);
vector2_t vector2_add(vector2_t a, vector2_t b);
vector2_t vector2_mult_by_scalar(vector2_t v, float x);
vector2_t vector2_div_by_scalar(vector2_t v, float x);
vector2_t vector2_add_by_scalar(vector2_t v, float x);
vector2_t vector2_clamp(vector2_t a, float min, float max);
float vector2_euclidean_distance(vector2_t v1, vector2_t v2);
float vector2_manhattan_distance(vector2_t v1, vector2_t v2);
vector2_t vector2_normalize(vector2_t v);

#endif
