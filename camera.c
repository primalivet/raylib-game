#include <raylib.h>
#include "camera.h"

Camera2D InitCam(Vector2 target, Vector2 offset) 
{
  Camera2D camera;
  camera.target   = target;
  camera.offset   = offset;
  camera.rotation = 0;
  camera.zoom     = 1.0f;
  return camera;
}

void UpdateCam(Camera2D *camera, Vector2 target) 
{
    camera->target = target;
}
