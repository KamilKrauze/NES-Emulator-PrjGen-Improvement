#include "processorExecution.h"
#include "opcodeFunction.h"
#include "memory.h"

void _ORA(ArchState *archState)
{
    archState->A |= archState->dataBus;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _AND(ArchState *archState)
{
    archState->A &= archState->dataBus;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _BIT(ArchState *archState)
{
    uint8_t result = archState->A & archState->dataBus;
    !result ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->dataBus & OVERFLOW_FLAG ? archState->status |= OVERFLOW_FLAG : archState->status &= ~OVERFLOW_FLAG;
    archState->dataBus >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _EOR(ArchState *archState)
{
    archState->A ^= archState->dataBus;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _ADC(ArchState *archState)
{
    uint8_t result = archState->A + archState->dataBus + (archState->status & CARRY_FLAG);

    if (archState->dataBus == 0x7F && (archState->status & CARRY_FLAG))
    {
        archState->status |= OVERFLOW_FLAG;
    }
    else
    {
        wrongSign(archState->A, archState->dataBus + (archState->status & CARRY_FLAG)) ? archState->status |= OVERFLOW_FLAG : archState->status &= ~OVERFLOW_FLAG;
    }

    if (archState->dataBus == 0xFF && (archState->status & CARRY_FLAG))
    {
        archState->status |= CARRY_FLAG;
    }
    else
    {
        overflow(archState->A, archState->dataBus + (archState->status & CARRY_FLAG)) ? archState->status |= CARRY_FLAG : archState->status &= ~CARRY_FLAG;
    }

    archState->A = result;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _SBC(ArchState *archState)
{
    uint8_t result = archState->A + -archState->dataBus + -!(archState->status & CARRY_FLAG);

    if (-archState->dataBus == 0x80 && !(archState->status & CARRY_FLAG))
    {
        archState->status |= OVERFLOW_FLAG;
    }
    else
    {
        wrongSign(archState->A, -archState->dataBus + -!(archState->status & CARRY_FLAG)) ? archState->status |= OVERFLOW_FLAG : archState->status &= ~OVERFLOW_FLAG;
    }

    if (-archState->dataBus == 0x01 && !(archState->status & CARRY_FLAG))
    {
        archState->status |= CARRY_FLAG;
    }
    else
    {
        overflow(archState->A, -archState->dataBus + -!(archState->status & CARRY_FLAG)) ? archState->status |= CARRY_FLAG : archState->status &= ~CARRY_FLAG;
    }

    archState->A = result;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _CMP(ArchState *archState)
{
    archState->A >= archState->dataBus ? archState->status |= CARRY_FLAG : archState->status &= ~CARRY_FLAG;
    archState->A == archState->dataBus ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    (archState->A - archState->dataBus) >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _CPX(ArchState *archState)
{
    archState->X >= archState->dataBus ? archState->status |= CARRY_FLAG : archState->status &= ~CARRY_FLAG;
    archState->X == archState->dataBus ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    (archState->X - archState->dataBus) >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _CPY(ArchState *archState)
{
    archState->Y >= archState->dataBus ? archState->status |= CARRY_FLAG : archState->status &= ~CARRY_FLAG;
    archState->Y == archState->dataBus ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    (archState->Y - archState->dataBus) >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _STA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->dataBus = archState->A;
    writeData(archState, PPU, cartridgeData);
    archState->instCycleState = 0;
}

void _STX(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->dataBus = archState->X;
    writeData(archState, PPU, cartridgeData);
    archState->instCycleState = 0;
}

void _STY(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->dataBus = archState->Y;
    writeData(archState, PPU, cartridgeData);
    archState->instCycleState = 0;
}

void _LDA(ArchState *archState)
{
    archState->A = archState->dataBus;
    !archState->A ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->A >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _LDX(ArchState *archState)
{
    archState->X = archState->dataBus;
    !archState->X ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->X >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _LDY(ArchState *archState)
{
    archState->Y = archState->dataBus;
    !archState->Y ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->Y >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
    archState->instCycleState = 0;
}

void _DEC(ArchState *archState)
{
    --(archState->tempReg);
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _INC(ArchState *archState)
{
    ++(archState->tempReg);
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _LSR(ArchState *archState)
{
    archState->status &= ~CARRY_FLAG;
    archState->status |= archState->tempReg & CARRY_FLAG;
    archState->tempReg >>= 1;
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _ASL(ArchState *archState)
{
    archState->status &= ~CARRY_FLAG;
    archState->status |= archState->tempReg >> 7;
    archState->tempReg <<= 1;
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _ROL(ArchState *archState)
{
    uint8_t result = (archState->tempReg << 1) + (archState->status & CARRY_FLAG);
    archState->status &= ~CARRY_FLAG;
    archState->status |= archState->tempReg >> 7;
    archState->tempReg = result;
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _ROR(ArchState *archState)
{
    uint8_t result = (archState->tempReg >> 1) + ((archState->status & CARRY_FLAG) << 7);
    archState->status &= ~CARRY_FLAG;
    archState->status |= archState->tempReg & CARRY_FLAG;
    archState->tempReg = result;
    !archState->tempReg ? archState->status |= ZERO_FLAG : archState->status &= ~ZERO_FLAG;
    archState->tempReg >> 7 ? archState->status |= NEGATIVE_FLAG : archState->status &= ~NEGATIVE_FLAG;
}

void _JMP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    archState->tempReg = archState->dataBus;
    incrementPC(archState);

    ++(archState->cycleNo);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    incrementPC(archState);

    archState->ADL = archState->tempReg;
    archState->ADH = archState->dataBus;

    ++(archState->cycleNo);
}

void _JMP_INDIRECT(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    archState->tempReg = archState->dataBus;
    incrementPC(archState);

    ++(archState->cycleNo);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    incrementPC(archState);

    archState->ADL = archState->tempReg;
    archState->ADH = archState->dataBus;

    ++(archState->cycleNo);

    readData(archState, PPU, cartridgeData);
    archState->tempReg = archState->dataBus;
    ++(archState->ADL);

    ++(archState->cycleNo);

    readData(archState, PPU, cartridgeData);
    archState->ADL = archState->tempReg;
    archState->ADH = archState->dataBus;

    ++(archState->cycleNo);
}

void _JSR(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    archState->tempReg = archState->dataBus;
    incrementPC(archState);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->dataBus = archState->PCH;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);

    ++(archState->cycleNo);

    archState->dataBus = archState->PCL;
    archState->ADL = archState->stackPtr;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);

    ++(archState->cycleNo);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);
    archState->PCL = archState->tempReg;
    archState->PCH = archState->dataBus;

    ++(archState->cycleNo);
}

void _RTS(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    ++(archState->stackPtr);
    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->PCL = archState->dataBus;

    ++(archState->cycleNo);

    ++(archState->stackPtr);
    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->PCH = archState->dataBus;


    ++(archState->cycleNo);

    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    incrementPC(archState);

    ++(archState->cycleNo);
}

void _RTI(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    ++(archState->stackPtr);
    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->status = archState->dataBus;

    ++(archState->cycleNo);

    ++(archState->stackPtr);
    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->PCL = archState->dataBus;


    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->PCH = archState->dataBus;
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    incrementPC(archState);

    ++(archState->cycleNo);
}

void _PHP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    archState->dataBus = archState->status | 0x30;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);

    ++(archState->cycleNo);
}

void _PLP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    ++(archState->stackPtr);
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->status = archState->dataBus & ~0x10;

    ++(archState->cycleNo);
}

void _PHA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    archState->dataBus = archState->A;
    writeData(archState, PPU, cartridgeData);
    --(archState->stackPtr);

    ++(archState->cycleNo);
}

void _PLA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData)
{
    archState->ADL = archState->PCL;
    archState->ADH = archState->PCH;
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    archState->ADH = 0x01;
    ++(archState->stackPtr);
    readData(archState, PPU, cartridgeData);

    ++(archState->cycleNo);

    archState->ADL = archState->stackPtr;
    readData(archState, PPU, cartridgeData);
    archState->A = archState->dataBus;

    ++(archState->cycleNo);
}

void BRANCH(ArchState *archState)
{
    int flag;
    int shiftAmount;

    switch (archState->IR & 0xC0)
    {
        case 0x00:
            flag = NEGATIVE_FLAG;
            shiftAmount = 7;
            break;
        case 0x40:
            flag = OVERFLOW_FLAG;
            shiftAmount = 6;
            break;
        case 0x80:
            flag = CARRY_FLAG;
            shiftAmount = 0;
            break;
        case 0xC0:
            flag = ZERO_FLAG;
            shiftAmount = 1;
            break;
    }

    incrementPC(archState);

    ++(archState->cycleNo);

    if (((archState->IR & ~0xC0) >> 5) == (archState->status >> shiftAmount))
    {
        archState->tempReg = archState->PCL;
        archState->PCL += archState->dataBus;
        archState->ADL = archState->PCL;

        ++(archState->cycleNo);
        if (overflow(archState->tempReg, archState->dataBus))
        {
            if (archState->dataBus & NEGATIVE_FLAG)
            {
                --(archState->PCH);
            }
            else
            {
                ++(archState->PCH);
            }

            archState->ADH = archState->PCH;
            ++(archState->cycleNo);\
        }
    }
}
