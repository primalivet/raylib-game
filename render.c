#include "raylib.h"
#include "render.h"

void render_begin(camera_t *camera) {
  Camera2D rlcamera = (Camera2D) {
    .zoom = camera->zoom,
    .offset = (Vector2){ .x = camera->offset.x, .y = camera->offset.y },
    .target = (Vector2){ .x = camera->target.x, .y = camera->target.y },
    .rotation =camera->rotation,
  };
  BeginDrawing();
  // TODO: move colors away from raylib
  ClearBackground(CLITERAL(Color) { 29, 42, 47, 255 }); 
  BeginMode2D(rlcamera);
}

void render_reset(){};

void render_end() {
  EndMode2D();
  EndDrawing();
}
