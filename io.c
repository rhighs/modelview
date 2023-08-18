#include "io.h"

b8 __PRESSED_KEYS[322] = { FALSE };
b8 io_is_key_pressed(SDL_KeyCode code) { return  __PRESSED_KEYS[code]; }
void io_change_state(SDL_KeyCode code, const b8 pressed) { __PRESSED_KEYS[code] = pressed; }
