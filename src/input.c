#include <raylib.h>
#include "input.h"
#include "player.h"
#include "renderer.h"

void handle_input(Player *player) {
    Vector2 player_move_direction = {0};
    Vector2 mouse_delta = GetMouseDelta();
    float multiplier = 1;
    float mouse_sensitivity = 1;
    PollInputEvents();
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
    SetMousePosition(1920 / 2.f, 1080 / 2.f);
    HideCursor();
    player_move(player, player_move_direction, multiplier);
    player_rotate(player, mouse_delta.x * mouse_sensitivity);
}
