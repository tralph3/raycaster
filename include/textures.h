#ifndef TEXTURES_H
#define TEXTURES_H

#include "dynarray.h"
#include <raylib.h>

typedef struct {
    int count;
    int capacity;
    Texture2D *items;
} TextureArr;

TextureArr load_all_textures(void) {
    TextureArr texture_arr = {0};
    da_append(&texture_arr, LoadTexture("./assets/textures/eagle.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/redbrick.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/purplestone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/greystone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/bluestone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/mossy.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/wood.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/colorstone.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/barrel.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/pillar.png"));
    da_append(&texture_arr, LoadTexture("./assets/textures/greenlight.png"));
    return texture_arr;
}

#endif
