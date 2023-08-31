#include <math.h>
#include <raylib.h>

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

Vector2 normalize_vector2(Vector2 v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  Vector2 normalized = {v.x / length, v.y / length};
  return normalized;
}
