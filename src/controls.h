#ifndef CONTROLS_H
#define CONTORLS_H

#include <SDL2/SDL.h>
#include "processorArchitechture.h"

void readStandardController(ArchState *archState);

const SDL_Scancode A_KEY = SDL_SCANCODE_Z;
const SDL_Scancode B_KEY = SDL_SCANCODE_X;
const SDL_Scancode SELECT_KEY = SDL_SCANCODE_BACKSPACE;
const SDL_Scancode START_KEY = SDL_SCANCODE_RETURN;
const SDL_Scancode UP_KEY = SDL_SCANCODE_UP;
const SDL_Scancode DOWN_KEY = SDL_SCANCODE_DOWN;
const SDL_Scancode LEFT_KEY = SDL_SCANCODE_LEFT;
const SDL_Scancode RIGHT_KEY = SDL_SCANCODE_RIGHT;

#endif
