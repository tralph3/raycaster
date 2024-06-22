#include "renderer.h"
#include "player.h"

void check_collission(Player *player) {
    int prev_map_x = player->oldPosition.x;
    int prev_map_y = player->oldPosition.y;
    int curr_map_x = player->position.x;
    int curr_map_y = player->position.y;
    int tile_x = world_map[curr_map_x + prev_map_y * MAP_WIDTH];
    int tile_y = world_map[prev_map_x + curr_map_y * MAP_WIDTH];
    if (tile_x > 0)
      player->position.x = player->oldPosition.x;
    if (tile_y > 0)
      player->position.y = player->oldPosition.y;
}
