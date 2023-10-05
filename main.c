#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <raylib.h>
#include <string.h>

//----------------------------------------------------------------------------- 
// DEFINES
//----------------------------------------------------------------------------- 

#define TITLE         "Game"
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

#define ENEMIES_COUNT 1
#define TILE_SIZE     16
#define TILES_PER_ROW 20
#define TARGET_FPS    60
#define BACKGROUND    CLITERAL(Color){ 29, 42, 47, 255 }         

//----------------------------------------------------------------------------- 
// OWNED INCLUDES
//----------------------------------------------------------------------------- 

#include "vector2.h"
#include "level.h"
#include "camera.h"
#include "physics.h"
#include "entity.h"
#include "a_star.h"

Vector2 from_world_to_grid(Vector2 position, float tile_size) {
  return (Vector2){
    (position.x - (tile_size / 2.0f)),
    (position.y - (tile_size / 2.0f))
  };
}

Vector2 from_grid_to_world_approx(Vector2 position, float tile_size) {
  return (Vector2){
    ((position.x * tile_size) + (tile_size / 2.0f)),
    ((position.y * tile_size) + (tile_size / 2.0f))
  };
}


void read_input(physics_body *player_body) {
  // Reset player direction
  player_body->direction.x = 0.0f;
  player_body->direction.y = 0.0f;

  // Set player direction based on input 
  if (IsKeyDown(KEY_LEFT))  player_body->direction.x -= 1.0f;
  if (IsKeyDown(KEY_RIGHT)) player_body->direction.x += 1.0f;
  if (IsKeyDown(KEY_UP))    player_body->direction.y -= 1.0f;
  if (IsKeyDown(KEY_DOWN))  player_body->direction.y += 1.0f;

  // Normalize player direction
  player_body->direction = normalize_vector2(player_body->direction);
}

void update_ai_body(physics_body *body, physics_body *player_body) {
  entity *entity = entities_get_entity(body->entity_id);
  if (CheckCollisionRecs(body->aabb, player_body->aabb)) {
    body->is_active = false;
    entity->is_active  = false;
  }

  float player_distance        = euclidean_distance(body->position, player_body->position);
  bool is_far_away_from_player = (player_distance / TILE_SIZE) > 7.0f;  // TODO: move number to config
  bool is_next_to_player       = (player_distance / TILE_SIZE) < 2.0f;
  bool generate_waypoints      = !is_far_away_from_player && entity->waypoints == NULL;
  bool follow_waypoints        = entity->waypoints != NULL && entity->current_waypoint_index < entity->waypoints->length;
  bool clear_waypoints         = is_next_to_player || (entity->waypoints != NULL && entity->current_waypoint_index >= entity->waypoints->length);

  if (generate_waypoints) {
    IntVector2 origin              = intvec2_from_vec2((Vector2){ body->position.x / TILE_SIZE, body->position.y / TILE_SIZE });
    IntVector2 goal                = intvec2_from_vec2((Vector2){ player_body->position.x / TILE_SIZE, player_body->position.y / TILE_SIZE });
    entity->waypoints              = astar_search(&origin, &goal);
    entity->current_waypoint_index = 0;
  } 

  if (follow_waypoints) {
    Vector2 *waypoint                  = dynlist_get_at(entity->waypoints, entity->current_waypoint_index);                                     // Tile Waypoint e.g. x:25, y:5
    Vector2 coords                     = { (waypoint->x * TILE_SIZE) + (TILE_SIZE / 2.0f - body->aabb.width/2.0f),
                                           (waypoint->y * TILE_SIZE) + (TILE_SIZE / 2.0f - body->aabb.height/2.0f) };                           // Waypoint in coords, e.g. x:400, y:80
    float coords_distance              = euclidean_distance((Vector2){ body->position.x, body->position.y }, (Vector2){ coords.x, coords.y });  // Distance between player and waypoint
    bool is_within_waypoint_threshhold = (coords_distance / TILE_SIZE) < 2.0f;                                                                  // TODO: move number to config
    Vector2 normalized_waypoint        = normalize_vector2((Vector2){ coords.x - body->position.x, coords.y - body->position.y }); 
    body->direction                    = normalized_waypoint;                                                                                   // Set player direction

    if (is_within_waypoint_threshhold) {
      entity->current_waypoint_index++; // Move to next waypoint
    }
  } 

  if (clear_waypoints) {
    body->direction = (Vector2){ 0.0f, 0.0f };
    free_reconstructed_path(entity->waypoints);
    entity->waypoints              = NULL;
    entity->current_waypoint_index = 0;
  }
}

bool show_debug_body = false;
void draw_debug_body(physics_body *body) {
  int column_size = 80;
  int column_height = 70;
  const char *velocity = TextFormat(
    "Velocity\nx: %f\ny: %f", 
    body->velocity.x, 
    body->velocity.y
  );
  const char *position = TextFormat(
    "Position\nx: %f\ny: %f", 
    body->aabb.x, 
    body->aabb.y
  );
  const char *acceleration = TextFormat(
    "Acceleration\nx: %f\ny: %f", 
    body->acceleration.x, 
    body->acceleration.y
  );
  const char *direction = TextFormat(
    "Direction\nx: %f\ny: %f", 
    body->direction.x, 
    body->direction.y
  );
  DrawRectangleRec((Rectangle){0,0, SCREEN_WIDTH, column_height}, BLACK);
  DrawText(velocity,      column_size  *  0 + 10,10,  10,  BLUE);
  DrawText(position,      column_size  *  1 + 10,10,  10,  GREEN);
  DrawText(acceleration,  column_size  *  2 + 10,10,  10,  YELLOW);
  DrawText(direction,     column_size  *  3 + 10,10,  10,  RED);
}

//----------------------------------------------------------------------------- 
// MAIN
//----------------------------------------------------------------------------- 

int main(void) 
{
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, TITLE);
  SetTargetFPS(TARGET_FPS);

  physics_init(9.8f);
  entities_init();

  Texture tileset   = LoadTexture("resources/tiles-16.png");
  level   level     = load_level( TILE_SIZE, TILES_PER_ROW, "resources/level1.map", "resources/level1.def", &tileset);
  int **collision_mask = malloc(sizeof(int) * level.height);
  for (int y = 0; y < level.height; y++) {
    collision_mask[y] = malloc(sizeof(int) * level.width);
    for (int x = 0; x < level.width; x++) {
      collision_mask[y][x] = level.tile_defs[level.tilemap[y][x]].is_walkable;
    }
  }
  astar_allocate(level.width, level.height, collision_mask);

  for (size_t i = 0; i < ENEMIES_COUNT; i++) {
    entities_add_entity( RED,
                        (Vector2){(25.0f + i) * level.tile_size, (5.0f + i) * level.tile_size },
                        TILE_SIZE / 1.5f, 
                        TILE_SIZE / 1.5f,
                        (Vector2){0.0f, 0.0f },
                        (Vector2){0.0f, 0.0f},
                        (Vector2){0.0f, 0.0f},
                        1.0f,
                        0.35f,
                        2.0f,
                        false);
  }

  size_t  player_id = entities_add_entity( WHITE,
                                          (Vector2){4.0f * level.tile_size, 4.0f * level.tile_size} , 
                                          TILE_SIZE / 1.5f,
                                          TILE_SIZE / 1.5f,
                                          (Vector2){0.0f, 0.0f },
                                          (Vector2){0.0f, 0.0f},
                                          (Vector2){0.0f, 0.0f},
                                          1.0f,
                                          0.25f,
                                          4.0f,
                                          true);
  entity *player = entities_get_entity(player_id);
  physics_body *player_body = physics_get_body(player->body_id);

  Camera2D camera = init_camera(
    (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f), player_body->aabb.y + (player_body->aabb.height / 2.0f)}, 
    (Vector2) { SCREEN_WIDTH/2.0f, SCREEN_HEIGHT/2.0f }, 
    2.0f
  );

  while(!WindowShouldClose()) 
  {
    // update player body from user input
    read_input(player_body);

    // update AI bodies
    for (size_t i = 0; i < entities_count(); i++) {
      if (i == player_id) continue;
      entity *ai_entity = entities_get_entity(i);
      physics_body *ai_body = physics_get_body(ai_entity->body_id);
      update_ai_body(ai_body, player_body);
    }

    // respawn inactive AI entities
    for(size_t i = 0; i < entities_count() - 1; i++) {
      entity *entity = entities_get_entity(i);  
      if (entity->is_active) continue;
      entities_add_entity(YELLOW,
                          (Vector2){(25.0f + i) * level.tile_size, (5.0f + i) * level.tile_size}, 
                          TILE_SIZE / 1.5f,
                          TILE_SIZE / 1.5f,
                          (Vector2){0.0f, 0.0f },
                          (Vector2){0.0f, 0.0f},
                          (Vector2){0.0f, 0.0f},
                          1.0f,
                          0.35f,
                          2.0f,
                          false);
    }


    update_camera(&camera, (Vector2){player_body->aabb.x + (player_body->aabb.width / 2.0f), player_body->aabb.y + (player_body->aabb.height / 2.0f)}); 
    physics_update(&level);

    BeginDrawing();
    ClearBackground(BACKGROUND);

    // Player debug
    if (IsKeyPressed(KEY_P)){
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
      if (!entity->is_active) continue;
      physics_body *entity_body = physics_get_body(entity->body_id);

      if (entity->waypoints != NULL && entity->current_waypoint_index < entity->waypoints->length) {
        Vector2 *waypoint = dynlist_get_at(entity->waypoints, entity->current_waypoint_index);
        Vector2 coords    = (Vector2){ (waypoint->x * TILE_SIZE) + (TILE_SIZE / 2.0f ), (waypoint->y * TILE_SIZE) + (TILE_SIZE / 2.0f ) };                           // Waypoint in coords, e.g. x:400, y:80
        DrawLineEx(entity_body->position, coords, 2.0f, entity->color);
        Vector2 textPos = (Vector2) {
          entity_body->position.x - 20,
          entity_body->position.y - entity_body->aabb.height * 2
        };
        DrawText(TextFormat("Waypoint i: %d (x: %f, %f)", entity->current_waypoint_index, coords.x, coords.y), textPos.x, textPos.y, 10, entity->color);
      }

      DrawRectangleRec(entity_body->aabb, entity->color);
    }

    EndMode2D();
    EndDrawing();
  }

  entities_deinit();
  physics_deinit();
  astar_free();
  unload_level(&level);
  CloseWindow();

  return 0;
}
