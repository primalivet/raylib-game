#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "entity.h"
#include "level.h"
#include "vector2.h"

bool physics_intersect_rects(Rectangle a, Rectangle b) {
  return (a.x < b.x + b.width && // a.left is left of b.right
  a.x + a.width > b.x && // a.right is right of b.left
  a.y < b.y + b.height && // a.top is above b.bottom
  a.y + a.height > b.y); // a.bottom is below b.top
}

void physics_update(entities_t *entities, level_t *level) {

  // Update physics and proposed positions for all entities

  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];

    // Apply direction from input
    if (entity->type == ENTITY_TYPE_PLAYER) {
      entity_input_comp_t *input = &entity->player.input;

      if (input != NULL) {
        entity->physics.direction.x = 0.0f;
        entity->physics.direction.y = 0.0f;

        if (input->up)    entity->physics.direction.y -= 1.0f;
        if (input->down)  entity->physics.direction.y += 1.0f;
        if (input->left)  entity->physics.direction.x -= 1.0f;
        if (input->right) entity->physics.direction.x += 1.0f;
      }
    } else {
      switch(entity->npc.behaviour) {
        case ENTITY_BEHAVIOUR_PATROL: 
          // TODO: update to more complex behaviour
          if (entity->physics.direction.x == 0.0f && entity->physics.direction.y == 0.0f) {
            int random_x = rand() % 2;
            int random_y = rand() % 2;
            entity->physics.direction.x = (float)random_x;
            entity->physics.direction.y = (float)random_y;
          }
          break;
        default:
        case ENTITY_BEHAVIOUR_STATIONARY:
          entity->physics.direction.x = 0.0f;
          entity->physics.direction.y = 0.0f;
          break;
      }
    }

    // Normalize direction
    float length = sqrt(entity->physics.direction.x * entity->physics.direction.x + entity->physics.direction.y * entity->physics.direction.y);
    if (length != 0.0f) {
      entity->physics.direction.x /= length;
      entity->physics.direction.y /= length;
    }

    // Apply friction (down to but not under 0.0f, prevent sliding)
    if      (entity->physics.velocity.y < 0.0f) entity->physics.velocity.y = fmin(entity->physics.velocity.y + entity->physics.friction, 0.0f);
    else if (entity->physics.velocity.y > 0.0f) entity->physics.velocity.y = fmax(entity->physics.velocity.y - entity->physics.friction, 0.0f);
    if      (entity->physics.velocity.x < 0.0f) entity->physics.velocity.x = fmin(entity->physics.velocity.x + entity->physics.friction, 0.0f);
    else if (entity->physics.velocity.x > 0.0f) entity->physics.velocity.x = fmax(entity->physics.velocity.x - entity->physics.friction, 0.0f);

    // Apply direction and speed to velocity
    entity->physics.velocity.x += entity->physics.direction.x * entity->physics.speed;
    entity->physics.velocity.y += entity->physics.direction.y * entity->physics.speed;

    // Clamp velocity
    float max_velocity = 2.0f; // Example maximum velocity (should be set on the physics_body?)
    if (entity->physics.velocity.x > max_velocity) entity->physics.velocity.x = max_velocity;
    if (entity->physics.velocity.x < -max_velocity) entity->physics.velocity.x = -max_velocity;
    if (entity->physics.velocity.y > max_velocity) entity->physics.velocity.y = max_velocity;
    if (entity->physics.velocity.y < -max_velocity) entity->physics.velocity.y = -max_velocity;

    entity->physics.proposed_position = (vector2_t){ .x = entity->physics.position.x + entity->physics.velocity.x, 
                                                     .y = entity->physics.position.y + entity->physics.velocity.y };

  }

  // Check world collision for all entities

  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];
    // Calculate proposed position

    // Get tile on all corners of proposed position X and Y
    vector2_t proposed_position_x_tl = (vector2_t){ .x = entity->physics.proposed_position.x, .y = entity->physics.position.y };
    vector2_t proposed_position_x_tr = (vector2_t){ .x = entity->physics.proposed_position.x + entity->physics.aabb.width, .y = entity->physics.position.y };
    vector2_t proposed_position_x_bl = (vector2_t){ .x = entity->physics.proposed_position.x, .y = entity->physics.position.y + entity->physics.aabb.height };
    vector2_t proposed_position_x_br = (vector2_t){ .x = entity->physics.proposed_position.x + entity->physics.aabb.width, .y = entity->physics.position.y + entity->physics.aabb.height };
    vector2_t proposed_position_y_tl = (vector2_t){ .x = entity->physics.position.x, .y = entity->physics.proposed_position.y };
    vector2_t proposed_position_y_tr = (vector2_t){ .x = entity->physics.position.x + entity->physics.aabb.width, .y = entity->physics.proposed_position.y };
    vector2_t proposed_position_y_bl = (vector2_t){ .x = entity->physics.position.x, .y = entity->physics.proposed_position.y + entity->physics.aabb.height };
    vector2_t proposed_position_y_br = (vector2_t){ .x = entity->physics.position.x + entity->physics.aabb.width, .y = entity->physics.proposed_position.y + entity->physics.aabb.height };

    // World Collision detection

    // Get tile index on all corners of proposed position X and Y
    // TODO: Check out of bounds
    // TODO: Abstract converting to tile index
    int tile_idx_x_tl = level->tilemap[(int)floor(proposed_position_x_tl.y / level->tileset_tile_size)][(int)floor(proposed_position_x_tl.x / level->tileset_tile_size)];
    int tile_idx_x_tr = level->tilemap[(int)floor(proposed_position_x_tr.y / level->tileset_tile_size)][(int)floor(proposed_position_x_tr.x / level->tileset_tile_size)];
    int tile_idx_x_bl = level->tilemap[(int)floor(proposed_position_x_bl.y / level->tileset_tile_size)][(int)floor(proposed_position_x_bl.x / level->tileset_tile_size)];
    int tile_idx_x_br = level->tilemap[(int)floor(proposed_position_x_br.y / level->tileset_tile_size)][(int)floor(proposed_position_x_br.x / level->tileset_tile_size)];
    int tile_idx_y_tl = level->tilemap[(int)floor(proposed_position_y_tl.y / level->tileset_tile_size)][(int)floor(proposed_position_y_tl.x / level->tileset_tile_size)];
    int tile_idx_y_tr = level->tilemap[(int)floor(proposed_position_y_tr.y / level->tileset_tile_size)][(int)floor(proposed_position_y_tr.x / level->tileset_tile_size)];
    int tile_idx_y_bl = level->tilemap[(int)floor(proposed_position_y_bl.y / level->tileset_tile_size)][(int)floor(proposed_position_y_bl.x / level->tileset_tile_size)];
    int tile_idx_y_br = level->tilemap[(int)floor(proposed_position_y_br.y / level->tileset_tile_size)][(int)floor(proposed_position_y_br.x / level->tileset_tile_size)];

    // Check for collision on all corners of proposed position X and Y
    bool collision_world_x_tl = !level->tiledef[tile_idx_x_tl].is_walkable;
    bool collision_world_x_tr = !level->tiledef[tile_idx_x_tr].is_walkable;
    bool collision_world_x_bl = !level->tiledef[tile_idx_x_bl].is_walkable;
    bool collision_world_x_br = !level->tiledef[tile_idx_x_br].is_walkable;
    bool collision_world_y_tl = !level->tiledef[tile_idx_y_tl].is_walkable;
    bool collision_world_y_tr = !level->tiledef[tile_idx_y_tr].is_walkable;
    bool collision_world_y_bl = !level->tiledef[tile_idx_y_bl].is_walkable;
    bool collision_world_y_br = !level->tiledef[tile_idx_y_br].is_walkable;

    // Sum up as axis collision (this will allow for sliding)
    bool collision_world_x = collision_world_x_tl || collision_world_x_tr || collision_world_x_bl || collision_world_x_br;
    bool collision_world_y = collision_world_y_tl || collision_world_y_tr || collision_world_y_bl || collision_world_y_br;

    // Collision response
    if (entity->type == ENTITY_TYPE_PLAYER) {
      if (collision_world_x) {
        entity->physics.velocity.x = -entity->physics.velocity.x; // Invert velocity
      } else {
        entity->physics.position.x = entity->physics.proposed_position.x;
      }

      if (collision_world_y) {
        entity->physics.velocity.y = -entity->physics.velocity.y; // Invert velocity
      } else {
        entity->physics.position.y = entity->physics.proposed_position.y;
      }
    } else {
      switch(entity->npc.behaviour) {
        case ENTITY_BEHAVIOUR_PATROL: 
          // TODO: update to more complex behaviour
          if (collision_world_x) {
            entity->physics.direction.x = -entity->physics.direction.x; // Invert direction
            entity->physics.velocity.x = -entity->physics.velocity.x; // Invert velocity
          } else {
            entity->physics.position.x = entity->physics.proposed_position.x;
          }
          if (collision_world_y) {
            entity->physics.direction.y = -entity->physics.direction.y; // Invert direction
            entity->physics.velocity.y = -entity->physics.velocity.y; // Invert velocity
          } else {
            entity->physics.position.y = entity->physics.proposed_position.y;
          }
          break;
        default:
        case ENTITY_BEHAVIOUR_STATIONARY:
          break;
      }
    }
  }

  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];
    entity_physics_comp_t *physics_body = &entity->physics;

    // Entities collision response
    bool collision_entity_x = false;
    bool collision_entity_y = false;
    for (int j = 0; j < entities->entities_count; j++) {
      entity_t *other_entity = entities->entities[j];
      if (entity->id == other_entity->id) continue; // Skip self collision
      Rectangle proposed_aabb_x = (Rectangle){ .x = entity->physics.proposed_position.x, .y = entity->physics.position.y, 
        .width = entity->physics.aabb.width, .height = entity->physics.aabb.height };
      Rectangle proposed_aabb_y = (Rectangle){ .x = entity->physics.position.x, .y = entity->physics.proposed_position.y,
        .width = entity->physics.aabb.width, .height = entity->physics.aabb.height };

      collision_entity_x = collision_entity_x || physics_intersect_rects(proposed_aabb_x, other_entity->physics.aabb);
      collision_entity_y = collision_entity_y || physics_intersect_rects(proposed_aabb_y, other_entity->physics.aabb);
    }

    if (entity->id == 0) {
      TraceLog(LOG_WARNING, "Collision Entity X: %d, Y: %d", collision_entity_x, collision_entity_y);
    }


    // Update AABB position
    physics_body->aabb.x = physics_body->position.x;
    physics_body->aabb.y = physics_body->position.y;
  }
}

