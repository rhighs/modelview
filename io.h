#ifndef IO_H
#define IO_H

#include <SDL2/SDL_keycode.h>

#include "types.h"

b8 io_is_key_pressed(SDL_KeyCode code);

void io_change_state(SDL_KeyCode code, b8 pressed);

#endif // IO_H
