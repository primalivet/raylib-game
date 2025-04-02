#include <raylib.h>
#include <stdbool.h>
#include "level.h"
#include "camera.h"
#include "vector2.h"
#include "render.h"
#include "window.h"
#include "entity.h"
#include "entity_input.h"
#include "entity_physics.h"
#include "debug.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main() {
  window_t window = {0};
  window_options_t window_options = {0};
  window_options.width            = SCREEN_WIDTH;
  window_options.height           = SCREEN_HEIGHT;
  window_options.title            = "Raylib game";
  window_options.taget_fps        = 60;
  window_open(&window, &window_options);

  vector2_t screen_center = (vector2_t){.x = window.width / 2.0f, .y = window.height / 2.0f};

  entities_t entities = {0};
  entities_options_t entities_options = {0};
  entities_options.entities_path = "resources/level1.entities.def";
  entities_init(&entities, &entities_options);

  camera_t camera = {0};
  camera_options_t camera_options = {0};
  camera_options.target           = entities.player.physics.position;
  camera_options.offset           = screen_center;
  camera_options.zoom             = 2.0f;
  camera_init(&camera, &camera_options);

  level_t level = {0};
  level_options_t level_options       = {0};
  level_options.tileset_tile_size     = 16;
  level_options.tileset_tiles_per_row = 20;
  level_options.tileset_path          = "resources/level1.tileset.png";
  level_options.tiledef_count         = 54;
  level_options.tiledef_path          = "resources/level1.tiledef.def";
  level_options.tilemap_rows          = 40;
  level_options.tilemap_cols          = 40;
  level_options.tilemap_path          = "resources/level1.tilemap.def";
  level_init(&level, &level_options);
  
  while (!WindowShouldClose()) {
    input_update(&entities.player.player.input);
    physics_update(&entities.player, &level);
    for (int i = 0; i < entities.enemies_count; i++) physics_update(&entities.enemies[i], &level);
    camera_update(&camera, &entities.player.physics.position);

    render_begin();
    camera_start(&camera);

    level_draw(&level);
    entities_draw(&entities);

    camera_end();
    debug_draw(&entities);
    render_end();
  }

  /*physics_free(&physics);*/
  level_free(&level);
  window_close();
}
