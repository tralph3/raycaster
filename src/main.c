#include <raylib.h>
#include <raymath.h>
#include <dirent.h>
#include "map.h"

#include "game.h"
#include "textures.h"
#include "renderer.h"

void generate_map(void) {
  MapTile data[81] = {
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){4, 0, 0, 1}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 0, 0, 0}, (MapTile){13, 5, 7, 0}, (MapTile){13, 0, 0, 1},
    (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 5, 7, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 0, 0, 1}, (MapTile){13, 5, 7, 1}, (MapTile){13, 0, 0, 1},
  };
  Map map = {
      .player_start_position = (Vector2){1.5, 1.5},
      .size = 81,
      .width = 9,
      .version = 1,
      .data = data,
  };
  save_map(&map, "./assets/maps/test.map");
}

int main(void) {
#if 0
  generate_map();
#else

    unsigned int config_flags = FLAG_FULLSCREEN_MODE | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT;
    SetConfigFlags(config_flags);
    SpriteArr sprites = {0};
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycaster");
    InitAudioDevice();
    Sound bg = LoadSound("./assets/sounds/bg.wav");
    HideCursor();
    Game game = create_new_game();
    game_run(&game);
#endif
    return 0;
}
