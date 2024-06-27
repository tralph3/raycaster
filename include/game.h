#ifndef GAME_H
#define GAME_H

#include "textures.h"
#include "map.h"
#include "player.h"
#include <raylib.h>

typedef enum {
  GAME_STATE_PLAY = 1,
  GAME_STATE_EDITOR,
} GameState;

typedef struct {
  Map map;
  GameState state;
  Player player;
  TextureArr textures;
  SpriteArr sprites;
  Texture2D sky;
} Game;

void game_run(Game *);
Game create_new_game(void);

#endif
