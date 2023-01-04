#ifndef LOGGING_H
#define LOGGING_H

void printLog(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData, FILE *log);
char *dissassemble(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);


#endif
