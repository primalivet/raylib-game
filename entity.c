#include "raylib.h"
#include "entity.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  int i = 0;
  while(fgets(entities_buffer, sizeof(entities_buffer), entities_file) && i < MAX_ENTITIES) {
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

    // HINT: Player entity has to come first in entity file
    if (i == 0) {
      entity_player_t entity = {0};
      entity.physics.position = (vector2_t){ .x = position_x, .y = position_y };
      entity.physics.velocity               = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity.physics.direction              = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity.physics.friction               = friction;
      entity.physics.speed                  = speed;
      entity.physics.reset_dir_frames_delay = 6;
      entity.physics.aabb =  (Rectangle){ .x = position_x - (width / 2.0f), 
        .y = position_y - (height / 2.0f), 
        .width = width, 
        .height = height };

      entity.type = ENTITY_TYPE_PLAYER;
      entity.color = RED;
      entity.input.up = false;
      entity.input.down = false;
      entity.input.left = false;
      entity.input.right = false;
      entities->player = entity;
      entities->entities[i++] = (entity_t)entity;
    } else {
      entity_npc_t entity = {0};
      entity.physics.position = (vector2_t){ .x = position_x, .y = position_y };
      entity.physics.velocity               = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity.physics.direction              = (vector2_t){ .x = 0.0f, .y  = 0.0f };
      entity.physics.friction               = friction;
      entity.physics.speed                  = speed;
      entity.physics.reset_dir_frames_delay = 6;
      entity.physics.aabb =  (Rectangle){ .x = position_x - (width / 2.0f), 
        .y = position_y - (height / 2.0f), 
        .width = width, 
        .height = height };

      entity.type = ENTITY_TYPE_PLAYER;
      entity.color = RED;
      entity.type = ENTITY_TYPE_NPC;
      entity.color = YELLOW;
      entities->enemies[i++] = entity;
      entities->entities[i++] = (entity_t)entity;
    }
  }
}

/**
 * HINT: Player entity has to come first in entity file
 */
void entities_init(entities_t *entities, entities_options_t *entities_options) {
  entities->enemies_count = MAX_ENTITIES - 1;
  entities->entities_count = MAX_ENTITIES;
  entities_load(entities, entities_options);
}

void entities_draw(entities_t *entities) {
  DrawRectangleRec(entities->player.physics.aabb, entities->player.color);

  for (int i = 0; i < entities->enemies_count; i++) {
    entity_npc_t *enemy = &entities->enemies[i];
    DrawRectangleRec(enemy->physics.aabb, enemy->color);
  }
}
