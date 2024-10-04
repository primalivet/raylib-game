#include <math.h>
#include "vector2.h"

vector2_t vector2_normalize(vector2_t v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  vector2_t normalized = {v.x / length, v.y / length};
  return normalized;
}

vector2_t vector2_add(vector2_t a, vector2_t b) 
{
  vector2_t sum = a;
  sum.x += b.x;
  sum.y += b.y;
  return sum;
}

vector2_t vector2_sub(vector2_t a, vector2_t b) 
{
  vector2_t sum = a;
  sum.x -= b.x;
  sum.y -= b.y;
  return sum;
}

/**
 * Get the magnitude "straight line distance" (from 0,0) of a vector
*/
float vector2_magnitude(vector2_t v) 
{
  return sqrt(v.x * v.x + v.y * v.y);
}

vector2_t vector2_mult_by_scalar(vector2_t v, float x) 
{
  vector2_t mult = v;
  mult.x *= x;
  mult.y *= x;
  return mult;
}

vector2_t vector2_div_by_scalar(vector2_t a, float x) 
{
  vector2_t div = a;
  div.x /= x;
  div.y /= x;
  return div;
}

vector2_t vector2_add_by_scalar(vector2_t a, float x) 
{
  vector2_t div = a;
  div.x += x;
  div.y += x;
  return div;
}

float vector2_euclidean_distance(vector2_t v1, vector2_t v2) 
{
  float a = v2.x - v1.x;
  float b = v2.y - v1.y;
  return sqrt(a * a + b * b);
}

float vector2_manhattan_distance(vector2_t v1, vector2_t v2) 
{
  float a = fabs(v2.x - v1.x);
  float b = fabs(v2.y - v1.y);
  return a + b;
}
