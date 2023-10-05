#include "dynlist.h"
#include "entity.h"
#include "physics.h"

static entities_state state;

void entities_init()
{
  state.entities  = dynlist_allocate(sizeof(entity), 0);
}

void entities_deinit()
{
  dynlist_free(state.entities);
  state.entities = NULL;
}

size_t entities_count()
{
  return state.entities->length;
}

entity *entities_get_entity(size_t id)
{
  return dynlist_get_at(state.entities, id);
}

size_t entities_add_entity(Color color,Vector2 position, float width, float height, Vector2 direction,Vector2 velocity, Vector2 acceleration, float acceleration_factor, float friction, float max_speed,  bool is_kinematic)
{
  size_t id = state.entities->length;
  for (size_t i = 0; i < state.entities->length; ++i)
  {
    entity *entity = dynlist_get_at(state.entities, i);
    if (!entity->is_active) 
    {
      id = i;
      break;
    }
  }

  size_t body_id = physics_add_body(position, width, height, direction, velocity, acceleration, acceleration_factor, friction, max_speed, is_kinematic, id);

  entity new_entity = {
    .color    = color,
    .body_id  = body_id,
    .waypoints = NULL,
    .current_waypoint_index = 0,
    .is_active = true
  };

  if (id == state.entities->length) {
    dynlist_append(state.entities, &new_entity);
  } else {
    entity *existing_entity = dynlist_get_at(state.entities, id);
    *existing_entity = new_entity;
  }

  return id;
}
