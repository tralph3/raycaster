#ifndef EDITOR_H
#define EDITOR_H

#include "renderer.h"
#include "map.h"
#include <raylib.h>

#define TILE_SIZE 40

typedef enum {
  LAYER_FLOOR = 0,
  LAYER_WALL,
  LAYER_CEILING,
} Layer;

typedef struct {
  int current_tile;
  Camera2D camera;
  Map *map;
  int layer;
} MapEditor;

void draw_map_tile(MapEditor *, Renderer*, Vector2, MapTile*);
void draw_editor_interface(Renderer*, MapEditor*);
void editor_input(MapEditor *);
void draw_gui(MapEditor *, Renderer *);

#endif
