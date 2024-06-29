#include "map.h"
#include "renderer.h"
#include "editor.h"
#include "textures.h"
#include <raylib.h>

void draw_map_tile(Renderer *renderer, Vector2 position, MapTile *tile) {
  Texture2D tile_texture = get_texture(&renderer->textures, tile->wall_id);
  Rectangle source = {0, 0, tile_texture.width, tile_texture.height};
  Rectangle destination = {position.x * TILE_SIZE, position.y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
  if (tile->type == TILE_TYPE_WALL)
    DrawTexturePro(tile_texture, source, destination, Vector2Zero(), 0, WHITE);
}

void draw_current_tile(Renderer *renderer, MapEditor *editor) {
  Texture2D tile_texture = get_texture(&renderer->textures, editor->current_tile);
  Rectangle source = {0, 0, tile_texture.width, tile_texture.height};
  Rectangle destination = {renderer->screen_width - TILE_SIZE * 5 - 10, renderer->render_height - TILE_SIZE * 5 - 10, TILE_SIZE * 5, TILE_SIZE * 5};
  DrawTexturePro(tile_texture, source, destination, Vector2Zero(), 0, WHITE);
}

void draw_editor_interface(Renderer *renderer, MapEditor *editor) {
  BeginDrawing();
  draw_current_tile(renderer, editor);
  ClearBackground(BLACK);
  BeginMode2D(editor->camera);
  for (unsigned int x = 0; x < editor->map->width; ++x) {
    for (unsigned int y = 0; y <  editor->map->size / editor->map->width; ++y) {
      Vector2 position = {x, y};
      MapTile map_tile = get_tile_at_point(editor->map, position);
      draw_map_tile(renderer, (Vector2){x, y}, &map_tile);
    }
  }
  EndMode2D();
  EndDrawing();
}

void place_tile(Vector2 position, MapEditor *editor) {
  if (!is_in_bounds(editor->map, position)) return;
  MapTile tile = {0};
  tile.wall_id = editor->current_tile;
  tile.type = TILE_TYPE_WALL;
  set_tile_at_point(editor->map, position, tile);
}

void editor_input(MapEditor *editor) {
  Vector2 mouse_delta = GetMouseDelta();
  Camera2D *camera = &editor->camera;
  float scroll_wheel_amount = GetMouseWheelMove();

  if (IsMouseButtonDown(0)) {
      Vector2 mouse_pos_in_world =
        GetScreenToWorld2D(GetMousePosition(), editor->camera);
      Vector2 pointed_tile = {mouse_pos_in_world.x / TILE_SIZE, mouse_pos_in_world.y / TILE_SIZE};
      place_tile(pointed_tile, editor);
  }

  if (IsMouseButtonDown(1))
    camera->target = Vector2Subtract(camera->target, mouse_delta);

  if (scroll_wheel_amount != 0)
    camera->zoom += scroll_wheel_amount * 0.1;

  if (IsKeyPressed(KEY_ONE))
    editor->current_tile = 2;
  if (IsKeyPressed(KEY_TWO))
    editor->current_tile = 3;
  if (IsKeyPressed(KEY_THREE))
    editor->current_tile = 4;
  if (IsKeyPressed(KEY_FOUR))
    editor->current_tile = 5;
}
