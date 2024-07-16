#ifndef GAME_H
#define GAME_H

#include "renderer.h"
#include "textures.h"
#include "map.h"
#include "player.h"
#include "editor.h"
#include <raylib.h>

typedef struct Game Game;
typedef struct GameState GameState;

struct GameState {
  void (*loop)(Game *);
  bool change_state;
  GameState *next_state;
};

struct Game {
  Map map;
  GameState current_state;
  Player player;
  TextureArr textures;
  SpriteArr sprites;
  Texture2D sky;
  Renderer renderer;
  MapEditor editor;
};

void game_run(Game *);
Game create_new_game(void);
void state_play_loop(Game *);
void state_editor_loop(Game *);
void state_test_map_loop(Game *);

extern GameState state_play;
extern GameState state_editor;
extern GameState state_test_map;

#endif
