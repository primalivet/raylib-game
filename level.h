#include <stdbool.h>
#include <stdio.h>
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
  int        width;
  int        height;
  int        tile_size;
  int        tiles_per_row;
  int        tiles_count;
  Texture   *tileset;
  int      **tilemap;
  tile_def  *tile_defs;
  int      **collision_mask;
} level;

// Alias for consistent naming
// TODO: Learn OpenGL and replace raylib textures
typedef Texture2D texture_t;

typedef struct {
  int index;
  bool is_walkable;
} tiledef_t; 

typedef struct {

  int         tileset_tile_size;
  int         tileset_tiles_per_row;
  const char *tileset_path;
  int         tiledef_count;
  const char *tiledef_path;
  int         tilemap_rows;
  int         tilemap_cols;
  const char *tilemap_path;
} level_options_t;

typedef struct {
  int          tileset_tile_size;
  int          tileset_tiles_per_row;
  const char  *tileset_path;
  texture_t    tileset;

  int          tiledef_count;
  const char  *tiledef_path;
  FILE        *tiledef_file;
  tiledef_t   *tiledef;

  int          tilemap_rows;
  int          tilemap_cols;
  const char  *tilemap_path;
  FILE        *tilemap_file;
  int        **tilemap;

  int        **collision_maks;

} level_t;

void level_init(level_t *level, level_options_t *level_options);
void level_free(level_t *level);

void level_update(level_t *level);
void level_draw(level_t *level);


/*
level load_level(int tile_size, int tiles_per_row, const char *tilemap_path, const char *tile_defs_path, Texture2D *tileset);
void  unload_level(level *level);
void  draw_level(level *level);
*/

#endif
