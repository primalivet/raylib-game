#ifndef ENTITY_H
#define ENTITY_H
#include <raylib.h>
#include "vector2.h"

#define MAX_ENTITIES 10
#define MAX_BULLETS 100 

typedef struct entity_animation_t {
  Texture2D texture;
  int tile_size;
  int frame_count;
  int frame_current;
  int frame_counter;
  int frame_row;
  int frames_per_update;
  Rectangle current_clip;
} entity_animation_t;

typedef struct entity_physics_comp_t {
  int       entity_id;
  vector2_t position;
  vector2_t proposed_position;
  vector2_t direction;
  vector2_t velocity;
  float     speed;
  float     friction;
  Rectangle aabb;
  int       reset_dir_frames_delay;
} entity_physics_comp_t;

typedef struct entity_input_comp_t {
  bool up;
  bool down;
  bool left;
  bool right;
  bool space;
  bool space_prev;
} entity_input_comp_t;

typedef enum entity_behaviour_t {
  ENTITY_BEHAVIOUR_STATIONARY = 0,
  ENTITY_BEHAVIOUR_PATROL = 1
} entity_behaviour_t;

typedef enum entity_type_t {
  ENTITY_TYPE_PLAYER,
  ENTITY_TYPE_NPC,
} entity_type_t;

typedef enum entity_facing_t {
  ENTITY_FACING_NORTH,
  ENTITY_FACING_SOUTH,
  ENTITY_FACING_EAST,
  ENTITY_FACING_WEST
} entity_facing_t;

typedef struct entity_npc_t {
  entity_behaviour_t behaviour;
} entity_npc_t;

typedef struct entity_player_t {
  entity_input_comp_t   input;
} entity_player_t;

typedef struct entity_t {
  int id;
  int health;
  int active;
  entity_facing_t facing;
  entity_animation_t animation;
  entity_type_t type;
  entity_physics_comp_t physics;
  Color    color;

  union {
    entity_player_t player;
    entity_npc_t npc;
  };
} entity_t;

typedef struct entity_bullet_t {
    vector2_t direction;
    vector2_t position;
    vector2_t velocity;
    bool active;
} entity_bullet_t;

typedef struct entities_options_t {
  const char *entities_path;
} entities_options_t;

typedef struct entities_t {
  entity_t *player;
  entity_t *enemies[MAX_ENTITIES - 1];
  int      enemies_count;
  entity_t *entities[MAX_ENTITIES];
  int      entities_count;
  entity_bullet_t *bullets[MAX_BULLETS];
  int     bullets_count;
} entities_t;


void entities_init(entities_t *entities, entities_options_t *entities_options);
void entities_free(entities_t *entities);

#endif
