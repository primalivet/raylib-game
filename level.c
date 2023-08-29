#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "level.h"

static bool is_comment_line(const char *line)
{
  while (*line)
  {
    if (*line == '#') return true;
    if (!isspace((unsigned int)*line)) return false;
    line++;
  }
  return false;
}

level load_level(int tile_size, int tiles_per_row, const char *tilemap_path, const char *tile_defs_path, Texture2D *tileset) 
{
  FILE *tilemap_data_file = fopen(tilemap_path, "r");
  if (tilemap_data_file == NULL) 
  {
    printf("Error loading level file!");
    exit(1);
  }

  int width, height;

  if (fscanf(tilemap_data_file, "%d %d\n", &width, &height) != 2) 
  {
    printf("Error reading tilemap width and height\n");
    fclose(tilemap_data_file);
    exit(1);
  }

  int tiles_count = (tileset->width /tile_size) * (tileset->height /tile_size);
  int **tilemap  = (int **)malloc(height * sizeof(int*));

  for (int i = 0; i < height; i++) 
  {
    tilemap[i] = (int *)malloc(width * sizeof(int));
  }

  char tilemap_line_buffer[1000]; // Buffer line cannot exeed
  
  int y = 0;
  while(fgets(tilemap_line_buffer, sizeof(tilemap_line_buffer), tilemap_data_file)) 
  {
    if (is_comment_line(tilemap_line_buffer)) continue;
    // Replace unix and windows line ending with null terminator
    char *newline                = strchr(tilemap_line_buffer, '\n'); 
    if (newline) *newline        = '\0';
    char *carriage_return         = strchr(tilemap_line_buffer, '\r');
    if (carriage_return) *newline = '\0';
    char *token                  = strtok(tilemap_line_buffer, " \n"); 
    int x                        = 0;
    while (token) 
    {
      int tilemap_index = atoi(token); 
      tilemap[y][x] = tilemap_index;
      token         = strtok(NULL, " "); 
      x++;
    }
    y++;
  }

  fclose(tilemap_data_file);

  FILE *tiledef_data_file = fopen(tile_defs_path, "r");
  if (tiledef_data_file == NULL) 
  {
    printf("Error loading level tile definitions file!");
  }

  tile_def *tile_defs = (tile_def *)malloc(tiles_count * sizeof(tile_def));
  char    tiledef_buffer[1000]; // Buffer line cannot exeed

  while(fgets(tiledef_buffer, sizeof(tiledef_buffer), tiledef_data_file)) 
  {
    if (is_comment_line(tilemap_line_buffer)) continue;
    // Replace unix and windows line ending with null terminator
    char *newline                = strchr(tiledef_buffer, '\n'); 
    if (newline) *newline        = '\0'; 
    char *carriage_return         = strchr(tiledef_buffer, '\r');
    if (carriage_return) *newline = '\0'; 

    char *token                  = strtok(tiledef_buffer, " \n");
    int tilemap_index                = atoi(token);
    token                        = strtok(NULL, " \n");
    int is_walkable               = atoi(token);
    tile_defs[tilemap_index]          = (tile_def) { .index = tilemap_index, .is_walkable = is_walkable };
  }

  level level;
  level.width         = width;
  level.height        = height;
  level.tile_size     = tile_size;
  level.tiles_per_row = tiles_per_row;
  level.tileset       = tileset;
  level.tiles_count   = tiles_count;
  level.tilemap       = tilemap;
  level.tile_defs     = tile_defs;

  return level;
}

void unload_level(level *level) 
{
  for (int y = 0; y < level->height; y++) 
  {
    free(level->tilemap[y]);
  }
  free(level->tilemap);
}

void draw_level(level *level) 
{
  for (int y = 0; y < level->height; y++) 
  {
   for (int x = 0; x < level->width; x++) 
    {
      int index        = level->tilemap[y][x];
      int source_x      = (index % level->tiles_per_row) * level->tile_size;
      int source_y      = (index) / level->tiles_per_row * level->tile_size;
      int source_w      = level->tile_size;
      int source_h      = level->tile_size;
      Rectangle source = { source_x      , source_y      , source_w  , source_h };
      Rectangle dest   = { x * level->tile_size, y * level->tile_size, level->tile_size, level->tile_size }; 

      DrawTexturePro(*level->tileset, source, dest, (Vector2){0,0}, 0.0f, WHITE);
   }
  }
}
