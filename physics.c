#include "physics.h"
#include "dynlist.h"
#include "utils.h"
#include "vector2.h"

static physics_state state;

void physics_init(float gravity)
{
  state.bodies  = list_create(sizeof(physics_body), 0);
  state.gravity = gravity;
  // TODO: implement tick tick rate
}

void physics_deinit()
{
  list_destroy(state.bodies);
  state.bodies = NULL;
}

physics_body *physics_get_body(size_t id)
{
  return list_get(state.bodies, id);
}

static physics_body physics_create_default_body()
{
  physics_body body = (physics_body) {
    .velocity     = {0.0f, 0.0f},
    .acceleration = {0.0f, 0.0f},
    .aabb         = {0.0f, 0.0f, 0.0f, 0.0f},
    .is_kinematic = false,
    .is_active    = false
  };
  return body;
}

size_t physics_add_body(Vector2 velocity, Vector2 acceleration, Rectangle aabb, bool is_kinematic)
{
  size_t id = state.bodies->length;

  for (size_t i = 0; i < state.bodies->length; ++i)
  {
    physics_body *body = list_get(state.bodies, i);
    if (!body->is_active)
    { 
      id = i; 
      break;
    }
  }
  physics_body new_body = physics_create_default_body();
  new_body.velocity     = velocity;
  new_body.acceleration = acceleration;
  new_body.aabb         = aabb;
  new_body.is_kinematic = is_kinematic;
  new_body.is_active    = true;

  if (id == state.bodies->length) {
    list_append(state.bodies, &new_body);
  }
  else {
    physics_body *existing_body = list_get(state.bodies, id);
    *existing_body = new_body;
  }

  return id;
}

void physics_update()
{
  physics_body *body;
  for (size_t i = 0; i < state.bodies->length; i++)
  {
    body = list_get(state.bodies, i);
    body->velocity = normalize_vector2(body->velocity);
    body->aabb.x += body->velocity.x;
    body->aabb.y += body->velocity.y;

    /* if (!body->is_kinematic) { */
    /*   Vector2 proposed_position = add_vector2((Vector2){body->aabb.x, body->aabb.y}, add_vector2(body->velocity, body->acceleration)); */
    /*   body->aabb.x = proposed_position.x; */
    /*   body->aabb.y = proposed_position.y; */
    /* } */
  }
}

