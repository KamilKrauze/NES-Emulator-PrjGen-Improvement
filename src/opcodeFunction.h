#ifndef OPCODE_FUNCTION_H
#define  OPCODE_FUNCTION_H

#include "processorArchitechture.h"

void _ORA(ArchState *archState);
void _AND(ArchState *archState);
void _BIT(ArchState *archState);
void _EOR(ArchState *archState);
void _ADC(ArchState *archState);
void _SBC(ArchState *archState);
void _CMP(ArchState *archState);
void _CPX(ArchState *archState);
void _CPY(ArchState *archState);
void _STA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _STX(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _STY(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _LDA(ArchState *archState);
void _LDX(ArchState *archState);
void _LDY(ArchState *archState);
void _DEC(ArchState *archState);
void _INC(ArchState *archState);
void _LSR(ArchState *archState);
void _ASL(ArchState *archState);
void _ROL(ArchState *archState);
void _ROR(ArchState *archState);
void _JMP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _JMP_INDIRECT(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _JSR(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _RTS(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _RTI(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _PHP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _PLP(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _PHA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void _PLA(ArchState *archState, PPUArchitechture *PPU, CartridgeData *cartridgeData);
void BRANCH(ArchState *archState);

#endif
