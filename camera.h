#include <raylib.h>
#include "vector2.h"

#ifndef CAMERA_H
#define CAMERA_H

typedef struct {
  vector2_t offset;
  vector2_t target;
  float     rotation;
  float     zoom; 
} camera_t;

typedef struct {
  vector2_t offset;
  vector2_t target;
  float     zoom; 
} camera_options_t;

void camera_init(camera_t *camera, camera_options_t *camera_options);
void camera_start(camera_t *camera);
void camera_end();
void camera_update(camera_t *camera, vector2_t *new_target); 

#endif
