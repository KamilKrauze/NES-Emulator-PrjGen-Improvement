#include <SDL2/SDL.h>
#include "processorArchitechture.h"
#include "PPUArchitechture.h"

#ifndef RENDERING_H
#define RENDERING_H

bool SDLinit();
void SDLclose();

void PPUInit(PPUArchitechture *PPU, CartridgeData *cartridgeData);
void PPUDeallocate(PPUArchitechture* PPU);

void readPPUMemory(PPUArchitechture *PPU, CartridgeData *cartridgeData, uint16_t address);
void writePPUMemory(PPUArchitechture *PPU, CartridgeData *cartridgeData, uint16_t address, uint8_t data);

void render(PPUArchitechture *PPU, ArchState *archState, CartridgeData *cartridgeData);

#endif
