#ifndef PROCESSOR_ARCHITECHTURE_H
#define PROCESSOR_ARCHITECHTURE_H

#include <stdint.h>
#include <string.h>
#include <fstream>

typedef struct
{
    uint8_t *WRAM;
    uint8_t *PPURegisters;
    uint8_t *APUIORegisters;
    uint8_t *cartridgeMemory;
} MemoryMap;

typedef struct
{
    uint8_t PCL;
    uint8_t PCH;
    uint8_t IR;
    uint8_t dataBus;
    uint8_t ADL;
    uint8_t ADH;
    uint8_t X;
    uint8_t Y;
    uint8_t A;
    uint8_t stackPtr;
    uint8_t tempReg;
    uint8_t status;
    uint8_t instCycleState;
    MemoryMap *memory;

    uint8_t controllerBuffer;

    int instNo;
    int cycleNo;
} ArchState;

typedef struct
{
    std::ifstream *file;

    uint8_t fileType; // 0 for invalid, 1 for iNES, 2 for NES 2.0
    uint8_t mirrorType; // 0 for horizontal, 1 for vertical, 8 for four-screen
    int mapperNo;
    int PRGROMSize;
    int CHRROMSize;
    bool containsBatteryRAM;
    bool containsTrainer;
} CartridgeData;

const uint8_t PPUCTRL   = 0;
const uint8_t PPUMASK   = 1;
const uint8_t PPUSTATUS = 2;
const uint8_t OAMADDR   = 3;
const uint8_t OAMDATA   = 4;
const uint8_t PPUSCROLL = 5;
const uint8_t PPUADDR   = 6;
const uint8_t PPUDATA   = 7;

const uint8_t SQ1_VOL = 0;
const uint8_t SQ1_SWEEP = 1;
const uint8_t SQ1_LO = 2;
const uint8_t SQ1_HI = 3;
const uint8_t SQ2_VOL = 4;
const uint8_t SQ2_SWEEP = 5;
const uint8_t SQ2_LO = 6;
const uint8_t SQ2_HI = 7;
const uint8_t TRI_LINEAR = 8;
const uint8_t TRI_LO = 10;
const uint8_t TRI_HI = 11;
const uint8_t NOISE_VOL = 12;
const uint8_t NOISE_LO = 14;
const uint8_t NOISE_HI = 15;
const uint8_t DMC_FREQ = 16;
const uint8_t DMC_RAW = 17;
const uint8_t DMC_START = 18;
const uint8_t DMC_LEN = 19;
const uint8_t OAMDMA = 20;
const uint8_t SND_CHN = 21;
const uint8_t JOY1 = 22;
const uint8_t JOY2 = 23;


const uint8_t CARRY_FLAG     = 0x01;
const uint8_t ZERO_FLAG      = 0x02;
const uint8_t INTERRUPT_FLAG = 0x04;
const uint8_t DECIMAL_FLAG   = 0x08;
const uint8_t BREAK_FLAG     = 0x10;
const uint8_t UNUSED_FLAG    = 0x20;
const uint8_t OVERFLOW_FLAG  = 0x40;
const uint8_t NEGATIVE_FLAG  = 0x80;

const uint8_t ADDRESSING_MODE_FLAG   = 0x1C;
const uint8_t ZERO_PAGE_ADDRESSING   = 0x04;
const uint8_t ABSOLUTE_ADDRESSING    = 0x0C;
const uint8_t ZERO_PAGE_X_ADDRESSING = 0x14;
const uint8_t ABSOLUTE_X_ADDRESSING  = 0x1C;
const uint8_t ABSOLUTE_Y_ADDRESSING  = 0x18;
const uint8_t INDIRECT_X_ADDRESSING  = 0x00;
const uint8_t INDIRECT_Y_ADDRESSING  = 0x10;

const uint8_t ORA = 0x01;
const uint8_t AND = 0x21;
const uint8_t BIT = 0x20;
const uint8_t EOR = 0x41;
const uint8_t ADC = 0x61;
const uint8_t STA = 0x81;
const uint8_t STX = 0x82;
const uint8_t STY = 0x80;
const uint8_t LDA = 0xA1;
const uint8_t LDX = 0xA2;
const uint8_t LDY = 0xA0;
const uint8_t CMP = 0xC1;
const uint8_t CPX = 0xE0;
const uint8_t CPY = 0xC0;
const uint8_t SBC = 0xE1;
const uint8_t DEC = 0xC2;
const uint8_t INC = 0xE2;
const uint8_t ASL = 0x02;
const uint8_t LSR = 0x42;
const uint8_t ROL = 0x22;
const uint8_t ROR = 0x62;

const uint8_t IMMEDIATE_ORA = 0x09;
const uint8_t IMMEDIATE_AND = 0x29;
const uint8_t IMMEDIATE_EOR = 0x49;
const uint8_t IMMEDIATE_ADC = 0x69;
const uint8_t IMMEDIATE_LDA = 0xA9;
const uint8_t IMMEDIATE_LDX = 0xA2;
const uint8_t IMMEDIATE_LDY = 0xA0;
const uint8_t IMMEDIATE_CMP = 0xC9;
const uint8_t IMMEDIATE_CPX = 0xE0;
const uint8_t IMMEDIATE_CPY = 0xC0;
const uint8_t IMMEDIATE_SBC = 0xE9;

const uint8_t ACCUMULATOR_ASL = 0x0A;
const uint8_t ACCUMULATOR_ROL = 0x2A;
const uint8_t ACCUMULATOR_LSR = 0x4A;
const uint8_t ACCUMULATOR_ROR = 0x6A;

const uint8_t JMP = 0x4C;
const uint8_t INDIRECT_JMP = 0x6C;

const uint8_t BRANCH_FLAG =  0x10;
const uint8_t BPL = 0x10;
const uint8_t BMI = 0x30;
const uint8_t BVC = 0x50;
const uint8_t BVS = 0x70;
const uint8_t BCC = 0x90;
const uint8_t BCS = 0xB0;
const uint8_t BNE = 0xD0;
const uint8_t BEQ = 0xF0;

const uint8_t BRK = 0x00;
const uint8_t JSR = 0x20;
const uint8_t RTI = 0x40;
const uint8_t RTS = 0x60;

const uint8_t PHP = 0x08;
const uint8_t CLC = 0x18;
const uint8_t PLP = 0x28;
const uint8_t SEC = 0x38;
const uint8_t PHA = 0x48;
const uint8_t CLI = 0x58;
const uint8_t PLA = 0x68;
const uint8_t SEI = 0x78;
const uint8_t DEY = 0x88;
const uint8_t TYA = 0x98;
const uint8_t TAY = 0xA8;
const uint8_t CLV = 0xB8;
const uint8_t INY = 0xC8;
const uint8_t CLD = 0xD8;
const uint8_t INX = 0xE8;
const uint8_t SED = 0xF8;

const uint8_t TXA = 0x8A;
const uint8_t TXS = 0x9A;
const uint8_t TAX = 0xAA;
const uint8_t TSX = 0xBA;
const uint8_t DEX = 0xCA;
const uint8_t NOP = 0xEA;

const uint8_t RST = 0;
const uint8_t IRQ = 1;
const uint8_t B   = 2;
const uint8_t NMI = 3;

const int colours[] = {0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400, 0x503000, 0x007800,
0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000, 0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058,
0xF83800, 0xE45C10, 0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000, 0xF8F8F8, 0x3CBCFC,
0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044, 0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878,
0x000000, 0x000000, 0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8, 0xF8D878, 0xD8F878,
0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000};

#endif
