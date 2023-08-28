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

#include "player.h"
#include "level.h"
#include "camera.h"


//----------------------------------------------------------------------------- 
// MAIN
//----------------------------------------------------------------------------- 

int main(void) 
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  SetTargetFPS(TARGET_FPS);

  Texture tileset = LoadTexture("resources/tiles-16.png");
  level level     = load_level( TILE_SIZE, "resources/level1.map", "resources/level1.def", &tileset);
  player player   = init_player(TILE_SIZE, (Vector2){3.0f * TILE_SIZE, 3.0f * TILE_SIZE}, (Vector2){0.0f, 0.0f});
  Camera2D camera = init_camera(player.position, (Vector2) { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f }, 2.0f);

  while(!WindowShouldClose()) 
  {
    update_player(&player, &level);
    update_camera(&camera, player.position); 

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(BACKGROUND);
    draw_level(&level);
    draw_player(&player);

    EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  unload_level(&level);

  return 0;
}
