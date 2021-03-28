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
