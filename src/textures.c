#include <raylib.h>
#include <raymath.h>

#include "textures.h"
#include "dynarray.h"

TexturePixels load_texture(const char* path) {
  Image image = LoadImage(path);
  Color *original_colors = LoadImageColors(image);
  Color *pixels = malloc(image.width * image.height * sizeof(Color));
  for (int y = 0; y < image.height; ++y) {
    for (int x = 0; x < image.width; ++x) {
      pixels[y + x * image.height] = original_colors[x + y * image.width];
    }
  }
  free(original_colors);
  TexturePixels texture_pixels = {
    .pixels = pixels,
    .texture = LoadTextureFromImage(image),
  };
  UnloadImage(image);
  return texture_pixels;
}

TextureArr load_all_textures(void) {
    TextureArr texture_arr = {0};
    da_append(&texture_arr, load_texture("./assets/textures/empty.png"));
    da_append(&texture_arr, load_texture("./assets/textures/warning_tape.png"));
    da_append(&texture_arr, load_texture("./assets/textures/floor_tile.png"));
    da_append(&texture_arr, load_texture("./assets/textures/redbrick.png"));
    da_append(&texture_arr, load_texture("./assets/textures/grass.png"));
    da_append(&texture_arr, load_texture("./assets/textures/purplestone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/greystone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/bluestone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/mossy.png"));
    da_append(&texture_arr, load_texture("./assets/textures/wood.png"));
    da_append(&texture_arr, load_texture("./assets/textures/colorstone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/barrel.png"));
    da_append(&texture_arr, load_texture("./assets/textures/pillar.png"));
    da_append(&texture_arr, load_texture("./assets/textures/greenlight.png"));
    da_append(&texture_arr, load_texture("./assets/textures/chase.png"));
    da_append(&texture_arr, load_texture("./assets/textures/bullet_decal.png"));
    da_append(&texture_arr, load_texture("./assets/textures/sky.png"));
    return texture_arr;
}

inline TexturePixels get_texture(TextureArr *texture_arr, TextureID id) {
  return texture_arr->items[(short)Clamp(id, 0, texture_arr->count - 1)];
}

inline Color get_texture_pixel(TexturePixels pixels, int x, int y) {
  return pixels.pixels[y + x * pixels.texture.height];
}
