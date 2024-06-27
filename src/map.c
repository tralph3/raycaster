#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include "map.h"

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
  int *walls = malloc(map.size * sizeof(int));
  int *ceiling = malloc(map.size * sizeof(int));
  int *floor = malloc(map.size * sizeof(int));
  fread(walls, sizeof(int), map.size, fp);
  fread(ceiling, sizeof(int), map.size, fp);
  fread(floor, sizeof(int), map.size, fp);
  map.walls = walls;
  map.ceiling = ceiling;
  map.floor = floor;
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
  fwrite(map->walls, sizeof(int), map->size, fp);
  fwrite(map->ceiling, sizeof(int), map->size, fp);
  fwrite(map->floor, sizeof(int), map->size, fp);
  fclose(fp);
}

inline int get_wall_at_point(Map *map, Vector2 position) {
  return map->walls[(int)position.x + (int)position.y * map->width];
}

inline int get_ceiling_at_point(Map *map, Vector2 position) {
  return map->ceiling[(int)position.x + (int)position.y * map->width];
}

inline int get_floor_at_point(Map *map, Vector2 position) {
  return map->floor[(int)position.x + (int)position.y * map->width];
}
