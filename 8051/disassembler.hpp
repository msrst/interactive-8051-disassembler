/******************************************************************
 * Copyright (C) 2020 Matthias Rosenthal
 * 
 * This file is part of The Interactive 8051 Disassembler.
 * 
 * The Interactive 8051 Disassembler is licensed under Creative
 * Commons-Attribution-Noncommercial-NoDerivative (CC BY-NC-ND).
 * See https://creativecommons.org/licenses/by-nc-nd/4.0/legalcode
 * 
 * Additionally, The Interactive 8051 Disassembler including
 * binaries generated from its source code is only allowed to be
 * used for non-commercial purposes.
 *
 * The Interactive 8051 Disassembler is distributed in the hope 
 * that it will be useful, but WITHOUT ANY WARRANTY; without 
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR 
 * A PARTICULAR PURPOSE.
 *****************************************************************/
 
#ifndef DISASSEMBLER_8051_HPP_INCLUDED
#define DISASSEMBLER_8051_HPP_INCLUDED

#include <string>
#include <memory>

#include "disassembly.hpp"

namespace disas8051
{

enum {
	NONE = 0,
	ADDR11 = 1,  // 8 bits from argument + 3 high bits from opcode
	ADDR16 = 2,  // A 16-bit address destination. Used by LCALL and LJMP
	DIRECT = 3,  // An internal data RAM location (0-127) or SFR (128-255).
    IMMED = 4,   // Immediate value (literal) - 8 Bit
    IMMED16 = 5, // Immediate Value (16 bit)
    BIT = 6,     // A bit within one of the bitfield bytes
	OFFSET = 7,  // same as direct?
    A = 8,       // Accum directly. Used only for second arg for a few ops
    C = 9,       // C
	IR0 = 10,    // @R0
	IR1 = 11,    // @R1
	R0 = 12,     // R0
	R1 = 13,     // R1
	R2 = 14,     // R2
	R3 = 15,     // R3
	R4 = 16,     // R4
	R5 = 17,     // R5
	R6 = 18,     // R6
	R7 = 19      // R7
};

struct Instruct8051 {
    const char *name;   //static portion of the decoded instruction
    int length;         //# of bytes this instruction spans
    int arg1;           //Type of the first argument
    int arg2;           //Type of the second argument
    bool isJump;        //Does this instruction always jump
    bool isCondJump;    //Does this instruction maybe branch? (also calls)
};

//all 256 op codes
extern Instruct8051 OpCodes[];
//Starting at 0x80 and going through 0xFF these are the SFR's for a ENE KB9012
extern const char *SFR[];

class Disassembler
{
private:
	logger::logger_base *logger;
public:
	Disassembler(logger::logger_base *logger);

    Instruction disassembleInstruction(const ustring &buf, uint64_t address);

    void followCodePath(const ustring &buf, uint64_t startAddress, Disassembly *disassembly);
    
private:
	int interpretArgument(int32_t *outInt, const uint8_t *inBuffer, int opType, uint32_t address, int op);
	int32_t resolveAddress(int32_t rawAddress, uint32_t currentAddress); // calculates the actual address (resolving X-Bus remappings)
	void disassembleSwitchStatement(const ustring &buf, uint64_t callAddress, uint64_t startAddress, bool isI16Switch, Disassembly *disassembly);
};

} // namespace disas8051

#endif // DISASSEMBLER_8051_HPP_INCLUDED
