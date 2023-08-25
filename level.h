#include <stdbool.h>
#include <raylib.h>

#ifndef LEVEL_H
#ifndef TILE_SIZE
#define TILE_SIZE 16
#endif

#ifndef TILES_PER_ROW
#define TILES_PER_ROW 20
#endif

#define LEVEL_H
typedef struct 
{
  int  index;
  bool isWalkable;
} TileDef;

typedef struct 
{
  int     width;
  int     height;
  int     tileSize;
  int     tilesCount;
  Texture *tileSet;
  int     **tileMap;
  TileDef *tileDefs;
} Level;

Level LoadLevel(int tileSize, const char *tilemapFilePath, const char *tiledefsFilePath, Texture2D *tileSet);
void  UnloadLevel(Level *level);
void  DrawLevel(Level *level);

#endif
