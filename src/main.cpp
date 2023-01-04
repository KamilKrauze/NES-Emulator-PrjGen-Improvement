#include <iostream>
#include "processorArchitechture.h"
#include "processorExecution.h"
#include "opcodeFunction.h"
#include "memory.h"
#include "rendering.h"
#include "controls.h"
#include "parser.h"
#include "logging.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Incorrect number of arguments");

        return 1;
    }

    if (!SDLinit())
    {
        printf("Error initialising SDL\n");

        return -1;
    }

    char *fileName = argv[1];

    ArchState *archState = new ArchState;
    PPUArchitechture *PPU = new PPUArchitechture;
    CartridgeData *cartridgeData = new CartridgeData;
    cartridgeData->file = openFile(fileName);
    readHeader(cartridgeData);

    initProcessor(archState);
    memoryInit(archState, cartridgeData);
    PPUInit(PPU, cartridgeData);
    reset(archState, PPU, cartridgeData);

    FILE *log;
    log = fopen("mynestest.log", "w");

    bool flag = false;
    bool quit = false;

    SDL_Event e;

    uint32_t startTime = SDL_GetTicks();

    while (!quit)
    {
        // Donkey Kong NB change between cycle 12536410 and 15871248
        // DK NT change at frame 534, around CPU cycle 15871200
        // DK address 0334 == 4 at cycle 15843586
        // DK address 002E == 49 at cycle 15842839

        if (archState->cycleNo >= 15842600)
        {
            flag = false;
        }

//        if (archState->PCH == 0xF3 && archState->PCL == 0xA7 && archState->X == 0x08)
//        {
//            flag = true;
//        }

        if (archState->memory->APUIORegisters[JOY1] & 0x01)
        {
            readStandardController(archState);
        }

        if (!PPU->NMIflag || archState->instCycleState != 0)
        {
            render(PPU, archState, cartridgeData);
            render(PPU, archState, cartridgeData);
            render(PPU, archState, cartridgeData);
        }

        switch (archState->instCycleState)
        {
            case 0:
                if (flag)
                {
                    archState->ADL = archState->PCL;
                    archState->ADH = archState->PCH;
                    readData(archState, PPU, cartridgeData);
                    archState->IR = archState->dataBus;
                    printLog(archState, PPU, cartridgeData, log);

                    SDL_Delay(100);
                }

                if (PPU->NMIflag)
                {
                    _NMI(archState, PPU, cartridgeData);
                    PPU->NMIflag = false;
                }
                else
                {
                    cycle0(archState, PPU, cartridgeData);
                }
                break;
            case 1:
                cycle1(archState, PPU, cartridgeData);
                break;
            case 2:
                cycle2(archState, PPU, cartridgeData);
                break;
            case 3:
                cycle3(archState, PPU, cartridgeData);
                break;
            case 4:
                cycle4(archState, PPU, cartridgeData);
                break;
            case 5:
                cycle5(archState, PPU, cartridgeData);
                break;
            case 6:
                cycle6(archState, PPU, cartridgeData);
                break;
        }

        uint32_t endTime = SDL_GetTicks();

        if (endTime - startTime >= 16)
        {
            SDL_PollEvent(&e);

            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            startTime = SDL_GetTicks();
        }
    }

    fclose(log);
    closeFile(cartridgeData->file);
    delete cartridgeData;
    delete PPU;
    deallocate(archState);
    SDLclose();

    return 0;
}
