#include <raylib.h>
#include "camera.h"

Camera2D init_camera(Vector2 target, Vector2 offset, float zoom) 
{
  Camera2D camera;
  camera.target   = target;
  camera.offset   = offset;
  camera.rotation = 0;
  camera.zoom     = zoom;
  return camera;
}

void update_camera(Camera2D *camera, Vector2 target) 
{
    camera->target = target;
}
