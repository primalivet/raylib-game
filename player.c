#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>
#include "player.h"
#include "level.h"
#include "vector2.h"

Player InitPlayer(int size, Vector2 position, Vector2 velocity) 
{
  Player player;
  player.size     = size;
  player.position = position;
  player.velocity = velocity;
  player.color    = WHITE;
  return player;
}

bool CheckTilemapCollision(Rectangle *object, Level *level) 
{
  bool colliding = false;
  for (int y = 0; y < level->height; y++)
  {
    for (int x = 0; x < level->width; x++)
    {
      int index = level->tileMap[y][x];
      TileDef tileDef = level->tileDefs[index];

      if (!tileDef.isWalkable ) {
        Rectangle worldRect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        if ((object->x < (worldRect.x + worldRect.width) && (object->x + object->width) > worldRect.x) &&
        (object->y < (worldRect.y + worldRect.height) && (object->y + object->height) > worldRect.y))
        {
        colliding = true;
        }
      }
    }
  }
  return colliding;
}

void UpdatePlayer(Player *player, Level *level) 
{
  TextFormat("W: %d, H: %d", level->width, level->height);
  player->velocity = (Vector2){0.0f, 0.0f};

  Vector2 userInputVelocity = (Vector2){0.0f, 0.0f};
  Vector2 proposedPosition  = { player->position.x, player->position.y }; // just start at the "prev position"
  bool colliding            = false;

  if (IsKeyDown(KEY_LEFT))  userInputVelocity.x = -2.0f;
  if (IsKeyDown(KEY_RIGHT)) userInputVelocity.x =  2.0f;
  proposedPosition.x = player->position.x + player->velocity.x + userInputVelocity.x;
  colliding = CheckTilemapCollision(&(Rectangle){ proposedPosition.x, proposedPosition.y, player->size, player->size }, level);
  if (colliding) { proposedPosition.x = player->position.x; } 


  if (IsKeyDown(KEY_UP))    userInputVelocity.y = -2.0f;
  if (IsKeyDown(KEY_DOWN))  userInputVelocity.y =  2.0f;
  proposedPosition.y = player->position.y + player->velocity.y + userInputVelocity.y;
  colliding          = CheckTilemapCollision(&(Rectangle){ proposedPosition.x, proposedPosition.y, player->size, player->size }, level);
  if (colliding) { proposedPosition.y = player->position.y; } 

  Rectangle levelBoundary = {
    0.0f, 
    0.0f, 
    level->width * TILE_SIZE - player->size, 
    level->height * TILE_SIZE - player->size 
  };

  // Clamp position to level boundary
  proposedPosition = ClampVector2(proposedPosition, levelBoundary);

  // TODO: Use the corners to determine collision with non walkable tiles. If
  // colliding, modify proposedPosition to a non colliding position Vector2
  //
  // TODO: Support "sliding", meaning player who is colliding on top side should
  // be able to move right and left and down but not up.
  //

  // proposedTopLeftCorner     = { proposedPosition.x, proposedPosition.y };
  // Vector2 proposedTopRightCorner    = { proposedPosition.x + player->size,
  // proposedPosition.y }; Vector2 proposedBottomRightCorner = {
  // proposedPosition.x + player->size, proposedPosition.y + player->size };
  // Vector2 proposedBottomLeftCorner  = { proposedPosition.x,
  // proposedPosition.y + player->size };

  player->position = proposedPosition;
}

void DrawPlayer(Player *player) 
{
  DrawTextEx(
    GetFontDefault(), 
    TextFormat("X: %f\n Y: %f", player->position.x, player->position.y),
    (Vector2){player->position.x + 10.0f + player->size, player->position.y },
    5.0f,
    1.0f,
    WHITE
  );
  DrawRectangle(player->position.x, player->position.y, player->size, player->size, player->color);
}
