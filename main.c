#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>

//----------------------------------------------------------------------------- 
// DEFINES
//----------------------------------------------------------------------------- 

#define TITLE         "Game"
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define TILE_SIZE     16
#define TILES_PER_ROW 20
#define TARGET_FPS    60
#define BACKGROUND    CLITERAL(Color){ 29, 42, 47, 255 }         

//----------------------------------------------------------------------------- 
// OWNED INCLUDES
//----------------------------------------------------------------------------- 

#include "level.h"
#include "camera.h"
#include "physics.h"
#include "entity.h"


void read_input(physics_body *player_body) {
  player_body->velocity.x = 0.0f;
  player_body->velocity.y = 0.0f;
  if (IsKeyDown(KEY_LEFT))  player_body->velocity.x -= 2.0f;
  if (IsKeyDown(KEY_RIGHT)) player_body->velocity.x += 2.0f;
  if (IsKeyDown(KEY_UP))    player_body->velocity.y -= 2.0f;
  if (IsKeyDown(KEY_DOWN))  player_body->velocity.y += 2.0f;
}

//----------------------------------------------------------------------------- 
// MAIN
//----------------------------------------------------------------------------- 

int main(void) 
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  SetTargetFPS(TARGET_FPS);

  physics_init(9.8f);
  entities_init();

  Texture tileset   = LoadTexture("resources/tiles-16.png");
  level   level     = load_level( TILE_SIZE, TILES_PER_ROW, "resources/level1.map", "resources/level1.def", &tileset);
  size_t  player_id = entities_add_entity(
    RED,
    (Vector2){0.0f, 0.0f},
    (Vector2){0.0f, 0.0f},
    (Rectangle){4.0f * level.tile_size, 4.0f * level.tile_size, TILE_SIZE, TILE_SIZE},
    true 
  );
  entity *player = entities_get_entity(player_id);
  physics_body *player_body = physics_get_body(player->body_id);

  Camera2D camera = init_camera(
    (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f), player_body->aabb.y + (player_body->aabb.height / 2.0f)}, 
    (Vector2) { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f }, 
    2.0f
  );

  while(!WindowShouldClose()) 
  {
    read_input(player_body);
    update_camera(&camera, (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f), player_body->aabb.y + (player_body->aabb.height / 2.0f)}); 

    physics_update();

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(BACKGROUND);
    draw_level(&level);

    DrawRectangleRec(player_body->aabb, player->color);

    EndMode2D();
    EndDrawing();
  }

  entities_deinit();
  physics_deinit();
  unload_level(&level);
  CloseWindow();

  return 0;
}
