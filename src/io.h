#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <SDL2/SDL_scancode.h>

#include "core/types.h"

struct LoadedImage {
    u32 width;
    u32 height;
    u32 nchannels;
    u32 len;
    u8 *data;
};

b8 io_is_key_pressed(SDL_Scancode code);

void io_change_state(SDL_Scancode code, b8 pressed);

u32 io_read_file(const char *path, u8 **buf_ptr);

LoadedImage io_read_image_file(const char *image_path);

#endif // IO_H
