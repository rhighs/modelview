#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <SDL2/SDL_scancode.h>

#include "types.h"

struct LoadedImage {
    u32 width;
    u32 height;
    u32 nchannels;
    u32 len;
    u8 *data;
};

#define IO_LOG(STREAM, FMT, ...)\
        do{fprintf(STREAM, "[%s:%s:%d]: " FMT "\n", __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__);}while(0)

b8 io_is_key_pressed(SDL_Scancode code);

void io_change_state(SDL_Scancode code, b8 pressed);

u32 io_read_file(const char *path, u8 **buf_ptr);

LoadedImage io_read_image_file(const char *image_path);

#endif // IO_H
