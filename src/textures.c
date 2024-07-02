#include <math.h>
#include <raylib.h>
#include <raymath.h>

#include "textures.h"
#include "dynarray.h"

TextureArr load_all_textures(void) {
    TextureArr texture_arr = {0};
    da_append(&texture_arr, LoadTexture("./assets/textures/empty.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/warning_tape.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/floor_tile.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/redbrick.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/eagle.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/purplestone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/greystone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/bluestone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/mossy.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/wood.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/colorstone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/barrel.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/pillar.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/greenlight.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/chase.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/bullet_decal.png"));
    return texture_arr;
}

inline Texture2D get_texture(TextureArr *texture_arr, TextureID id) {
  return texture_arr->items[(int)Clamp(id, 0, texture_arr->count - 1)];
}
