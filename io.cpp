#include <SDL2/SDL_scancode.h>
#include <assert.h>

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
