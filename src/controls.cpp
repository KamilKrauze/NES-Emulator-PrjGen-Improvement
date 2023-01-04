#include "controls.h"

void readStandardController(ArchState *archState)
{
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    SDL_PumpEvents();

    if (keyboardState[SDL_SCANCODE_Z])
        archState->controllerBuffer |= 0x01;

    if (keyboardState[SDL_SCANCODE_X])
        archState->controllerBuffer |= 0x02;

    if (keyboardState[SDL_SCANCODE_BACKSPACE])
        archState->controllerBuffer |= 0x04;

    if (keyboardState[SDL_SCANCODE_RETURN])
        archState->controllerBuffer |= 0x08;

    if (keyboardState[SDL_SCANCODE_UP])
        archState->controllerBuffer |= 0x10;

    if (keyboardState[SDL_SCANCODE_DOWN])
        archState->controllerBuffer |= 0x20;

    if (keyboardState[SDL_SCANCODE_LEFT])
        archState->controllerBuffer |= 0x40;

    if (keyboardState[SDL_SCANCODE_RIGHT])
        archState->controllerBuffer |= 0x80;
}
