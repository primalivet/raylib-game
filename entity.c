#include "raylib.h"
#include "entity.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector2.h"

// TODO: move to common place, duplicated in enemy.c
static bool is_comment_line(const char *line) {
  while (*line) {
    if (*line == '#')
      return true;
    if (!isspace((unsigned int)*line))
      return false;
    line++;
  }
  return false;
}

void entities_load(entities_t *entities, entities_options_t *entities_options) {
  FILE *entities_file = fopen(entities_options->entities_path, "r");
  if (entities_file == NULL) {
    TraceLog(LOG_FATAL,"ENTITY: Error loading entities file!");
    exit(1);
  } else TraceLog(LOG_INFO, "ENTITY: Loaded entities file successfully");


  char entities_buffer[1000];
  int entities_count = 0;
  int enemies_count = 0;
  while(fgets(entities_buffer, sizeof(entities_buffer), entities_file) && entities_count < MAX_ENTITIES) {
    if (is_comment_line(entities_buffer)) continue;
    char *token = NULL;
    token = strtok(entities_buffer, " \n");
    /*char *id = token;*/
    token = strtok(NULL, " \n");
    int position_x = atof(token);
    token = strtok(NULL, " \n");
    int position_y = atof(token);
    token = strtok(NULL, " \n");
    int width = atof(token);
    token = strtok(NULL, " \n");
    int height = atof(token);
    token = strtok(NULL, " \n");
    float speed = atof(token);
    token = strtok(NULL, " \n");
    float friction = atof(token);
    token = strtok(NULL, " \n");
    int behaviour = atoi(token);

    entity_t * entity = malloc(sizeof(entity_t));
    if (entity == NULL) {
      TraceLog(LOG_FATAL, "ENTITY: Failed to allocate memory for enemy");
      exit(1);
    }
    // HINT: Player entity has to come first in entity file
    if (entities_count == 0) {
      entity->id                             = entities_count;
      entity->type                           = (entity_type_t)ENTITY_TYPE_PLAYER;
      entity->physics.position               = (vector2_t){ .x = position_x, .y = position_y };
      entity->physics.proposed_position      = (vector2_t){ .x = position_x, .y = position_y };
      entity->physics.velocity               = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity->physics.direction              = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity->physics.friction               = friction;
      entity->physics.speed                  = speed;
      entity->physics.reset_dir_frames_delay = 6;
      entity->physics.aabb                   =  (Rectangle){ .x = position_x, .y = position_y, .width = width,  .height = height };
      entity->color                          = RED;
      entity->player.input.up                = false;
      entity->player.input.down              = false;
      entity->player.input.left              = false;
      entity->player.input.right             = false;
      entities->player                       = entity;
    } else {
      entity->id                             = entities_count;
      entity->type                           = (entity_type_t)ENTITY_TYPE_NPC;
      entity->physics.position               = (vector2_t){ .x = position_x, .y = position_y };
      entity->physics.proposed_position      = (vector2_t){ .x = position_x, .y = position_y };
      entity->physics.velocity               = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity->physics.direction              = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity->physics.friction               = friction;
      entity->physics.speed                  = speed;
      entity->physics.reset_dir_frames_delay = 6;
      entity->physics.aabb                   = (Rectangle){ .x = position_x, .y = position_y, .width = width,  .height = height };
      entity->color                          = RED;
      entity->type                           = ENTITY_TYPE_NPC;
      entity->color                          = YELLOW;
      entity->npc.behaviour                  = (entity_behaviour_t)behaviour;
      entities->enemies[enemies_count]       = entity;

      enemies_count++;
    }
    entities->entities[entities_count] = entity;
    entities_count++;
  }
  entities->enemies_count = enemies_count;
  entities->entities_count = entities_count;
}

/**
 * HINT: Player entity has to come first in entity file
 */
void entities_init(entities_t *entities, entities_options_t *entities_options) {
  // TODO: increament these as they are read from resources file
  entities->player = NULL;

  for (int i = 0; i < MAX_ENTITIES; i++) {
    entities->enemies[i] = NULL;
  }

  for (int i = 0; i < MAX_ENTITIES - 1; i++) {
    entities->enemies[i] = NULL;
  }

  entities->enemies_count = 0;
  entities->entities_count = 0;
  entities_load(entities, entities_options);
}

void entities_draw(entities_t *entities) {
  entity_t *player = entities->player;

  DrawRectangleLinesEx(player->physics.aabb, 1, player->color);

  vector2_t player_center = (vector2_t){ .x = player->physics.position.x + (player->physics.aabb.width / 2), 
                                         .y = player->physics.position.y + (player->physics.aabb.height / 2) };
  vector2_t player_dir_end = (vector2_t){ .x = player->physics.position.x + (player->physics.aabb.width / 2) + ((player->physics.aabb.width  / 2) * player->physics.direction.x),
                                          .y = player->physics.position.y + (player->physics.aabb.height / 2) + ((player->physics.aabb.width / 2) * player->physics.direction.y) };
    
  DrawLineV(TO_RL_VEC2(player_center), TO_RL_VEC2(player_dir_end), player->color);

  for (int i = 0; i < entities->enemies_count; i++) {
    entity_t *enemy = entities->enemies[i];
    DrawRectangleLinesEx(enemy->physics.aabb, 1, enemy->color);
  }
}

void entities_free(entities_t *entities) {
  for (int i = 0; i < entities->entities_count; i++) {
    if (entities->entities[i] != NULL) {
      free(entities->entities[i]);
      entities->entities[i] = NULL;
    }
  }
  for (int i = 0; i < entities->enemies_count; i++) {
    if (entities->enemies[i] != NULL) {
      entities->enemies[i] = NULL;
    }
  }
  entities->player = NULL;
  entities->enemies_count = 0;
  entities->entities_count = 0;
}
