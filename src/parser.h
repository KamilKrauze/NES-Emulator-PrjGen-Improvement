#ifndef PARSER_H
#define PARSER_H

std::ifstream *openFile(char *fileName);
void closeFile(std::ifstream *file);
void readHeader(CartridgeData *cartridgeData);
uint8_t readByte(CartridgeData *cartridgeData, int address);

#endif
