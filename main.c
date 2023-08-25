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
  Level level     = LoadLevel( TILE_SIZE, "resources/level1.map", "resources/level1.def", &tileset);
  Player player   = InitPlayer(TILE_SIZE, (Vector2){2.0f * TILE_SIZE, 2.0f * TILE_SIZE}, (Vector2){0.0f, 0.0f});
  Camera2D camera = InitCam(player.position, (Vector2) { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f });

  while(!WindowShouldClose()) 
  {
    UpdatePlayer(&player);

    UpdateCam(&camera, player.position); 

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(BACKGROUND);
    DrawLevel(&level);
    DrawPlayer(&player);

    EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  UnloadLevel(&level);

  return 0;
}
