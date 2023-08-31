#ifndef ENTITY_H
#define ENTITY_H
#include "dynlist.h"
#include "physics.h"

typedef struct {
  Color color; // TODO: remove when replaced by sprite
  size_t body_id;
  bool is_active;
} entity;

typedef struct {
  dynlist *entities;
} entities_state;

void entities_init();
void entities_deinit();
size_t entities_count();
entity *entities_get_entity(size_t id);
size_t entities_add_entity(Color color, Rectangle aabb,Vector2 direction, Vector2 velocity, Vector2 acceleration, float acceleration_factor, float friction, float max_speed,  bool is_kinematic);

#endif
