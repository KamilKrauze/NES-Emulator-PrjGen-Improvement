#include "processorArchitechture.h"
#include "PPUArchitechture.h"

#ifndef PROCESSOR_EXECUTION_H
#define PROCESSOR_ARCHITECHTURE_H

void initProcessor(ArchState *archState);
void incrementPC(ArchState *archState);
bool overflow(uint8_t x, uint8_t add);
bool wrongSign(int8_t x, int8_t add);
void reset(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _NMI(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void reset(ArchState *archState, CartridgeData *cartridgeData);
void cycle0(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle1(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle2(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle3(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle4(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle5(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void cycle6(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);

#endif
