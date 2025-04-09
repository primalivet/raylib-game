#include "debug.h"
#include <raylib.h>
#include <stdio.h>

void debug_draw(entities_t *entities, bool show_debug) {
  if (!show_debug) return;
  // This function should be called after EndMode2D() to draw on top of the world

  // Define the debug box dimensions and position (screen space, not world space)
  Rectangle debugBox = { 10, 10, 250, 120 };

  // Draw the debug box with a semi-transparent background
  DrawRectangleRec(debugBox, (Color){ 0, 0, 0, 200 });
  DrawRectangleLinesEx(debugBox, 1, WHITE);

  // Title
  DrawText("Player Debug Info", debugBox.x + 10, debugBox.y + 5, 12, YELLOW);

  int y = debugBox.y + 25;
  int lineHeight = 16;
  char debugText[128];

  // Display player position with reduced font size
  sprintf(debugText, "Pos: X=%.1f Y=%.1f", 
          entities->player->physics.position.x, entities->player->physics.position.y);
  DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
  y += lineHeight;

  // Display player velocity with reduced font size
  sprintf(debugText, "Vel: X=%.1f Y=%.1f", 
          entities->player->physics.velocity.x, entities->player->physics.velocity.y);
  DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
  y += lineHeight;

  // Display player direction with reduced font size
  sprintf(debugText, "Dir: X=%.1f Y=%.1f", 
          entities->player->physics.direction.x, entities->player->physics.direction.y);
  DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
  y += lineHeight;

  // Display player speed and friction with reduced font size
  sprintf(debugText, "Speed: %.1f  Friction: %.1f", 
          entities->player->physics.speed, entities->player->physics.friction);
  DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
  y += lineHeight;

  // Display AABB dimensions
  sprintf(debugText, "AABB: X=%.1f Y=%.1f W=%.1f H=%.1f", 
          entities->player->physics.aabb.x, entities->player->physics.aabb.y,
          entities->player->physics.aabb.width, entities->player->physics.aabb.height);
  DrawText(debugText, debugBox.x + 10, y, 10, WHITE);

  // Display player health
  sprintf(debugText, "Health: %d", entities->player->health);
  DrawText(debugText, debugBox.x + 10, y + lineHeight, 10, WHITE);

  for (int i = 0; i < entities->entities_count; i++) {
    entity_t *entity = entities->entities[i];

    DrawRectangleLinesEx(entity->physics.aabb, 1, entity->color);
    vector2_t player_center = (vector2_t){ .x = entity->physics.position.x + (entity->physics.aabb.width / 2), 
      .y = entity->physics.position.y + (entity->physics.aabb.height / 2) };
    vector2_t player_dir_end = (vector2_t){ .x = entity->physics.position.x + (entity->physics.aabb.width / 2) + ((entity->physics.aabb.width  / 2) * entity->physics.direction.x),
      .y = entity->physics.position.y + (entity->physics.aabb.height / 2) + ((entity->physics.aabb.width / 2) * entity->physics.direction.y) };

    DrawLineV(TO_RL_VEC2(player_center), TO_RL_VEC2(player_dir_end), entity->color);
    for (int i = 0; i < entities->enemies_count; i++) {
      entity_t *enemy = entities->enemies[i];
      DrawRectangleLinesEx(enemy->physics.aabb, 1, enemy->color);
    }
  }
}
