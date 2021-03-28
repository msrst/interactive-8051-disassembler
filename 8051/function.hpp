/************************************************************************
 * Copyright (C) 2020-2021 Matthias Rosenthal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>
 ***********************************************************************/
 
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
