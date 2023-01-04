#include "processorArchitechture.h"
#include "parser.h"
#include "PPUArchitechture.h"
#include "rendering.h"

uint16_t make16Bit(uint8_t lowerBits, uint8_t upperBits)
{
    return (uint16_t) lowerBits | (uint16_t) upperBits << 8;
}

void memoryInit(ArchState *archState, CartridgeData *cartridgeData)
{
    archState->memory = new MemoryMap;

    archState->memory->WRAM = new uint8_t[2048];
    memset(archState->memory->WRAM, 0, 2048);

    archState->memory->PPURegisters = new uint8_t[8];
    memset(archState->memory->PPURegisters, 0, 8);

    archState->memory->APUIORegisters = new uint8_t[32];
    memset(archState->memory->APUIORegisters, 0, 32);

    archState->memory->cartridgeMemory = new uint8_t[16384 * cartridgeData->PRGROMSize];

    for (int i = 0; i < 16384 * cartridgeData->PRGROMSize; ++i)
    {
        archState->memory->cartridgeMemory[i] = readByte(cartridgeData, i + 16);
    }
}

void deallocate(ArchState *archState)
{
    delete[] archState->memory->APUIORegisters;
    delete[] archState->memory->PPURegisters;
    delete[] archState->memory->WRAM;
    delete[] archState->memory->cartridgeMemory;
    delete archState->memory;

    delete archState;
}

void coarseXIncrement(PPUArchitechture *PPU)
{
    if ((PPU->v & 0x001F) == 31)
    {
        PPU->v &= ~0x001F;
        PPU->v ^= 0x0400;
    }
    else
    {
        PPU->v += 1;
    }
}

void YIncrement(PPUArchitechture *PPU)
{
    if ((PPU->v & 0x7000) != 0x7000)
    {
        PPU->v += 0x1000;
    }
    else
    {
        PPU->v &= ~0x7000;
        int y = (PPU->v & 0x03E0) >> 5;

        if (y == 29)
        {
            y = 0;
            PPU->v ^= 0x0800;
        }
        else if (y == 31)
        {
            y = 0;
        }
        else
        {
            y += 1;
        }

        PPU->v = (PPU->v & ~0x03E0) | (y << 5);
    }
}

void readData(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    uint16_t address = make16Bit(archState->ADL, archState->ADH);

    if (address >= 0x0000 && address < 0x2000)
    {
        archState->dataBus = archState->memory->WRAM[address % 2048];
    }
    else if (address >= 0x2000 && address < 0x4000)
    {
        archState->dataBus = archState->memory->PPURegisters[address % 8];

        switch (address % 8)
        {
            case 2:
                //printf("\n%02X  %d\n", archState->memory->PPURegisters[address % 8], PPU->cycleNo);
                PPU->w = false;

                if (!PPU->flag)
                {
                    archState->memory->PPURegisters[address % 8] &= 0x7F;
                }

                if (PPU->cycleNo > 0 && PPU->cycleNo < 4)
                {
                    //printf("\n%d  %02X  %02X\n", PPU->cycleNo, archState->dataBus, archState->dataBus & 0x7F);
                    archState->dataBus &= 0x7F;
                }

                break;
            case 4:
                break;
            case 7:
                //printf("\n%04X  %d    %02X     r\n\n", address, archState->instCycleState, archState->IR);

                if (!PPU->flag)
                {
                    readPPUMemory(PPU, cartridgeData, PPU->v);
                    archState->memory->PPURegisters[address % 8] = PPU->dataBus;
                }

                if (!(PPU->flag))
                {
                    //printf("\n%04X   %d     %02X    %d   r\n\n", PPU->v, archState->instCycleState, archState->IR, archState->cycleNo);

                    if (PPU->scanlineNo < 240 && archState->memory->PPURegisters[PPUMASK] & 0x18)
                    {
                        coarseXIncrement(PPU);
                        YIncrement(PPU);
                    }
                    else
                    {
                        PPU->v += (0x04 & archState->memory->PPURegisters[PPUCTRL]) ? 32 : 1;
                    }
                }

                break;
        }
    }
    else if (address >= 0x4000 && address < 0x4020)
    {
        if (address - 0x4000 == JOY1)
        {
            archState->dataBus &= ~0x01;
            archState->dataBus |= archState->controllerBuffer & 0x01;
            archState->controllerBuffer = (archState->controllerBuffer >> 1);
        }
        else if (address - 0x4000 != OAMDMA)
        {
            archState->dataBus = archState->memory->APUIORegisters[address - 0x4000];
        }
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        // Read from cartirdge
        if (cartridgeData->PRGROMSize == 1 && address >= 0xC000)
        {
            address -= 0x4000;
        }

        address -= 0x8000;

        archState->dataBus = archState->memory->cartridgeMemory[address];
    }
}

void writeData(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    uint16_t address = make16Bit(archState->ADL, archState->ADH);
    PPU->oldWrite = archState->dataBus;

    if (address >= 0x0000 && address < 0x2000)
    {
        archState->memory->WRAM[address % 2048] = archState->dataBus;
    }
    else if (address >= 0x2000 && address < 0x4000)
    {
        archState->memory->PPURegisters[address % 8] = archState->dataBus;
        uint16_t data = archState->memory->PPURegisters[address % 8];

        archState->memory->PPURegisters[PPUSTATUS] = (archState->memory->PPURegisters[PPUSTATUS] & 0xE0) | (archState->dataBus & ~0xE0);

        switch (address % 8)
        {
            case 0:
                data &= 0x03;
                data <<= 10;
                PPU->t &= 0xF3FF;
                PPU->t |=  data;
                break;
            case 1:
                break;
            case 3:
                break;
            case 4:
                PPU->OAM[archState->memory->PPURegisters[OAMADDR]] = data;
                ++(archState->memory->PPURegisters[OAMADDR]);

                break;
            case 5:
                if (!PPU->w)
                {
                    PPU->x |= data & 0x07;
                    data >>= 3;
                    PPU->t &= 0xFFE0;
                    PPU->t |= data;
                    PPU->w = true;
                }
                else
                {
                    uint16_t part1 = data & 0x07;
                    uint16_t part2 = data & 0xF8;

                    part1 <<= 12;
                    part2 <<= 2;

                    PPU->t &= 0x0C1F;
                    PPU->t |= part1 | part2;

                    PPU->w = false;
                }

                break;
            case 6:
                if (!PPU->w)
                {
                    data &= 0x3F;
                    data <<= 8;
                    PPU->t &= 0x00FF;
                    PPU->t |= data;
                    PPU->w = true;
                }
                else
                {
                    PPU->t &= 0xFF00;
                    PPU->t |= data;
                    PPU->v = PPU->t;
                    PPU->w = false;
                }

                break;
            case 7:
                writePPUMemory(PPU, cartridgeData, PPU->v, archState->memory->PPURegisters[address % 8]);

                if (!(PPU->flag))
                {
                    if (PPU->scanlineNo < 240 && archState->memory->PPURegisters[PPUMASK] & 0x18)
                    {
                        coarseXIncrement(PPU);
                        YIncrement(PPU);
                    }
                    else
                    {
                        PPU->v += (0x04 & archState->memory->PPURegisters[PPUCTRL]) ? 32 : 1;
                    }
                }

                break;
        }
    }
    else if (address >= 0x4000 && address < 0x4020)
    {
        archState->memory->APUIORegisters[address - 0x4000] = archState->dataBus;

        switch (address % 32)
        {
            case OAMDMA:
                int OAMcycles = (archState->cycleNo % 2) ? 514 : 513;

                uint8_t oldADL = archState->ADL;
                uint8_t oldADH = archState->ADH;
                uint8_t oldData = archState->dataBus;

                archState->ADL = archState->memory->PPURegisters[OAMADDR];
                archState->ADH = archState->memory->APUIORegisters[OAMDMA];

                for (int i = 0; i < OAMcycles; ++i)
                {
                    render(PPU, archState, cartridgeData);
                    render(PPU, archState, cartridgeData);
                    render(PPU, archState, cartridgeData);

                    if (!((OAMcycles == 514 && i == 0) || (OAMcycles - i == 1)))
                    {
                        if ((archState->cycleNo % 2) == 0)
                        {
                            readData(archState, PPU, cartridgeData);
                        }
                        else
                        {
                            PPU->OAM[archState->memory->PPURegisters[OAMADDR]] = archState->dataBus;
                            ++(archState->memory->PPURegisters[OAMADDR]);
                            ++(archState->ADL);
                        }
                    }

                    ++archState->cycleNo;
                }

                archState->ADL = oldADL;
                archState->ADH = oldADH;
                archState->dataBus = oldData;
        }
    }
}
