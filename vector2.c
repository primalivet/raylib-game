#include <math.h>
#include <raylib.h>
#include "vector2.h"

IntVector2 intvec2_from_vec2(Vector2 vec) {
  return (IntVector2) { (int)vec.x, (int)vec.y };
}

Vector2 vec2_from_rect(Rectangle rect) {
  return (Vector2) { rect.x, rect.y };
}

Vector2 normalize_vector2(Vector2 v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  Vector2 normalized = {v.x / length, v.y / length};
  return normalized;
}

Vector2 clamp_vector2(Vector2 position, Rectangle boundary) 
{
  Vector2 clamped = position;
  if (clamped.x < boundary.x) clamped.x = boundary.x;
  if (clamped.y < boundary.y) clamped.y = boundary.y;
  if (clamped.x > boundary.width) clamped.x = boundary.width;
  if (clamped.y > boundary.height) clamped.y = boundary.height;
  return clamped;
}

Vector2 add_vector2(Vector2 a, Vector2 b) 
{
  Vector2 sum = a;
  sum.x += b.x;
  sum.y += b.y;
  return sum;
}

Vector2 add_x_vector2(Vector2 a, Vector2 b) 
{
  Vector2 sum = a;
  sum.x += b.x;
  return sum;
}

Vector2 add_y_vector2(Vector2 a, Vector2 b) 
{
  Vector2 sum = a;
  sum.y += b.y;
  return sum;
}

Vector2 seek_vector2(Vector2 origin, Vector2 target) {
  Vector2 seek = {target.x - origin.x, target.y - origin.y};
  return seek;
}

/**
 * Get the magnitude "straight line distance" (from 0,0) of a vector
*/
float magnitude_vector2(Vector2 v) 
{
  return sqrt(v.x * v.x + v.y * v.y);
}

Vector2 mult_vector2(Vector2 v, float x) 
{
  Vector2 mult = v;
  mult.x *= x;
  mult.y *= x;
  return mult;
}

Vector2 mult_x_vector2(Vector2 v, float x) 
{
  Vector2 mult = v;
  mult.x *= x;
  return mult;
}

Vector2 mult_y_vector2(Vector2 v, float x) 
{
  Vector2 mult = v;
  mult.y *= x;
  return mult;
}

float euclidean_distance(Vector2 v1, Vector2 v2) 
{
  float a = v2.x - v1.x;
  float b = v2.y - v1.y;
  return sqrt(a * a + b * b);
}

float manhattan_distance(Vector2 v1, Vector2 v2) 
{
  float a = fabs(v2.x - v1.x);
  float b = fabs(v2.y - v1.y);
  return a + b;
}


// TODO: move to better place
Rectangle set_rect_origin_from_vector2(Rectangle r , Vector2 v)
{
  r.x = v.x;
  r.y = v.y;
  return r;
}

Vector2 get_vec_from_rect(Rectangle r)
{
  return (Vector2){r.x, r.y};
}
