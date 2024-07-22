#include "map.h"
#include "renderer.h"
#include "editor.h"
#include "textures.h"
#include "gui.h"
#include <raymath.h>
#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>

Vector2 rectangle_tool_start_pos = {-1, -1};

void draw_map_tile(MapEditor *editor, Renderer *renderer, Vector2 position, MapTile *tile) {
  Texture2D wall_texture = get_texture(&renderer->textures, tile->wall_id).texture;
  Texture2D ceiling_texture = get_texture(&renderer->textures, tile->ceiling_id).texture;
  Texture2D floor_texture = get_texture(&renderer->textures, tile->floor_id).texture;

  switch (editor->layer) {
  case LAYER_FLOOR: {
    Rectangle source = {0, 0, floor_texture.width, floor_texture.height};
    Rectangle destination = {position.x * TILE_SIZE, position.y * TILE_SIZE,
                             TILE_SIZE, TILE_SIZE};
    DrawTexturePro(floor_texture, source, destination, Vector2Zero(), 0, WHITE);
    source.width = wall_texture.width;
    source.height = wall_texture.height;
    DrawTexturePro(wall_texture, source, destination, Vector2Zero(), 0, ColorAlpha(WHITE, 0.4f));
  } break;
  case LAYER_WALL: {
    Rectangle source = {0, 0, floor_texture.width, floor_texture.height};
    Rectangle destination = {position.x * TILE_SIZE, position.y * TILE_SIZE,
                             TILE_SIZE, TILE_SIZE};
    DrawTexturePro(floor_texture, source, destination, Vector2Zero(), 0, ColorBrightness(WHITE, -0.6f));
    source.width = wall_texture.width;
    source.height = wall_texture.height;
    DrawTexturePro(wall_texture, source, destination, Vector2Zero(), 0, WHITE);
    source.width = ceiling_texture.width;
    source.height = ceiling_texture.height;
    DrawTexturePro(ceiling_texture, source, destination, Vector2Zero(), 0, ColorAlpha(WHITE, 0.2f));
  } break;
  case LAYER_CEILING: {
    Rectangle source = {0, 0, floor_texture.width, floor_texture.height};
    Rectangle destination = {position.x * TILE_SIZE, position.y * TILE_SIZE,
                             TILE_SIZE, TILE_SIZE};
    DrawTexturePro(floor_texture, source, destination, Vector2Zero(), 0, ColorBrightness(WHITE, -0.8f));
    source.width = wall_texture.width;
    source.height = wall_texture.height;
    DrawTexturePro(wall_texture, source, destination, Vector2Zero(), 0, ColorBrightness(WHITE, -0.6f));
    source.width = ceiling_texture.width;
    source.height = ceiling_texture.height;
    DrawTexturePro(ceiling_texture, source, destination, Vector2Zero(), 0, WHITE);
  } break;
  default:
    exit(1);
    break;
  }
}

void draw_current_tile(Renderer *renderer, MapEditor *editor) {
  Texture2D tile_texture = get_texture(&renderer->textures, editor->current_tile).texture;
  Rectangle source = {0, 0, tile_texture.width, tile_texture.height};
  Rectangle destination = {renderer->screen_width - TILE_SIZE * 5 - 10, renderer->render_height - TILE_SIZE * 5 - 10, TILE_SIZE * 5, TILE_SIZE * 5};
  DrawTexturePro(tile_texture, source, destination, Vector2Zero(), 0, WHITE);
}

void draw_gui(MapEditor *editor, Renderer *renderer) {
  const char *layer_label;
  switch (editor->layer){
  case LAYER_WALL:
    layer_label = "WALL";
    break;
  case LAYER_CEILING:
    layer_label = "CEILING";
    break;
  case LAYER_FLOOR:
    layer_label = "FLOOR";
    break;
  default:
    layer_label = "UNKNOWN";
    break;
  }

  int map_width = editor->map->width;
  int map_height = editor->map->size / editor->map->width;

  GUISpinnerValueLabel((Rectangle){10, renderer->screen_height - 50, 160, 40}, &editor->layer, 0, 2, layer_label);
  GUISpinnerValue((Rectangle){200, renderer->screen_height - 50, 160, 40}, &map_width, 1, 1000);
  GUISpinnerValue((Rectangle){390, renderer->screen_height - 50, 160, 40}, &map_height, 1, 1000);
  if (GUIButton((Rectangle){10, renderer->screen_height - 100, 160, 40}, "Save Map"))
    save_map(editor->map, "./assets/maps/test.map");
  resize_map(editor->map, map_width, map_height);
}

void draw_editor_interface(Renderer *renderer, MapEditor *editor) {
  BeginDrawing();
  ClearBackground(BLACK);
  BeginMode2D(editor->camera);
  unsigned int map_width = editor->map->width;
  unsigned int map_height = editor->map->size / editor->map->width;
  for (unsigned int x = 0; x < map_width; ++x) {
    for (unsigned int y = 0; y < map_height; ++y) {
      Vector2 position = {x, y};
      MapTile map_tile = get_tile_at_point(editor->map, position);
      draw_map_tile(editor, renderer, (Vector2){x, y}, &map_tile);
    }
  }
  DrawRectangleLinesEx((Rectangle){0,0, map_width * TILE_SIZE, map_height * TILE_SIZE}, 2, WHITE);
  EndMode2D();
  draw_current_tile(renderer, editor);
  draw_gui(editor, renderer);
  EndDrawing();
}

void place_tile(Vector2 position, MapEditor *editor) {
  if (!is_in_bounds(editor->map, position)) return;
  MapTile tile = get_tile_at_point(editor->map, position);
  if (editor->layer == LAYER_WALL)
    tile.wall_id = editor->current_tile;
  else if (editor->layer == LAYER_CEILING)
    tile.ceiling_id = editor->current_tile;
  else if (editor->layer == LAYER_FLOOR)
    tile.floor_id = editor->current_tile;

  if (tile.wall_id > 0 && editor->layer == LAYER_WALL)
    tile.type = TILE_TYPE_WALL;
  else if (tile.wall_id == 0)
    tile.type = TILE_TYPE_EMPTY;
  set_tile_at_point(editor->map, position, tile);
}

void editor_tool_pencil(MapEditor *editor) {
  Vector2 mouse_pos_in_world =
    GetScreenToWorld2D(GetMousePosition(), editor->camera);
  Vector2 pointed_tile = {mouse_pos_in_world.x / TILE_SIZE, mouse_pos_in_world.y / TILE_SIZE};
  place_tile(pointed_tile, editor);
}

#include <stdio.h>
void editor_tool_rectangle(MapEditor *editor) {
  if (IsMouseButtonPressed(0) && !is_in_bounds(editor->map, rectangle_tool_start_pos)) {
    Vector2 mouse_pos_in_world = GetScreenToWorld2D(GetMousePosition(), editor->camera);
    Vector2 pointed_tile = {mouse_pos_in_world.x / TILE_SIZE, mouse_pos_in_world.y / TILE_SIZE};
    if (is_in_bounds(editor->map, pointed_tile))
      rectangle_tool_start_pos = pointed_tile;
  }
  if (!is_in_bounds(editor->map, rectangle_tool_start_pos))
    return;

  if (IsKeyDown(KEY_ESCAPE)) {
    rectangle_tool_start_pos = (Vector2){-1, -1};
    return;
  }

  if (IsMouseButtonReleased(0)) {
    Vector2 mouse_pos_in_world = GetScreenToWorld2D(GetMousePosition(), editor->camera);
    Vector2 rectangle_tool_end_pos = {mouse_pos_in_world.x / TILE_SIZE, mouse_pos_in_world.y / TILE_SIZE};
    if (!is_in_bounds(editor->map, rectangle_tool_end_pos)) {
      rectangle_tool_start_pos = (Vector2){-1, -1};
      return;
    }
    int area_start_x;
    int area_start_y;
    if (rectangle_tool_start_pos.x < rectangle_tool_end_pos.x) {
      area_start_x = rectangle_tool_start_pos.x;
    } else {
      area_start_x = rectangle_tool_end_pos.x;
    }
    if (rectangle_tool_start_pos.y < rectangle_tool_end_pos.y) {
      area_start_y = rectangle_tool_start_pos.y;
    } else {
      area_start_y = rectangle_tool_end_pos.y;
    }
    int area_width = fabs(rectangle_tool_start_pos.x - rectangle_tool_end_pos.y);
    int area_height = fabs(rectangle_tool_start_pos.y - rectangle_tool_end_pos.y);
    for (int y = area_start_y; y < area_height; ++y) {
      for (int x = area_start_x; x < area_width; ++x) {
        place_tile((Vector2){x, y}, editor);
      }
    }
    rectangle_tool_start_pos = (Vector2){-1, -1};
  }
}

void editor_input(MapEditor *editor) {
  Vector2 mouse_delta = GetMouseDelta();
  Camera2D *camera = &editor->camera;

  if (IsMouseButtonDown(0)) {
    switch (editor->current_tool) {
    case EDITOR_TOOL_PENCIL:
      editor_tool_pencil(editor);
      break;
    case EDITOR_TOOL_RECTANGLE:
      // TODO: This whole function is broken because raylib doesn't properly detect mouse button input
      editor_tool_rectangle(editor);
      break;
    default:
      break;
    }
  }

  if (IsMouseButtonDown(2))
    camera->target = Vector2Subtract(camera->target, Vector2Scale(mouse_delta, 1/(camera->zoom)));

  if (IsMouseButtonDown(1)) {
    Vector2 mouse_pos_in_world =
      GetScreenToWorld2D(GetMousePosition(), editor->camera);
    Vector2 pointed_tile = {mouse_pos_in_world.x / TILE_SIZE,
                            mouse_pos_in_world.y / TILE_SIZE};
    int prev_tile = editor->current_tile;
    editor->current_tile = 0;
    place_tile(pointed_tile, editor);
    editor->current_tile = prev_tile;
  }

  float scroll_wheel_amount = GetMouseWheelMove();
  if (scroll_wheel_amount != 0) {
    Vector2 mouse_world_position = GetScreenToWorld2D(GetMousePosition(), *camera);
    camera->offset = GetMousePosition();
    camera->target = mouse_world_position;
    float scale_factor = 1.0f + (0.25f*fabsf(scroll_wheel_amount));
    if (scroll_wheel_amount < 0) scale_factor = 1.0f/scale_factor;
    camera->zoom = Clamp(camera->zoom*scale_factor, 0.125f, 64.0f);
  }

  for (char i = 1; i < 10; ++i) {
    if (IsKeyPressed(i + 48))
      editor->current_tile = i;
  }
}
