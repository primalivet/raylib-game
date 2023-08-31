#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * Game
 * TODO: go over the other "steering behaviors", which was popularized by Craig
 * Reynolds.
 * colliding
 * TODO: bullets should decrease enemy health
 * TODO: enemy "touches" should decrease player health
 * TODO: player increasing speed as long as no collision or no 180 deg change in
 * direction
 * TODO: player "drag", basically when at high speed and changing direction the
 * change should be slower
 */

/**
 * GLOBALS
 */

#define TITLE "Game"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define BULLETS_MAX_COUNT 100
#define BULLET_SPEED 10.0f

#define PLATER_JUMP_HEIGHT_MAX 80.0f
#define PLAYER_VELOCITY_MAX 150.0f

#define ENEMY_MAX_COUNT 10
#define ENEMY_VELOCITY_WEIGHT 80.0f
#define ENEMY_AVOIDANCE_RADIUS 10.0f
#define ENEMY_SEEK_WEIGHT 1.0f
#define ENEMY_SEPERATION_WEIGHT 1.0f
#define ENEMY_AVOIDANCE_WEIGHT 0.5f

#define COLLISION_ACCELERATION 20.0f

const int TILE_SIZE = 16;

/**
 * TYPEDEFS
 */

typedef enum {
  NORTH,
  NORTH_EAST,
  NORTH_WEST,
  WEST,
  EAST,
  SOUTH,
  SOUTH_EAST,
  SOUTH_WEST,
  UNDEFINED
} Direction;

typedef struct {
  Vector2 position;
  Vector2 velocity;
  Direction direction;
  float radius;
  Color color;
  float jumpHeight;
  float maxJumpHeight;
  bool jumping;
  bool falling;
  bool alive;
} Actor;

typedef struct {
  Vector2 position;
  Direction direction;
  float speed;
  bool onScreen;
} Bullet;

typedef struct {
  int width;
  int height;
} World;

/**
 * BULLET FUNCTIONS
 */

void ShootBullet(Bullet bullets[], Actor *shooter) {
  for (int i = 0; i < BULLETS_MAX_COUNT; i++) {
    if (!bullets[i].onScreen) {
      bullets[i].onScreen = true;
      bullets[i].direction = shooter->direction;
      bullets[i].position = shooter->position;
      bullets[i].speed = BULLET_SPEED;
      break;
    }
  }
}

void UpdateBullets(float deltaTime, Bullet bullets[], World *world) {
  for (int i = 0; i < BULLETS_MAX_COUNT; i++) {
    switch (bullets[i].direction) {
    case NORTH:
      bullets[i].position.y -= bullets[i].speed;
      break;
    case NORTH_WEST:
      bullets[i].position.x -= bullets[i].speed;
      bullets[i].position.y -= bullets[i].speed;
      break;
    case NORTH_EAST:
      bullets[i].position.x += bullets[i].speed;
      bullets[i].position.y -= bullets[i].speed;
      break;
    case SOUTH:
      bullets[i].position.y += bullets[i].speed;
      break;
    case SOUTH_WEST:
      bullets[i].position.x -= bullets[i].speed;
      bullets[i].position.y += bullets[i].speed;
      break;
    case SOUTH_EAST:
      bullets[i].position.x += bullets[i].speed;
      bullets[i].position.y += bullets[i].speed;
      break;
    case WEST:
      bullets[i].position.x -= bullets[i].speed;
      break;
    case EAST:
      bullets[i].position.x += bullets[i].speed;
      break;
    case UNDEFINED:
      break;
    }
  }

  for (int i = 0; i < BULLETS_MAX_COUNT; i++) {
    if (bullets[i].position.x < 0 || bullets[i].position.x > world->width ||
        bullets[i].position.y < 0 || bullets[i].position.y > world->height) {
      bullets[i].onScreen = false;
    }
  }
}

void DrawBullets(Bullet bullets[], World *world) {
  for (int i = 0; i < BULLETS_MAX_COUNT; i++) {
    if (bullets[i].onScreen) {
      DrawCircleV(bullets[i].position, 2, RED);
    }
  }
}

/**
 * VECTOR2 FUNCTIONS
 */

/**
 * Normalize a Vector2
 */
Vector2 normalize_vector2(Vector2 v) {
  float length = sqrt(v.x * v.x + v.y * v.y);
  if (length == 0) {
    return v; // prevent div by zero
  }
  Vector2 normalized = {v.x / length, v.y / length};
  return normalized;
}

/**
 * GetDistance
 */
float GetDistance(Vector2 v1, Vector2 v2) {
  float distance = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
  return distance;
}

/**
 * Get a vector pointing from "origin" towards "target"
 */
Vector2 GetSeekVector2(Vector2 origin, Vector2 target) {
  Vector2 seek = {target.x - origin.x, target.y - origin.y};
  return normalize_vector2(seek);
}

/**
 * Get a vector in a direction to avoid collision
 */
Vector2 GetSeperationVector2(Actor *actor, Actor *actors) {
  Vector2 seperation = {0, 0};
  int count = 0;

  for (int i = 0; i < ENEMY_MAX_COUNT; i++) {
    if (actor != &actors[i]) {
      float distance = GetDistance(actors[i].position, actor->position);
      if (distance > 0 && distance < ENEMY_AVOIDANCE_RADIUS) {
        // enemy is to close to other enemies
        Vector2 away = {actor->position.x - actors[i].position.x,
                        actor->position.y - actors[i].position.y};

        seperation.x += away.x;
        seperation.y += away.y;
        count++;
      }
    }

    if (count > 0) {
      seperation.x /= count;
      seperation.y /= count;
      // Might need to scale here
      seperation = normalize_vector2(seperation);
    }
  }
  return seperation;
}

Vector2 GetCohesionAvoidanceVector2(Actor *actor, Actor *actors) {
  Vector2 avaragePosition = {0, 0};
  int count = 0;
  for (int i = 0; i < ENEMY_MAX_COUNT; i++) {
    if (actor != &actors[i]) {
      avaragePosition.x += actors[i].position.x;
      avaragePosition.y += actors[i].position.y;
      count++;
    }
  }

  if (count > 0) {
    avaragePosition.x /= count;
    avaragePosition.y /= count;
    Vector2 awayFromAvaragePosition = {actor->position.x - avaragePosition.x,
                                       actor->position.y - avaragePosition.y};
    return normalize_vector2(awayFromAvaragePosition);
  }

  return (Vector2){0, 0};
}

Vector2 GetRandomEdgeSpawnPosition(World *world) {
  int randomEdge = GetRandomValue(1, 4);
  int randomPlace = GetRandomValue(10, fmin(world->width, world->height) - 10);
  Vector2 position;

  switch (randomEdge) {
  case 1: // Top
    position.x = randomPlace;
    position.y = 20;
    break;
  case 2: // Left
    position.x = 20;
    position.y = randomPlace;
    break;
  case 3: // Right
    position.x = world->width - 20;
    position.y = randomPlace;
    break;
  case 4: // Bottom
    position.x = randomPlace;
    position.y = world->height - 20;
    break;
  }

  return position;
}

/**
 * ACTOR FUNCTIONS
 */

/**
 * CheckCollisionActors
 */
bool CheckCollisionActors(Actor *a, Actor *b) {
  float distance = GetDistance(a->position, b->position);
  if ((a->jumping && !b->jumping) || (!a->jumping && b->jumping)) {
    return false;
  }
  if ((a->falling && !b->falling) || (!a->falling && b->falling)) {
    return false;
  }
  return distance < (a->radius + b->radius);
}

void CollisionActors(Actor *a, Actor *b) {
  // TODO: this seem to be not working at all. Velocity is basically as is.
  Vector2 collision = {b->position.x - a->position.x,
                       b->position.y - a->position.y};
  Vector2 normalized = normalize_vector2(collision);
  a->velocity.x = normalized.x * COLLISION_ACCELERATION;
  a->velocity.y = normalized.y * COLLISION_ACCELERATION;
  b->velocity.x = -normalized.x * COLLISION_ACCELERATION;
  b->velocity.y = -normalized.y * COLLISION_ACCELERATION;
}

/**
 * PLAYER FUNCTIONS
 */

Actor CreatePlayer(float positionX, float positionY, Direction direction) {
  Actor player;
  player.position.x = positionX;
  player.position.y = positionY;
  player.velocity.x = 0.0f;
  player.velocity.y = 0.0f;
  player.direction = NORTH;
  player.radius = 20.0f;
  player.color = BLACK;
  player.jumpHeight = 0.0f;
  player.maxJumpHeight = PLATER_JUMP_HEIGHT_MAX;
  player.jumping = false;
  player.falling = false;
  player.alive = true;
  return player;
}

Vector2 GetVelocityFromDirection(float deltaTime, Direction dir, float speed) {
  Vector2 velocity = {0, 0};
  switch (dir) {
  case NORTH:
    velocity.x = 0;
    velocity.y = -(deltaTime * speed);
    break;
  case SOUTH:
    velocity.x = 0;
    velocity.y = deltaTime * speed;
    break;
  case WEST:
    velocity.x = -(deltaTime * speed);
    velocity.y = 0;
    break;
  case EAST:
    velocity.x = deltaTime * speed;
    velocity.y = 0;
    break;
  case NORTH_WEST:
    velocity.x = -(deltaTime * speed);
    velocity.y = -(deltaTime * speed);
    break;
  case NORTH_EAST:
    velocity.x = deltaTime * speed;
    velocity.y = -(deltaTime * speed);
    break;
  case SOUTH_WEST:
    velocity.x = -(deltaTime * speed);
    velocity.y = deltaTime * speed;
    break;
  case SOUTH_EAST:
    velocity.x = deltaTime * speed;
    velocity.y = deltaTime * speed;
    break;
  case UNDEFINED:
    velocity.x = 0;
    velocity.y = 0;
    break;
  }

  return velocity;
}

void SetPlayerVelocity(float deltaTime, Actor *player, Direction direction,
                       bool moving) {
  Vector2 velocity = GetVelocityFromDirection(deltaTime, player->direction,
                                              PLAYER_VELOCITY_MAX);

  player->velocity = velocity;

  if (moving) {
    player->position.x += player->velocity.x;
    player->position.y += player->velocity.y;
  } else {
    player->position.x -= 0;
    player->position.y -= 0;
  }
}

void SetPlayerDirection(Actor *player) {
  if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_LEFT)) {
    player->direction = NORTH_WEST;
  } else if (IsKeyDown(KEY_UP) && IsKeyDown(KEY_RIGHT)) {
    player->direction = NORTH_EAST;
  } else if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_LEFT)) {
    player->direction = SOUTH_WEST;
  } else if (IsKeyDown(KEY_DOWN) && IsKeyDown(KEY_RIGHT)) {
    player->direction = SOUTH_EAST;
  } else if (IsKeyDown(KEY_RIGHT)) {
    player->direction = EAST;
  } else if (IsKeyDown(KEY_LEFT)) {
    player->direction = WEST;
  } else if (IsKeyDown(KEY_UP)) {
    player->direction = NORTH;
  } else if (IsKeyDown(KEY_DOWN)) {
    player->direction = SOUTH;
  }
}

void UpdatePlayer(float deltaTime, Actor *player, Bullet bullets[],
                  World *world) {
  bool moving = IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) ||
                IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT);

  SetPlayerDirection(player);
  SetPlayerVelocity(deltaTime, player, player->direction, moving);

  // JUMPING
  if (!player->jumping && player->jumpHeight <= 0.0f && IsKeyDown(KEY_SPACE)) {
    player->jumping = true;
  }
  if (player->jumping && (player->jumpHeight < player->maxJumpHeight)) {
    player->jumpHeight += deltaTime * 150.0f;
  }
  if (player->jumpHeight >= player->maxJumpHeight) {
    player->jumping = false;
    player->falling = true;
  }
  if (!player->jumping && player->jumpHeight > 0.0f) {
    player->jumpHeight -= deltaTime * 200.0f;
  }
  if (player->jumpHeight <= 0.0f) {
    player->falling = false;
  }

  // WORLD BONDARIES
  if (player->position.x > world->width) {
    player->position.x = 0;
  }
  if (player->position.x < 0) {
    player->position.x = world->width;
  }
  if (player->position.y > world->height) {
    player->position.y = 0;
  }
  if (player->position.y < 0) {
    player->position.y = world->height;
  }

  // SHOOTING
  if (IsKeyPressed(KEY_S)) {
    ShootBullet(bullets, player);
  }
}

void DrawPlayer(Actor *player) {
  DrawCircleV(player->position, player->radius + player->jumpHeight / 4,
              player->color);
}

/**
 * WORLD FUNCTIONS
 */

World CreateWorld(int width, int height) {
  World world;
  world.width = width;
  world.height = height;
  return world;
}

/**
 * ENEMY FUNCTIONS
 */

Actor CreateEnemy(World *world, Actor *player) {
  Actor enemy;
  Vector2 position = GetRandomEdgeSpawnPosition(world);
  Direction direction = NORTH;
  enemy.velocity.x = 0;
  enemy.velocity.y = 0;
  enemy.position = position;
  enemy.direction = direction;
  enemy.radius = 10.0f;
  enemy.color = RED;
  enemy.jumpHeight = 0.0f;
  enemy.maxJumpHeight = 0.0f;
  enemy.jumping = false;
  enemy.falling = false;
  enemy.alive = true;
  return enemy;
}

void UpdateEnemies(float deltaTime, Actor enemies[], World *world,
                   Actor *player) {
  for (int i = 0; i < ENEMY_MAX_COUNT; i++) {
    if (!enemies[i].alive) {
      enemies[i] = CreateEnemy(world, player);
    } else {
      Vector2 seek = GetSeekVector2(enemies[i].position, player->position);
      Vector2 seperation = GetSeperationVector2(&enemies[i], enemies);
      Vector2 avoidance = GetCohesionAvoidanceVector2(&enemies[i], enemies);

      enemies[i].velocity.x = (seek.x * ENEMY_SEEK_WEIGHT) +
                              (seperation.x * ENEMY_SEPERATION_WEIGHT) +
                              (avoidance.x * ENEMY_AVOIDANCE_WEIGHT);
      enemies[i].velocity.y = (seek.y * ENEMY_SEEK_WEIGHT) +
                              (seperation.y * ENEMY_SEPERATION_WEIGHT) +
                              (avoidance.y * ENEMY_AVOIDANCE_WEIGHT);
      enemies[i].position.x += enemies[i].velocity.x * ENEMY_VELOCITY_WEIGHT * deltaTime;
      enemies[i].position.y += enemies[i].velocity.y * ENEMY_VELOCITY_WEIGHT * deltaTime;
    }
  }
}

void DrawEnemies(Actor enemies[]) {
  for (int i = 0; i < ENEMY_MAX_COUNT; i++) {
    if (enemies[i].alive) {
      DrawCircleV(enemies[i].position, enemies[i].radius, enemies[i].color);
    }
  }
}

int main(void) {
  const char *title = TITLE;

  Bullet bullets[BULLETS_MAX_COUNT] = {0};
  World world = CreateWorld(SCREEN_WIDTH, SCREEN_HEIGHT);
  Actor enemies[ENEMY_MAX_COUNT] = {0};
  Actor player =
      CreatePlayer((float)world.width / 2, (float)world.height / 2, NORTH);

  InitWindow(world.width, world.height, title);

  Texture tileset = LoadTexture("tiles-16.png");
  SetTargetFPS(60);

  Rectangle sourceRec;
  sourceRec.width = TILE_SIZE;
  sourceRec.height = TILE_SIZE;
  sourceRec.x = TILE_SIZE * 2;
  sourceRec.y = TILE_SIZE * 0;
  Vector2 position = {0, 0};

  while (!WindowShouldClose()) {

    float dt = GetFrameTime();

    // Update
    UpdatePlayer(dt, &player, bullets, &world);
    UpdateBullets(dt, bullets, &world);
    UpdateEnemies(dt, enemies, &world, &player);

    // Draw
    BeginDrawing();
    ClearBackground(DARKGRAY);
    DrawTextureRec(tileset, sourceRec, position, WHITE);
    DrawBullets(bullets, &world);
    for (int i = 0; i < ENEMY_MAX_COUNT; i++) {
      if (CheckCollisionActors(&player, &enemies[i])) {
        enemies[i].color = BLUE;
        CollisionActors(&player, &enemies[i]);
      }
    }
    DrawEnemies(enemies);
    DrawPlayer(&player);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
