#include <math.h>
#include "entity.h"
#include "vector2.h"

void animation_update(entity_t *entity) {
  // TODO: Only for player right now
  if (entity->type != ENTITY_TYPE_PLAYER) return;

  bool is_moving = vector2_magnitude(entity->physics.velocity) > 0.0f;

  int animation_row = 1;
  if (is_moving) {
    vector2_t direction = entity->physics.direction;
    if (fabs(direction.x) > fabs(direction.y)) {
      if (direction.x > 0) animation_row = 3;
      else animation_row = 2;
    } else {
      if (direction.y > 0) animation_row = 1;
      else animation_row = 0;
    }
    entity->player.animation.frame_counter++;
    if (entity->player.animation.frame_counter >= entity->player.animation.frames_per_update) {
      entity->player.animation.frame_counter = 0;
      entity->player.animation.frame_current = (entity->player.animation.frame_current + 1) % entity->player.animation.frame_count;
    }
  } else {
    entity->player.animation.frame_current = 0;
  }

  entity->player.animation.current_clip = (Rectangle) {
    .x = entity->player.animation.frame_current * entity->player.animation.tile_size,
    .y = animation_row * entity->player.animation.tile_size,
    .width = entity->player.animation.tile_size,
    .height = entity->player.animation.tile_size
  };
}

void animation_draw(entity_t *entity) {
  // TODO: Only for player right now
  if (entity->type != ENTITY_TYPE_PLAYER) return;

  Rectangle source = entity->player.animation.current_clip;
  Rectangle dest = {
    .x = entity->physics.position.x - ((entity->player.animation.tile_size - entity->physics.aabb.width) / 2),
    .y = entity->physics.position.y - ((entity->player.animation.tile_size - entity->physics.aabb.height) / 2),
    .width = entity->player.animation.tile_size,
    .height = entity->player.animation.tile_size
  };
  DrawTexturePro(entity->player.animation.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}
