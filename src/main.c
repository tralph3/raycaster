#include <raylib.h>
#include <raymath.h>
#include <dirent.h>
#include "map.h"

#include "dynarray.h"
#include "player.h"
#include "textures.h"
#include "renderer.h"
#include "input.h"
#include "physics.h"

int main(void) {
    Map map = load_map("assets/maps/test.map");
    Player player = {
        .position = map.player_start_position,
        .old_position = map.player_start_position,
        .direction = {1, 0},
        .cameraPlane = {0, 1},
        .speed = 2,
        .size = 0.3,
        .rotation_speed = 0.001,
    };
    unsigned int config_flags = FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_FULLSCREEN_MODE;
    SetConfigFlags(config_flags);

    SpriteArr sprites = {0};
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
        check_collission(&player, &map);
        draw_everything(&player, &textures, &sprites, &map);
    }
    return 0;
}
