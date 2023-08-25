#include <raylib.h>
#include "player.h"

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
