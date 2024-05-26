#include <raylib.h>
#include <raymath.h>
#include <dirent.h>

#include "player.h"
#include "textures.h"
#include "renderer.h"
#include "input.h"
#include "physics.h"

int main(void) {
    Player player = {
        .position = {100, 100},
        .oldPosition = {100, 100},
        .direction = {1, 0},
        .cameraPlane = {0, 1},
        .speed = 200,
        .rotation_speed = 0.001,
    };
    unsigned int config_flags = FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE;
    SetConfigFlags(config_flags);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");
    InitAudioDevice();
    Sound bg = LoadSound("./assets/sounds/bg.wav");
    HideCursor();

    TextureArr textures = load_all_textures();

    while (!WindowShouldClose()) {
        if (!IsSoundPlaying(bg)) {
            PlaySound(bg);
        }
        handle_input(&player);
        check_collission(&player);
        draw_everything(&player, &textures);
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
