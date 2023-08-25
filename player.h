#include <raylib.h>

#ifndef PLAYER_H
#define PLAYER_H

typedef struct 
{
  Vector2 position;
  Vector2 velocity;
  int size;
} Player;

Player InitPlayer(int size, Vector2 position, Vector2 velocity);
void   UpdatePlayer(Player *player);
void   DrawPlayer(Player *player);

#endif
