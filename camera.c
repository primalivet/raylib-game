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

void camera_update(camera_t *camera, vector2_t *new_target) {
  camera->target = *new_target;
}
