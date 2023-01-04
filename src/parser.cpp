#include <fstream>
#include "processorArchitechture.h"

std::ifstream *openFile(char *fileName)
{
    std::ifstream *file = new std::ifstream(fileName, std::ios_base::openmode::_S_bin);

    return file;
}

void closeFile(std::ifstream *file)
{
    delete file;
}

void readHeader(CartridgeData *cartridgeData)
{
    char header[16];
    cartridgeData->file->seekg(0);

    for (int i = 0; i < 16; ++i)
    {
        header[i] = cartridgeData->file->get();
    }

    if (header[0] == 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A)
    {
        cartridgeData->fileType = 1;
    }
    else
    {
        cartridgeData->fileType = 0;
        return;
    }

    if ((header[7] & 0x0C) == 0x08)
    {
        cartridgeData->fileType = 2;
    }

    cartridgeData->PRGROMSize = header[4]; // 16 KB units
    cartridgeData->CHRROMSize = header[5]; // 8KB units
    cartridgeData->mirrorType = header[6] & 0x01; // If mirror type if 0, horizontal mirroring, if 1, vertical mirroring
    cartridgeData->containsBatteryRAM = header[6] & 0x02;
    cartridgeData->containsTrainer = header[6] & 0x04;
    cartridgeData->mirrorType = header[6] & 0x08 ? 2 : cartridgeData->mirrorType; // If mirror type is 2, provide 4-screen VRAM

    cartridgeData->mapperNo = (header[6] & 0xF0) >> 4;
    cartridgeData->mapperNo |= (header[7] & 0xF0);
}

uint8_t readByte(CartridgeData *cartridgeData, int fileOffset)
{
    cartridgeData->file->seekg(fileOffset);

    return cartridgeData->file->get();
}
