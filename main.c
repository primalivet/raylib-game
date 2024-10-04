#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "level.h"
#include "player.h"
#include "input.h"
#include "camera.h"
#include "vector2.h"
#include "render.h"
#include "window.h"
#include "physics.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// TODO: create and manage physics bodies seperatley from player, 
// so that we can calculate physics bodies speratley, then 
// render the entities based of their bodies.

int main() {
  window_t window = {0};
  window_options_t window_options = {0};
  window_options.width            = SCREEN_WIDTH;
  window_options.height           = SCREEN_HEIGHT;
  window_options.title            = "Raylib game";
  window_options.taget_fps        = 60;
  window_open(&window, &window_options);

  vector2_t screen_center = (vector2_t){.x = window.width / 2.0f, .y = window.height / 2.0f};

  input_t input   = {0};
  input_init(&input);

  player_t player = {0};
  player_options_t player_options = {0};
  player_options.position         = screen_center;
  player_options.width            = 10.0f;
  player_options.height           = 10.0f;
  player_options.speed            = 1.0f;
  player_options.friction         = 0.2f;
  player_options.reset_dir_frames_delay = 6;
  player_init(&player, &player_options);

  camera_t camera = {0};
  camera_options_t camera_options = {0};
  camera_options.target           = player.position;
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

  physics_t physics = {0};
  physics_options_t physics_options = {0};
  physics_options.bodies_count     = 2;
  physics_options.enemies_path = "resources/level1.enemies.def";
  physics_init(&physics, &physics_options);

  while (!WindowShouldClose()) {
    input_update(&input);
    player_update(&player, &input);
    camera_update(&camera, &player);

    render_begin(&camera);

    level_draw(&level);
    player_draw(&player);
    physics_draw_TEMP(&physics);

    render_end();
  }

  physics_free(&physics);
  level_free(&level);
  window_close();
}
