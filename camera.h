#include <raylib.h>

#ifndef CAMERA_H
#define CAMERA_H


Camera2D InitCam(Vector2 target, Vector2 offset);
void     UpdateCam(Camera2D *camera, Vector2 target);

#endif
