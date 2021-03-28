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
