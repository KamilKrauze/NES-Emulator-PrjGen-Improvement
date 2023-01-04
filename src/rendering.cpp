#include <SDL.h>
#include "memory.h"
#include "rendering.h"
#include "PPUArchitechture.h"
#include "parser.h"
#include "globals.h"

bool SDLinit()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialise SDL! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if ((gWindow = SDL_CreateWindow("NESEmulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0)) == NULL)
    {
        printf("Failed to create window! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if ((gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
    {
        printf("Failed to create renderer! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    if (!SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "1"))
    {
        printf("Warning: Linear filtering not enabled!");
    }

    return true;

}

void SDLclose()
{
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = NULL;
    gWindow = NULL;
}

void PPUInit(PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    PPU->OAM = new uint8_t[256];
    PPU->secondaryOAM = new uint8_t[32];
    PPU->VRAM = new uint8_t[2048];
    PPU->paletteRAM = new uint8_t[32];

    memset(PPU->OAM, 0xFF, 256);
    memset(PPU->secondaryOAM, 0xFF, 32);
    memset(PPU->VRAM, 0x00, 2048);
    memset(PPU->paletteRAM, 0x00, 32);

    PPU->spritesFound = 0;
    PPU->spriteNumber = 0;
    PPU->spriteByte = 0;
    PPU->spriteBus = 0;
    PPU->spriteInRange = false;
    PPU->dataBus = 0;

    PPU->flag = false;
    PPU->NMIflag = false;

    PPU->v = 0;
    PPU->t = 0;
    PPU->x = 0;
    PPU->w = false;
    PPU->evenFrame = true;
    PPU->flag = false;
    PPU->oldWrite = 0;

    PPU->tiles = new uint8_t[8192 * cartridgeData->CHRROMSize];

    for (int i = 0; i < 8192 * cartridgeData->CHRROMSize; ++i)
    {
        PPU->tiles[i] = readByte(cartridgeData, 16 + 16384 * cartridgeData->PRGROMSize + i);
    }

    PPU->frame = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    PPU->frameBuffer = new uint32_t[SCREEN_WIDTH * SCREEN_HEIGHT];

    PPU->cycleNo = 0;
    PPU->scanlineNo = 0;
    PPU->frameNo = 1;
}

void PPUDeallocate(PPUArchitechture* PPU)
{
    delete[] PPU->OAM;
    delete[] PPU->secondaryOAM;
    delete[] PPU->VRAM;
    delete[] PPU->paletteRAM;
    delete[] PPU->tiles;
    delete[] PPU->frameBuffer;
    SDL_DestroyTexture(PPU->frame);
}

void readPPUMemory(PPUArchitechture *PPU, CartridgeData *cartridgeData, uint16_t address)
{
    if (address <= 0x1FFF)
    {
        PPU->dataBus = PPU->tiles[address];
    }
    else if (address >= 0x2000 && address <= 0x3EFF)
    {
        address = 0x2000 + (address % 0x1000);

        if (cartridgeData->mirrorType == 0)
        {
            if (address <= 0x27FF)
            {
                PPU->dataBus = PPU->VRAM[address % 1024];
            }
            else
            {
                PPU->dataBus = PPU->VRAM[1024 + (address % 1024)];
            }
        }
        else if (cartridgeData->mirrorType == 1)
        {
            if (address <= 0x23FF || (address >= 0x2800 && address <= 0x2BFF))
            {
                PPU->dataBus = PPU->VRAM[address % 1024];
            }
            else
            {
                PPU->dataBus = PPU->VRAM[1024 + (address % 1024)];
            }
        }
    }
    else if (address >= 0x3F00 && address <= 0x3FFF)
    {
        address = address % 0x0020;

        if (address == 0x10 || address == 0x14 || address == 0x18 || address == 0x1C)
        {
            address -= 0x10;
        }

        PPU->dataBus = PPU->paletteRAM[address];
    }
}

void writePPUMemory(PPUArchitechture *PPU, CartridgeData *cartridgeData, uint16_t address, uint8_t data)
{
    if (address >= 0x2000 && address <= 0x3EFF)
    {
        address = 0x2000 + (address % 0x1000);

        if (cartridgeData->mirrorType == 0)
        {
            if (address <= 0x27FF)
            {
                PPU->VRAM[address % 1024] = data;
            }
            else
            {
                PPU->VRAM[1024 + (address % 1024)] = data;
            }
        }
        else if (cartridgeData->mirrorType == 1)
        {
            if (address <= 0x23FF || (address >= 0x2800 && address <= 0x2BFF))
            {
                PPU->VRAM[address % 1024] = data;
            }
            else
            {
                PPU->VRAM[1024 + (address % 1024)] = data;
            }
        }
    }
    else if (address >= 0x3F00 && address <= 0x3FFF)
    {
        address = address % 0x0020;

        if (address == 0x10 || address == 0x14 || address == 0x18 || address == 0x1C)
        {
            address -= 0x10;
        }

        PPU->paletteRAM[address] = data;
    }
}

void render(PPUArchitechture *PPU, ArchState *archState, CartridgeData *cartridgeData)
{
    if (PPU->scanlineNo == 0 && PPU->cycleNo == 0)
    {
        ++(PPU->frameNo);
    }

    if (PPU->scanlineNo >= 0 && PPU->scanlineNo <= 239 && (archState->memory->PPURegisters[PPUMASK] & 0x18))
    {
        if (PPU->cycleNo >= 1 && PPU->cycleNo <= 64)
        {
            PPU->spritesFound = 0;
            PPU->spriteNumber = 0;
            PPU->spriteByte = 0;
            PPU->spriteInRange = false;

            (PPU->cycleNo % 2) ? PPU->spriteBus = 0xFF : PPU->secondaryOAM[(PPU->cycleNo - 1) / 2] = 0xFF;
        }
        else if (PPU->cycleNo >= 65 && PPU->cycleNo <= 256 && PPU->spritesFound < 8)
        {
            uint8_t OAMAddress;

            OAMAddress = PPU->spriteNumber * 4 + PPU->spriteByte;
            PPU->spriteBus = PPU->OAM[OAMAddress];

            if (PPU->spriteNumber < 64 && PPU->spritesFound < 8)
            {
                (PPU->cycleNo % 2) ? PPU->spriteBus = PPU->OAM[OAMAddress] : PPU->secondaryOAM[PPU->spritesFound * 4 + PPU->spriteByte] = PPU->spriteBus;

                if (!(PPU->cycleNo % 2) && !PPU->spriteInRange)
                {
                    if (((PPU->scanlineNo - PPU->spriteBus) >= 0 && (PPU->scanlineNo - PPU->spriteBus) <= 7) ||
                    ((PPU->scanlineNo - PPU->spriteBus) >= 0 && (PPU->scanlineNo - PPU->spriteBus) <= 16
                     && (archState->memory->PPURegisters[PPUCTRL] & 0x20)))
                    {
                        PPU->spriteInRange = true;
                        PPU->isSpriteZero[PPU->spritesFound] = true;

                        ++PPU->spriteByte;
                    }
                    else
                    {
                        PPU->secondaryOAM[PPU->spritesFound * 4 + PPU->spriteByte] = 0xFF;
                        ++PPU->spriteNumber;
                    }
                }
                else if (!(PPU->cycleNo % 2) && PPU->spriteInRange)
                {
                    ++PPU->spriteByte;

                    if (PPU->spriteByte == 4)
                    {
                        PPU->spriteByte = 0;
                        ++PPU->spriteNumber;
                        ++PPU->spritesFound;
                        PPU->spriteInRange = false;
                    }
                }
            }
            else if (PPU->spriteNumber < 64 && PPU->spritesFound >= 8)
            {
                if (((PPU->scanlineNo - PPU->spriteBus) >= 0 && (PPU->scanlineNo - PPU->spriteBus) <= 7) ||
                    ((PPU->scanlineNo - PPU->spriteBus) >= 0 && (PPU->scanlineNo - PPU->spriteBus) <= 16
                     && (archState->memory->PPURegisters[PPUCTRL] & 0x20)))
                {
                    archState->memory->PPURegisters[PPUSTATUS] |= 0x20;
                    PPU->spriteInRange = true;
                }
                else
                {
                    ++PPU->spriteNumber;
                    ++PPU->spriteByte;

                    if (PPU->spriteByte == 4)
                    {
                        PPU->spriteByte = 0;
                    }
                }

                if (PPU->spriteInRange)
                {
                    ++PPU->spriteNumber;
                    ++PPU->spriteByte;

                    if (PPU->spriteByte == 4)
                    {
                        PPU->spriteByte = 0;
                        PPU->spriteInRange = false;
                    }
                }
            }
        }
        else if (PPU->cycleNo >= 257 && PPU->cycleNo <= 320)
        {
            if (PPU->cycleNo == 257)
            {
                PPU->spriteNumber = 0;
                PPU->spriteByte = 0;

                for (int i = 0; i < 8; ++i)
                {
                    PPU->spriteActive[i] = false;
                }
            }

            if (PPU->cycleNo % 8 != 6 && PPU->cycleNo % 8 != 0)
            {
                PPU->spriteBus = PPU->secondaryOAM[PPU->spriteNumber * 4 + PPU->spriteByte];
            }

            uint16_t address;

            if (archState->memory->PPURegisters[PPUCTRL] & 0x20)
            {
                address = 0x1000 * (PPU->spriteIndexNumber[PPU->spriteNumber] & 0x01);
                address += (((uint16_t) (PPU->spriteIndexNumber[PPU->spriteNumber] & 0x01)) << 4);
            }
            else
            {
                address = (archState->memory->PPURegisters[PPUCTRL] & 0x08) ? 0x1000 : 0x0000;
                address += (((uint16_t) PPU->spriteIndexNumber[PPU->spriteNumber]) << 4);
            }

            if (PPU->spriteAttributeRegisters[PPU->spriteNumber] & 0x80)
            {
                address += 8 - (PPU->scanlineNo - PPU->spriteYPos[PPU->spriteNumber] + 1);
            }
            else
            {
                address += PPU->scanlineNo - PPU->spriteYPos[PPU->spriteNumber];
            }

            switch (PPU->cycleNo % 8)
            {
                case 1:
                    PPU->spriteYPos[PPU->spriteNumber] = PPU->spriteBus;
                    ++PPU->spriteByte;
                    break;
                case 2:
                    PPU->spriteIndexNumber[PPU->spriteNumber] = PPU->spriteBus;
                    ++PPU->spriteByte;
                    break;
                case 3:
                    PPU->spriteAttributeRegisters[PPU->spriteNumber] = PPU->spriteBus;
                    ++PPU->spriteByte;
                    break;
                case 4:
                    PPU->spriteXCounters[PPU->spriteNumber] = PPU->spriteBus;
                    PPU->spriteByte = 0;
                    break;
                case 5:
                    readPPUMemory(PPU, cartridgeData, address);
                    PPU->spriteBus = PPU->dataBus;
                    break;
                case 6:
                    PPU->spritePatternRegisters[PPU->spriteNumber] = PPU->spriteBus;
                    break;
                case 7:
                    readPPUMemory(PPU, cartridgeData, address+8);
                    PPU->spriteBus = PPU->dataBus;
                    break;
                case 0:
                    PPU->spritePatternRegisters[PPU->spriteNumber + 8] = PPU->spriteBus;
                    ++PPU->spriteNumber;
                    break;
            }
        }
    }

    if (PPU->scanlineNo < 240 && (archState->memory->PPURegisters[PPUMASK] & 0x18) && PPU->cycleNo != 0)
    {
        if ((PPU->cycleNo > 0 && PPU->cycleNo < 257 && PPU->scanlineNo != -1))
        {
            uint8_t colourNumber =  (((PPU->tileRegister2 << PPU->x) & 0x8000) >> 14) | (((PPU->tileRegister1 << PPU->x) & 0x8000) >> 15);
            uint8_t paletteNumber = PPU->attributeRegister1;

            if (((PPU->v - 2) & 0x1F) % 4 >= 2)
            {
                paletteNumber >>= 2;
            }

            if ((((PPU->v - 2) >> 5) & 0x1F) % 4 >= 2)
            {
                paletteNumber >>= 4;
            }

            paletteNumber &= 0x03;
            uint32_t pixel = PPU->paletteRAM[paletteNumber * 4 + colourNumber];

            if (colourNumber == 0 || (!(archState->memory->PPURegisters[PPUMASK] & 0x02) && PPU->cycleNo <= 8))
            {
                pixel = PPU->paletteRAM[0];
            }

            uint8_t spritePixelColour;

            if(PPU->scanlineNo > 0)
            {
                for (int i = 7; i >= 0; --i)
                {
                    if (PPU->spriteXCounters[i] == 0)
                    {
                        PPU->spriteActive[i] = true;
                    }

                    if (PPU->spriteActive[i] && !((PPU->spriteAttributeRegisters[i] & 0x20) && colourNumber != 0) &&
                        (!(archState->memory->PPURegisters[PPUMASK] & 0x10) || ((archState->memory->PPURegisters[PPUMASK] & 0x10) && PPU->cycleNo > 8)))
                    {
                        if (PPU->spriteAttributeRegisters[i] & 0x40)
                        {
                            spritePixelColour = (PPU->spritePatternRegisters[i] & 0x01) | ((PPU->spritePatternRegisters[i+8] & 0x01) << 1);
                            PPU->spritePatternRegisters[i] >>= 1;
                            PPU->spritePatternRegisters[i+8] >>= 1;
                        }
                        else
                        {
                            spritePixelColour = ((PPU->spritePatternRegisters[i] & 0x80) >> 7) | ((PPU->spritePatternRegisters[i+8] & 0x80) >> 6);
                            PPU->spritePatternRegisters[i] <<= 1;
                            PPU->spritePatternRegisters[i+8] <<= 1;
                        }

                        if (spritePixelColour != 0 && PPU->spriteYPos[i] < 0xEF)
                        {
                            pixel = PPU->paletteRAM[(PPU->spriteAttributeRegisters[i] & 0x03) * 4 + spritePixelColour + 16];

                            if (PPU->isSpriteZero[i] && PPU->cycleNo != 256)
                            {
                                archState->memory->PPURegisters[PPUSTATUS] |= 0x40;
                            }
                        }
                    }

                    --(PPU->spriteXCounters[i]);

                    if (PPU->spriteXCounters[i] == 248)
                    {
                        PPU->spriteActive[i] = false;
                    }
                }
            }

            PPU->frameBuffer[PPU->scanlineNo * SCREEN_WIDTH + (PPU->cycleNo - 1)] = colours[pixel];
        }

        if (PPU->cycleNo < 329)
        {
            PPU->tileRegister1 <<= 1;
            PPU->tileRegister2 <<= 1;
        }

        switch (PPU->cycleNo % 8)
        {
            case 1:
            {
                uint16_t address = 0x2000 | (PPU->v & 0x0FFF);

                readPPUMemory(PPU, cartridgeData, address);
                break;
            }
            case 2:
                PPU->tempRegisters[0] = PPU->dataBus;
                break;
            case 3:
            {
                uint16_t address = 0x23C0 | (PPU->v & 0x0C00) | ((PPU->v >> 4) & 0x38) | ((PPU->v >> 2) & 0x07);

                readPPUMemory(PPU, cartridgeData, address);
                break;
            }
            case 4:
                PPU->tempRegisters[1] = PPU->dataBus;
                break;
            case 5:
            {
                uint16_t address = ((archState->memory->PPURegisters[PPUCTRL] >> 4) & 0x01) * 0x1000 | (PPU->tempRegisters[0] << 4) | (PPU->v >> 12);

                readPPUMemory(PPU, cartridgeData, address);
                break;
            }
            case 6:
                PPU->tempRegisters[2] = PPU->dataBus;
                break;
            case 7:
            {
                uint16_t address = ((archState->memory->PPURegisters[PPUCTRL] >> 4) & 0x01) * 0x1000 | (PPU->tempRegisters[0] << 4) | (PPU->v >> 12) + 8;

                readPPUMemory(PPU, cartridgeData, address);
                break;
            }
            case 0:
                PPU->tempRegisters[3] = PPU->dataBus;

                uint16_t tileByte1 = PPU->tempRegisters[2];
                uint16_t tileByte2 = PPU->tempRegisters[3];

                PPU->tileRegister1 &= 0xFF00;
                PPU->tileRegister2 &= 0xFF00;

                PPU->tileRegister1 |= tileByte1;
                PPU->tileRegister2 |= tileByte2;

                PPU->attributeRegister1 = PPU->attributeRegister2;
                PPU->attributeRegister2 = PPU->tempRegisters[1];

                if ((PPU->cycleNo > 0 && PPU->cycleNo < 257) || PPU->cycleNo >= 328)
                {
                    coarseXIncrement(PPU);
                }

                break;
        }

        if (PPU->cycleNo == 256)
        {
            YIncrement(PPU);
        }
        else if (PPU->cycleNo == 257)
        {
            PPU->v = (PPU->v & ~0x841F) | (PPU->t & 0x041F);
        }
        else if (PPU->cycleNo >= 280 && PPU->cycleNo <= 304 && PPU->scanlineNo == -1)
        {
            PPU->v = (PPU->v & ~0xFBE0) | (PPU->t & 0x7BE0);
        }

        if (PPU->cycleNo >= 257 && PPU->cycleNo <= 320)
        {
            archState->memory->PPURegisters[OAMADDR] = 0;
        }
    }

    ++PPU->cycleNo;

    if (PPU->cycleNo == 341 || (PPU->scanlineNo == -1 && PPU->cycleNo == 240 && !PPU->evenFrame && (archState->memory->PPURegisters[PPUMASK] & 0x18 == 0x18)))
    {
        PPU->cycleNo = 0;
        ++PPU->scanlineNo;

//        for (int i = 0; i < 960; ++i)
//        {
//            if (i % 32 == 0)
//            {
//                printf("\n");
//            }
//
//            printf("%02X ", PPU->VRAM[i]);
//        }
//
//        printf("\n");
    }

    if (PPU->scanlineNo == 261)
    {
        PPU->scanlineNo = -1;
        PPU->evenFrame = !PPU->evenFrame;
    }

    if (PPU->scanlineNo == -1 && PPU->cycleNo == 1)
    {
        archState->memory->PPURegisters[PPUSTATUS] &= 0x1F;
    }

    if (PPU->scanlineNo == 241 && PPU->cycleNo == 1)
    {
        SDL_UpdateTexture(PPU->frame, NULL, PPU->frameBuffer, SCREEN_WIDTH * sizeof(uint32_t));
        SDL_RenderCopy(gRenderer, PPU->frame, NULL, NULL);
        SDL_RenderPresent(gRenderer);
        archState->memory->PPURegisters[PPUSTATUS] |= 0x80;

        if ((archState->memory->PPURegisters[PPUCTRL] & 0x80) && (archState->memory->PPURegisters[PPUSTATUS] & 0x80))
        {
            PPU->NMIflag = true;
        }
    }
}
