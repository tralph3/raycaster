#include "game.h"
#include "map.h"
#include "renderer.h"
#include "physics.h"
#include "input.h"
#include "player.h"
#include "textures.h"
#include "dynarray.h"
#include <raylib.h>
#include <stdio.h>

Game create_new_game(void) {
  Map map = load_map("./assets/maps/test2.map");
  SpriteArr sprites = {0};
  Sprite sprite = {
      .position = (Vector2){10.5, 6.5},
      .texture_id = 10,
  };
  da_append(&sprites, sprite);
  Image screen_image = LoadImageFromScreen();
  TextureArr textures = load_all_textures();
  Texture2D sky = LoadTexture("./assets/textures/sky.png");
  UnloadImage(screen_image);
  Game game = {
      .map = map,
      .state = GAME_STATE_PLAY,
      .player =
          create_new_player(map.player_start_position, PLAYER_DIRECTION_RIGHT),
      .textures = textures,
      .sprites = sprites,
      .sky = sky,
  };
  return game;
}

void game_run(Game *game) {
  while (!WindowShouldClose()) {
    /* if (!IsSoundPlaying(bg)) { */
    /*   PlaySound(bg); */
    /* } */
    handle_input(&game->player);
    check_collission(game);
    update_player_camera(&game->player);
    draw_everything(game);
  }
}
