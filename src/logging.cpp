#include <fstream>
#include <SDL.h>
#include "processorExecution.h"
#include "processorArchitechture.h"
#include "memory.h"
#include "logging.h"

char *intToHexStr(uintmax_t x, int bytes)
{
    const char *digits = "0123456789ABCDEF";
    int length = bytes*2;

    char *hexStr = new char[length+1];

    for (int i = length-1; i >= 0; --i)
    {
        hexStr[i] = digits[x & 0x0F];
        x >>= 4;
    }

    hexStr[length] = 0;

    return hexStr;
}

void printLog(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData, FILE* log)
{
//    printf("PCL: 0x%02hhX\nPCH: 0x%02hhX\nIR: 0x%02hhX\nData Bus: 0x%02hhX\nADL: 0x%02hhX\nADH: 0x%02hhX\nX: 0x%02hhX\nY: 0x%02hhX\nA: 0x%02hhX/%hhu\n",
//           archState->PCL, archState->PCH, archState->IR, archState->dataBus, archState->ADL, archState->ADH, archState->X, archState->Y
//           ,archState->A, archState->A);
//
//    printf("Stack Ptr: 0x%02hhX\nStatus: 0x%02hhX\nTemp Reg: 0x%02hhX\nCycle: %d\nInstruction Number: %d\n\n",
//           archState->stackPtr, archState->status, archState->tempReg, archState->cycleNo, archState->instNo);

    PPU->flag = true;

    char *assembly = dissassemble(archState, PPU, cartridgeData);
    uint16_t address = make16Bit(archState->PCL, archState->PCH);

    int PPUCycles = archState->cycleNo * 3;
    int PPUCycles1 = PPUCycles / 341;
    int PPUCycles2 = PPUCycles % 341;

    printf("%04hX  %sA:%02hhX X:%02hhX Y:%02hhX P:%02hhX SP:%02hhX PPU:%2d,%3d,%3d CYC:%d\n", address,
           assembly, archState->A, archState->X, archState->Y, archState->status, archState->stackPtr,
           PPU->frameNo, PPU->scanlineNo, PPU->cycleNo, archState->cycleNo);

    fprintf(log, "%04hX  %sA:%02hhX X:%02hhX Y:%02hhX P:%02hhX SP:%02hhX PPU:%2d,%3d,%3d CYC:%d\n", address,
           assembly, archState->A, archState->X, archState->Y, archState->status, archState->stackPtr,
           PPU->frameNo, PPU->scanlineNo, PPU->cycleNo, archState->cycleNo);

    PPU->flag = false;

    delete[] assembly;
}

char *dissassemble(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    char *assembly = new char[43];
    uint8_t mnemonicPos = 10;

    memset(assembly, ' ', 42);
    assembly[42] = 0;

    uint8_t oldPCL = archState->PCL;
    uint8_t oldPCH = archState->PCH;

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;

    readData(archState, PPU, cartridgeData);
    uint8_t opcode = archState->dataBus;
    char *opcodeHex = intToHexStr(opcode, 1);
    //printf("%s\n\n", opcodeHex);

    incrementPC(archState);
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    uint8_t byte2 = archState->dataBus;
    char *byte2Hex = intToHexStr(byte2, 1);
    //printf("%s\n\n", byte2Hex);

    incrementPC(archState);
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    uint8_t byte3 = archState->dataBus;
    char *byte3Hex = intToHexStr(byte3, 1);
    //printf("%s\n\n", byte3Hex);

    archState->PCL = oldPCL;
    archState->PCH = oldPCH;

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;

    strncpy(assembly, opcodeHex, 2);

    bool defaultPicked = false;

    switch (opcode)
    {
        case BRK:
            strncpy(assembly+mnemonicPos, "BRK", 3);
            break;
        case RTI:
            strncpy(assembly+mnemonicPos, "RTI", 3);
            break;
        case RTS:
            strncpy(assembly+mnemonicPos, "RTS", 3);
            break;
        case PHP:
            strncpy(assembly+mnemonicPos, "PHP", 3);
            break;
        case PLP:
            strncpy(assembly+mnemonicPos, "PLP", 3);
            break;
        case PHA:
            strncpy(assembly+mnemonicPos, "PHA", 3);
            break;
        case PLA:
            strncpy(assembly+mnemonicPos, "PLA", 3);
            break;
        case DEY:
            strncpy(assembly+mnemonicPos, "DEY", 3);
            break;
        case TAY:
            strncpy(assembly+mnemonicPos, "TAY", 3);
            break;
        case INY:
            strncpy(assembly+mnemonicPos, "INY", 3);
            break;
        case INX:
            strncpy(assembly+mnemonicPos, "INX", 3);
            break;
        case CLC:
            strncpy(assembly+mnemonicPos, "CLC", 3);
            break;
        case SEC:
            strncpy(assembly+mnemonicPos, "SEC", 3);
            break;
        case CLI:
            strncpy(assembly+mnemonicPos, "CLI", 3);
            break;
        case SEI:
            strncpy(assembly+mnemonicPos, "SEI", 3);
            break;
        case TYA:
            strncpy(assembly+mnemonicPos, "TYA", 3);
            break;
        case CLV:
            strncpy(assembly+mnemonicPos, "CLV", 3);
            break;
        case CLD:
            strncpy(assembly+mnemonicPos, "CLD", 3);
            break;
        case SED:
            strncpy(assembly+mnemonicPos, "SED", 3);
            break;
        case TXA:
            strncpy(assembly+mnemonicPos, "TXA", 3);
            break;
        case TXS:
            strncpy(assembly+mnemonicPos, "TXS", 3);
            break;
        case TAX:
            strncpy(assembly+mnemonicPos, "TAX", 3);
            break;
        case TSX:
            strncpy(assembly+mnemonicPos, "TSX", 3);
            break;
        case DEX:
            strncpy(assembly+mnemonicPos, "DEX", 3);
            break;
        case NOP:
            strncpy(assembly+mnemonicPos, "NOP", 3);
            break;
        case JSR:
            strncpy(assembly+mnemonicPos, "JSR", 3);
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte3Hex, 2);
            strncpy(assembly+17, byte2Hex, 2);
            break;
        case JMP:
            strncpy(assembly+mnemonicPos, "JMP", 3);
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte3Hex, 2);
            strncpy(assembly+17, byte2Hex, 2);
            break;
        case INDIRECT_JMP:
        {
            strncpy(assembly+mnemonicPos, "JMP", 3);
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);

            assembly[14] = '(';
            assembly[15] = '$';

            strncpy(assembly+16, byte3Hex, 2);
            strncpy(assembly+18, byte2Hex, 2);

            assembly[20] = ')';
            assembly[22] = '=';

            archState->PCL = byte2;
            archState->PCH = byte3;

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            readData(archState, PPU, cartridgeData);
            uint8_t lowerJump = archState->dataBus;
            ++archState->PCL;

            archState->ADL = archState->PCL;
            readData(archState, PPU, cartridgeData);
            uint8_t upperJump = archState->dataBus;

            uint16_t jumpAddress = make16Bit(lowerJump, upperJump);
            char *jumpAddressHex = intToHexStr(jumpAddress, 2);
            strncpy(assembly+24, jumpAddressHex, 4);

            archState->PCL = oldPCL;
            archState->PCH = oldPCH;

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] jumpAddressHex;

            break;
        }
        default:
            defaultPicked = true;
            break;
    }

    if (!defaultPicked)
    {
        return assembly;
    }

    if ((opcode & 0x1F) == 0x10)
    {
        switch (opcode)
        {
            case BPL:
                strncpy(assembly+mnemonicPos, "BPL", 3);
                break;
            case BMI:
                strncpy(assembly+mnemonicPos, "BMI", 3);
                break;
            case BVC:
                strncpy(assembly+mnemonicPos, "BVC", 3);
                break;
            case BVS:
                strncpy(assembly+mnemonicPos, "BVS", 3);
                break;
            case BCC:
                strncpy(assembly+mnemonicPos, "BCC", 3);
                break;
            case BCS:
                strncpy(assembly+mnemonicPos, "BCS", 3);
                break;
            case BNE:
                strncpy(assembly+mnemonicPos, "BNE", 3);
                break;
            case BEQ:
                strncpy(assembly+mnemonicPos, "BEQ", 3);
                break;
        }

        strncpy(assembly+3, byte2Hex, 2);

        uint16_t address = make16Bit(archState->PCL, archState->PCH);
        address += byte2 + 2;
        char *addressHex = intToHexStr(address, 2);

        assembly[14] = '$';
        strncpy(assembly+15, addressHex, 4);

        archState->PCL = oldPCL;
        archState->PCH = oldPCH;

        archState->ADL = archState->PCL;
        archState->ADH = archState->PCH;

        delete[] addressHex;

        return assembly;
    }

    switch (opcode & 0xE3)
    {
        case ORA:
            strncpy(assembly+mnemonicPos, "ORA", 3);
            break;
        case AND:
            strncpy(assembly+mnemonicPos, "AND", 3);
            break;
        case EOR:
            strncpy(assembly+mnemonicPos, "EOR", 3);
            break;
        case ADC:
            strncpy(assembly+mnemonicPos, "ADC", 3);
            break;
        case STA:
            strncpy(assembly+mnemonicPos, "STA", 3);
            break;
        case LDA:
            strncpy(assembly+mnemonicPos, "LDA", 3);
            break;
        case CMP:
            strncpy(assembly+mnemonicPos, "CMP", 3);
            break;
        case SBC:
            strncpy(assembly+mnemonicPos, "SBC", 3);
            break;
        case ASL:
            strncpy(assembly+mnemonicPos, "ASL", 3);
            break;
        case ROL:
            strncpy(assembly+mnemonicPos, "ROL", 3);
            break;
        case LSR:
            strncpy(assembly+mnemonicPos, "LSR", 3);
            break;
        case ROR:
            strncpy(assembly+mnemonicPos, "ROR", 3);
            break;
        case STX:
            strncpy(assembly+mnemonicPos, "STX", 3);
            break;
        case LDX:
            strncpy(assembly+mnemonicPos, "LDX", 3);
            break;
        case DEC:
            strncpy(assembly+mnemonicPos, "DEC", 3);
            break;
        case INC:
            strncpy(assembly+mnemonicPos, "INC", 3);
            break;
        case BIT:
            strncpy(assembly+mnemonicPos, "BIT", 3);
            break;
        case STY:
            strncpy(assembly+mnemonicPos, "STY", 3);
            break;
        case LDY:
            strncpy(assembly+mnemonicPos, "LDY", 3);
            break;
        case CPY:
            strncpy(assembly+mnemonicPos, "CPY", 3);
            break;
        case CPX:
            strncpy(assembly+mnemonicPos, "CPX", 3);
            break;
    }

    if (((opcode & 0x1F) == 0x09) || ((opcode & 0x1F) == 0x02) || ((opcode & 0x1F) == 0x00))
    {
        strncpy(assembly+3, byte2Hex, 2);
        assembly[14] = '#';
        assembly[15] = '$';
        strncpy(assembly+16, byte2Hex, 2);

        return assembly;
    }

    if ((opcode & 0x1F) == 0x0A)
    {
        assembly[14] = 'A';

        return assembly;
    }

    switch (opcode & 0x1C)
    {
        case ZERO_PAGE_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte2Hex, 2);
            assembly[18] = '=';

            archState->PCL = byte2;
            archState->PCH = 0x00;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+20, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] dataHex;
            break;
        }
        case ABSOLUTE_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte3Hex, 2);
            strncpy(assembly+17, byte2Hex, 2);
            assembly[20] = '=';

            archState->PCL = byte2;
            archState->PCH = byte3;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+22, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] dataHex;
            break;
        }
        case ZERO_PAGE_X_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte2Hex, 2);
            assembly[17] = ',';
            assembly[18] = 'X';
            assembly[20] = '@';

            if ((opcode & ~ADDRESSING_MODE_FLAG) != STX && (opcode & ~ADDRESSING_MODE_FLAG) != LDX)
            {
                archState->PCL = byte2 + archState->X;
            }
            else
            {
                archState->PCL = byte2 + archState->Y;
                assembly[18] = 'Y';
            }

            archState->PCH = 0x00;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            char *addressHex = intToHexStr(archState->PCL, 1);
            strncpy(assembly+22, addressHex, 2);
            readData(archState, PPU, cartridgeData);

            assembly[25] = '=';

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+27, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] addressHex;
            delete[] dataHex;
            break;
        }
        case ABSOLUTE_X_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte3Hex, 2);
            strncpy(assembly+17, byte2Hex, 2);
            assembly[19] = ',';
            assembly[20] = 'X';

            archState->PCH = byte3;

            if ((opcode & ~ADDRESSING_MODE_FLAG) != STX && (opcode & ~ADDRESSING_MODE_FLAG) != LDX)
            {
                archState->PCL = byte2 + archState->X;

                if (overflow(byte2, archState->X))
                {
                    ++archState->PCH;
                }
            }
            else
            {
                archState->PCL = byte2 + archState->Y;

                if (overflow(byte2, archState->Y))
                {
                    ++archState->PCH;
                }

                assembly[20] = 'Y';
            }

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            assembly[22] = '@';

            uint16_t address = make16Bit(archState->PCL, archState->PCH);
            char *addressHex = intToHexStr(address, 2);
            strncpy(assembly+24, addressHex, 4);
            assembly[29] = '=';

            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+31, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] addressHex;
            delete[] dataHex;
            break;
        }
        case ABSOLUTE_Y_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            strncpy(assembly+6, byte3Hex, 2);
            assembly[14] = '$';
            strncpy(assembly+15, byte3Hex, 2);
            strncpy(assembly+17, byte2Hex, 2);
            assembly[19] = ',';
            assembly[20] = 'Y';

            archState->PCL = byte2 + archState->Y;
            archState->PCH = byte3;

            if (overflow(byte2, archState->Y))
            {
                ++archState->PCH;
            }

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            assembly[22] = '@';

            uint16_t address = make16Bit(archState->PCL, archState->PCH);
            char *addressHex = intToHexStr(address, 2);
            strncpy(assembly+24, addressHex, 4);
            assembly[29] = '=';

            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+31, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] addressHex;
            delete[] dataHex;
            break;
        }
        case INDIRECT_X_ADDRESSING:
        {
            strncpy(assembly+3, byte2Hex, 2);
            assembly[14] = '(';
            assembly[15] = '$';
            strncpy(assembly+16, byte2Hex, 2);
            assembly[18] = ',';
            assembly[19] = 'X';
            assembly[20] = ')';
            assembly[22] = '@';


            archState->PCL = byte2 + archState->X;
            archState->PCH = 0x00;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            char *baseAddressHex = intToHexStr(archState->PCL, 1);
            strncpy(assembly+24, baseAddressHex, 2);
            assembly[27] = '=';

            readData(archState, PPU, cartridgeData);
            uint8_t addressLow = archState->dataBus;
            ++archState->PCL;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);
            uint8_t addressHigh = archState->dataBus;
            uint16_t address = make16Bit(addressLow, addressHigh);
            char *addressHex = intToHexStr(address, 2);
            strncpy(assembly+29, addressHex, 4);

            assembly[34] = '=';

            archState->PCL = addressLow;
            archState->PCH = addressHigh;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+36, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] baseAddressHex;
            delete[] addressHex;
            delete[] dataHex;
            break;
        }
        case INDIRECT_Y_ADDRESSING:
            strncpy(assembly+3, byte2Hex, 2);
            assembly[14] = '(';
            assembly[15] = '$';
            strncpy(assembly+16, byte2Hex, 2);
            assembly[18] = ')';
            assembly[19] = ',';
            assembly[20] = 'Y';
            assembly[22] = '=';

            archState->PCL = byte2;
            archState->PCH = 0x00;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);
            uint8_t addressLow = archState->dataBus;
            ++archState->PCL;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);
            uint8_t addressHigh = archState->dataBus;
            uint16_t address = make16Bit(addressLow, addressHigh);
            char *addressHex = intToHexStr(address, 2);
            strncpy(assembly+24, addressHex, 4);
            assembly[29] = '@';

            archState->PCL = addressLow + archState->Y;
            archState->PCH = addressHigh;

            if (overflow(addressLow, archState->Y))
            {
                ++archState->PCH;
            }

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            address += archState->Y;
            addressHex = intToHexStr(address, 2);

            strncpy(assembly+31, addressHex, 4);

            assembly[36] = '=';

            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);

            uint8_t data = archState->dataBus;
            char *dataHex = intToHexStr(data, 1);
            strncpy(assembly+38, dataHex, 2);
            archState->PCL = oldPCL;
            archState->PCH = oldPCH;
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;

            delete[] addressHex;
            delete[] dataHex;
            break;
    }

    delete[] opcodeHex;
    delete[] byte2Hex;
    delete[] byte3Hex;

    return assembly;
}
