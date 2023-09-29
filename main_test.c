#include <raylib.h>
#include "level.h"
#include "physics.h"
#include "a_star.h"
#include <stdio.h>
#include "utils.h"
#include "dynlist.h"

#define TILE_SIZE     16
#define TILES_PER_ROW 20

int test_count = 0;

int test_astar_search() {
  InitWindow(420, 320, "Test window");
  Texture tileset   = LoadTexture("resources/tiles-16.png");
  level   level     = load_level( TILE_SIZE, TILES_PER_ROW, "resources/level1.map", "resources/level1.def", &tileset);
  Rectangle origin =  (Rectangle){ 3 * TILE_SIZE, 3 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
  Rectangle goal = (Rectangle){ 3 * TILE_SIZE , 10 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
  astar_allocate(&level);
  dynlist *path = astar_search(&origin, &goal);

  while(!WindowShouldClose()) {
    BeginDrawing();
    for (int y = 0; y < level.height; y++) {
      for (int x = 0; x < level.width; x++) {
        if (level.tile_defs[level.tilemap[y][x]].is_walkable) {
        DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, WHITE);
        }
        
      }
    }

    for (size_t i = 0; i < path->length; i++) {
      astar_node *step = dynlist_get_at(path, i);
      DrawRectangle(step->x * TILE_SIZE, step->y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
    }

    DrawRectangleRec(origin, RED);
    DrawRectangleRec(goal, GREEN);

    ClearBackground(RAYWHITE);
    EndDrawing();
  }

  CloseWindow();

  astar_free();
  return 0;
}

int main() {
  test_count++;
  test_astar_search();
  printf("completed, ran %d tests\n", test_count);
  return 0;
}
