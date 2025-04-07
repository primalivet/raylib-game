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
    .x = entity->player.animation.frame_current * 16,
    .y = animation_row * 16,
    .width = 16,
    .height = 16
  };
}

void animation_draw(entity_t *entity) {
  // TODO: Only for player right now
  if (entity->type != ENTITY_TYPE_PLAYER) return;

  Rectangle source = entity->player.animation.current_clip;
  Rectangle dest = entity->physics.aabb;
  DrawTexturePro(entity->player.animation.texture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
}
