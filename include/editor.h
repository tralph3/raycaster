#ifndef EDITOR_H
#define EDITOR_H

#include "renderer.h"
#include "map.h"
#include <raylib.h>

#define TILE_SIZE 40

typedef struct {
  int current_tile;
  Camera2D camera;
  Map *map;
} MapEditor;

void draw_map_tile(Renderer*, Vector2, MapTile*);
void draw_editor_interface(Renderer*, MapEditor*);
void editor_input(MapEditor*);

#endif
