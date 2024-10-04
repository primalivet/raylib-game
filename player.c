#include <math.h>
#include <raylib.h>
#include "vector2.h"
#include "input.h"
#include "player.h"

void player_init(player_t *player, player_options_t *player_options) {
  player->position               = player_options->position;
  player->direction              = (vector2_t){.x = 0.0f, .y = 0.0f};
  player->velocity               = (vector2_t){.x = 0.0f, .y = 0.0f};
  player->speed                  = player_options->speed; //1.0f;
  player->friction               = player_options->friction; // 0.2f;
  player->reset_dir_frames_delay = player_options->reset_dir_frames_delay;
  player->aabb      = (Rectangle){ .x      = player->position.x - (player_options->width / 2.0f),
                                   .y      = player->position.y - (player_options->height / 2.0f), 
                                   .width  = player_options->width, 
                                   .height = player_options->height };
}

void player_update(player_t *player, input_t *input) {
  /**
   * Apply friction, but stop when 0.0f so we dont get negative friction / auto sliding
   */
  if      (player->velocity.y < 0.0f) player->velocity.y = fmin(player->velocity.y + player->friction, 0.0f);
  else if (player->velocity.y > 0.0f) player->velocity.y = fmax(player->velocity.y - player->friction, 0.0f);
  if      (player->velocity.x < 0.0f) player->velocity.x = fmin(player->velocity.x + player->friction, 0.0f);
  else if (player->velocity.x > 0.0f) player->velocity.x = fmax(player->velocity.x - player->friction, 0.0f);

  /**
   * Apply input direction
   */
  if      (input->up)    player->direction.y = -1.0f;
  else if (input->down)  player->direction.y =  1.0f;
  if      (input->left)  player->direction.x = -1.0f;
  else if (input->right) player->direction.x =  1.0f;

  /**
   * Delay reset of direction axis
   *
   * NOTE: direction is reset with a delay over an amount of frames to be
   * more tolerant towards the user releasing 2 opposite directional input 
   * keys with some delay. This way the player keeps the "last used direction"
   */
  static int frames_counter = 0;
  frames_counter++;
  if (frames_counter == player->reset_dir_frames_delay) {
    if ( !(input->up || input->down) && (input->left || input->right)) player->direction.y = 0.0f; // reset y
    if ((input->up || input->down) && !(input->left || input->right)) player->direction.x = 0.0f;  // reset x
    frames_counter = 0;
  }

  /**
   * Apply input velocity
   */
  if (input->up)    player->velocity.y = -2.0f;
  if (input->down)  player->velocity.y =  2.0f;
  if (input->left)  player->velocity.x = -2.0f;
  if (input->right) player->velocity.x =  2.0f;

  /**
   * Adjust position from velocity scaled by speed
   */
  player->position.y += player->velocity.y * player->speed;
  player->position.x += player->velocity.x * player->speed;

  /**
   * Center aabb around position
   */
  player->aabb.x = player->position.x - player->aabb.width / 2.0f;
  player->aabb.y = player->position.y - player->aabb.height / 2.0f;
}

void player_draw(player_t *player) {
    DrawRectangleRec(player->aabb, RED);
    DrawRectangleRec((Rectangle){.x = player->position.x + (player->direction.x * (player->aabb.width / 2)) - 2.0f,
                                 .y = player->position.y + (player->direction.y * (player->aabb.height / 2)) - 2.0f,
                                 .width = 4.0f,
                                 .height = 4.0f},
                     GREEN);
}

