#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

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
  bool falling;
} Actor;

typedef struct {
  Vector2 position;
  Direction direction;
  float speed;
} Bullet;

typedef struct {
  int width;
  int height;
} World;

Bullet *shootBullet(Actor *player) {
  Bullet *bullet = malloc(sizeof(Bullet));
  bullet->position = player->position;
  bullet->direction = player->direction;
  bullet->speed = 10.0f;
  return bullet;
}

int main(void) {
  const char *title = "Game";

  Bullet *bullets[100] = {0};

  World world;
  world.width = 800;
  world.height = 800;

  Actor player;
  player.position.x = (float)world.width / 2;
  player.position.y = (float)world.height / 2;
  player.direction = NORTH;
  player.speed = 0.0f;
  player.maxSpeed = 12.0f;
  player.moving = false;
  player.jumpHeight = 0.0f;
  player.maxJumpHeight = 30.0f;
  player.jumping = false;
  player.falling = false;

  InitWindow(world.width, world.height, title);

  SetTargetFPS(60);

  // TODO: remove when bullets is dynamic array
  int bulletsCount = 0;

  while (!WindowShouldClose()) {
    player.moving = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) ||
                    IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN);

    // SPEED
    if (player.moving && player.speed < player.maxSpeed) {
      player.speed = player.speed < player.maxSpeed / 2 ? player.speed + 0.2f
                                                        : player.speed + 0.05f;
    }

    if (!player.moving && player.speed > 0) {
      player.speed = player.speed > 0.0f ? player.speed - 0.3f : 0.0f;
    }

    // DIRECTION
    if (!player.falling) {
      if (IsKeyDown(KEY_RIGHT))
        player.direction = EAST;
      if (IsKeyDown(KEY_LEFT))
        player.direction = WEST;
      if (IsKeyDown(KEY_UP))
        player.direction = NORTH;
      if (IsKeyDown(KEY_DOWN))
        player.direction = SOUTH;
      if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_LEFT))
        player.direction = NORTH_WEST;
      if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_RIGHT))
        player.direction = NORTH_EAST;
      if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_LEFT))
        player.direction = SOUTH_WEST;
      if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_RIGHT))
        player.direction = SOUTH_EAST;
    }

    // PLAYER POSITION
    if (player.speed > 0) {
      switch (player.direction) {
      case NORTH:
        player.position.y -= player.speed;
        break;
      case NORTH_WEST:
        player.position.x -= player.speed;
        player.position.y -= player.speed;
        break;
      case NORTH_EAST:
        player.position.x += player.speed;
        player.position.y -= player.speed;
        break;
      case SOUTH:
        player.position.y += player.speed;
        break;
      case SOUTH_WEST:
        player.position.x -= player.speed;
        player.position.y += player.speed;
        break;
      case SOUTH_EAST:
        player.position.x += player.speed;
        player.position.y += player.speed;
        break;
      case WEST:
        player.position.x -= player.speed;
        break;
      case EAST:
        player.position.x += player.speed;
        break;
      }
    }

    // JUMPING
    if (!player.jumping && player.jumpHeight <= 0.0f && IsKeyDown(KEY_SPACE))
      player.jumping = true;
    if (player.jumping && (player.jumpHeight < player.maxJumpHeight))
      player.jumpHeight += 3.0f;
    if (player.jumpHeight >= player.maxJumpHeight)
      player.jumping = false;
    player.falling = true;
    if (!player.jumping && player.jumpHeight > 0.0f)
      player.jumpHeight -= 2.0f;
    if (player.jumpHeight <= 0.0f)
      player.falling = false;

    // WORLD BONDARIES
    if (player.position.x > world.width)
      player.position.x = 0;
    if (player.position.x < 0)
      player.position.x = world.width;
    if (player.position.y > world.height)
      player.position.y = 0;
    if (player.position.y < 0)
      player.position.y = world.height;

    // BULLETS

    for (int i = 0; i < 30; i++) {
      if (bullets[i] != NULL) {
        switch (bullets[i]->direction) {
        case NORTH:
          bullets[i]->position.y -= bullets[i]->speed;
          break;
        case NORTH_WEST:
          bullets[i]->position.x -= bullets[i]->speed;
          bullets[i]->position.y -= bullets[i]->speed;
          break;
        case NORTH_EAST:
          bullets[i]->position.x += bullets[i]->speed;
          bullets[i]->position.y -= bullets[i]->speed;
          break;
        case SOUTH:
          bullets[i]->position.y += bullets[i]->speed;
          break;
        case SOUTH_WEST:
          bullets[i]->position.x -= bullets[i]->speed;
          bullets[i]->position.y += bullets[i]->speed;
          break;
        case SOUTH_EAST:
          bullets[i]->position.x += bullets[i]->speed;
          bullets[i]->position.y += bullets[i]->speed;
          break;
        case WEST:
          bullets[i]->position.x -= bullets[i]->speed;
          break;
        case EAST:
          bullets[i]->position.x += bullets[i]->speed;
          break;
        }
      }
    }

    // SHOOTING
    if (IsKeyPressed(KEY_S)) {
      bullets[bulletsCount] = shootBullet(&player);
      bulletsCount += 1;
    }

    BeginDrawing();

    ClearBackground(DARKGRAY);
    DrawCircleV(player.position, 20 + player.jumpHeight / 2, BLACK);

    // Draw remaining bullets
    for (int i = 0; i < 30; i++) {
      if (bullets[i]) {
        DrawCircleV(bullets[i]->position, 2, BLACK);
      }
    }

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
