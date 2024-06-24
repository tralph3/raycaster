#include "renderer.h"
#include "player.h"
#include <raymath.h>

// TODOOOOO: THIS SHIT IS SHITY CODE FIX IT BEFORE I DIE
void check_collission(Player *player) {
  float half_psize = player->size * 0.5;
  Vector2 top_left_corner_x = (Vector2){player->position.x - half_psize,
                                      player->oldPosition.y - half_psize};
  Vector2 top_right_corner_x = (Vector2){player->position.x + half_psize,
                                       player->oldPosition.y - half_psize};
  Vector2 bottom_left_corner_x = (Vector2){player->position.x - half_psize,
                                         player->oldPosition.y + half_psize};
  Vector2 bottom_right_corner_x = (Vector2){player->position.x + half_psize,
                                          player->oldPosition.y + half_psize};
  int top_left_tile_x =
    world_map[(int)top_left_corner_x.x + (int)top_left_corner_x.y * MAP_WIDTH];
  int top_right_tile_x =
    world_map[(int)top_right_corner_x.x + (int)top_right_corner_x.y * MAP_WIDTH];
  int bottom_left_tile_x =
    world_map[(int)bottom_left_corner_x.x + (int)bottom_left_corner_x.y * MAP_WIDTH];
  int bottom_right_tile_x = world_map[(int)bottom_right_corner_x.x +
                                    (int)bottom_right_corner_x.y * MAP_WIDTH];
  Vector2 top_left_corner_y = (Vector2){player->oldPosition.x - half_psize,
                                      player->position.y - half_psize};
  Vector2 top_right_corner_y = (Vector2){player->oldPosition.x + half_psize,
                                       player->position.y - half_psize};
  Vector2 bottom_left_corner_y = (Vector2){player->oldPosition.x - half_psize,
                                         player->position.y + half_psize};
  Vector2 bottom_right_corner_y = (Vector2){player->oldPosition.x + half_psize,
                                          player->position.y + half_psize};
  int top_left_tile_y =
    world_map[(int)top_left_corner_y.x + (int)top_left_corner_y.y * MAP_WIDTH];
  int top_right_tile_y =
    world_map[(int)top_right_corner_y.x + (int)top_right_corner_y.y * MAP_WIDTH];
  int bottom_left_tile_y =
    world_map[(int)bottom_left_corner_y.x + (int)bottom_left_corner_y.y * MAP_WIDTH];
  int bottom_right_tile_y = world_map[(int)bottom_right_corner_y.x +
                                      (int)bottom_right_corner_y.y * MAP_WIDTH];

  if (top_left_tile_x > 0 || top_right_tile_x > 0 || bottom_left_tile_x > 0 ||
      bottom_right_tile_x > 0)
    player->position.x = player->oldPosition.x;
  if (top_left_tile_y > 0 || top_right_tile_y > 0 || bottom_left_tile_y > 0 ||
      bottom_right_tile_y > 0)
    player->position.y = player->oldPosition.y;
}
