#include <SDL2/SDL_scancode.h>
#include <assert.h>

#include "io.h"

b8 __PRESSED_KEYS[SDL_NUM_SCANCODES] = { FALSE };

b8 io_is_key_pressed(SDL_Scancode code) { 
#ifdef MDEBUG
    fprintf(stdout, "[%s:%d] test keycode %d\n",
            __FUNCTION__, __LINE__, (u32)code);
#endif
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
