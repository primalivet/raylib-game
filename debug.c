#include "debug.h"
#include <raylib.h>
#include <stdio.h>

void debug_draw(entity_player_t *player) {
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
            player->physics.position.x, player->physics.position.y);
    DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
    y += lineHeight;
    
    // Display player velocity with reduced font size
    sprintf(debugText, "Vel: X=%.1f Y=%.1f", 
            player->physics.velocity.x, player->physics.velocity.y);
    DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
    y += lineHeight;
    
    // Display player direction with reduced font size
    sprintf(debugText, "Dir: X=%.1f Y=%.1f", 
            player->physics.direction.x, player->physics.direction.y);
    DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
    y += lineHeight;
    
    // Display player speed and friction with reduced font size
    sprintf(debugText, "Speed: %.1f  Friction: %.1f", 
            player->physics.speed, player->physics.friction);
    DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
    y += lineHeight;
    
    // Display AABB dimensions
    sprintf(debugText, "AABB: X=%.1f Y=%.1f W=%.1f H=%.1f", 
            player->physics.aabb.x, player->physics.aabb.y,
            player->physics.aabb.width, player->physics.aabb.height);
    DrawText(debugText, debugBox.x + 10, y, 10, WHITE);
}
