#include <raylib.h>
#include <raymath.h>
#include <dirent.h>
#include <stdio.h>

#include "dynarray.h"
#include "player.h"
#include "textures.h"
#include "renderer.h"
#include "input.h"
#include "physics.h"

void move_barrel(Player *player, Sprite *barrel) {
    float delta_time = GetFrameTime();
    Vector2 direction = Vector2Normalize(Vector2Subtract(player->position, barrel->position));
    barrel->position =
      Vector2Add(barrel->position, Vector2Scale(direction, 2 * delta_time));
}

int main(void) {
    Player player = {
        .position = {1.5, 1.5},
        .oldPosition = {1, 1},
        .direction = {1, 0},
        .cameraPlane = {0, 1},
        .speed = 2,
        .size = 0.3,
        .rotation_speed = 0.001,
    };
    unsigned int config_flags = FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE;
    SetConfigFlags(config_flags);

    SpriteArr sprites = {0};
    Sprite sprite = {.texture_id = 8, .position = (Vector2){20.5, 6.5}};
    da_append(&sprites, sprite);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");
    InitAudioDevice();
    Sound bg = LoadSound("./assets/sounds/bg.wav");
    HideCursor();

    TextureArr textures = load_all_textures();

    while (!WindowShouldClose()) {
        /* if (!IsSoundPlaying(bg)) */
        /*     PlaySound(bg); */
        handle_input(&player);
        move_barrel(&player, &sprites.items[0]);
        check_collission(&player);
        draw_everything(&player, &textures, &sprites);
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
