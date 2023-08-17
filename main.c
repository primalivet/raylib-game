#include <stdio.h>
#include <raylib.h>
#include <math.h>

typedef enum {
  NORTH,
  NORTH_EAST,
  NORTH_WEST,
  WEST,
  EAST,
  SOUTH,
  SOUTH_EAST,
  SOUTH_WEST
} Direction;

typedef struct {
  Vector2 position;
  Direction direction;
  float speed;
  float maxSpeed;
  bool moving;
  float jumpHeight;
  float maxJumpHeight;
  bool jumping;
} Actor;

typedef struct {
  int width;
  int height;
} World;

int main(void)
{
  const char* title = "Game";

  World world;
  world.width = 800;
  world.height = 800;

  Actor player;
  player.position.x = (float)world.width / 2;
  player.position.y = (float)world.height / 2;
  player.direction = NORTH;
  player.speed = 0.0f;
  player.maxSpeed = 15.0f;
  player.moving = false;
  player.jumpHeight = 0.0f;
  player.maxJumpHeight = 10.0f;
  player.jumping = false;

  InitWindow(world.width, world.height, title);

  SetTargetFPS(60);

  while(!WindowShouldClose())
  {
    player.moving = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN);

    // SPEED
    if (player.moving && player.speed < player.maxSpeed) {
      player.speed = player.speed < player.maxSpeed / 2 ? player.speed + 0.2f : player.speed + 0.05f;
    } 

    if (!player.moving && player.speed > 0) {
      player.speed = player.speed > 0.0f ? player.speed - 0.3f : 0.0f;
    }

    // DIRECTION
    if (IsKeyDown(KEY_RIGHT)) player.direction = EAST;
    if (IsKeyDown(KEY_LEFT)) player.direction = WEST;
    if (IsKeyDown(KEY_UP)) player.direction = NORTH;
    if (IsKeyDown(KEY_DOWN)) player.direction = SOUTH;
    if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_LEFT)) player.direction = NORTH_WEST;
    if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_RIGHT)) player.direction = NORTH_EAST;
    if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_LEFT)) player.direction = SOUTH_WEST;
    if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_RIGHT)) player.direction = SOUTH_EAST;

    // POSITION
    if (player.speed > 0) {
      switch (player.direction) {
        case NORTH: player.position.y -= player.speed; break;
        case NORTH_WEST: player.position.x -= player.speed; player.position.y -= player.speed; break;
        case NORTH_EAST: player.position.x += player.speed; player.position.y -= player.speed; break;
        case SOUTH: player.position.y += player.speed; break;
        case SOUTH_WEST: player.position.x -= player.speed; player.position.y += player.speed; break;
        case SOUTH_EAST: player.position.x += player.speed; player.position.y += player.speed; break;
        case WEST: player.position.x -= player.speed; break;
        case EAST: player.position.x += player.speed; break;
      }
    }

    // JUMPING
    if (player.jumpHeight <= 0 && IsKeyDown(KEY_SPACE)) {
      player.jumping = true;
    }

    if (player.jumping && player.jumpHeight < player.maxJumpHeight) {
      player.jumpHeight += 1.0f;
    }

    if (player.jumpHeight >= player.maxJumpHeight) {
      player.jumping = false;
      player.jumpHeight -= 2.0f;
    }

    // WORLD BONDARIES
    if (player.position.x > world.width) player.position.x = 0;
    if (player.position.x < 0) player.position.x = world.width;
    if (player.position.y > world.height) player.position.y = 0;
    if (player.position.y < 0) player.position.y = world.height;


    BeginDrawing();

    ClearBackground(RED);
    DrawCircleV(player.position, 20 + player.jumpHeight, WHITE);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}

