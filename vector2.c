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

Vector2 normalize_vector2(Vector2 v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  Vector2 normalized = {v.x / length, v.y / length};
  return normalized;
}
