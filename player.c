#include <stdio.h>
#include <raylib.h>
#include <stdbool.h>
#include "player.h"
#include "level.h"
#include "vector2.h"

player init_player(int size, Vector2 position, Vector2 velocity) 
{
  player player;
  player.size     = size;
  player.position = position;
  player.velocity = velocity;
  player.color    = WHITE;
  return player;
}

bool check_tilemap_collision(Rectangle *object, level *level) 
{
  bool colliding = false;
  for (int y = 0; y < level->height; y++)
  {
    for (int x = 0; x < level->width; x++)
    {
      int tilemap_index = level->tilemap[y][x];
      tile_def tile_def = level->tile_defs[tilemap_index];

      if (!tile_def.is_walkable ) {
        Rectangle tile_rect = { x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
        if ((object->x < (tile_rect.x + tile_rect.width) && (object->x + object->width) > tile_rect.x) &&
        (object->y < (tile_rect.y + tile_rect.height) && (object->y + object->height) > tile_rect.y))
        {
        colliding = true;
        }
      }
    }
  }
  return colliding;
}

void update_player(player *player, level *level) 
{
  player->velocity            = (Vector2){0.0f, 0.0f};
  Vector2 user_input_velocity = (Vector2){0.0f, 0.0f};
  Vector2 proposed_position   = { player->position.x, player->position.y }; // just start at the "prev position"
  bool colliding              = false;

  if (IsKeyDown(KEY_LEFT))  user_input_velocity.x = -2.0f;
  if (IsKeyDown(KEY_RIGHT)) user_input_velocity.x =  2.0f;

  proposed_position.x = player->position.x + player->velocity.x + user_input_velocity.x;
  colliding           = check_tilemap_collision( &(Rectangle){ proposed_position.x, proposed_position.y, player->size, player->size }, level);

  if (colliding) proposed_position.x = player->position.x; 


  if (IsKeyDown(KEY_UP))    user_input_velocity.y = -2.0f;
  if (IsKeyDown(KEY_DOWN))  user_input_velocity.y =  2.0f;

  proposed_position.y = player->position.y + player->velocity.y + user_input_velocity.y;
  colliding           = check_tilemap_collision(&(Rectangle){ proposed_position.x, proposed_position.y, player->size, player->size }, level);

  if (colliding) proposed_position.y = player->position.y; 

  Rectangle level_boundary = {
    0.0f, 
    0.0f, 
    level->width * TILE_SIZE - player->size, 
    level->height * TILE_SIZE - player->size 
  };

  proposed_position = clamp_vector2(proposed_position, level_boundary);
  player->position  = proposed_position;
}

void draw_player(player *player) 
{
  DrawRectangle(player->position.x, player->position.y, player->size, player->size, player->color);
}
