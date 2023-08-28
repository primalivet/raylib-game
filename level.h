#include <stdbool.h>
#include <raylib.h>

#ifndef LEVEL_H
#define LEVEL_H

typedef struct 
{
  int  index;
  bool is_walkable;
} tile_def;

typedef struct 
{
  int     width;
  int     height;
  int     tile_size;
  int     tiles_per_row;
  int     tiles_count;
  Texture *tileset;
  int     **tilemap;
  tile_def *tile_defs;
} level;

level load_level(int tile_size, int tiles_per_row, const char *tilemap_path, const char *tile_defs_path, Texture2D *tileset);
void  unload_level(level *level);
void  draw_level(level *level);

#endif
