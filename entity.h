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
size_t entities_add_entity(Color color, Vector2 velocity, Vector2 acceleration, Rectangle aabb, bool is_kinematic);

#endif
