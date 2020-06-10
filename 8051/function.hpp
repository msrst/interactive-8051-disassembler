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
 
#ifndef FUNCTION_8051_HPP_INCLUDED
#define FUNCTION_8051_HPP_INCLUDED

#include <string>
#include <inttypes.h>
#include <vector>

namespace disas8051 {

class Function
{
public:
    std::string name;
    int posX, posY; // for graph in GUI
    bool isInterrupt;
    uint64_t endAddress; // address after the last instruction
    bool isShownInGraph = true;

	Function() = default;
	Function(std::string name, bool isInterrupt = false, int posX = 10, int posY = 10) {
		this->name = name;
		this->isInterrupt = isInterrupt;
		this->posX = posX;
		this->posY = posY;
		// the other params are parsed by disassembler/disassembly
	}
    
    struct Block
    {
		uint64_t begin;
		uint64_t end; // address after the last instruction
		
		bool contains(uint64_t address) const {
			return (address >= begin) && (address < end);
		}
	};
	std::vector<Block> blocks;
	
	bool contains(uint64_t address);
	
	std::string getBlockSummary();
	
	// sometimes, e. g. when jumping to another part of the function, the current block
	// is ended but one instruction later the function continues with another block.
	// This function resolves that.
	void defragmentBlocks();
};

} // namespace disas8051

#endif // FUNCTION_8051_HPP_INCLUDED
