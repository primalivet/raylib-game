#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
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

// TODO: move to common place, duplicated in level.c
static bool is_comment_line(const char *line)
{
  while (*line)
  {
    if (*line == '#') return true;
    if (!isspace((unsigned int)*line)) return false;
    line++;
  }
  return false;
}

void load_enemies(const char *enemies_path) {
  FILE *enemies_data_file = fopen(enemies_path, "r");
  if (enemies_data_file == NULL) {
    printf("Error: Could not open enemies data file\n");
    exit(1);
  }

  int enemy_count;
  if (fscanf(enemies_data_file, "%d", &enemy_count)) {
    printf("Error: Could read enemy count in enemies data file\n");
    fclose(enemies_data_file);
    exit(1);
  }

  enemy *enemies = (enemy *)malloc(enemy_count * sizeof(enemy));

  if (enemies == NULL) {
    printf("Error: Could not allocate memory for enemies\n");
    fclose(enemies_data_file);
    exit(1);
  }

  char enemies_line_buffer[1000];
  int i = 0;
  while(fgets(enemies_line_buffer, sizeof(enemies_line_buffer), enemies_data_file)) {
    if (is_comment_line(enemies_line_buffer)) continue;
    // Replace unix and windows line ending with null terminator
    enemy enemy;
    int id;
    int size;
    int matched = sscanf(enemies_line_buffer, "%d , %d", &id, &size);
    if (matched != 2) {
      printf("Unexpected amount of columns at line %d in %s", i + 1, enemies_path);
      exit(1);
    }

    enemy.id = id;
    enemy.size = size;

    enemies[i] = enemy;
  }
  fclose(enemies_data_file);
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
