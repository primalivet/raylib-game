#include "vector2.h"

#ifndef PHYSICS_H
#define PHYSICS_H

typedef struct {
  vector2_t position;
  vector2_t direction;
  vector2_t velocity;
  float     speed;
  float     friction;
  Rectangle aabb;
  int       reset_dir_frames_delay;
} physics_body_t;

typedef struct {
  int bodies_count;
  const char *enemies_path;
} physics_options_t;

typedef struct {
  int            bodies_active;
  int            bodies_count;
  const char    *enemies_path;
  physics_body_t *bodies;
} physics_t;

void physics_init(physics_t *physics, physics_options_t *physics_options);
void physics_free(physics_t *physics);
void physics_update(physics_t *physics);
void physics_draw_TEMP(physics_t *physics);
/*
#include <raylib.h>
#include "dynlist.h"
#include "level.h"

typedef struct {
  Vector2 position;           // Logical position, viewed as the center of the body (AABB will be centered around this position)
  Rectangle aabb;             // AABB = axis aligned bounding box
  Vector2 direction;          // Normalized direction vector
  Vector2 velocity;           // Current velocity (speed + direction)
  Vector2 acceleration;       // Acceleration from any force
  float acceleration_factor;   // Factor/multiplyer of acceleration
  float friction;             // Friction coefficient between 0.0f (no friction) or 1.0f (very high friction)
  float max_speed;
  bool is_kinematic;          // Is the body controlled by input? If false, the body is dynamic and affected more by other forces
  bool is_active;
  size_t entity_id;
} physics_body;

typedef struct {
  float gravity;
  dynlist *bodies;
} physics_state;

void physics_init(float gravity);
void physics_deinit();
physics_body *physics_get_body(size_t id);
size_t physics_add_body(Vector2 position, float width, float height, Vector2 direction, Vector2 velocity, Vector2 acceleration, float acceleration_factor, float friction, float max_speed, bool is_kinematic, size_t entity_id);
void physics_update(level *level);
*/

#endif
