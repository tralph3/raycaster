#include "game.h"
#include "map.h"
#include "renderer.h"
#include "player.h"
#include "textures.h"
#include "dynarray.h"
#include "editor.h"
#include <raylib.h>

GameState state_play = {0};
GameState state_editor = {0};
GameState state_test_map = {0};

Game create_new_game(void) {
  SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

  InitWindow(0, 0, "Raycaster");
  InitAudioDevice();
  state_play.loop = state_play_loop;
  state_play.change_state = false;
  state_play.next_state = NULL;

  state_editor.loop = state_editor_loop;
  state_editor.change_state = false;
  state_editor.next_state = &state_test_map;

  state_test_map.loop = state_test_map_loop;
  state_test_map.change_state = false;
  state_test_map.next_state = &state_editor;

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
      .current_state = state_editor,
      .player = create_new_player(map.player_start_position,
                                  PLAYER_DIRECTION_RIGHT, aspect_ratio),
      .textures = textures,
      .sprites = sprites,
      .sky = sky,
      .renderer = renderer,
  };
  return game;
}

void game_run(Game *game) {
  Camera2D editor_camera = {0};
  editor_camera.target = Vector2Zero();
  editor_camera.zoom = 1;
  MapEditor editor = {
    .current_tile = 1,
    .camera = editor_camera,
    .map = &game->map,
  };
  game->editor = editor;
  while (!WindowShouldClose()) {
    game->current_state.loop(game);
    if (game->current_state.change_state) {
      game->current_state.change_state = false;
      game->current_state = *game->current_state.next_state;
    }
  }
}

void state_play_loop(Game *game) {
  HideCursor();
  handle_input(&game->player);
  check_collission(&game->player, &game->map);
  update_player_camera(&game->player);
  draw_everything(&game->renderer, &game->player, &game->map);
}

void state_editor_loop(Game *game) {
  ShowCursor();
  draw_editor_interface(&game->renderer, &game->editor);
  editor_input(&game->editor);
  if (IsKeyPressed(KEY_T))
    game->current_state.change_state = true;
}

void state_test_map_loop(Game *game) {
  state_play_loop(game);
  if (IsKeyPressed(KEY_T))
    game->current_state.change_state = true;
}