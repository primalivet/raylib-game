#include <math.h>
#include "entity.h"
#include "vector2.h"

void animation_update(entities_t *entities) {
  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];
    bool is_moving = vector2_magnitude(entity->physics.velocity) > 0.0f;
    bool has_direction = vector2_magnitude(entity->physics.direction) > 0.0f;

    if (is_moving && has_direction) {
      vector2_t direction = entity->physics.direction;
      if (fabs(direction.x) > fabs(direction.y)) {
        if (direction.x > 0) entity->animation.frame_row = 3;
        else entity->animation.frame_row = 2;
      } else {
        if (direction.y > 0) entity->animation.frame_row = 1;
        else entity->animation.frame_row = 0;
      }
      entity->animation.frame_counter++;
      if (entity->animation.frame_counter >= entity->animation.frames_per_update) {
        entity->animation.frame_counter = 0;
        entity->animation.frame_current = (entity->animation.frame_current + 1) % entity->animation.frame_count;
      }
    } else {
      entity->animation.frame_current = 0;
    }

    entity->animation.current_clip = (Rectangle) {
      .x = entity->animation.frame_current * entity->animation.tile_size,
      .y = entity->animation.frame_row * entity->animation.tile_size,
      .width = entity->animation.tile_size,
      .height = entity->animation.tile_size
    };
  }
}

void animation_draw(entities_t *entities) {
  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];
    Rectangle source = entity->animation.current_clip;
    Rectangle dest = {
      .x = entity->physics.position.x - ((entity->animation.tile_size - entity->physics.aabb.width) / 2),
      .y = entity->physics.position.y - ((entity->animation.tile_size - entity->physics.aabb.height) / 2),
      .width = entity->animation.tile_size,
      .height = entity->animation.tile_size
    };
    DrawTexturePro(entity->animation.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
  }
}
