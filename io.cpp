#include <SDL2/SDL_scancode.h>
#include <assert.h>

#include <stb_image.h>

#include "io.h"

b8 __PRESSED_KEYS[SDL_NUM_SCANCODES] = { FALSE };

b8 io_is_key_pressed(SDL_Scancode code) { 
    assert((u32)code < SDL_NUM_SCANCODES);
    return  __PRESSED_KEYS[code];
}

void io_change_state(SDL_Scancode code, const b8 pressed) {
#ifdef MDEBUG
    fprintf(stdout, "[%s:%d] keycode %d state: %d\n",
            __FUNCTION__, __LINE__, (u32)code, pressed);
#endif
    assert((u32)code < SDL_NUM_SCANCODES);
    __PRESSED_KEYS[code] = pressed;
}

u32 io_read_file(const char* filepath, u8 **buf_ptr) {
#ifdef MDEBUG
    fprintf(stdout, "[%s:%d] reading file: %s\n",
            __FUNCTION__, __LINE__, filepath);
#endif
    FILE *file = fopen(filepath, "r");
    fseek(file, 0, SEEK_END);
    u32 filesize = ftell(file);
    rewind(file);

    *buf_ptr = (u8 *)malloc(filesize+1);
    if (!fread(*buf_ptr, sizeof(char), filesize, file)) {
        fprintf(stderr, "Could not read file: %s\n", filepath);
        *buf_ptr = NULL;
        return 0;
    }

    return filesize;
}

LoadedImage io_read_image_file(const char * image_path) {
    i32 image_width, image_height, nr_channels;
    stbi_set_flip_vertically_on_load(1);
    u8 *image_data = stbi_load(image_path, &image_width, &image_height, &nr_channels, 0);

    if (stbi_failure_reason()) {
        fprintf(stderr, "Failed reading %s reason: %s\n",
                image_path, stbi_failure_reason());
        exit(1);
    }

    LoadedImage result;
    result.width = (u32)image_width;
    result.height = (u32)image_height;
    result.nchannels = (u32)nr_channels;
    result.len = (u32)(image_width * image_height);
    result.data = image_data;

    return result;
}

