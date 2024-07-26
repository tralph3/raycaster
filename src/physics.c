#include "map.h"
#include "player.h"

#include <stdlib.h>
#include <raymath.h>
#include <stdbool.h>

bool is_square_colliding(Vector2 square_position, float square_size, Map *map) {
  float half_square_size = square_size / 2.f;
  Vector2 top_left_corner =
    Vector2SubtractValue(square_position, half_square_size);

  Vector2 top_right_corner = (Vector2){square_position.x + half_square_size,
                                       square_position.y - half_square_size};
  Vector2 bottom_left_corner = (Vector2){square_position.x - half_square_size,
                                         square_position.y + half_square_size};
  Vector2 bottom_right_corner =
    Vector2AddValue(square_position, half_square_size);

  MapTile *top_left_tile;
  GET_TILE_AT_POINT_DEFAULT_WALL(top_left_tile, map, top_left_corner);
  MapTile *top_right_tile;
  GET_TILE_AT_POINT_DEFAULT_WALL(top_right_tile, map, top_right_corner);
  MapTile *bottom_left_tile;
  GET_TILE_AT_POINT_DEFAULT_WALL(bottom_left_tile, map, bottom_left_corner);
  MapTile *bottom_right_tile;
  GET_TILE_AT_POINT_DEFAULT_WALL(bottom_right_tile, map, bottom_right_corner);

  return top_left_tile->type == TILE_TYPE_WALL || top_right_tile->type == TILE_TYPE_WALL || bottom_left_tile->type == TILE_TYPE_WALL || bottom_right_tile->type == TILE_TYPE_WALL;
}

void check_collission(Player *player, Map *map) {
  Vector2 new_x_position = {player->position.x, player->old_position.y};
  Vector2 new_y_position = {player->old_position.x, player->position.y};

  if (is_square_colliding(new_x_position, player->size, map)) {
    player->position.x = player->old_position.x;
  }
  if (is_square_colliding(new_y_position, player->size, map)) {
    player->position.y = player->old_position.y;
  }
}
