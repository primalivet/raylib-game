#include <stdbool.h>
#include <raylib.h>

#ifndef LEVEL_H
#define LEVEL_H
#ifndef TILE_SIZE
#define TILE_SIZE 16
#endif

#ifndef TILES_PER_ROW
#define TILES_PER_ROW 20
#endif

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
  int     tiles_count;
  Texture *tileset;
  int     **tilemap;
  tile_def *tile_defs;
} level;

level load_level(int tile_size, const char *tilemap_path, const char *tile_defs_path, Texture2D *tileset);
void  unload_level(level *level);
void  draw_level(level *level);

#endif
