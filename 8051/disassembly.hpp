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
 
#ifndef DISASSEMBLY_8051_HPP_INCLUDED
#define DISASSEMBLY_8051_HPP_INCLUDED

#include <map>
#include <vector>
#include <unordered_map>
#include <inttypes.h>

#include "../utils/logger.h"
#include "instruction.hpp"
#include "function.hpp"
#include "remap_call.hpp"
#include "ignore_block.hpp"
#include "switch_statement.hpp"

namespace disas8051 {
	
class Disassembler;

class Disassembly {
private:
    logger::logger_base *logger;
    std::map<uint64_t, std::string> pendingComments; // stores comments after reading the meta file before calling resolveComments()
    wxStyledTextCtrl *textCtrl;
public:
    Disassembly(logger::logger_base *logger) {
        this->logger = logger;
    }

    ustring buf; // must be set by user before printing
    std::map<uint64_t, IgnoreBlock> ignoreBlocks;
    std::map<uint64_t, Instruction> instructions;
    std::unordered_map<uint64_t, std::vector<uint64_t>> jumpTargets; // target address, source addresses
    std::map<uint64_t, Function> functions;
    std::map<uint64_t, RemapCall> remapCalls;
    
    std::vector<SwitchStatement> switchStatements;
    std::map<uint64_t, int> caseBlocks; // the integer is the switchStatement index

    std::map<int, uint64_t> addressByLine; // for text ctrl

    void printToWx(wxStyledTextCtrl *textCtrl); // stores the pointer for referral, e. g. in updateFunctionName

    int findLineByAddress(int address); // returns -1 if address out of range, this function is very costly

    int openMetaFile(std::string filename);
    int saveMetaFile(std::string filename);
    void resolveRemapCalls(Disassembler *disassembler); // if the address or the target address is not already disassembled, this is also done here.
    void resolveComments(); // applies all comments read from the meta file to the instructions
    void autoComment();
    void resolveFunctionEndAddresses();

    void addFunction(int address, Function function); // also disassembles the new function, if necessary (with a new disassembler)
    void updateFunctionName(int address, std::string newName);
    void updateInstructionComment(int address, std::string newComment);
    void deleteFunction(int address);

    std::map<uint64_t, Function>::iterator findFunction(int addressIn);

    std::vector<uint64_t> GetFunctionTargetAddresses(uint64_t functionAddress);
    
    bool seemsToBeInvalidCode(uint64_t address); // false if called with invalid address
    
    std::string formatWithFunction(uint64_t address); // e. g. "1ab8 (myfunc)"
    
    int findSwitchStatementOnExact(uint64_t address); // returns -1 if not found, else the index of the switch statement

private:
    void updateLineAtAddress(int address);
    void recurseFunction(std::map<uint64_t, Function>::iterator itFunction, std::map<uint64_t, Function>::iterator itNextFunction, uint64_t address);
};

} // namespace disas8051

#endif // DISASSEMBLY_8051_HPP_INCLUDED
