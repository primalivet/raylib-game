#ifndef PHYSICS_H
#define PHYSICS_H
#include <raylib.h>
#include "dynlist.h"

typedef struct {
  Rectangle aabb; // aabb = axis aligned bounding box
  Vector2 velocity;
  Vector2 acceleration;
  bool is_kinematic;
  bool is_active;
} physics_body;

typedef struct {
  float gravity;
  dynlist *bodies;
} physics_state;

void physics_init(float gravity);
void physics_deinit();
physics_body *physics_get_body(size_t id);
size_t physics_add_body(Vector2 velocity, Vector2 acceleration, Rectangle aabb, bool is_kinematic);
void physics_update();

#endif
