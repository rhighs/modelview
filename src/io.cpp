#include <SDL2/SDL_scancode.h>
#include <assert.h>

#include <stb_image.h>
#include <sys/stat.h>

#include "io.h"
#include "core/error.h"

b8 __PRESSED_KEYS[SDL_NUM_SCANCODES] = { FALSE };

b8 io_is_key_pressed(SDL_Scancode code) { 
    assert((u32)code < SDL_NUM_SCANCODES);
    return  __PRESSED_KEYS[code];
}

void io_change_state(SDL_Scancode code, const b8 pressed) {
#ifdef IODEBUG
    IO_LOG(stdout, "keycode %d state = %d", (u32)code, pressed);
#endif
    assert((u32)code < SDL_NUM_SCANCODES);
    __PRESSED_KEYS[code] = pressed;
}

static
i32 __read_file_size(const char *filepath) {
    struct stat stat_buf;
    int rc = stat(filepath, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

u32 io_read_file(const char* filepath, u8 **buf_ptr) {
    IO_LOG(stdout, "reading file %s", filepath);
    FILE *file = fopen(filepath, "rb");

    i32 filesize = __read_file_size(filepath);
    if (filesize == -1) {
        IO_LOG(stderr, "could not read file %s", filepath);
        *buf_ptr = NULL;
        return 0;
    }

    u8* buffer = (u8*)malloc(filesize + 1);
    while (!feof(file)) {
        fread(buffer, 1, filesize, file);
    }
    fclose(file);
    buffer[filesize] = '\0';
    *buf_ptr = buffer;
    return filesize;
}

LoadedImage io_read_image_file(const char * image_path) {
    i32 image_width, image_height, nr_channels;
    stbi_set_flip_vertically_on_load(1);
    u8 *image_data = stbi_load(image_path, &image_width, &image_height, &nr_channels, 0);

    char *stbi_reason = NULL;
    if ((stbi_reason = (char *)stbi_failure_reason()) != NULL) {
        IO_LOG(stderr, "failed reading %s reason: \"%s\" - exiting...", image_path, stbi_reason);
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
