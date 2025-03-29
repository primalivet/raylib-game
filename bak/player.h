#include "vector2.h"
#include "input.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
  vector2_t position;
  vector2_t directon;
  float     speed;
  float     friction;
  float     width;
  float     height;
  int       reset_dir_frames_delay;
} player_options_t;

typedef struct {
  vector2_t position;
  vector2_t direction;
  vector2_t velocity;
  float     speed;
  float     friction;
  Rectangle aabb;
  int       reset_dir_frames_delay;
} player_t;

void player_init(player_t *player, player_options_t *player_options);
void player_update(player_t *player, entity_input_comp_t *input);
void player_draw(player_t *player);

#endif
