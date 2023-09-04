#include <math.h>
#include "physics.h"
#include "dynlist.h"
#include "utils.h"
#include "vector2.h"
#include "level.h"
#include "entity.h"

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
  // These bogus values are just to initialize all field. They should be set
  // explicily on the return value from this function
  physics_body body = (physics_body) {
    .aabb          = {0.0f, 0.0f, 0.0f, 0.0f},
    .direction     = {0.0f, 0.0f},
    .velocity      = {0.0f, 0.0f},
    .acceleration  = {0.0f, 0.0f},
    .friction      = 1.0f,
    .max_speed     = 0.0f,
    .is_kinematic  = false,
    .is_active     = false,
    .entity_id     = -1 // should be initialized in some other way perferably
  };
  return body;
}

size_t physics_add_body(Rectangle aabb, Vector2 direction, Vector2 velocity, Vector2 acceleration, float acceleration_factor, float friction, float max_speed,  bool is_kinematic, size_t entity_id)
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
  new_body.aabb         = aabb;
  new_body.direction    = direction;
  new_body.velocity     = velocity;
  new_body.acceleration = acceleration;
  new_body.acceleration_factor = acceleration_factor;
  new_body.friction     = friction;
  new_body.max_speed    = max_speed;
  new_body.is_kinematic = is_kinematic;
  new_body.is_active    = true;
  new_body.entity_id    = entity_id;

  if (id == state.bodies->length) {
    list_append(state.bodies, &new_body);
  }
  else {
    physics_body *existing_body = list_get(state.bodies, id);
    *existing_body = new_body;
  }

  return id;
}

// TODO: move elsewhere
bool physics_intersect_tilemap(Rectangle *object, level *level) 
{

  // TODO: could i retrive the 4 tiles around the player instead of looping over
  // every tile?

  bool colliding = false;
  for (int y = 0; y < level->height; y++)
  {
    for (int x = 0; x < level->width; x++)
    {
      int tilemap_index = level->tilemap[y][x];
      tile_def tile_def = level->tile_defs[tilemap_index];

      if (!tile_def.is_walkable ) {
        Rectangle tile_rect = { x * level->tile_size, y * level->tile_size, level->tile_size, level->tile_size };
        if ((object->x < (tile_rect.x + tile_rect.width) && (object->x + object->width) > tile_rect.x) &&
        (object->y < (tile_rect.y + tile_rect.height) && (object->y + object->height) > tile_rect.y))
        {
        colliding = true;
        }
      }
    }
  }
  return colliding;
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void physics_update(level *level)
{
  physics_body *body;
  for (size_t i = 0; i < state.bodies->length; i++)
  {
    body = list_get(state.bodies, i);
    /* entity *entity = entities_get_entity(body->entity_id); */

    // Apply acceleration
    body->acceleration = mult_vector2(body->direction, body->acceleration_factor);

    // Reset acceleration if no direction
    if (magnitude_vector2(body->direction) == 0)
    {
      body->acceleration = (Vector2){0.0f, 0.0f};
    }

    // Store the calculated "proposed" velocity seperately for x and y axis so
    // we can check for collisions indenpendently, and by that support "sliding
    // behavior"
    float proposed_velocity_x  = body->velocity.x;         // Copy of current velocity (x axis)
    proposed_velocity_x       += body->acceleration.x;     // Update velocity with acceleration 
    proposed_velocity_x       *= (1.0f - body->friction ); // Apply friction to velocity (friction_coefficient is used as 1.0f - MAGIC_NR)
    Rectangle proposed_aabb_x  = body->aabb;               // Copy of current AABB
    proposed_aabb_x.x         += proposed_velocity_x;      // Apply proposed velocity to AABB

    float proposed_velocity_y  = body->velocity.y;         // Copy of current velocity (y axis)
    proposed_velocity_y       += body->acceleration.y;     // Update velocity with acceleration
    proposed_velocity_y       *= (1.0f - body->friction ); // Apply friction to velocity (friction_coefficient is used as 1.0f - MAGIC_NR)
    Rectangle proposed_aabb_y  = body->aabb;               // Copy of current AABB
    proposed_aabb_y.y         += proposed_velocity_y;      // Apply proposed velocity to AABB


    if (physics_intersect_tilemap(&proposed_aabb_x , level)) {
      if (body->is_kinematic) {
        // TODO: add proper tilemap collision response for x axis
        float response_velocity_x = fabs(proposed_velocity_x) >= body->max_speed ? body->velocity.x * -1.0f : 0.0f;
        body->velocity.x = response_velocity_x;
      } else {
        body->velocity.x *= -1;
        body->direction.x *= -1;
      }
    } else {
      body->velocity.x = proposed_velocity_x;
    }

    if (physics_intersect_tilemap(&proposed_aabb_y , level)) {
      if (body->is_kinematic) {
        // TODO: add proper tilemap collision response
        float response_velocity_y = fabs(proposed_velocity_y) >= body->max_speed ? body->velocity.y * -1.0f : 0.0f;
        body->velocity.y = response_velocity_y;
      } else {
        body->velocity.y *= -1;
        body->direction.y *= -1;
      }
    } else {
      body->velocity.y = proposed_velocity_y;
    }

    // Lock the velocity to the max speed
    if (magnitude_vector2(body->velocity) > body->max_speed)
    {
      body->velocity = mult_vector2(normalize_vector2(body->velocity), body->max_speed);
    }

    // Update position
    body->aabb.x += body->velocity.x;
    body->aabb.y += body->velocity.y;
  }
}

