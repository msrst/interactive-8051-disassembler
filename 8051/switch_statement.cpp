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
 
#include "switch_statement.hpp"
#include "../utils/utils.hpp"

namespace disas8051 {

std::string SwitchStatement::ValuesCase::formatValues() {
	std::string valuesStr;
	for(int32_t value : values) {
		if(!valuesStr.empty()) {
			valuesStr += ",";
		}
		
		if(value == DEFAULT_CASE_VALUE) {
			valuesStr += "default";
		}
		else {
			valuesStr += "0x" + utils::Int_To_String_Hex(value);
		}
	}
	return valuesStr;
}

std::vector<SwitchStatement::ValuesCase> SwitchStatement::groupCasesByValues() const
{
	std::vector<ValuesCase> valuesCases;
	for(Case caseBlock : cases) {
		bool valuesCaseFound = false;
		
		for(ValuesCase valuesCase : valuesCases) {
			if(valuesCase.address == caseBlock.address) {
				valuesCase.values.push_back(caseBlock.value);
				valuesCaseFound = true;
				break;
			}
		}
		
		if(!valuesCaseFound) {
			valuesCases.emplace_back(caseBlock.value, caseBlock.address);
		}
	}
	return valuesCases;
}

} // namespace disas8051
