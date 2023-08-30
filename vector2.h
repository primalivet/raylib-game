#ifndef VECTOR2_H
#define VECTOR2_H
#include <raylib.h>

Vector2 clamp_vector2(Vector2 position, Rectangle boundary);
Vector2 add_vector2(Vector2 a, Vector2 b);
Vector2 normalize_vector2(Vector2 v);

#endif
