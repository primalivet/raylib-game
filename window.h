#ifndef WINDOW_H
#define WINDOW_H

typedef struct window_options_t {
  int         width;
  int         height;
  int         taget_fps;
  const char *title;
} window_options_t;

typedef struct window_t {
  int width;
  int height;
} window_t;

void window_open(window_t *window, window_options_t *window_options);
void window_close();

#endif
