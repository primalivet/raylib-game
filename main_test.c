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
  Rectangle origin =  (Rectangle){ 0 * TILE_SIZE, 0 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
  Rectangle goal = (Rectangle){ 5 * TILE_SIZE , 8 * TILE_SIZE, TILE_SIZE, TILE_SIZE };
  astar_allocate(level.width, level.height);
  dynlist *path = astar_search(&(Vector2){ origin.x / TILE_SIZE, origin.y / TILE_SIZE },
                               &(Vector2){ goal.x / TILE_SIZE, goal.y / TILE_SIZE });

  if (!path) {
    printf("PATH NOT FOUND\n");
    exit(1);
  }

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

  if(path) {
    printf("Path length: %lu\n", path->length);
    for (size_t i = 0; i < path->length; i++) {
      astar_node *step = dynlist_get_at(path, i);
      printf("x: %f, y: %f (f_cost: %f, g_cost: %f, h_cost: %f)\n", step->x, step->y, step->f_cost, step->g_cost, step->h_cost);
    }
  } else {
    printf("Path not found\n");
  }
  free_reconstructed_path(path);
  astar_free();

  return 0;
}

int main() {
  test_count++;
  test_astar_search();
  printf("completed, ran %d tests\n", test_count);
  return 0;
}
