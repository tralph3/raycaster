#ifndef EDITOR_H
#define EDITOR_H

#include "renderer.h"
#include "map.h"
#include <raylib.h>

#define TILE_SIZE 40

typedef enum {
    EDITOR_TOOL_PENCIL,
    EDITOR_TOOL_RECTANGLE,
} EditorTool;

typedef enum {
    LAYER_FLOOR = 0,
    LAYER_WALL,
    LAYER_CEILING,
} Layer;

typedef struct MapEditor {
    int current_tile;
    Camera2D camera;
    Map *map;
    int layer;
    EditorTool current_tool;
} MapEditor;

void draw_map_tile(MapEditor *, Renderer*, Vector2, MapTile*);
void draw_editor_interface(Renderer*, MapEditor*);
void editor_input(MapEditor *);
void draw_gui(MapEditor *, Renderer *);
void editor_tool_pencil(MapEditor*);
void editor_tool_rectangle(MapEditor*);

#endif
