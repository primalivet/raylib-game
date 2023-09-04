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

void update_ai_body(physics_body *ai_body, physics_body *player_body) {
  float distance = euclidean_distance((Vector2){ ai_body->aabb.x, ai_body->aabb.y }, (Vector2){ player_body->aabb.x, player_body->aabb.y});
  Vector2 seek   = normalize_vector2(seek_vector2((Vector2){ai_body->aabb.x,     ai_body->aabb.y},
                                                  (Vector2){player_body->aabb.x, player_body->aabb.y}));

  bool is_standing_still       = ai_body->direction.x == 0.0f && ai_body->direction.y == 0.0f;
  bool is_close_to_player      = ((distance / TILE_SIZE) < 5.0f) || ((distance / TILE_SIZE)< 5.0f);  // TODO: move number to config
  bool is_far_away_from_player = ((distance / TILE_SIZE) > 10.0f) && ((distance / TILE_SIZE) > 10.0f);  // TODO: move number to config

  // if not moving and distance is small start seeking
  if (is_standing_still) {
    if (is_close_to_player) { ai_body->direction = seek; }
  } else {
    if (is_far_away_from_player) { ai_body->direction = (Vector2) {0.0f, 0.0f}; } 
    else                         { ai_body->direction = seek; }
  }
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

  size_t enemy_ids[3];

  for (size_t i = 0; i < 2; i++) {
    size_t enemy_id = entities_add_entity(
      RED,
      (Rectangle){(25.0f + i) * level.tile_size, (5.0f + i) * level.tile_size, TILE_SIZE / 1.5f, TILE_SIZE / 1.5f},
      (Vector2){0.0f, 0.0f },
      (Vector2){0.0f, 0.0f},
      (Vector2){0.0f, 0.0f},
      1.0f,
      0.35f,
      2.0f,
      false 
    );
    enemy_ids[i] = enemy_id;
  }

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
    1.0f
  );

  while(!WindowShouldClose()) 
  {
    read_input(player_body);

    for (size_t i = 0; i < entities_count(); i++) {
      if (i == player_id) continue;
      entity *ai_entity = entities_get_entity(i);
      physics_body *ai_body = physics_get_body(ai_entity->body_id);
      update_ai_body(ai_body, player_body);
    }

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

    for (size_t i = 0; i < entities_count(); i++) {
      entity *entity = entities_get_entity(i);
      physics_body *entity_body = physics_get_body(entity->body_id);
      DrawRectangleRec(entity_body->aabb, entity->color);
    }

    EndMode2D();
    EndDrawing();
  }

  entities_deinit();
  physics_deinit();
  unload_level(&level);
  CloseWindow();

  return 0;
}
