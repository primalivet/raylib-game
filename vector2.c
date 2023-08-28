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

