#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "level.h"

Level LoadLevel(int tileSize, const char *tilemapFilePath, const char *tiledefsFilePath, Texture2D *tileset) 
{
  FILE *tilemap_data_file = fopen(tilemapFilePath, "r");
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

  int tilesCount = (tileset->width /tileSize) * (tileset->height /tileSize);
  int **tileMap  = (int **)malloc(height * sizeof(int*));

  for (int i = 0; i < height; i++) 
  {
    tileMap[i] = (int *)malloc(width * sizeof(int));
  }

  char tilemap_line_buffer[1000]; // Buffer line cannot exeed
  
  int y = 0;
  while(fgets(tilemap_line_buffer, sizeof(tilemap_line_buffer), tilemap_data_file)) 
  {
    // Replace unix and windows line ending with null terminator
    char *newline                = strchr(tilemap_line_buffer, '\n'); 
    if (newline) *newline        = '\0';
    char *carriageReturn         = strchr(tilemap_line_buffer, '\r');
    if (carriageReturn) *newline = '\0';
    char *token                  = strtok(tilemap_line_buffer, " \n"); 
    int x                        = 0;
    while (token) 
    {
      int tileIndex = atoi(token); 
      tileMap[y][x] = tileIndex;
      token         = strtok(NULL, " "); 
      x++;
    }
    y++;
  }

  fclose(tilemap_data_file);

  FILE *tiledef_data_file = fopen(tiledefsFilePath, "r");
  if (tiledef_data_file == NULL) 
  {
    printf("Error loading level tile definitions file!");
  }

  TileDef *tileDefs = (TileDef *)malloc(tilesCount * sizeof(TileDef));
  char    tiledef_buffer[1000]; // Buffer line cannot exeed

  while(fgets(tiledef_buffer, sizeof(tiledef_buffer), tiledef_data_file)) 
  {
    // Replace unix and windows line ending with null terminator
    char *newline                = strchr(tiledef_buffer, '\n'); 
    if (newline) *newline        = '\0'; 
    char *carriageReturn         = strchr(tiledef_buffer, '\r');
    if (carriageReturn) *newline = '\0'; 

    char *token                  = strtok(tiledef_buffer, " \n");
    int tileIndex                = atoi(token);
    token                        = strtok(NULL, " \n");
    int isWalkable               = atoi(token);
    tileDefs[tileIndex]          = (TileDef) { .index = tileIndex, .isWalkable = isWalkable };
  }

  Level level;
  level.width      = width;
  level.height     = height;
  level.tileSize   = tileSize;
  level.tileSet    = tileset;
  level.tilesCount = tilesCount;
  level.tileMap    = tileMap;
  level.tileDefs   = tileDefs;

  return level;
}

void UnloadLevel(Level *level) 
{
  for (int y = 0; y < level->height; y++) 
  {
    free(level->tileMap[y]);
  }
  free(level->tileMap);
}

void DrawLevel(Level *level) 
{
  for (int y = 0; y < level->height; y++) 
  {
   for (int x = 0; x < level->width; x++) 
    {
      int index        = level->tileMap[y][x];
      int sourceX      = (index % TILES_PER_ROW) * TILE_SIZE;
      int sourceY      = (index) / TILES_PER_ROW * TILE_SIZE;
      int sourceW      = TILE_SIZE;
      int sourceH      = TILE_SIZE;
      Rectangle source = { sourceX      , sourceY      , sourceW  , sourceH };
      Rectangle dest   = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE }; 

      DrawTexturePro(*level->tileSet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
   } 
  }
}
