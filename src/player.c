#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#include "player.h"
#include "renderer.h"

void player_move(Player *player, Vector2 direction, float multiplier) {
    float delta_time = GetFrameTime();
    direction = Vector2Normalize(direction);
    Vector2 fw_movement = Vector2Scale(player->direction, direction.y);
    Vector2 strafe_movement = Vector2Scale(Vector2Normalize(player->camera_plane), direction.x);
    direction = Vector2Add(fw_movement, strafe_movement);
    player->old_position = player->position;
    player->position = Vector2Add(
        player->position,
        Vector2Scale(direction, player->speed * multiplier * delta_time));
}

void player_rotate(Player *player, float amount) {
    player->direction = Vector2Rotate(
        player->direction, player->rotation_speed * amount);
    player->camera_plane =
      Vector2Rotate(player->camera_plane, player->rotation_speed * amount);
}

void player_pitch(Player *player, float amount) {
  player->pitch -= amount * 0.0005;
}

void update_player_camera(Player *player) {
    Vector2 target = Vector2Add(player->position, player->direction);
    player->camera.position.x = player->position.x;
    player->camera.position.z = player->position.y;
    player->camera.target.x = target.x;
    player->camera.target.z = target.y;
    player->camera.target.y = player->pitch;
}

Player create_new_player(Vector2 initial_position, PlayerDirection initial_direction, float aspect_ratio) {
    Player player = {
        .position = initial_position,
        .old_position = initial_position,
        .speed = 2,
        .size = 0.3,
        .rotation_speed = 0.001,
        .pitch = 0.5,
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
    float fov_x = fabs(Vector2Angle(
                                    Vector2Add(player.camera_plane, player.direction), player.direction)) * 2;
    float fov_y = 2 * atan(tan(fov_x/2.f)/aspect_ratio);
    Camera camera = {
        .position = (Vector3){0, 0.5, 0},
        .fovy = fov_y * RAD2DEG,
        .target = (Vector3){0, player.pitch, 0},
        .up = (Vector3){0, 1, 0},
        .projection = CAMERA_PERSPECTIVE,
    };
    player.camera = camera;
    update_player_camera(&player);
    return player;
}
