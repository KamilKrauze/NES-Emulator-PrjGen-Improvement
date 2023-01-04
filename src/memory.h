#ifndef MEMORY_H
#define MEMORY_H

#include "processorArchitechture.h"
#include "PPUArchitechture.h"

uint16_t make16Bit(uint8_t lowerBits, uint8_t upperBits);
void memoryInit(ArchState *archState, CartridgeData *cartridgeData);
void deallocate(ArchState *archState);
void coarseXIncrement(PPUArchitechture *PPU);
void YIncrement(PPUArchitechture *PPU);
void readData(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void writeData(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);

#endif
