#include <stdio.h>
#include "processorArchitechture.h"
#include "processorExecution.h"
#include "opcodeFunction.h"
#include "memory.h"
#include "rendering.h"

void initProcessor(ArchState *archState)
{
    archState->X = 0;
    archState->Y = 0;
    archState->A = 0;
    archState->status = 0x04;
    archState->stackPtr = 0xFD;
    archState->instCycleState = 0;
    archState->instNo = 0;
    archState->controllerBuffer = 0;

    archState->PCL = 0;
    archState->PCH = 0;
}

void incrementPC(ArchState *archState)
{
    ++(archState->PCL);

    if (overflow((archState->PCL)-1, 1))
    {
        ++(archState->PCH);
    }
}

bool overflow(uint8_t x, uint8_t add)
{
    return UINT8_MAX - x < add;
}

bool wrongSign(int8_t x, int8_t add)
{
    return (x > 0 && add > INT8_MAX - x) || (x < 0 && add < INT8_MIN - x);
}

void reset(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->cycleNo = 0;

    archState->ADL = 0;
    archState->ADH = 0;
    readData(archState, PPU, cartridgeData);

    ++(archState->ADL);
    readData(archState, PPU, cartridgeData);

    archState->ADH = 0x01;
    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);

    --(archState->ADL);
    readData(archState, PPU, cartridgeData);

    --(archState->ADL);
    readData(archState, PPU, cartridgeData);

    archState->ADH = 0xFF;
    archState->ADL = 0xFC;
    readData(archState, PPU, cartridgeData);

    ++(archState->ADL);
    archState->tempReg = archState->dataBus;
    readData(archState, PPU, cartridgeData);

    archState->PCL = archState->tempReg;
    archState->PCH = archState->dataBus;
}

void _NMI(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    readData(archState, PPU, cartridgeData);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    readData(archState, PPU, cartridgeData);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    archState->ADH = 0x01;
    archState->ADL = archState->stackPtr;
    archState->dataBus = archState->PCH;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    archState->ADL = archState->stackPtr;
    archState->dataBus = archState->PCL;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    archState->ADL = archState->stackPtr;
    archState->dataBus = archState->status;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    archState->ADH = 0xFF;
    archState->ADL = 0xFA;
    readData(archState, PPU, cartridgeData);
    ++(archState->cycleNo);

    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);
    render(PPU, archState, cartridgeData);

    ++(archState->ADL);
    archState->tempReg = archState->dataBus;
    readData(archState, PPU, cartridgeData);
    ++(archState->cycleNo);

    archState->PCL = archState->tempReg;
    archState->PCH = archState->dataBus;
}

void cycle0(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instNo);
    ++(archState->instCycleState);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;

    readData(archState, PPU, cartridgeData);
    archState->IR = archState->dataBus;

    incrementPC(archState);
    ++(archState->cycleNo);
}

void cycle1(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;

    readData(archState, PPU, cartridgeData);

    switch (archState->IR)
    {
        case IMMEDIATE_ORA:
            _ORA(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_AND:
            _AND(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_EOR:
            _EOR(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_ADC:
            _ADC(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_SBC:
            _SBC(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_CMP:
            _CMP(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_CPX:
            _CPX(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_CPY:
            _CPY(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_LDA:
            _LDA(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_LDX:
            _LDX(archState);
            incrementPC(archState);
            break;
        case IMMEDIATE_LDY:
            _LDY(archState);
            incrementPC(archState);
            break;
        case ACCUMULATOR_LSR:
            archState->tempReg = archState->A;
            _LSR(archState);
            archState->A = archState->tempReg;
            archState->instCycleState = 0;
            break;
        case ACCUMULATOR_ASL:
            archState->tempReg = archState->A;
            _ASL(archState);
            archState->A = archState->tempReg;
            archState->instCycleState = 0;
            break;
        case ACCUMULATOR_ROL:
            archState->tempReg = archState->A;
            _ROL(archState);
            archState->A = archState->tempReg;
            archState->instCycleState = 0;
            break;
        case ACCUMULATOR_ROR:
            archState->tempReg = archState->A;
            _ROR(archState);
            archState->A = archState->tempReg;
            archState->instCycleState = 0;
            break;
        case BPL:
            if (archState->status & NEGATIVE_FLAG) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BMI:
            if (!(archState->status & NEGATIVE_FLAG)) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BCC:
            if (archState->status & CARRY_FLAG) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BCS:
            if (!(archState->status & CARRY_FLAG)) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BNE:
            if (archState->status & ZERO_FLAG) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BEQ:
            if (!(archState->status & ZERO_FLAG)) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BVC:
            if (archState->status & OVERFLOW_FLAG) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BVS:
            if (!(archState->status & OVERFLOW_FLAG)) {archState->instCycleState = 0;}
            incrementPC(archState);
            break;
        case BRK:
            break;
        case RTI:
            break;
        case RTS:
            break;
        case PHP:
             break;
        case PLP:
            break;
        case PHA:
            break;
        case PLA:
            break;
        case SEC:
            archState->status |= CARRY_FLAG;
            archState->instCycleState = 0;
            break;
        case CLC:
            archState->status &= ~CARRY_FLAG;
            archState->instCycleState = 0;
            break;
        case SEI:
            archState->status |= INTERRUPT_FLAG;
            archState->instCycleState = 0;
            break;
        case CLI:
            archState->status &= ~INTERRUPT_FLAG;
            archState->instCycleState = 0;
            break;
        case SED:
            archState->status |= DECIMAL_FLAG;
            archState->instCycleState = 0;
            break;
        case CLD:
            archState->status &= ~DECIMAL_FLAG;
            archState->instCycleState = 0;
            break;
        case CLV:
            archState->status &= ~OVERFLOW_FLAG;
            archState->instCycleState = 0;
            break;
        case TAX:
            archState->X = archState->A;
            !archState->X ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->X >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case TXA:
            archState->A = archState->X;
            !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case TAY:
            archState->Y = archState->A;
            !archState->Y ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->Y >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case TYA:
            archState->A = archState->Y;
            !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case TSX:
            archState->X = archState->stackPtr;
            !archState->X ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->X >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case TXS:
            archState->stackPtr = archState->X;
            archState->instCycleState = 0;
            break;
        case INX:
            ++(archState->X);
            !archState->X ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->X >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case DEX:
            --(archState->X);
            !archState->X ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->X >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case INY:
            ++(archState->Y);
            !archState->Y ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->Y >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case DEY:
            --(archState->Y);
            !archState->Y ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
            archState->Y >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
            archState->instCycleState = 0;
            break;
        case NOP:
            archState->instCycleState = 0;
            break;
        default:
            incrementPC(archState);
    }

    archState->tempReg = archState->dataBus;
    ++(archState->cycleNo);
}

void cycle2(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    switch (archState->IR)
    {
        case JMP:
            archState->PCL = archState->tempReg;
            archState->PCH = archState->dataBus;
            archState->instCycleState = 0;
            break;
        case INDIRECT_JMP:
            readData(archState, PPU, cartridgeData);
            break;
        case BPL: case BMI: case BCC: case BCS: case BNE: case BEQ: case BVC: case BVS:
            if (!((overflow(archState->PCL, archState->tempReg) && !(archState->tempReg & NEGATIVE_FLAG)) ||
                (!overflow(archState->PCL, archState->tempReg) && (archState->tempReg & NEGATIVE_FLAG))))
            {archState->instCycleState = 0;}
            archState->PCL += archState->tempReg;
            break;
        case BRK:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            archState->dataBus = archState->PCH;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            break;
        case JSR:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            break;
        case RTI: case RTS:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            ++(archState->stackPtr);
            break;
        case PHP:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            archState->dataBus = archState->status | 0x30;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            archState->instCycleState = 0;
            break;
        case PLP:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            ++(archState->stackPtr);
            break;
        case PHA:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            archState->dataBus = archState->A;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            archState->instCycleState = 0;
            break;
        case PLA:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            ++(archState->stackPtr);
            break;
        default:
            switch (archState->IR & ADDRESSING_MODE_FLAG)
            {
                case ZERO_PAGE_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = 0x00;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case BIT:
                            _BIT(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case CPX:
                            _CPX(archState);
                            archState->instCycleState = 0;
                            break;
                        case CPY:
                            _CPY(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STX:
                            _STX(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STY:
                            _STY(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDX:
                            _LDX(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDY:
                            _LDY(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case ABSOLUTE_ADDRESSING:
                    incrementPC(archState);
                    break;
                case ZERO_PAGE_X_ADDRESSING: case INDIRECT_X_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = 0x00;
                    readData(archState, PPU, cartridgeData);

                    if ((archState->IR & ~ADDRESSING_MODE_FLAG) == LDX || (archState->IR & ~ADDRESSING_MODE_FLAG) == STX)
                    {
                        archState->tempReg += archState->Y;
                    }
                    else
                    {
                        archState->tempReg += archState->X;
                    }
                    break;
                case ABSOLUTE_X_ADDRESSING:
                    if ((archState->IR & ~ADDRESSING_MODE_FLAG) == LDX)
                    {
                        archState->tempReg += archState->Y;
                    }
                    else
                    {
                        archState->tempReg += archState->X;
                    }
                    incrementPC(archState);
                    break;
                case ABSOLUTE_Y_ADDRESSING:
                    archState->tempReg += archState->Y;
                    readData(archState, PPU, cartridgeData);
                    incrementPC(archState);
                    break;
                case INDIRECT_Y_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = 0x00;
                    readData(archState, PPU, cartridgeData);

                    ++(archState->tempReg);
                    break;
            }
    }

    ++(archState->cycleNo);
}

void cycle3(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    switch (archState->IR)
    {
        case INDIRECT_JMP:
            archState->ADL = archState->tempReg;
            archState->ADH = archState->dataBus;
            readData(archState, PPU, cartridgeData);
            ++(archState->tempReg);
            break;
        case BPL: case BMI: case BCC: case BCS: case BNE: case BEQ: case BVC: case BVS:
            if (archState->PCL < 127)
            {
                ++(archState->PCH);
            }
            else
            {
                --(archState->PCH);
            }
            archState->instCycleState = 0;
            break;
        case BRK:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            archState->dataBus = archState->PCL;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            break;
        case JSR:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            archState->dataBus = archState->PCH;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            break;
        case RTI:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            archState->status = archState->dataBus & ~BREAK_FLAG;
            archState->status |= UNUSED_FLAG;
            ++(archState->stackPtr);
            break;
        case RTS:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            archState->PCL = archState->dataBus;
            ++(archState->stackPtr);
            break;
        case PLP:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            archState->status = archState->dataBus & ~0x10;
            archState->status |= 0x20;
            archState->instCycleState = 0;
            break;
        case PLA:
            archState->ADL = archState->stackPtr;
            archState->ADH = 0x01;
            readData(archState, PPU, cartridgeData);
            archState->A = archState->dataBus;

            if (archState->A == 0)
            {
                archState->status |= ZERO_FLAG;
            }
            else
            {
                archState->status &= ~ZERO_FLAG;
            }

            if (archState->A >> 7)
            {
                archState->status |= NEGATIVE_FLAG;
            }
            else
            {
                archState->status &= ~NEGATIVE_FLAG;
            }

            archState->instCycleState = 0;
            break;
        default:
            switch (archState->IR & ADDRESSING_MODE_FLAG)
            {
                case ZERO_PAGE_ADDRESSING:
                    archState->tempReg = archState->dataBus;
                    writeData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case INC:
                            _INC(archState);
                            break;
                        case DEC:
                            _DEC(archState);
                            break;
                        case LSR:
                            _LSR(archState);
                            break;
                        case ASL:
                            _ASL(archState);
                            break;
                        case ROL:
                            _ROL(archState);
                            break;
                        case ROR:
                            _ROR(archState);
                            break;
                    }
                    break;
                case ABSOLUTE_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = archState->dataBus;

                    if ((archState->IR & ~ADDRESSING_MODE_FLAG) != STA && (archState->IR & ~ADDRESSING_MODE_FLAG) != STX && (archState->IR & ~ADDRESSING_MODE_FLAG) != STY)
                    {
                        readData(archState, PPU, cartridgeData);
                    }

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case BIT:
                            _BIT(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case CPX:
                            _CPX(archState);
                            archState->instCycleState = 0;
                            break;
                        case CPY:
                            _CPY(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STX:
                            _STX(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STY:
                            _STY(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDX:
                            _LDX(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDY:
                            _LDY(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case ZERO_PAGE_X_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = 0x00;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STX:
                            _STX(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case STY:
                            _STY(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDX:
                            _LDX(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDY:
                            _LDY(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case ABSOLUTE_X_ADDRESSING:
                {
                    uint8_t index;
                    archState->ADL = archState->tempReg;
                    archState->ADH = archState->dataBus;
                    archState->tempReg = archState->dataBus;
                    readData(archState, PPU, cartridgeData);

                    if ((archState->IR & ~ADDRESSING_MODE_FLAG) == LDX)
                    {
                        index = archState->Y;
                    }
                    else
                    {
                        index = archState->X;
                    }

                    if (overflow(archState->ADL - index, index))
                    {
                        ++(archState->tempReg);
                    }
                    else
                    {
                        switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                        {
                            case ORA:
                                _ORA(archState);
                                archState->instCycleState = 0;
                                break;
                            case AND:
                                _AND(archState);
                                archState->instCycleState = 0;
                                break;
                            case EOR:
                                _EOR(archState);
                                archState->instCycleState = 0;
                                break;
                            case ADC:
                                _ADC(archState);
                                archState->instCycleState = 0;
                                break;
                            case SBC:
                                _SBC(archState);
                                archState->instCycleState = 0;
                                break;
                            case CMP:
                                _CMP(archState);
                                archState->instCycleState = 0;
                                break;
                            case LDA:
                                _LDA(archState);
                                archState->instCycleState = 0;
                                break;
                            case LDX:
                                _LDX(archState);
                                archState->instCycleState = 0;
                                break;
                            case LDY:
                                _LDY(archState);
                                archState->instCycleState = 0;
                                break;
                        }
                    }
                    break;
                }
                case ABSOLUTE_Y_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = archState->dataBus;
                    archState->tempReg = archState->dataBus;
                    readData(archState, PPU, cartridgeData);

                    if (overflow(archState->ADL - archState->Y, archState->Y))
                    {
                        ++(archState->tempReg);
                    }
                    else
                    {
                        switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                        {
                            case ORA:
                                _ORA(archState);
                                archState->instCycleState = 0;
                                break;
                            case AND:
                                _AND(archState);
                                archState->instCycleState = 0;
                                break;
                            case EOR:
                                _EOR(archState);
                                archState->instCycleState = 0;
                                break;
                            case ADC:
                                _ADC(archState);
                                archState->instCycleState = 0;
                                break;
                            case SBC:
                                _SBC(archState);
                                archState->instCycleState = 0;
                                break;
                            case CMP:
                                _CMP(archState);
                                archState->instCycleState = 0;
                                break;
                            case LDA:
                                _LDA(archState);
                                archState->instCycleState = 0;
                                break;
                        }
                    }
                    break;
                case INDIRECT_X_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    readData(archState, PPU, cartridgeData);
                    ++(archState->tempReg);
                    break;
                case INDIRECT_Y_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->tempReg = archState->dataBus;
                    readData(archState, PPU, cartridgeData);

                    archState->tempReg += archState->Y;
                    break;
            }
    }

    ++(archState->cycleNo);
}

void cycle4(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    switch (archState->IR)
    {
        case INDIRECT_JMP:
            archState->ADL = archState->tempReg;
            archState->tempReg = archState->dataBus;
            readData(archState, PPU, cartridgeData);

            archState->PCL = archState->tempReg;
            archState->PCH = archState->dataBus;
            archState->instCycleState = 0;
            break;
        case BRK:
            archState->ADL = archState->stackPtr;
            archState->dataBus = archState->status | 0x10;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            break;
        case JSR:
            archState->ADL = archState->stackPtr;
            archState->dataBus = archState->PCL;
            writeData(archState, PPU, cartridgeData);
            --(archState->stackPtr);
            break;
        case RTI:
            archState->ADL = archState->stackPtr;
            readData(archState, PPU, cartridgeData);
            archState->PCL = archState->dataBus;
            ++(archState->stackPtr);
            break;
        case RTS:
            archState->ADL = archState->stackPtr;
            readData(archState, PPU, cartridgeData);
            archState->PCH = archState->dataBus;
            break;
        default:
            switch (archState->IR & ADDRESSING_MODE_FLAG)
            {
                case ZERO_PAGE_ADDRESSING:
                    archState->dataBus = archState->tempReg;
                    writeData(archState, PPU, cartridgeData);
                    archState->instCycleState = 0;
                    break;
                case ABSOLUTE_ADDRESSING: case ZERO_PAGE_X_ADDRESSING:
                    archState->tempReg = archState->dataBus;
                    writeData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case INC:
                            _INC(archState);
                            break;
                        case DEC:
                            _DEC(archState);
                            break;
                        case LSR:
                            _LSR(archState);
                            break;
                        case ASL:
                            _ASL(archState);
                            break;
                        case ROL:
                            _ROL(archState);
                            break;
                        case ROR:
                            _ROR(archState);
                            break;
                    }
                    break;
                case ABSOLUTE_X_ADDRESSING:
                    archState->ADH = archState->tempReg;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDX:
                            _LDX(archState);
                            archState->instCycleState = 0;
                            break;
                        case LDY:
                            _LDY(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case ABSOLUTE_Y_ADDRESSING:
                    archState->ADH = archState->tempReg;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case INDIRECT_X_ADDRESSING:
                    ++archState->ADL;
                    archState->tempReg = archState->dataBus;
                    readData(archState, PPU, cartridgeData);
                    break;
                case INDIRECT_Y_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = archState->dataBus;
                    archState->tempReg = archState->dataBus;
                    readData(archState, PPU, cartridgeData);

                    if (overflow(archState->ADL-archState->Y, archState->Y))
                    {
                        ++(archState->tempReg);
                    }
                    else
                    {
                        switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                        {
                            case ORA:
                                _ORA(archState);
                                archState->instCycleState = 0;
                                break;
                            case AND:
                                _AND(archState);
                                archState->instCycleState = 0;
                                break;
                            case EOR:
                                _EOR(archState);
                                archState->instCycleState = 0;
                                break;
                            case ADC:
                                _ADC(archState);
                                archState->instCycleState = 0;
                                break;
                            case SBC:
                                _SBC(archState);
                                archState->instCycleState = 0;
                                break;
                            case CMP:
                                _CMP(archState);
                                archState->instCycleState = 0;
                                break;
                            case LDA:
                                _LDA(archState);
                                archState->instCycleState = 0;
                                break;
                        }
                    }
                    break;
            }
    }

    ++(archState->cycleNo);
}

void cycle5(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    switch (archState->IR)
    {
        case BRK:
            archState->ADL = 0xFE;
            archState->ADH = 0xFF;
            readData(archState, PPU, cartridgeData);
            archState->tempReg = archState->dataBus;
            break;
        case JSR:
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);

            archState->PCL = archState->tempReg;
            archState->PCH = archState->dataBus;
            archState->instCycleState = 0;
            break;
        case RTI:
            archState->ADL = archState->stackPtr;
            readData(archState, PPU, cartridgeData);
            archState->PCH = archState->dataBus;
            archState->instCycleState = 0;
            break;
        case RTS:
            archState->ADL = archState->PCL;
            archState->ADH = archState->PCH;
            readData(archState, PPU, cartridgeData);
            incrementPC(archState);
            archState->instCycleState = 0;
            break;
        default:
            switch (archState->IR & ADDRESSING_MODE_FLAG)
            {
                case ABSOLUTE_ADDRESSING: case ZERO_PAGE_X_ADDRESSING:
                    archState->dataBus = archState->tempReg;
                    writeData(archState, PPU, cartridgeData);
                    archState->instCycleState = 0;
                    break;
                case ABSOLUTE_X_ADDRESSING:
                    archState->tempReg = archState->dataBus;
                    writeData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case INC:
                            _INC(archState);
                            break;
                        case DEC:
                            _DEC(archState);
                            break;
                        case LSR:
                            _LSR(archState);
                            break;
                        case ASL:
                            _ASL(archState);
                            break;
                        case ROL:
                            _ROL(archState);
                            break;
                        case ROR:
                            _ROR(archState);
                            break;
                    }
                    break;
                case INDIRECT_X_ADDRESSING:
                    archState->ADL = archState->tempReg;
                    archState->ADH = archState->dataBus;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
                case INDIRECT_Y_ADDRESSING:
                    archState->ADH = archState->tempReg;
                    readData(archState, PPU, cartridgeData);

                    switch (archState->IR & ~ADDRESSING_MODE_FLAG)
                    {
                        case ORA:
                            _ORA(archState);
                            archState->instCycleState = 0;
                            break;
                        case AND:
                            _AND(archState);
                            archState->instCycleState = 0;
                            break;
                        case EOR:
                            _EOR(archState);
                            archState->instCycleState = 0;
                            break;
                        case ADC:
                            _ADC(archState);
                            archState->instCycleState = 0;
                            break;
                        case SBC:
                            _SBC(archState);
                            archState->instCycleState = 0;
                            break;
                        case CMP:
                            _CMP(archState);
                            archState->instCycleState = 0;
                            break;
                        case STA:
                            _STA(archState, PPU, cartridgeData);
                            archState->instCycleState = 0;
                            break;
                        case LDA:
                            _LDA(archState);
                            archState->instCycleState = 0;
                            break;
                    }
                    break;
            }
    }

    ++(archState->cycleNo);
}

void cycle6(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    ++(archState->instCycleState);

    switch (archState->IR)
    {
        case BRK:
            archState->ADL = 0xFF;
            readData(archState, PPU, cartridgeData);

            archState->PCL = archState->tempReg;
            archState->PCH = archState->dataBus;
            archState->instCycleState = 0;
        default:
            if ((archState->IR & ADDRESSING_MODE_FLAG) == ABSOLUTE_X_ADDRESSING)
            {
                archState->dataBus = archState->tempReg;
                writeData(archState, PPU, cartridgeData);
                archState->instCycleState = 0;
                break;
            }
    }

    ++(archState->cycleNo);
}
