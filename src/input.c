#include <raylib.h>
#include "input.h"
#include "player.h"

void handle_input(Player *player) {
    Vector2 player_move_direction = {0};
    Vector2 mouse_delta = GetMouseDelta();
    float multiplier = 1;
    float mouse_sensitivity = 2;
    if (IsKeyPressed(KEY_ESCAPE))
        CloseWindow();
    if (IsKeyDown(KEY_LEFT_SHIFT))
        multiplier = 3.f;

    if (IsKeyDown(KEY_W))
        player_move_direction.y += 1;
    if (IsKeyDown(KEY_S))
        player_move_direction.y -= 1;
    if (IsKeyDown(KEY_D))
        player_move_direction.x += 1;
    if (IsKeyDown(KEY_A))
        player_move_direction.x -= 1;
    /* if (IsKeyDown(KEY_LEFT_CONTROL)) { */
    /*     player->height = Lerp(player->height, 0.1, GetFrameTime() * 10); */
    /*     player->speed = 1; */
    /* } */
    /* else if (IsKeyDown(KEY_LEFT_ALT)) */
    /*     player->height = Lerp(player->height, 0.9, GetFrameTime() * 10); */
    /* else { */
    /*     player->height = Lerp(player->height, 0.5, GetFrameTime() * 10); */
    /*     player->speed = 2; */
    /* } */
    player_move(player, player_move_direction, multiplier);
    player_pitch(player, -mouse_delta.y * 0.001);
    player_rotate(player, mouse_delta.x * mouse_sensitivity);
}
