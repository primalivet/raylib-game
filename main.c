#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>

//----------------------------------------------------------------------------- 
// DEFINES
//----------------------------------------------------------------------------- 

#define TITLE         "Game"
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define TILE_SIZE     16
#define TILES_PER_ROW 20
#define TARGET_FPS    60
#define BACKGROUND    CLITERAL(Color){ 29, 42, 47, 255 }         

//----------------------------------------------------------------------------- 
// OWNED INCLUDES
//----------------------------------------------------------------------------- 

#include "vector2.h"
#include "level.h"
#include "camera.h"
#include "physics.h"
#include "entity.h"


void read_input(physics_body *player_body) {
  // Reset player direction
  player_body->direction.x = 0.0f;
  player_body->direction.y = 0.0f;

  // Set player direction based on input 
  if (IsKeyDown(KEY_LEFT))  player_body->direction.x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT)) player_body->direction.x += 1.0f;
  if (IsKeyDown(KEY_UP))    player_body->direction.y -= 1.0f;
  if (IsKeyDown(KEY_DOWN))  player_body->direction.y += 1.0f;

  // Normalize player direction
  player_body->direction = normalize_vector2(player_body->direction);
}

bool show_player_debug_body = false;
void draw_player_debug_body(physics_body *body) {
  int column_size = 80;
  int column_height = 70;
  const char *velocity = TextFormat(
    "Velocity\nx: %f\ny: %f", 
    body->velocity.x, 
    body->velocity.y
  );
  const char *position = TextFormat(
    "Position\nx: %f\ny: %f", 
    body->aabb.x, 
    body->aabb.y
  );
  const char *acceleration = TextFormat(
    "Acceleration\nx: %f\ny: %f", 
    body->acceleration.x, 
    body->acceleration.y
  );
  const char *direction = TextFormat(
    "Direction\nx: %f\ny: %f", 
    body->direction.x, 
    body->direction.y
  );
  DrawRectangleRec((Rectangle){0,0, SCREEN_WIDTH, column_height}, BLACK);
  DrawText(velocity,      column_size  *  0 + 10,10,  10,  BLUE);
  DrawText(position,      column_size  *  1 + 10,10,  10,  GREEN);
  DrawText(acceleration,  column_size  *  2 + 10,10,  10,  YELLOW);
  DrawText(direction,     column_size  *  3 + 10,10,  10,  RED);
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
    WHITE,
    (Rectangle){4.0f * level.tile_size, 4.0f * level.tile_size, TILE_SIZE / 1.5f, TILE_SIZE / 1.5f},
    (Vector2){0.0f, 0.0f },
    (Vector2){0.0f, 0.0f},
    (Vector2){0.0f, 0.0f},
    1.0f,
    0.25f,
    4.0f,
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

    physics_update(&level);

    BeginDrawing();
    ClearBackground(BACKGROUND);
    if (IsKeyPressed(KEY_P)){
      show_player_debug_body = !show_player_debug_body;
    }
    if (show_player_debug_body) {
      draw_player_debug_body(player_body);
    }
    BeginMode2D(camera);

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
