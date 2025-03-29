#include <raylib.h>
#include "camera.h"
#include "vector2.h"
#include "entity.h"

void camera_init(camera_t *camera, camera_options_t *camera_options) {
  camera->target   = camera_options->target;
  camera->offset   = camera_options->offset;
  camera->zoom     = camera_options->zoom;
  camera->rotation = 0.0f;
}

void camera_start(camera_t *camera) {
  Camera2D rlcamera = (Camera2D) {
    .zoom = camera->zoom,
    .offset = (Vector2){ .x = camera->offset.x, .y = camera->offset.y },
    .target = (Vector2){ .x = camera->target.x, .y = camera->target.y },
    .rotation =camera->rotation,
  };
  BeginMode2D(rlcamera);
}

void camera_end() {
  EndMode2D();
}

void camera_update(camera_t *camera, vector2_t *new_target) {
  camera->target = *new_target;
}
