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
 
#ifndef SWITCH_STATEMENT_8051_HPP_INCLUDED
#define SWITCH_STATEMENT_8051_HPP_INCLUDED

#include <string>
#include <vector>
#include <inttypes.h>

namespace disas8051 {

class SwitchStatement
{
public:
	uint64_t address; // this is the address of the call of the CCASE or ICASE function.
	
	bool isI16; // whether it is "switched" for 8- or 16-bit values	

	static const int32_t DEFAULT_CASE_VALUE = 0x10000; // this is ok because we only have 16- and 8-bit switches
	
	struct Case
	{
		int32_t value; // it's a default case if this value equals DEFAULT_CASE_VALUE
		uint64_t address;
	};
	std::vector<Case> cases;
	
	uint64_t getDataLength() {
		if(isI16) {
			return cases.size() * 4;
		}
		else {
			return cases.size() * 3 + 1; // the default case takes always 4 bytes
		}
	}
	
	struct ValuesCase
	{
		std::vector<int32_t> values;
		uint64_t address;
		
		ValuesCase(int32_t value, uint64_t address) {
			values.push_back(value);
			this->address = address;
		}
		
		std::string formatValues();
	};
	std::vector<ValuesCase> groupCasesByValues() const;
};

} // namespace disas8051

#endif // SWITCH_STATEMENT_8051_HPP_INCLUDED
