#include "raylib.h"
#include "window.h"

void window_open(window_t *window, window_options_t *window_options) {
  window->width = window_options->width;
  window->height = window_options->height;
  InitWindow(window_options->width, window_options->height, window_options->title);
  SetTargetFPS(window_options->taget_fps);
}

void window_close() {
  CloseWindow();
};
