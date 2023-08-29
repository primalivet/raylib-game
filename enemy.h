#ifndef ENEMY_H
#define ENEMY_H

#include <raylib.h>
#include "level.h"

typedef struct {
  Vector2 position;
  Vector2 velocity;
  int size;
} enemy;

enemy init_enemy(int size, Vector2 position);
void update_enemy(enemy *enemy, level *level);
void draw_enemy(enemy *enemy);

#endif
