#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "raylib.h"
#include "physics.h"
#include "vector2.h"

// TODO: move to common place, duplicated in enemy.c
static bool is_comment_line(const char *line) {
  while (*line) {
    if (*line == '#')
      return true;
    if (!isspace((unsigned int)*line))
      return false;
    line++;
  }
  return false;
}

void physics_load_bodies(physics_t *physics) {
  FILE *bodies_file = fopen(physics->enemies_path, "r");
  if (bodies_file == NULL) {
    TraceLog(LOG_FATAL,"PHYSICS: Error loading physics bodies file!");
    exit(1);
  } else TraceLog(LOG_INFO, "LEVEL: Loaded physics bodies file successfully");


  char bodies_buffer[1000];
  int i = 0;
  while(fgets(bodies_buffer, sizeof(bodies_buffer), bodies_file) && i < physics->bodies_count) {
    if (is_comment_line(bodies_buffer)) continue;
    char *token = NULL;
    token = strtok(bodies_buffer, " \n");
    int position_x = atof(token);
    token = strtok(NULL, " \n");
    int position_y = atof(token);
    token = strtok(NULL, " \n");
    int width = atof(token);
    token = strtok(NULL, " \n");
    int height = atof(token);

    physics_body_t body = {0};
    body.position = (vector2_t){ .x = position_x, .y = position_y };
    body.velocity               = (vector2_t){ .x = 0.0f, .y  = 0.0f };
    body.direction              = (vector2_t){ .x = 0.0f, .y  = 0.0f };
    body.friction               = 0.2f;
    body.speed                  = 0.5f;
    body.reset_dir_frames_delay = 6;
    body.aabb =  (Rectangle){ .x = position_x - (width / 2.0f), 
                              .y = position_y - (height / 2.0f), 
                              .width = width, 
                              .height = height };


    physics->bodies[i] = body;
    i++;
  }
}

void physics_init(physics_t *physics, physics_options_t *physics_options) {
  physics->bodies_count  = physics_options->bodies_count;
  physics->bodies_active = physics_options->bodies_count;
  physics->enemies_path = physics_options->enemies_path;
  physics->bodies        = (physics_body_t *)malloc(physics_options->bodies_count * sizeof(physics_body_t));
  if (physics->bodies == NULL) {
    TraceLog(LOG_FATAL, "PHYSICS: Failed to allocate memory for bodies");
    physics_free(physics);
    exit(1);
  }

  physics_load_bodies(physics);
}

void physics_free(physics_t *physics) {
  if (physics->bodies) {
    free(physics->bodies);
    physics->bodies = NULL;
    TraceLog(LOG_INFO, "PHYSICS: Successfully freed tiledef");
  }
}

void physics_draw_TEMP(physics_t *physics) {

  for (int i = 0; i < physics->bodies_active; i++) {
    physics_body_t *body = &physics->bodies[i];
    DrawRectangleRec(body->aabb, YELLOW);
  }
}

/*#include <math.h>
#include "physics.h"
#include "dynlist.h"
#include "utils.h"
#include "vector2.h"
#include "level.h"
#include "entity.h"

static physics_state state;

void physics_init(float gravity)
{
  state.bodies  = dynlist_allocate(sizeof(physics_body), 0);
  state.gravity = gravity;
  // TODO: implement tick tick rate
}

void physics_deinit()
{
  dynlist_free(state.bodies);
  state.bodies = NULL;
}

physics_body *physics_get_body(size_t id)
{
  return dynlist_get_at(state.bodies, id);
}

static physics_body physics_create_default_body()
{
  // These bogus values are just to initialize all field. They should be set
  // explicily on the return value from this function
  physics_body body = (physics_body) {
    .position      = {0.0f, 0.0f},
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

Rectangle physics_get_aabb(const physics_body *body)
{
  return (Rectangle){
    body->position.x - (body->aabb.width / 2.0f),
    body->position.y - (body->aabb.height / 2.0f),
    body->aabb.width,
    body->aabb.height,
  };
}

size_t physics_add_body(Vector2 position, float width, float height, Vector2 direction, Vector2 velocity, Vector2 acceleration, float acceleration_factor, float friction, float max_speed,  bool is_kinematic, size_t entity_id)
{
  size_t id = state.bodies->length;

  for (size_t i = 0; i < state.bodies->length; ++i)
  {
    physics_body *body = dynlist_get_at(state.bodies, i);
    if (!body->is_active)
    { 
      id = i; 
      break;
    }
  }
  physics_body new_body = physics_create_default_body();
  new_body.position            = position;
  new_body.aabb                = (Rectangle){ position.x - (width / 2.0f), position.y - (height / 2.0f), width, height };
  new_body.direction           = direction;
  new_body.velocity            = velocity;
  new_body.acceleration        = acceleration;
  new_body.acceleration_factor = acceleration_factor;
  new_body.friction            = friction;
  new_body.max_speed           = max_speed;
  new_body.is_kinematic        = is_kinematic;
  new_body.is_active           = true;
  new_body.entity_id           = entity_id;

  if (id == state.bodies->length) {
    dynlist_append(state.bodies, &new_body);
  }
  else {
    physics_body *existing_body = dynlist_get_at(state.bodies, id);
    *existing_body = new_body;
  }

  return id;
}

dynlist *get_nearby_non_walkable_tiles_for_world_position(Vector2 world_position, level *level) {
  dynlist *nearby_tiles = dynlist_allocate(sizeof(Vector2), 8);
  int tile_x = (int) floor(world_position.x / level->tile_size);
  int tile_y = (int) floor(world_position.y / level->tile_size);

  // create cardinal positions
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      int x = tile_x + dx;
      int y = tile_y + dy;

      // skip if  out of bounds
      if (x < 0 || x >= level->width || y < 0 || y >= level->height) {
        continue;
      }

      int tilemap_index = level->tilemap[y][x];
      tile_def tile_def = level->tile_defs[tilemap_index];
      if (!tile_def.is_walkable) {
        Vector2 tile_position = { (float)x , (float)y };
        dynlist_append(nearby_tiles, &tile_position);
      }
    }

  }
  return nearby_tiles;
}

bool physics_intersect_rect(Rectangle *first, Rectangle *second) {
  bool intersects = (
    (first->x < (second->x + second->width) &&  // Left   edge of first is to the right of the right edge of second
    (first->x + first->width) > second->x) &&   // Right  edge of first is to the left  of the left  edge of second
    (first->y < (second->y + second->height) && // Top    edge of first is above the bottom edge of second
    (first->y + first->height) > second->y)     // Bottom edge of first is below the top    edge of second
  );

  return intersects;
}

void physics_update(level *level) {
  physics_body *body;

  for (size_t i = 0; i < state.bodies->length; i++)
  {
    body = dynlist_get_at(state.bodies, i);

    // Apply acceleration
    body->acceleration = vector2_mult_by_scalar(body->direction, body->acceleration_factor);

    // Reset acceleration if no direction
    if (vector2_magnitude(body->direction) == 0)
    {
      body->acceleration = (Vector2){0.0f, 0.0f};
    }

    {
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

      {
        dynlist *nearby_non_walkable_tiles = get_nearby_non_walkable_tiles_for_world_position(body->position, level);

        bool collision_x = false;
        bool collision_y = false;
        for (size_t i = 0; i < nearby_non_walkable_tiles->length; i++) {
          Vector2 *nearby_non_walkable_tile_pos = dynlist_get_at(nearby_non_walkable_tiles, i);
          Rectangle nearby_non_walkable_tile = { 
            nearby_non_walkable_tile_pos->x * level->tile_size, 
            nearby_non_walkable_tile_pos->y * level->tile_size, 
            level->tile_size, 
            level->tile_size 
          };

          Rectangle proposed_aabb    = body->aabb;
          proposed_aabb.x           += proposed_velocity_x;
          proposed_aabb.y           += proposed_velocity_y;

          if (physics_intersect_rect(&proposed_aabb_x, &nearby_non_walkable_tile)) {
            collision_x = true;
          }

          if (physics_intersect_rect(&proposed_aabb_y, &nearby_non_walkable_tile)) {
            collision_y = true;
          }
        }

        dynlist_free(nearby_non_walkable_tiles);

        if (collision_x) {
          // TODO: handle differently for kinematic
          body->velocity.x = 0.0f;
        } else {
          body->velocity.x = proposed_velocity_x;
        }

        if (collision_y) {
          // TODO: handle differently for kinematic
          body->velocity.y = 0.0f;
        } else {
          body->velocity.y = proposed_velocity_y;
        }
      }
    }
    
    // Lock the velocity to the max speed
    if (vector2_magnitude(body->velocity) > body->max_speed)
    {
      body->velocity = vector2_normalize(body->velocity);
      body->velocity = vector2_mult_by_scalar(body->velocity, body->max_speed);
    }

    body->position = vector2_add(body->position, body->velocity);
    body->aabb = physics_get_aabb(body);
  }
}
*/
