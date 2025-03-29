#include "raylib.h"

void render_begin() {
  BeginDrawing();
  ClearBackground(CLITERAL(Color) { 29, 42, 47, 255 }); 
}

void render_end() {
  EndDrawing();
}
