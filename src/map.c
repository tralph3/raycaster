#include "map.h"
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

Map load_map(char *path) {
  Map map = {0};
  FILE *fp = fopen(path, "rb");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: Error opening file '%s'\n", path);
    exit(1);
  }
  fread(&map, sizeof(int) * 3 + sizeof(Vector2), 1, fp);
  if (map.size % map.width) {
    fprintf(stderr, "ERROR: Map '%s' is not rectangular\n", path);
    exit(1);
  }
  MapTile *data = malloc(map.size * sizeof(MapTile));
  fread(data, sizeof(MapTile), map.size, fp);
  map.data = data;
  fclose(fp);
  return map;
}

void save_map(Map *map, char *path) {
  FILE *fp = fopen(path, "wb");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: Error opening file '%s'\n", path);
    exit(1);
  }
  fwrite(map, sizeof(int) * 3 + sizeof(Vector2), 1, fp);
  fwrite(map->data, sizeof(MapTile), map->size, fp);
  fclose(fp);
}

inline MapTile get_tile_at_point(Map *map, Vector2 position) {
  if (!is_in_bounds(map, position))
    return (MapTile){0, 0, 0, TILE_TYPE_WALL};
  else
    return map->data[(int)position.x + (int)position.y * map->width];
}

inline void set_tile_at_point(Map *map, Vector2 position, MapTile tile) {
  if (is_in_bounds(map, position))
    map->data[(int)position.x + (int)position.y * map->width] = tile;
}

inline bool is_in_bounds(Map *map, Vector2 position) {
  return !(position.x < 0 || position.x >= map->width || position.y < 0 || position.y * map->width >= map->size);
}
