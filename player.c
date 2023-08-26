#include <stdio.h>
#include <raylib.h>
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

void UpdatePlayer(Player *player, Level *level) 
{
  TextFormat("W: %d, H: %d", level->width, level->height);
  player->velocity = (Vector2){0.0f, 0.0f};

  Vector2 userInputVelocity = (Vector2){0.0f, 0.0f};

  if (IsKeyDown(KEY_UP))    userInputVelocity.y = -2.0f;
  if (IsKeyDown(KEY_DOWN))  userInputVelocity.y =  2.0f;
  if (IsKeyDown(KEY_LEFT))  userInputVelocity.x = -2.0f;
  if (IsKeyDown(KEY_RIGHT)) userInputVelocity.x =  2.0f;

  Vector2 proposedPosition = {
    player->position.x + player->velocity.x + userInputVelocity.x,
    player->position.y + player->velocity.y + userInputVelocity.y,
  };

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
