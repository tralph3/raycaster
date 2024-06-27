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
    SetMousePosition(SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f);

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
    player_move(player, player_move_direction, multiplier);
    player_rotate(player, mouse_delta.x * mouse_sensitivity);
}
