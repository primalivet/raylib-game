#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>

//----------------------------------------------------------------------------- 
// DEFINES
//----------------------------------------------------------------------------- 

#define TITLE         "Game"
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

#define TILE_SIZE     16
#define TILES_PER_ROW 20
#define TARGET_FPS    60
#define BACKGROUND    CLITERAL(Color){ 29, 42, 47, 255 }         

//----------------------------------------------------------------------------- 
// TYPEDEFS
//----------------------------------------------------------------------------- 

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
  Texture tileSet;
  int     **tileMap;
  TileDef *tileDefs;
} Level;

typedef struct 
{
  Vector2 position;
  Vector2 velocity;
  int size;
} Player;

//----------------------------------------------------------------------------- 
// LEVEL FNS
//----------------------------------------------------------------------------- 

Level LoadLevel(int tileSize, const char *tilemapFilePath, const char *tiledefsFilePath, const char *tilesetFilePath) 
{
  Texture2D tileSet = LoadTexture(tilesetFilePath); 

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

  int tilesCount = (tileSet.width /tileSize) * (tileSet.height /tileSize);
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
  level.tileSet    = tileSet;
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

      DrawTexturePro(level->tileSet, source, dest, (Vector2){0,0}, 0.0f, WHITE);
   } 
  }
}

//----------------------------------------------------------------------------- 
// CAMERA FNS
//----------------------------------------------------------------------------- 

Camera2D InitCam(Vector2 target, Vector2 offset) 
{
  Camera2D camera;
  camera.target   = target;
  camera.offset   = offset;
  camera.rotation = 0;
  camera.zoom     = 1.0f;
  return camera;
}

void UpdateCam(Camera2D *camera, Vector2 target) 
{
    camera->target = target;
}

//----------------------------------------------------------------------------- 
// PLAYER FNS
//----------------------------------------------------------------------------- 

Player InitPlayer(int size, Vector2 position, Vector2 velocity) 
{
  Player player;
  player.size     = size;
  player.position = position;
  player.velocity = velocity;
  return player;
}

void UpdatePlayer(Player *player) 
{
  player->velocity = (Vector2){0.0f, 0.0f};

  if (IsKeyDown(KEY_UP))    player->velocity.y = -2.0f;
  if (IsKeyDown(KEY_DOWN))  player->velocity.y =  2.0f;
  if (IsKeyDown(KEY_LEFT))  player->velocity.x = -2.0f;
  if (IsKeyDown(KEY_RIGHT)) player->velocity.x =  2.0f;

  player->position.x += player->velocity.x;
  player->position.y += player->velocity.y;
}

void DrawPlayer(Player *player) 
{
  DrawRectangle(player->position.x, player->position.y, player->size, player->size, RED);
}

//----------------------------------------------------------------------------- 
// MAIN
//----------------------------------------------------------------------------- 

int main(void) 
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  SetTargetFPS(TARGET_FPS);

  Level level     = LoadLevel( TILE_SIZE, "level_1_tilemap.txt", "level_1_tiledef.txt", "tiles-16.png");
  Player player   = InitPlayer(TILE_SIZE, (Vector2){2.0f * TILE_SIZE, 2.0f * TILE_SIZE}, (Vector2){0.0f, 0.0f});
  Camera2D camera = InitCam(player.position, (Vector2) { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f });

  while(!WindowShouldClose()) 
  {
    UpdatePlayer(&player);

    UpdateCam(&camera, player.position); 

    BeginDrawing();
    BeginMode2D(camera);

    ClearBackground(BACKGROUND);
    DrawLevel(&level);
    DrawPlayer(&player);

    EndMode2D();
    EndDrawing();
  }

  CloseWindow();

  UnloadLevel(&level);

  return 0;
}
