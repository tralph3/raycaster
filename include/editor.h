#include "renderer.h"
#include "map.h"
#include <raylib.h>

#define TILE_SIZE 40

void draw_map_tile(Renderer*, Vector2, MapTile*);
void draw_editor_interface(Renderer*, Map*, Camera2D*);
void editor_input(Camera2D*);
