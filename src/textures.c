#include <raylib.h>

#include "textures.h"
#include "dynarray.h"


ColorArray load_texture(char *path) {
    Image image = LoadImage(path);
    Color *colors = LoadImageColors(image);
    ColorArray color_array = {
        .width = image.width,
        .height = image.height,
        .data = colors,
        .texture = LoadTexture(path),
    };
    UnloadImage(image);
    return color_array;
}

Color get_pixel_color(ColorArray* color_array, Vector2 value) {
  return color_array->data[(int)value.x + (int)value.y * color_array->width];
}

TextureArr load_all_textures(void) {
    TextureArr texture_arr = {0};
    da_append(&texture_arr, load_texture("./assets/textures/eagle.png"));
    da_append(&texture_arr, load_texture("./assets/textures/redbrick.png"));
    da_append(&texture_arr, load_texture("./assets/textures/purplestone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/greystone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/bluestone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/mossy.png"));
    da_append(&texture_arr, load_texture("./assets/textures/wood.png"));
    da_append(&texture_arr, load_texture("./assets/textures/colorstone.png"));
    da_append(&texture_arr, load_texture("./assets/textures/barrel.png"));
    da_append(&texture_arr, load_texture("./assets/textures/pillar.png"));
    da_append(&texture_arr, load_texture("./assets/textures/greenlight.png"));
    da_append(&texture_arr, load_texture("./assets/textures/warning_tape.png"));
    da_append(&texture_arr, load_texture("./assets/textures/chase.png"));
    return texture_arr;
}
