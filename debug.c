#include "debug.h"
#include <raylib.h>
#include <stdio.h>

void debug_draw(entities_t *entities) {
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
}
