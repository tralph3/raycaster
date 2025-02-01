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

    MapTile top_left_tile = get_tile_at_point_default_wall(map, top_left_corner);
    MapTile top_right_tile = get_tile_at_point_default_wall(map, top_right_corner);
    MapTile bottom_left_tile = get_tile_at_point_default_wall(map, bottom_left_corner);
    MapTile bottom_right_tile = get_tile_at_point_default_wall(map, bottom_right_corner);

    return top_left_tile.type != TILE_TYPE_EMPTY || top_right_tile.type != TILE_TYPE_EMPTY || bottom_left_tile.type != TILE_TYPE_EMPTY || bottom_right_tile.type != TILE_TYPE_EMPTY;
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

Vector2 get_line_intersection_point(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4) {
    float A1 = p2.y - p1.y;
    float B1 = p1.x - p2.x;
    float C1 = A1 * p1.x + B1 * p1.y;
    float A2 = p4.y - p3.y;
    float B2 = p3.x - p4.x;
    float C2 = A2 * p3.x + B2 * p3.y;
    float denominator = A1 * B2 - A2 * B1;

    return (Vector2){
        .x = (B2 * C1 - B1 * C2) / denominator,
        .y = (A1 * C2 - A2 * C1) / denominator
    };
}
