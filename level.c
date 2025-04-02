#include "level.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Add more sofisticated tiledef like the below to enable more features like pressure plate, timed, switchable etc.
// ```
// # index is_walkable behavior behavior_param1 behavior_param2
// 00 0 0 0 0
// 01 0 0 0 0
// ...
// 15 1 2 500 1000  # Tile 15 is walkable, has behavior 2 (timed), toggles between states every 500ms, delay 1000ms
// ...
// 25 1 1 0 0       # Tile 25 is walkable, has behavior 1 (pressure plate), no params
// ```
//
// Where behavior could be:
// - 0: Static (default)
// - 1: Pressure plate (changes when player stands on it)
// - 2: Timed (changes state periodically)
// - 3: Switchable (changes when a switch is activated)

// TODO: move to common place, duplicated in enemy.c
static bool is_comment_line(const char *line) {
  while (*line) {
    if (*line == '#')
      return true;
    if (!isspace((unsigned int)*line))
      return false;
    line++;
  }
  return false;
}

static void level_load_tileset(level_t *level) {
  level->tileset = LoadTexture(level->tileset_path);
}

static void level_load_tilemap(level_t *level) {
  FILE *tilemap_file = fopen(level->tilemap_path, "r");
  if (tilemap_file == NULL) {
    TraceLog(LOG_FATAL,"LEVEL: Error loading tilemap file!");
    exit(1);
  } else TraceLog(LOG_INFO, "LEVEL: Loaded tilemap file successfully");

  char tilemap_buffer[1000];
  int y = 0;
  while(fgets(tilemap_buffer, sizeof(tilemap_buffer), tilemap_file) && y < level->tilemap_rows) {
    if (is_comment_line(tilemap_buffer)) continue;
    char *token = strtok(tilemap_buffer, " \n");
    int x = 0;
    while (token) {
      int tilemap_tile = atoi(token);
      level->tilemap[y][x] = tilemap_tile;
      token = strtok(NULL, " ");
      x++;
    }
    y++;
  }

  if (fclose(tilemap_file) != 0) TraceLog(LOG_WARNING, "LEVEL: Failed to close tilemap file");
  else  TraceLog(LOG_INFO, "LEVEL: Close tilemap file successfully");
}

static void level_load_tiledef(level_t *level) {
  FILE *tiledef_file = fopen(level->tiledef_path, "r");
  if (tiledef_file == NULL) {
    TraceLog(LOG_FATAL, "LEVEL: Error loading tiledef file!");
    exit(1);
  } else TraceLog(LOG_INFO, "LEVEL: Loaded tiledef file successfully");

  char tiledef_buffer[1000];
  int y = 0;
  while(fgets(tiledef_buffer, sizeof(tiledef_buffer), tiledef_file) && y < level->tiledef_count) {
    if (is_comment_line(tiledef_buffer)) continue;
    char *token = NULL;
    token = strtok(tiledef_buffer, " \n");
    int index = atoi(token);
    if (index > level->tiledef_count - 1) {
      TraceLog(LOG_FATAL, "LEVEL: Tile definition index exeeded tiledef_count");
    }
    token = strtok(NULL, " \n");
    int is_walkable = atoi(token);
    if (is_walkable != 0 && is_walkable != 1) {
      TraceLog(LOG_FATAL, "LEVEL: Tile definition is_walkable has to be a either 1 or 0");
    }
    tiledef_t tiledef = {0};
    tiledef.index = index;
    tiledef.is_walkable = is_walkable;

    level->tiledef[index] = tiledef;
  }

  if (fclose(tiledef_file) != 0) TraceLog(LOG_WARNING, "LEVEL: Failed to close tiledef file");
  else  TraceLog(LOG_INFO, "LEVEL: Close tiledef file successfully");
}

void level_load_collision_maks(level_t *level) {
  level->collision_maks = NULL;
}

void level_init(level_t *level, level_options_t *level_options){
  level->tileset_tile_size     = level_options->tileset_tile_size;
  level->tileset_tiles_per_row = level_options->tileset_tiles_per_row;
  level->tileset_path          = level_options->tileset_path;
  level->tiledef_count         = level_options->tiledef_count;
  level->tiledef_path          = level_options->tiledef_path;
  level->tilemap_rows          = level_options->tilemap_rows;
  level->tilemap_cols          = level_options->tilemap_cols;
  level->tilemap_path          = level_options->tilemap_path;

  level->tilemap = (int**)malloc(level->tilemap_rows * sizeof(int *));
  if (level->tilemap == NULL) {
    TraceLog(LOG_FATAL, "LEVEL: Failed to allocate memory for tilemap");
    level_free(level);
    exit(1);
  }
  for (int y = 0; y < level->tilemap_rows; y++) {
    level->tilemap[y] = (int*)malloc(level->tilemap_cols * sizeof(int));
    if (level->tilemap[y] == NULL) {
      TraceLog(LOG_FATAL, "LEVEL: Failed to allocate memory for tilemap row");
      level_free(level);
      exit(1);
    }
  }

  level->tiledef = (tiledef_t *)malloc(level->tiledef_count * sizeof(tiledef_t));
  if (level->tiledef == NULL) {
    TraceLog(LOG_FATAL, "LEVEL: Failed to allocate memory for tiledef");
    level_free(level);
    exit(1);
  }

  level->collision_maks = (int **)malloc(level->tilemap_rows * sizeof(int *));
  if (level->collision_maks == NULL) {
    TraceLog(LOG_FATAL, "LEVEL: Failed to allocate memory for collision_maks");
    level_free(level);
    exit(1);
  }
  for (int y = 0; y < level->tilemap_rows; y++) {
    level->collision_maks[y] = malloc(level->tilemap_cols * sizeof(int));
    if (level->collision_maks[y] == NULL) {
      TraceLog(LOG_FATAL, "LEVEL: Failed to allocate memory for collision_maks row");
      level_free(level);
      exit(1);
    }
  }

  level_load_tileset(level);
  level_load_tilemap(level);
  level_load_tiledef(level);
  level_load_collision_maks(level);
}

void level_free(level_t *level) {
  UnloadTexture(level->tileset);
  TraceLog(LOG_INFO, "LEVEL: Successfully freed tileset");
  
  if (level->tilemap) {
    for (int y = 0; y < level->tilemap_rows; y++) {
      if (level->tilemap[y]) free(level->tilemap[y]);
    }
    free(level->tilemap);
    level->tilemap = NULL;
    TraceLog(LOG_INFO, "LEVEL: Successfully freed tilemap");
  }

  if (level->tiledef) {
    free(level->tiledef);
    level->tiledef = NULL;
    TraceLog(LOG_INFO, "LEVEL: Successfully freed tiledef");
  }

  if (level->collision_maks) {
    for (int y = 0; y < level->tilemap_rows; y++) {
      if (level->collision_maks[y]) free(level->collision_maks[y]);
    }
    free(level->collision_maks);
    level->collision_maks = NULL;
    TraceLog(LOG_INFO, "LEVEL: Successfully freed collision_maks");
  }
}

void level_draw(level_t *level) {
  for (int y = 0; y < level->tilemap_rows; y++) {
    for (int x = 0; x < level->tilemap_cols; x++) {
      int index = level->tilemap[y][x];
      int source_x = (index % level->tileset_tiles_per_row) * level->tileset_tile_size;
      int source_y = (index) / level->tileset_tiles_per_row * level->tileset_tile_size;
      int source_w = level->tileset_tile_size;
      int source_h = level->tileset_tile_size;
      Rectangle source = {source_x, source_y, source_w, source_h};
      Rectangle dest = {x * level->tileset_tile_size, y * level->tileset_tile_size,
                        level->tileset_tile_size, level->tileset_tile_size};

      DrawTexturePro(level->tileset, source, dest, (Vector2){0, 0}, 0.0f,
                     WHITE);
    }
  }
}
