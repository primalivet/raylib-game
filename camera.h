#include <raylib.h>

#ifndef CAMERA_H
#define CAMERA_H


Camera2D init_camera(Vector2 target, Vector2 offset, float zoom);
void     update_camera(Camera2D *camera, Vector2 target);

#endif
