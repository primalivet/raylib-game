#include "dynlist.h"
#include "entity.h"
#include "physics.h"

static entities_state state;

void entities_init()
{
  state.entities  = list_create(sizeof(entity), 0);
}

void entities_deinit()
{
  list_destroy(state.entities);
  state.entities = NULL;
}

size_t entities_count()
{
  return state.entities->length;
}

entity *entities_get_entity(size_t id)
{
  return list_get(state.entities, id);
}

size_t entities_add_entity(Color color, Vector2 velocity, Vector2 acceleration, Rectangle aabb, bool is_kinematic)
{
  size_t id = state.entities->length;
  for (size_t i = 0; i < state.entities->length; ++i)
  {
    entity *entity = list_get(state.entities, i);
    if (!entity->is_active) 
    {
      id = i;
      break;
    }
  }

  size_t body_id = physics_add_body(velocity, acceleration, aabb, is_kinematic);

  entity new_entity = {
    .color    = color,
    .body_id  = body_id
  };

  if (id == state.entities->length) {
    list_append(state.entities, &new_entity);
  } else {
    entity *existing_entity = list_get(state.entities, id);
    *existing_entity = new_entity;
  }

  return id;
}
