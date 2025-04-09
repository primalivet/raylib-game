#include <raylib.h>
#include <stdlib.h>
#include "entity.h"

void bullets_update(entities_t *entities) {
  for (int i = 0; i < entities->bullets_count; i++) {
    entity_t *entity = entities->entities[i];
    if (entity->type != ENTITY_TYPE_PLAYER) continue;
    if (entity->player.input.fire) {
      entity_bullet_t *bullet = malloc(sizeof(entity_bullet_t));
      bullet->direction = entity->physics.direction;
      bullet->position = entity->physics.position;
      bullet->velocity = (vector2_t){0.0f, 0.0f};
      if (entities->bullets_count >= MAX_BULLETS) {
        // TODO: Maybe remove first and add to the end, requires shifting though
        free(bullet);
        continue;
      }
      entities->bullets[entities->bullets_count] = bullet;
      entities->bullets_count++;
    }
  }
}

void bullets_draw(entities_t *entities) {
  for (int i = 0; i < entities->bullets_count; i++) {
    entity_bullet_t *bullet = entities->bullets[i];
    DrawCircle(bullet->position.x, bullet->position.y, 1.0f, RED);
  }
}
