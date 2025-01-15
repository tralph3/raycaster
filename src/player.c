#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"

void player_move(Player *player, Vector2 direction, float multiplier) {
    float delta_time = GetFrameTime();
    direction = Vector2Normalize(direction);
    if (direction.x != 0 || direction.y != 0) {
        player->velocity = Vector2Scale(direction, player->speed * multiplier * delta_time);
    } else {
        player->velocity = Vector2Lerp(player->velocity, Vector2Zero(), 20 * delta_time);
    }

    Vector2 fw_movement = Vector2Scale(Vector2Normalize(player->direction), player->velocity.y);
    Vector2 strafe_movement = Vector2Scale(Vector2Normalize(player->camera_plane), player->velocity.x);
    Vector2 movement = Vector2Add(fw_movement, strafe_movement);
    player->old_position = player->position;
    player->position = Vector2Add(player->position, movement);
}

void player_pitch(Player *player, float amount) {
    player->plane_height = Clamp(player->plane_height + amount, 0, 1);
}

void player_rotate(Player *player, float amount) {
    player->direction = Vector2Rotate(
        player->direction, player->rotation_speed * amount);
    player->camera_plane =
        Vector2Rotate(player->camera_plane, player->rotation_speed * amount);
}

Player create_new_player(Vector2 initial_position, PlayerDirection initial_direction) {
    Player player = {
        .position = initial_position,
        .old_position = initial_position,
        .speed = 2,
        .size = 0.3,
        .rotation_speed = 0.001,
        .plane_height = 0.5,
        .height = 0.5,
        .velocity = {0},
    };
    Vector2 direction = {0};
    Vector2 camera_plane = {0};

    switch (initial_direction) {
    case PLAYER_DIRECTION_UP:
        direction.x = 0;
        direction.y = -1;
        camera_plane.x = 1;
        camera_plane.y = 0;
        break;
    case PLAYER_DIRECTION_DOWN:
        direction.x = 0;
        direction.y = 1;
        camera_plane.x = -1;
        camera_plane.y = 0;
        break;
    case PLAYER_DIRECTION_LEFT:
        direction.x = -1;
        direction.y = 0;
        camera_plane.x = 0;
        camera_plane.y = -1;
        break;
    case PLAYER_DIRECTION_RIGHT:
        direction.x = 1;
        direction.y = 0;
        camera_plane.x = 0;
        camera_plane.y = 1;
        break;
    default:
        fprintf(stderr, "ERRROR: Unknown direction '%d'\n", initial_direction);
        exit(1);
    }
    player.direction = direction;
    player.camera_plane = camera_plane;
    return player;
}
