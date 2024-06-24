#include "map.h"
#include "player.h"
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
  int top_left_tile = get_wall_at_point(map, top_left_corner);
  int top_right_tile = get_wall_at_point(map, top_right_corner);
  int bottom_left_tile = get_wall_at_point(map, bottom_left_corner);
  int bottom_right_tile = get_wall_at_point(map, bottom_right_corner);
  return top_left_tile > 0 || top_right_tile > 0 || bottom_left_tile > 0 || bottom_right_tile > 0;
}

void check_collission(Player *player, Map* map) {
  Vector2 new_x_position = {player->position.x, player->old_position.y};
  Vector2 new_y_position = {player->old_position.x, player->position.y};

  if (is_square_colliding(new_x_position, player->size, map)) {
    player->position.x = player->old_position.x;
  }
  if (is_square_colliding(new_y_position, player->size, map)) {
    player->position.y = player->old_position.y;
  }
}
