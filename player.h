#include <raylib.h>
#include "level.h"

#ifndef PLAYER_H
#define PLAYER_H
#define HITBOX_TOLERANCE 2;

typedef struct 
{
  Vector2 position;
  Vector2 velocity;
  int size;
  Color color;
} player;

player init_player(int size, Vector2 position, Vector2 velocity);
void   update_player(player *player, level *level);
void   draw_player(player *player);

#endif
