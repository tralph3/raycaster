#include "map.h"
#include "renderer.h"
#include "editor.h"
#include "textures.h"
#include <raylib.h>

void draw_map_tile(Renderer *renderer, Vector2 position, MapTile *tile) {
  Texture2D tile_texture = get_texture(&renderer->textures, tile->wall_id);
  Rectangle source = {0, 0,
                                                         tile_texture.width,
                                                         tile_texture.height};
  Rectangle destination = {position.x * TILE_SIZE, position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  if (tile->type == TILE_TYPE_WALL)
    DrawTexturePro(tile_texture, source, destination, Vector2Zero(), 0, WHITE);
}

void draw_editor_interface(Renderer *renderer, Map *map, Camera2D *camera) {
  BeginDrawing();
  ClearBackground(BLACK);
  BeginMode2D(*camera);
  for (unsigned int x = 0; x < map->width; ++x) {
    for (unsigned int y = 0; y <  map->size / map->width; ++y) {
      Vector2 position = {x, y};
      MapTile map_tile = get_tile_at_point(map, position);
      draw_map_tile(renderer, (Vector2){x, y}, &map_tile);
    }
  }
  EndMode2D();
  EndDrawing();
}

void editor_input(Camera2D *camera) {
  Vector2 mouse_delta = GetMouseDelta();
  float scroll_wheel_amount = GetMouseWheelMove();

  if (IsMouseButtonDown(0))
    camera->target = Vector2Subtract(camera->target, mouse_delta);

  if (scroll_wheel_amount != 0)
    camera->zoom += scroll_wheel_amount * 0.1;
}
