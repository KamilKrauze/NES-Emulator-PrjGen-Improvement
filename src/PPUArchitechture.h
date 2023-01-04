#ifndef PPU_ARCHITECHTURE_H
#define PPU_ARCHITECHTURE_H

#include <queue>
#include <SDL2/SDL.h>

typedef struct
{
    uint8_t *OAM;
    uint8_t *secondaryOAM;
    uint8_t *VRAM;
    uint8_t *paletteRAM;

    uint8_t spritesFound;
    uint8_t spriteNumber;
    uint8_t spriteByte;
    uint8_t spriteBus;
    bool spriteInRange;
    uint8_t dataBus;

    bool flag;
    bool NMIflag;

    uint16_t v;
    uint16_t t;
    uint8_t x;
    bool w;

    uint16_t tileRegister1;
    uint16_t tileRegister2;
    uint8_t attributeRegister1;
    uint8_t attributeRegister2;

    uint8_t spriteYPos[8];
    uint8_t spriteIndexNumber[8];
    uint8_t spriteAttributeRegisters[8];
    uint8_t spriteXCounters[8];
    uint8_t spritePatternRegisters[16];
    uint8_t spriteActive[8];
    bool isSpriteZero[8];

    uint16_t tempRegisters[4];

    uint8_t *tiles;
    SDL_Texture *frame;
    uint32_t *frameBuffer;

    uint8_t oldWrite;

    int cycleNo;
    int scanlineNo;
    int frameNo;
    bool evenFrame;
} PPUArchitechture;

#endif
