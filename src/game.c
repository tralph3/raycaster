#include "game.h"
#include "gui.h"
#include "map.h"
#include "renderer.h"
#include "player.h"
#include "textures.h"
#include "input.h"
#include "physics.h"
#include "editor.h"
#include <raylib.h>
#include <stdlib.h>
#include <string.h>

GameState state_play = {0};
GameState state_editor = {0};
GameState state_test_map = {0};

Game create_new_game(void) {
    SetConfigFlags(
        FLAG_FULLSCREEN_MODE |
        FLAG_WINDOW_RESIZABLE |
        /* FLAG_VSYNC_HINT | */
        FLAG_MSAA_4X_HINT
        );

    InitWindow(0, 0, "Raycaster");
    InitAudioDevice();
    state_play.loop = state_play_loop;
    state_play.setup = state_play_setup;
    state_play.next_state = NULL;

    state_editor.loop = state_editor_loop;
    state_editor.setup = state_editor_setup;
    state_editor.next_state = &state_test_map;

    state_test_map.loop = state_test_map_loop;
    state_test_map.setup = state_test_map_setup;
    state_test_map.next_state = &state_editor;

    Map map = load_map("./assets/maps/test.map");
    map.skybox = 16;

    Renderer renderer = {0};
    init_renderer(&renderer);
    Game game = {
        .map = map,
        .current_state = state_editor,
        .player = create_new_player(map.player_start_position, PLAYER_DIRECTION_RIGHT),
        .renderer = renderer,
    };

    gui_settings.font = GetFontDefault();
    gui_settings.main_background = LIME;
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
        .layer = LAYER_WALL,
        .current_tool = EDITOR_TOOL_PENCIL,
    };
    game->editor = editor;
    game->current_state.setup(game);
    while (!WindowShouldClose()) {
        game->current_state.loop(game);
        if (game->current_state.change_state) {
            game->current_state.change_state = false;
            game->current_state = *game->current_state.next_state;
            game->current_state.setup(game);
        }
    }

    CloseWindow();
}

void state_play_setup(Game *game) {
    (void)game;
    DisableCursor();
}

void state_play_loop(Game *game) {

    handle_input(&game->player);
    check_collission(&game->player, &game->map);
    draw_everything(&game->renderer, &game->player, &game->map);
}

void state_editor_setup(Game *game) {
    (void)game;
    ShowCursor();
}

void state_editor_loop(Game *game) {
    draw_editor_interface(&game->renderer, &game->editor);

    if (IsKeyPressed(KEY_T))
        game->current_state.change_state = true;
}

void state_test_map_setup(Game *game) {
    (void)game;
    DisableCursor();
}

void state_test_map_loop(Game *game) {
    state_play_loop(game);
    if (IsKeyPressed(KEY_T))
        game->current_state.change_state = true;
}
