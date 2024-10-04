#include "level.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/*
level load_level(int tile_size, int tiles_per_row, const char *tilemap_path,
                 const char *tile_defs_path, Texture2D *tileset) {
  // Read level source file

  FILE *tilemap_data_file = fopen(tilemap_path, "r");
  if (tilemap_data_file == NULL) {
    printf("Error loading level file!");
    exit(1);
  }

  // Create dimensions

  int width, height;

  if (fscanf(tilemap_data_file, "%d %d\n", &width, &height) != 2) {
    printf("Error reading tilemap width and height\n");
    fclose(tilemap_data_file);
    exit(1);
  }

  // Create tiledefs

  int tiles_count =
      (tileset->width / tile_size) * (tileset->height / tile_size);
  int **tilemap = (int **)malloc(height * sizeof(int *));

  for (int i = 0; i < height; i++) {
    tilemap[i] = (int *)malloc(width * sizeof(int));
  }

  char tilemap_line_buffer[1000]; // Buffer line cannot exeed

  int y = 0;
  while (fgets(tilemap_line_buffer, sizeof(tilemap_line_buffer),
               tilemap_data_file)) {
    if (is_comment_line(tilemap_line_buffer))
      continue;
    // Replace unix and windows line ending with null terminator
    char *newline = strchr(tilemap_line_buffer, '\n');
    if (newline)
      *newline = '\0';
    char *carriage_return = strchr(tilemap_line_buffer, '\r');
    if (carriage_return)
      *newline = '\0';
    char *token = strtok(tilemap_line_buffer, " \n");
    int x = 0;
    while (token) {
      int tilemap_index = atoi(token);
      tilemap[y][x] = tilemap_index;
      token = strtok(NULL, " ");
      x++;
    }
    y++;
  }

  fclose(tilemap_data_file);

  // Create tiledefs

  FILE *tiledef_data_file = fopen(tile_defs_path, "r");
  if (tiledef_data_file == NULL) {
    printf("Error loading level tile definitions file!");
  }

  tile_def *tile_defs = (tile_def *)malloc(tiles_count * sizeof(tile_def));
  char tiledef_buffer[1000]; // Buffer line cannot exeed

  while (fgets(tiledef_buffer, sizeof(tiledef_buffer), tiledef_data_file)) {
    if (is_comment_line(tilemap_line_buffer))
      continue;
    // Replace unix and windows line ending with null terminator
    char *newline = strchr(tiledef_buffer, '\n');
    if (newline)
      *newline = '\0';
    char *carriage_return = strchr(tiledef_buffer, '\r');
    if (carriage_return)
      *newline = '\0';

    char *token = strtok(tiledef_buffer, " \n");
    int tilemap_index = atoi(token);
    token = strtok(NULL, " \n");
    int is_walkable = atoi(token);
    tile_defs[tilemap_index] =
        (tile_def){.index = tilemap_index, .is_walkable = is_walkable};
  }

  // Create collision mask

  int **collision_mask = malloc(sizeof(int *) * height);
  if (!collision_mask) {
    printf("Failed to allocate collision mask\n");
    exit(1);
  }
  for (int y = 0; y < height; y++) {
    collision_mask[y] = malloc(sizeof(int) * width);
    for (int x = 0; x < width; x++) {
      int tile_index = tilemap[y][x];
      collision_mask[y][x] = tile_defs[tile_index].is_walkable;
    }
  }

  // Create level stuct

  level level;
  level.width = width;
  level.height = height;
  level.tile_size = tile_size;
  level.tiles_per_row = tiles_per_row;
  level.tileset = tileset;
  level.tiles_count = tiles_count;
  level.tilemap = tilemap;
  level.tile_defs = tile_defs;
  level.collision_mask = collision_mask;

  return level;
}

void unload_level(level *level) {
  for (int y = 0; y < level->height; y++) {
    free(level->tilemap[y]);
  }
  free(level->tile_defs);
  free(level->tilemap);
  for (int y = 0; y < level->height; y++) {
    free(level->collision_mask[y]);
  }
  free(level->collision_mask);
}

void draw_level(level *level) {
  for (int y = 0; y < level->height; y++) {
    for (int x = 0; x < level->width; x++) {
      int index = level->tilemap[y][x];
      int source_x = (index % level->tiles_per_row) * level->tile_size;
      int source_y = (index) / level->tiles_per_row * level->tile_size;
      int source_w = level->tile_size;
      int source_h = level->tile_size;
      Rectangle source = {source_x, source_y, source_w, source_h};
      Rectangle dest = {x * level->tile_size, y * level->tile_size,
                        level->tile_size, level->tile_size};

      DrawTexturePro(*level->tileset, source, dest, (Vector2){0, 0}, 0.0f,
                     WHITE);
    }
  }
}
*/
