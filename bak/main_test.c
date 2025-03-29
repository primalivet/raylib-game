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
  int **collision_mask = malloc(sizeof(int) * level.height);
  for (int y = 0; y < level.height; y++) {
    collision_mask[y] = malloc(sizeof(int) * level.width);
    for (int x = 0; x < level.width; x++) {
      collision_mask[y][x] = level.tile_defs[level.tilemap[y][x]].is_walkable;
    }
  }

  Rectangle player = {  250.000092, 64.000000, TILE_SIZE, TILE_SIZE };
  Rectangle enemy = { 400.000000, 80.000000,  TILE_SIZE, TILE_SIZE };

  IntVector2 goal = intvec2_from_vec2((Vector2){ player.x / TILE_SIZE, player.y / TILE_SIZE });
  IntVector2 origin = intvec2_from_vec2((Vector2){ enemy.x / TILE_SIZE, enemy.y / TILE_SIZE });

  astar_allocate(level.width, level.height, collision_mask);
  dynlist *path = astar_search( &origin, &goal);

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
      Vector2 *step = dynlist_get_at(path, i);
      DrawRectangle(step->x * TILE_SIZE, step->y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
    }

    DrawRectangleRec(player, GREEN);
    DrawRectangleRec(enemy, RED);

    ClearBackground(RAYWHITE);
    EndDrawing();
  }

  CloseWindow();

  if(path) {
    printf("Path length: %lu\n", path->length);
    for (size_t i = 0; i < path->length; i++) {
      Vector2 *step = dynlist_get_at(path, i);
      printf("x: %f, y: %f\n", step->x, step->y);
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
