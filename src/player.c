#include <raylib.h>
#include <raymath.h>

#include "player.h"

void player_move(Player *player, Vector2 direction, float multiplier) {
    float delta_time = GetFrameTime();
    direction = Vector2Normalize(direction);
    Vector2 fw_movement = Vector2Scale(player->direction, direction.y);
    Vector2 strafe_movement = Vector2Scale(player->cameraPlane, direction.x);
    direction = Vector2Add(fw_movement, strafe_movement);
    player->old_position = player->position;
    player->position = Vector2Add(
        player->position,
        Vector2Scale(direction, player->speed * multiplier * delta_time));
}

void player_rotate(Player *player, float amount) {
    player->direction = Vector2Rotate(
        player->direction, player->rotation_speed * amount);
    player->cameraPlane = Vector2Rotate(
        player->cameraPlane, player->rotation_speed * amount);
}
