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
  SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

  InitWindow(0, 0, "Raycaster");
  InitAudioDevice();
  HideCursor();


  Renderer renderer = {
    .render_width = GetMonitorWidth(GetCurrentMonitor()),
    .render_height = GetMonitorHeight(GetCurrentMonitor()),
    .screen_width = GetMonitorWidth(GetCurrentMonitor()),
    .screen_height = GetMonitorHeight(GetCurrentMonitor()),
  };
  Map map = load_map("./assets/maps/test.map");
  SpriteArr sprites = {0};
  Sprite sprite = {
      .position = (Vector2){10.5, 6.5},
      .texture_id = 10,
  };

  da_append(&sprites, sprite);
  TextureArr textures = load_all_textures();
  Texture2D sky = LoadTexture("./assets/textures/sky.png");
  renderer.textures = textures;
  renderer.sprites = sprites;
  float aspect_ratio = (float)renderer.render_width/renderer.render_height;
  Game game = {
      .map = map,
      .state = GAME_STATE_PLAY,
      .player =
          create_new_player(map.player_start_position, PLAYER_DIRECTION_RIGHT, aspect_ratio),
      .textures = textures,
      .sprites = sprites,
      .sky = sky,
      .renderer = renderer,
  };
  return game;
}

void game_run(Game *game) {
  while (!WindowShouldClose()) {
    /* if (!IsSoundPlaying(bg)) { */
    /*   PlaySound(bg); */
    /* } */
    switch (game->state) {
    case GAME_STATE_PLAY:
      handle_input(&game->player);
      check_collission(game);
      update_player_camera(&game->player);
      draw_everything(&game->renderer, &game->player, &game->map);
      break;
    case GAME_STATE_EDITOR:
      break;
    default:
      fprintf(stderr, "ERROR: Unknown game state '%d'", game->state);
      break;
    }

  }
}
