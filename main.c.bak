#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-----------------------------------------------------------------------------
// DEFINES
//-----------------------------------------------------------------------------

#define TITLE "Game"
#define SCREEN_WIDTH 640 * 2
#define SCREEN_HEIGHT 480 * 2
#define NPC_IS_FAR_FROM_PLAYER_THRESHOLD 7.0f
#define NPC_IS_CLOSE_TO_PLAYER_THRESHOLD 2.0f
#define NPC_IS_CLOSE_TO_WAYPOINT_THRESHOLD 2.0f
#define NPC_SEPERATION_VECTOR_SCALAR 10.0f

#define ENEMIES_COUNT 1
#define TILE_SIZE 16
#define TILES_PER_ROW 20
#define TARGET_FPS 60
#define BACKGROUND                                                             \
  CLITERAL(Color) { 29, 42, 47, 255 }

//-----------------------------------------------------------------------------
// OWNED INCLUDES
//-----------------------------------------------------------------------------

#include "a_star.h"
#include "camera.h"
#include "entity.h"
#include "level.h"
#include "physics.h"
#include "vector2.h"

void read_input(physics_body *player_body) {
  // Reset player direction
  player_body->direction = (Vector2){0.0f, 0.0f};

  // Set player direction based on input
  if (IsKeyDown(KEY_LEFT))
    player_body->direction.x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT))
    player_body->direction.x += 1.0f;
  if (IsKeyDown(KEY_UP))
    player_body->direction.y -= 1.0f;
  if (IsKeyDown(KEY_DOWN))
    player_body->direction.y += 1.0f;

  // Normalize player direction
  player_body->direction = vector2_normalize(player_body->direction);
}

void update_npc_body(physics_body *npc_body, physics_body *player_body,
                     level *level) {
  entity *entity = entities_get_entity(npc_body->entity_id);

  if (CheckCollisionRecs(npc_body->aabb, player_body->aabb)) {
    npc_body->is_active = false;
    entity->is_active = false;
  }

  float distance_to_player =
      vector2_euclidean_distance(npc_body->position, player_body->position);
  bool is_far_from_player =
      (distance_to_player / TILE_SIZE) > NPC_IS_FAR_FROM_PLAYER_THRESHOLD;
  bool is_close_to_player =
      (distance_to_player / TILE_SIZE) < NPC_IS_CLOSE_TO_PLAYER_THRESHOLD;
  bool generate_waypoints = !is_far_from_player && entity->waypoints == NULL;
  bool follow_waypoints =
      entity->waypoints != NULL &&
      entity->current_waypoint_index < entity->waypoints->length;
  bool clear_waypoints =
      is_close_to_player ||
      (entity->waypoints != NULL &&
       entity->current_waypoint_index >= entity->waypoints->length);

  if (generate_waypoints) {
    Vector2 origin = vector2_div_by_scalar(
        npc_body->position, TILE_SIZE); // Convert from coords to tilemap
    Vector2 goal = vector2_div_by_scalar(
        player_body->position, TILE_SIZE); // Convert from coords to tilemap
    entity->waypoints = astar_search(level, &origin, &goal); // Find path
    entity->current_waypoint_index = 0; // Set first waypoint as start
  }

  if (follow_waypoints) {
    Vector2 *tilemap_waypoint = dynlist_get_at(
        entity->waypoints, entity->current_waypoint_index); // Current waypoint
    Vector2 world_waypoint = vector2_mult_by_scalar(
        *tilemap_waypoint, TILE_SIZE); // Convert from tilemap to world
    world_waypoint.x +=
        (TILE_SIZE / 2.0f -
         npc_body->aabb.width / 2.0f); // Center in tile middle on x axis
    world_waypoint.y +=
        (TILE_SIZE / 2.0f -
         npc_body->aabb.height / 2.0f); // Center in tile middle on y axis
    float npc_distance_to_waypoint =
        vector2_euclidean_distance(npc_body->position, world_waypoint);
    bool is_npc_close_to_waypoint =
        (npc_distance_to_waypoint / TILE_SIZE) <
        NPC_IS_CLOSE_TO_WAYPOINT_THRESHOLD; // TODO: move magic number
    Vector2 npc_direction_to_waypoint =
        vector2_sub(world_waypoint, npc_body->position);
    Vector2 npc_current_tile_position = vector2_div_by_scalar(
        npc_body->position,
        TILE_SIZE); // Convert npc world position to tile position

    // Calculate seperation vector for NPC in relation to non-walkable tiles
    Vector2 seperation = (Vector2){0.0f, 0.0f};
    for (int dx = -1; dx <= 1; dx++) {
      for (int dy = -1; dy <= 1; dy++) {
        int x = npc_current_tile_position.x + dx;
        int y = npc_current_tile_position.y + dy;
        tile_def cardinal_tile_def = level->tile_defs[level->tilemap[y][x]];
        if (!cardinal_tile_def.is_walkable) {
          Vector2 cardinal_tile_position = (Vector2){x, y};
          Vector2 cardinal_world_position =
              vector2_mult_by_scalar(cardinal_tile_position, TILE_SIZE);
          cardinal_world_position = vector2_add_by_scalar(
              cardinal_world_position,
              (TILE_SIZE / 2.0f)); // Center in tile middle
          Vector2 direction_away_from_cardinal_world_position =
              vector2_sub(npc_body->position, cardinal_world_position);
          seperation = vector2_add(
              seperation,
              vector2_normalize(direction_away_from_cardinal_world_position));
        }
      }
    }

    // Apply seperation vector scaler
    seperation = vector2_normalize(seperation);
    seperation =
        vector2_mult_by_scalar(seperation, NPC_SEPERATION_VECTOR_SCALAR);

    // Update npc direction
    npc_body->direction = vector2_add(npc_direction_to_waypoint, seperation);
    npc_body->direction = vector2_normalize(npc_body->direction);

    if (is_npc_close_to_waypoint) {
      entity->current_waypoint_index++; // Move to next waypoint
    }
  }

  if (clear_waypoints) {
    npc_body->direction = (Vector2){0.0f, 0.0f};
    free_reconstructed_path(entity->waypoints);
    entity->waypoints = NULL;
    entity->current_waypoint_index = 0;
  }
}

bool show_debug_body = false;
void draw_debug_body(physics_body *body) {
  int column_size = 80;
  int column_height = 70;
  const char *velocity =
      TextFormat("Velocity\nx: %f\ny: %f", body->velocity.x, body->velocity.y);
  const char *position =
      TextFormat("Position\nx: %f\ny: %f", body->aabb.x, body->aabb.y);
  const char *acceleration = TextFormat(
      "Acceleration\nx: %f\ny: %f", body->acceleration.x, body->acceleration.y);
  const char *direction = TextFormat("Direction\nx: %f\ny: %f",
                                     body->direction.x, body->direction.y);
  DrawRectangleRec((Rectangle){0, 0, SCREEN_WIDTH, column_height}, BLACK);
  DrawText(velocity, column_size * 0 + 10, 10, 10, BLUE);
  DrawText(position, column_size * 1 + 10, 10, 10, GREEN);
  DrawText(acceleration, column_size * 2 + 10, 10, 10, YELLOW);
  DrawText(direction, column_size * 3 + 10, 10, 10, RED);
}

//-----------------------------------------------------------------------------
// MAIN
//-----------------------------------------------------------------------------

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  SetTargetFPS(TARGET_FPS);

  physics_init(9.8f);
  entities_init();

  Texture tileset = LoadTexture("resources/tiles-16.png");
  level level = load_level(TILE_SIZE, TILES_PER_ROW, "resources/level1.map",
                           "resources/level1.def", &tileset);

  for (size_t i = 0; i < ENEMIES_COUNT; i++) {
    entities_add_entity(
        RED,
        (Vector2){(25.0f + i) * level.tile_size, (5.0f + i) * level.tile_size},
        TILE_SIZE / 1.5f, TILE_SIZE / 1.5f, (Vector2){0.0f, 0.0f},
        (Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 1.0f, 0.35f, 2.0f, false);
  }

  size_t player_id = entities_add_entity(
      WHITE, (Vector2){4.0f * level.tile_size, 4.0f * level.tile_size},
      TILE_SIZE / 1.5f, TILE_SIZE / 1.5f, (Vector2){0.0f, 0.0f},
      (Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f}, 1.0f, 0.25f, 4.0f, true);
  entity *player = entities_get_entity(player_id);
  physics_body *player_body = physics_get_body(player->body_id);

  Camera2D camera = init_camera(
      (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f),
                player_body->aabb.y + (player_body->aabb.height / 2.0f)},
      (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, 4.0f);

  while (!WindowShouldClose()) {
    // update player body from user input
    read_input(player_body);

    // update AI bodies
    for (size_t i = 0; i < entities_count(); i++) {
      if (i == player_id)
        continue;
      entity *ai_entity = entities_get_entity(i);
      physics_body *ai_body = physics_get_body(ai_entity->body_id);
      update_npc_body(ai_body, player_body, &level);
    }

    // respawn inactive AI entities
    for (size_t i = 0; i < entities_count() - 1; i++) {
      entity *entity = entities_get_entity(i);
      if (entity->is_active)
        continue;
      entities_add_entity(YELLOW,
                          (Vector2){(25.0f + i) * level.tile_size,
                                    (5.0f + i) * level.tile_size},
                          TILE_SIZE / 1.5f, TILE_SIZE / 1.5f,
                          (Vector2){0.0f, 0.0f}, (Vector2){0.0f, 0.0f},
                          (Vector2){0.0f, 0.0f}, 1.0f, 0.35f, 1.0f, false);
    }

    update_camera(
        &camera,
        (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f),
                  player_body->aabb.y + (player_body->aabb.height / 2.0f)});
    physics_update(&level);

    BeginDrawing();
    ClearBackground(BACKGROUND);

    // Player debug
    if (IsKeyPressed(KEY_P)) {
      show_debug_body = !show_debug_body;
    }
    if (show_debug_body) {
      draw_debug_body(player_body);
    }

    BeginMode2D(camera);

    draw_level(&level);

    // Draw active entities
    for (size_t i = 0; i < entities_count(); i++) {
      entity *entity = entities_get_entity(i);
      if (!entity->is_active)
        continue;
      physics_body *entity_body = physics_get_body(entity->body_id);

      if (entity->waypoints != NULL &&
          entity->current_waypoint_index < entity->waypoints->length) {
        Vector2 *waypoint =
            dynlist_get_at(entity->waypoints, entity->current_waypoint_index);
        Vector2 coords = (Vector2){
            (waypoint->x * TILE_SIZE) + (TILE_SIZE / 2.0f),
            (waypoint->y * TILE_SIZE) +
                (TILE_SIZE / 2.0f)}; // Waypoint in coords, e.g. x:400, y:80
        DrawLineEx(entity_body->position, coords, 2.0f, entity->color);
        Vector2 textPos =
            (Vector2){entity_body->position.x - 20,
                      entity_body->position.y - entity_body->aabb.height * 2};
        DrawText(TextFormat("Waypoint i: %d (x: %f, %f)",
                            entity->current_waypoint_index, coords.x, coords.y),
                 textPos.x, textPos.y, 10, entity->color);
      }

      DrawRectangleRec(entity_body->aabb, entity->color);
    }

    EndMode2D();
    EndDrawing();
  }

  entities_deinit();
  physics_deinit();
  unload_level(&level);
  UnloadTexture(tileset);
  CloseWindow();

  return 0;
}
