#include <raylib.h>
#include "level.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef struct 
{
  Vector2 position;
  Vector2 velocity;
  int size;
  Color color;
} Player;

Player InitPlayer(int size, Vector2 position, Vector2 velocity);
void   UpdatePlayer(Player *player, Level *level);
void   DrawPlayer(Player *player);

#endif
