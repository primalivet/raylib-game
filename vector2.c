#include <math.h>
#include <raylib.h>
#include "vector2.h"

Vector2 vector2_normalize(Vector2 v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  Vector2 normalized = {v.x / length, v.y / length};
  return normalized;
}

Vector2 vector2_add(Vector2 a, Vector2 b) 
{
  Vector2 sum = a;
  sum.x += b.x;
  sum.y += b.y;
  return sum;
}

Vector2 vector2_sub(Vector2 a, Vector2 b) 
{
  Vector2 sum = a;
  sum.x -= b.x;
  sum.y -= b.y;
  return sum;
}

/**
 * Get the magnitude "straight line distance" (from 0,0) of a vector
*/
float vector2_magnitude(Vector2 v) 
{
  return sqrt(v.x * v.x + v.y * v.y);
}

Vector2 vector2_mult_by_scalar(Vector2 v, float x) 
{
  Vector2 mult = v;
  mult.x *= x;
  mult.y *= x;
  return mult;
}

Vector2 vector2_div_by_scalar(Vector2 a, float x) 
{
  Vector2 div = a;
  div.x /= x;
  div.y /= x;
  return div;
}

Vector2 vector2_add_by_scalar(Vector2 a, float x) 
{
  Vector2 div = a;
  div.x += x;
  div.y += x;
  return div;
}

float vector2_euclidean_distance(Vector2 v1, Vector2 v2) 
{
  float a = v2.x - v1.x;
  float b = v2.y - v1.y;
  return sqrt(a * a + b * b);
}

float vector2_manhattan_distance(Vector2 v1, Vector2 v2) 
{
  float a = fabs(v2.x - v1.x);
  float b = fabs(v2.y - v1.y);
  return a + b;
}
