#include <stdlib.h>
#include <raylib.h>
#include "enemy.h"
#include "vector2.h"
#include "player.h"

enemy init_enemy(int size, Vector2 position) {
  enemy enemy;
  enemy.size     = size;
  enemy.position = position;
  enemy.velocity = (Vector2){0.5f, 0.0f};
  return enemy;
}

void update_enemy(enemy *enemy, level *level) {

  // TODO: Implement enemy paths
  
  Vector2 proposed_position = { enemy->position.x + enemy->velocity.x, enemy->position.y };
  if (check_tilemap_collision(&(Rectangle){proposed_position.x, proposed_position.y, enemy->size, enemy->size }, level)) {
    if (enemy->velocity.x < 0) enemy->velocity.x = 0.5f;
    else enemy->velocity.x = -0.5f;
  } else {
    enemy->position = proposed_position;
  }
}

void draw_enemy(enemy *enemy) {
  DrawRectangle(enemy->position.x, enemy->position.y, enemy->size, enemy->size, RED);
}
