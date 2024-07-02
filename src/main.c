#include <raylib.h>
#include <raymath.h>
#include <dirent.h>
#include "map.h"

#include "game.h"
#include "textures.h"
#include "renderer.h"

void generate_map(void) {
  MapTile data[81] = {
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 0}, (MapTile){2, 0, 3, 1},
    (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1}, (MapTile){2, 0, 3, 1},
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
    Game game = create_new_game();
    game_run(&game);
#endif
    return 0;
}
