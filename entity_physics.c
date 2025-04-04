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
    entity_physics_comp_t *physics_body = &entity->physics;

    // Apply direction from input
    if (entity->type == ENTITY_TYPE_PLAYER) {
      entity_input_comp_t *input = &entity->player.input;

      if (input != NULL) {
        physics_body->direction.x = 0.0f;
        physics_body->direction.y = 0.0f;

        if (input->up)    physics_body->direction.y -= 1.0f;
        if (input->down)  physics_body->direction.y += 1.0f;
        if (input->left)  physics_body->direction.x -= 1.0f;
        if (input->right) physics_body->direction.x += 1.0f;
      }
    } else {
      switch(entity->npc.behaviour) {
        case ENTITY_BEHAVIOUR_PATROL: 
          // TODO: update to more complex behaviour
          if (physics_body->direction.x == 0.0f && physics_body->direction.y == 0.0f) {
            int random_x = rand() % 2;
            int random_y = rand() % 2;
            physics_body->direction.x = (float)random_x;
            physics_body->direction.y = (float)random_y;
          }
          break;
        default:
        case ENTITY_BEHAVIOUR_STATIONARY:
          physics_body->direction.x = 0.0f;
          physics_body->direction.y = 0.0f;
          break;
      }
    }

    // Normalize direction
    float length = sqrt(physics_body->direction.x * physics_body->direction.x + physics_body->direction.y * physics_body->direction.y);
    if (length != 0.0f) {
      physics_body->direction.x /= length;
      physics_body->direction.y /= length;
    }

    // Apply friction (down to but not under 0.0f, prevent sliding)
    if      (physics_body->velocity.y < 0.0f) physics_body->velocity.y = fmin(physics_body->velocity.y + physics_body->friction, 0.0f);
    else if (physics_body->velocity.y > 0.0f) physics_body->velocity.y = fmax(physics_body->velocity.y - physics_body->friction, 0.0f);
    if      (physics_body->velocity.x < 0.0f) physics_body->velocity.x = fmin(physics_body->velocity.x + physics_body->friction, 0.0f);
    else if (physics_body->velocity.x > 0.0f) physics_body->velocity.x = fmax(physics_body->velocity.x - physics_body->friction, 0.0f);

    // Apply direction and speed to velocity
    physics_body->velocity.x += physics_body->direction.x * physics_body->speed;
    physics_body->velocity.y += physics_body->direction.y * physics_body->speed;

    // Clamp velocity
    float max_velocity = 2.0f; // Example maximum velocity (should be set on the physics_body?)
    if (physics_body->velocity.x > max_velocity) physics_body->velocity.x = max_velocity;
    if (physics_body->velocity.x < -max_velocity) physics_body->velocity.x = -max_velocity;
    if (physics_body->velocity.y > max_velocity) physics_body->velocity.y = max_velocity;
    if (physics_body->velocity.y < -max_velocity) physics_body->velocity.y = -max_velocity;

    entity->physics.proposed_position = (vector2_t){ .x = physics_body->position.x + physics_body->velocity.x, 
                                                     .y = physics_body->position.y + physics_body->velocity.y };

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

