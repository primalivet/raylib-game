#include <stdlib.h>
#include <math.h>
#include "raylib.h"
#include "entity.h"
#include "level.h"
#include "vector2.h"

typedef struct {
  vector2_t n;
  vector2_t s;
  vector2_t w;
  vector2_t e;
  vector2_t nw;
  vector2_t ne;
  vector2_t sw;
  vector2_t se;
} cardinal_positions_t;

cardinal_positions_t surrounding_cardinal_positions(level_t *level, vector2_t *position) {
  int tile_x = (int)floor(position->x / level->tileset_tile_size);
  int tile_y = (int)floor(position->y / level->tileset_tile_size); 

  cardinal_positions_t cardinal_positions = {0};

  cardinal_positions.n  = (vector2_t){ .x = tile_x,     .y = tile_y - 1 };
  cardinal_positions.s  = (vector2_t){ .x = tile_x,     .y = tile_y + 1 };
  cardinal_positions.w  = (vector2_t){ .x = tile_x - 1, .y = tile_y     };
  cardinal_positions.e  = (vector2_t){ .x = tile_x + 1, .y = tile_y     };
  cardinal_positions.nw = (vector2_t){ .x = tile_x - 1, .y = tile_y - 1 };
  cardinal_positions.ne = (vector2_t){ .x = tile_x + 1, .y = tile_y - 1 };
  cardinal_positions.sw = (vector2_t){ .x = tile_x - 1, .y = tile_y + 1 };
  cardinal_positions.se = (vector2_t){ .x = tile_x + 1, .y = tile_y + 1 };

  return cardinal_positions;
}
void surrounding_cardinal_positions_list(vector2_t output[], level_t *level, vector2_t *position) {
  cardinal_positions_t cardinal_positions = surrounding_cardinal_positions(level, position);
  output[0] = cardinal_positions.n;
  output[1] = cardinal_positions.s;
  output[2] = cardinal_positions.w;
  output[3] = cardinal_positions.e;
  output[4] = cardinal_positions.nw;
  output[5] = cardinal_positions.ne;
  output[6] = cardinal_positions.sw;
  output[7] = cardinal_positions.se;
}

bool collision_intersect_rect(Rectangle first, Rectangle second) {
  bool intersects = (
    (first.x < (second.x + second.width) &&  // Left   edge of first is to the right of the right edge of second
    (first.x + first.width) > second.x) &&   // Right  edge of first is to the left  of the left  edge of second
    (first.y < (second.y + second.height) && // Top    edge of first is above the bottom edge of second
    (first.y + first.height) > second.y)     // Bottom edge of first is below the top    edge of second
  );
  return intersects;
}

/*void collision_update(level_t *level, entities_t *entities) {*/
/*  vector2_t cardinal_positions[8] = {0};*/
/*  surrounding_cardinal_positions_list(cardinal_positions, level, &entities->player.physics.position);*/
/**/
/*  for(int i = 0; i <= 7; i++) {*/
/*    int x = cardinal_positions[i].x;*/
/*    int y = cardinal_positions[i].y;*/
/**/
/*    int tile_idx = level->tilemap[y][x];*/
/*    tiledef_t tiledef = level->tiledef[tile_idx];*/
/**/
/*    if (!tiledef.is_walkable) {*/
/*      // TODO: handle surrounding collision*/
/*    }*/
/*  }*/
/*}*/

void physics_update(entity_physics_comp_t *physics_body, level_t *level,  entity_input_comp_t *input) {
  /**
   * apply friction, but stop when 0.0f so we dont get negative friction / auto sliding
   */
  if      (physics_body->velocity.y < 0.0f) physics_body->velocity.y = fmin(physics_body->velocity.y + physics_body->friction, 0.0f);
  else if (physics_body->velocity.y > 0.0f) physics_body->velocity.y = fmax(physics_body->velocity.y - physics_body->friction, 0.0f);
  if      (physics_body->velocity.x < 0.0f) physics_body->velocity.x = fmin(physics_body->velocity.x + physics_body->friction, 0.0f);
  else if (physics_body->velocity.x > 0.0f) physics_body->velocity.x = fmax(physics_body->velocity.x - physics_body->friction, 0.0f);

  /**
   * apply input direction
   */
  if (input != NULL) {
    if      (input->up)    physics_body->direction.y = -1.0f;
    else if (input->down)  physics_body->direction.y =  1.0f;
    if      (input->left)  physics_body->direction.x = -1.0f;
    else if (input->right) physics_body->direction.x =  1.0f;

    /**
   * delay reset of direction axis
   *
   * note: direction is reset with a delay over an amount of frames to be
   * more tolerant towards the user releasing 2 opposite directional input 
   * keys with some delay. this way the physics_body keeps the "last used direction"
   */
    static int frames_counter = 0;
    frames_counter++;
    if (frames_counter == physics_body->reset_dir_frames_delay) {
      if ( !(input->up || input->down) && (input->left || input->right)) physics_body->direction.y = 0.0f; // reset y
      if ((input->up || input->down) && !(input->left || input->right)) physics_body->direction.x = 0.0f;  // reset x
      frames_counter = 0;
    }
    /**
   * apply input velocity
   */
    if (input->up)    physics_body->velocity.y = -2.0f;
    if (input->down)  physics_body->velocity.y =  2.0f;
    if (input->left)  physics_body->velocity.x = -2.0f;
    if (input->right) physics_body->velocity.x =  2.0f;
  }

  vector2_t proposed_position = { .x = physics_body->position.x + (physics_body->velocity.x * physics_body->speed), 
                                  .y = physics_body->position.y + (physics_body->velocity.y * physics_body->speed) };
  Rectangle proposed_aabb =     { .x = proposed_position.x, 
                                  .y = proposed_position.y, 
                                  .width = physics_body->aabb.width, 
                                  .height = physics_body->aabb.height };

  vector2_t cardinal_positions[8] = {0};
  surrounding_cardinal_positions_list(cardinal_positions, level, &proposed_position);
  
  bool is_colliding = false;
  for(int i = 0; i <= 7; i++) {
    int x = cardinal_positions[i].x;
    int y = cardinal_positions[i].y;

    if ( y < 0 || y >= level->tilemap_rows ||
         x < 0 || x >= level->tilemap_cols) {
      /*TraceLog(LOG_ERROR, "Out of bounds (x: %d, y: %d)", x,y);*/
      continue;
    }

    int tile_idx = level->tilemap[y][x];
    tiledef_t tiledef = level->tiledef[tile_idx];
    Rectangle cardinal_tile = (Rectangle){ .x = x, .y = y, .width = level->tileset_tile_size, .height = level->tileset_tile_size};

    if (!tiledef.is_walkable) {
      is_colliding = collision_intersect_rect(proposed_aabb, cardinal_tile);
    }
  }

  // TODO: apply collision response if colliding
  // TODO: apply position if no colliding
  // TODO: handle wall sliding

  if (is_colliding) {
    physics_body->velocity = (vector2_t){ .x = 0.0f, .y = 0.0f };
    /*physics_body->aabb = proposed_aabb;*/
  } else {
    physics_body->position = proposed_position;
    physics_body->aabb = proposed_aabb;
  }
  


  ///**
  // * adjust position from velocity scaled by speed
  // */
  //physics_body->position.y += physics_body->velocity.y * physics_body->speed;
  //physics_body->position.x += physics_body->velocity.x * physics_body->speed;

  ///**
  // * center aabb around position
  // */
  //physics_body->aabb.x = physics_body->position.x - physics_body->aabb.width / 2.0f;
  //physics_body->aabb.y = physics_body->position.y - physics_body->aabb.height / 2.0f;
}
