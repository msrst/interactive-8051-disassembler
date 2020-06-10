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
 
#include "function.hpp"
#include "../utils/utils.hpp"

namespace disas8051 {

bool Function::contains(uint64_t address)
{
	for(const Block &block : blocks) {
		if(block.contains(address)) {
			return true;
		}
	}
	return false;
}

std::string Function::getBlockSummary()
{
	std::string ret = utils::Int_To_String_Hex(blocks[0].begin) + ".." + utils::Int_To_String_Hex(blocks[0].end);
	for(std::size_t i1 = 1; i1 < blocks.size(); i1++) {
		ret += ", " + utils::Int_To_String_Hex(blocks[i1].begin) + ".." + utils::Int_To_String_Hex(blocks[i1].end);
	}
	return ret;
}

void Function::defragmentBlocks()
{
	bool fragmented;
	do {
		fragmented = false;
		for(std::size_t iBlock = 0; iBlock < blocks.size(); iBlock++) {
			for(std::size_t iBlockCompare = iBlock; iBlockCompare < blocks.size(); iBlockCompare++) {
				if(blocks[iBlock].end == blocks[iBlockCompare].begin) {
					blocks[iBlock].end = blocks[iBlockCompare].end;
					blocks.erase(blocks.begin() + iBlockCompare);
					fragmented = true;
				}
				else if(blocks[iBlock].begin == blocks[iBlockCompare].end) {
					blocks[iBlock].begin = blocks[iBlockCompare].begin;
					blocks.erase(blocks.begin() + iBlockCompare);
					fragmented = true;
				}
			}
		}
	} while(fragmented);
}

} // namespace disas8051
