#ifndef VECTOR2_H
#define VECTOR2_H
#include <raylib.h>

typedef struct {
  int x;
  int y;
} IntVector2;

IntVector2 intvec2_from_vec2(Vector2 vec);
Vector2 vec2_from_rect(Rectangle rect);
Vector2 clamp_vector2(Vector2 position, Rectangle boundary);
float magnitude_vector2(Vector2 v);
Vector2 sub_vector2(Vector2 a, Vector2 b);
Vector2 add_vector2(Vector2 a, Vector2 b);
Vector2 add_x_vector2(Vector2 a, Vector2 b);
Vector2 add_y_vector2(Vector2 a, Vector2 b); 
Vector2 mult_vector2(Vector2 v, float x);
Vector2 mult_x_vector2(Vector2 v, float x);
Vector2 mult_y_vector2(Vector2 v, float x);
float euclidean_distance(Vector2 v1, Vector2 v2);
float manhattan_distance(Vector2 v1, Vector2 v2);
Vector2 seek_vector2(Vector2 origin, Vector2 target);
Rectangle set_rect_origin_from_vector2(Rectangle r , Vector2 v);
Vector2 get_vec_from_rect(Rectangle r);
Vector2 normalize_vector2(Vector2 v);

#endif
