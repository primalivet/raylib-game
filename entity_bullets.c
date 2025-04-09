#include <raylib.h>
#include <stdlib.h>
#include "entity.h"

void bullets_update(entities_t *entities) {
  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];
    if (entity->type != ENTITY_TYPE_PLAYER) continue;
    if (entity->player.input.space && !entity->player.input.space_prev) {
      int entity_is_moving = (entity->physics.direction.x != 0.0f || entity->physics.direction.y != 0.0f);
      for (int j = 0; j < entities->bullets_count; j++) {
        entity_bullet_t *bullet = entities->bullets[j];
        if (bullet->active == false) {
          bullet->active = true;
          bullet->position.x = entity->physics.aabb.x + (entity->physics.aabb.width / 2);
          bullet->position.y = entity->physics.aabb.y + (entity->physics.aabb.height / 2);
          bullet->velocity = (vector2_t){ .x = 0.0f, .y = 0.0f };
          if (entity_is_moving) {
            bullet->direction.x = entity->physics.direction.x;
            bullet->direction.y = entity->physics.direction.y;
          } else {
            switch(entity->facing) {
              case ENTITY_FACING_NORTH: bullet->direction = (vector2_t){ .x = 0.0f, .y = -1.0f }; break;
              case ENTITY_FACING_SOUTH: bullet->direction = (vector2_t){ .x = 0.0f, .y = 1.0f }; break;
              case ENTITY_FACING_EAST: bullet->direction = (vector2_t){ .x = 1.0f, .y = 0.0f }; break;
              case ENTITY_FACING_WEST: bullet->direction = (vector2_t){ .x = -1.0f, .y = 0.0f }; break;
            }
          }
          break;
        }
      }
    }
  }
}

void bullets_draw(entities_t *entities) {
  for (int i = 0; i < entities->bullets_count; i++) {
    if (entities->bullets[i]->active == false) continue;
    entity_bullet_t *bullet = entities->bullets[i];
    DrawCircle(bullet->position.x, bullet->position.y, 2.0f, RED);
  }
}
