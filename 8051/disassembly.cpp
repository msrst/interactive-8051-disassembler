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
 
#include <cctype>
#include "wx/stc/stc.h"

#include "../utils/utils.hpp"
#include "disassembly.hpp"
#include "disassembler.hpp"

namespace disas8051 {

std::string readLineWithoutPreWhitespaces(std::istream &istream)
{
    char buf[256];
    while(std::isspace(istream.peek())) {
        istream.get();
    }
    istream.getline(buf, 256);
    std::string str(buf);
    if(str.empty()) {
        return str;
    }
    else {
        // do not store \r in the string on linux (on windows, \r\n is automatically converted to \n)
        if(str[str.length() - 1] == '\r') {
            return str.substr(0, str.length() - 1);
        }
        else {
            return str;
        }
    }
}

void AppendBufAddress(wxStyledTextCtrl *textCtrl, uint64_t address)
{
    wxString text = wxString::Format(wxT("%5x "), (int)address);
    textCtrl->AppendText(text);
    textCtrl->SetStyling(text.length(), (int)Styles::BUF_ADDRESS);
}
void InsertBufAddress(wxStyledTextCtrl *textCtrl, int pos, uint64_t address)
{
    wxString text = wxString::Format(wxT("%5x "), (int)address);
    textCtrl->InsertText(pos, text);
    textCtrl->SetStyling(text.length(), (int)Styles::BUF_ADDRESS);
}

void Disassembly::printToWx(wxStyledTextCtrl *textCtrl)
{
    this->textCtrl = textCtrl;
    addressByLine.clear();

    TEXTCTRL_START_STYLING(textCtrl, textCtrl->GetTextLength());

    int undefinedBytes = 0;
    int currentLine = 0;
    for(uint64_t bufAddr = 0; bufAddr < buf.length(); bufAddr++) {
        auto it = instructions.find(bufAddr);
        if(it == instructions.end()) {
            if((undefinedBytes > 0) && (bufAddr % 16 == 0)) {
                textCtrl->AppendText("\n");
                addressByLine[currentLine] = bufAddr - undefinedBytes;
                currentLine++;
                textCtrl->SetStyling(undefinedBytes * 3 + 1, (int)Styles::BYTES);
                undefinedBytes = 0;
            }
            if(undefinedBytes == 0) {
                AppendBufAddress(textCtrl, bufAddr);
            }

            textCtrl->AppendText(wxString::FromUTF8((fn_FormatToHex(buf.substr(bufAddr, 1), true) + ' ').c_str()));
            undefinedBytes++;
        }
        else {
            if(undefinedBytes) {
                textCtrl->AppendText("\n");
                addressByLine[currentLine] = bufAddr - undefinedBytes;
                currentLine++;
                textCtrl->SetStyling(undefinedBytes * 3 + 1, (int)Styles::BYTES);
                undefinedBytes = 0;
            }
            AppendBufAddress(textCtrl, bufAddr);

            it->second.printToWx(textCtrl);
            addressByLine[currentLine] = bufAddr;
            currentLine++;
            bufAddr += it->second.length - 1; // -1 because its incremented by the for loop
            textCtrl->AppendText(wxT("\n"));
            textCtrl->SetStyling(1, (int)Styles::DEFAULT);
            
            int iSwitchStatement = findSwitchStatementOnExact(it->first);
            if(iSwitchStatement >= 0) {
				bufAddr = it->first + it->second.length;
				std::vector<SwitchStatement::ValuesCase> valuesCases = switchStatements[(std::size_t)iSwitchStatement].groupCasesByValues();
				for(SwitchStatement::ValuesCase valuesCase : valuesCases) {
					AppendBufAddress(textCtrl, bufAddr);
					
					std::string str = "case " + valuesCase.formatValues() + ": goto " + formatWithFunction(valuesCase.address);
					textCtrl->AppendText(wxString::FromUTF8(str.c_str()));
					textCtrl->SetStyling(str.length(), (int)Styles::DEFAULT);
					
					addressByLine[currentLine] = bufAddr;
					currentLine++;
					textCtrl->AppendText(wxT("\n"));
					textCtrl->SetStyling(1, (int)Styles::DEFAULT);
				}
				bufAddr += switchStatements[iSwitchStatement].getDataLength() - 1; // -1 because its incremented by the for loop
			}
        }
    }
    textCtrl->SetStyling(undefinedBytes * 3, (int)Styles::BYTES);
    addressByLine[currentLine] = buf.length() - undefinedBytes;
}

int Disassembly::findLineByAddress(int address)
{
    if((address < 0) || ((std::size_t)address >= buf.length())) {
        return -1;
    }

    for(auto it = addressByLine.begin(); it != addressByLine.end(); it++) {
        if(it->second >= (uint64_t)address) {
            return it->first;
        }
    }
    return -1;
}

int Disassembly::openMetaFile(std::string filename)
{
    std::ifstream istream(filename);
    if(!istream) {
        return -1;
    }

    std::string str;
    utils::string_to_int_limited_converter convertobj;
    int line = 0;
    while(true) {
        line++;
        istream >> str;
        if(istream.eof()) {
            break;
        }
        uint64_t address = convertobj.ConvertToLLI_Hex_Limited(str, 0, std::numeric_limits<long long int>::max());
        if(convertobj.MistakeHappened() || convertobj.LimitMistakeHappened()) {
            logger->LogError("file " + filename + " contains an invalid address on line " + std::to_string(line + 1));
            return -2;
        }

        istream >> str;
        if((str == "itr") || (str == "fnc")) {
            Function fnc;
            fnc.isInterrupt = (str == "itr");
            
            istream >> str;
            if(str == "shown") {
				fnc.isShownInGraph = true;
			}
			else if(str == "hidden") {
				fnc.isShownInGraph = false;
			}
			else {
				logger->LogError("file " + filename + " contains an invalid shown/hidden value on line " + std::to_string(line + 1));
                istream.close();
                return -2;
			}
			
            istream >> str;
            fnc.posX = (int)convertobj.ConvertToLLI_Limited(str, -15000, 15000);
            istream >> str;
            fnc.posY = (int)convertobj.ConvertToLLI_Limited(str, -15000, 15000);
            if(convertobj.MistakeHappened() || convertobj.LimitMistakeHappened()) {
                logger->LogError("file " + filename + " contains an invalid position on line " + std::to_string(line + 1));
                istream.close();
                return -2;
            }
            str = readLineWithoutPreWhitespaces(istream);
            fnc.name = str;
            if(functions.find(address) != functions.end()) {
                logger->LogWarning("file " + filename + " line " + std::to_string(line + 1) + 
                        ": Multiple definitions for function on address " + std::to_string(address) + ", using last one.");
            }
            functions[address] = fnc;
        }
        else if(str == "cmt") {
            str = readLineWithoutPreWhitespaces(istream);
            auto itComment = pendingComments.find(address);
            if(itComment == pendingComments.end()) {
                pendingComments[address] = str;
            }
            else {
                itComment->second += str;
            }
        }
        else if(str == "rcl") {
			auto itRemapCall = remapCalls.find(address);
			if(itRemapCall != remapCalls.end()) {
                logger->LogWarning("file " + filename + " line " + std::to_string(line + 1) + 
                        ": Multiple definitions for remap call on address " + std::to_string(address) + ", using last one.");
            }
			remapCalls[address] = RemapCall{};
		}
        else if(str == "ign") {
			auto itIgnoreBlock = ignoreBlocks.find(address);
			if(itIgnoreBlock != ignoreBlocks.end()) {
                logger->LogWarning("file " + filename + " line " + std::to_string(line + 1) + 
                        ": Multiple definitions for ignore on address " + std::to_string(address) + ", using last one.");
            }
			ignoreBlocks[address] = IgnoreBlock{};
		}
        else {
            logger->LogError("file " + filename + " line " + std::to_string(line + 1) + ": Invalid type \"" + str + '"');
            istream.close();
            return -2;
        }
    }
    istream.close();
    return 0;
}

int Disassembly::saveMetaFile(std::string filename)
{
    std::ofstream ostream(filename, std::ios_base::out);
    if(!ostream) {
        return -1;
    }

    for(const auto &pair : instructions) {
        if(!pair.second.comment.empty()) {
            std::string address = utils::Int_To_String_Hex(pair.first);
            ostream << std::string(5 - address.length(), ' ') << address << " cmt " << pair.second.comment << std::endl; 
        }
    }

    for(const auto &pair : functions) {
        std::string address = utils::Int_To_String_Hex(pair.first);
        ostream << std::string(5 - address.length(), ' ') << address;
        ostream << (pair.second.isInterrupt ? " itr " : " fnc ");
        ostream << (pair.second.isShownInGraph ? "shown   " : "hidden  ");
        ostream << pair.second.posX << " " << pair.second.posY << " " << pair.second.name << std::endl;
    }
    
    for(const auto &pair : remapCalls) {
        std::string address = utils::Int_To_String_Hex(pair.first);
        ostream << std::string(5 - address.length(), ' ') << address << " rcl" << std::endl;
	}
    
    for(const auto &pair : ignoreBlocks) {
        std::string address = utils::Int_To_String_Hex(pair.first);
        ostream << std::string(5 - address.length(), ' ') << address << " ign" << std::endl;
	}

    ostream.close();
    return 0;
}

void Disassembly::resolveRemapCalls(Disassembler *disassembler)
{
	for(auto &pair : remapCalls) {
		auto itInstruction = instructions.find(pair.first);
		if(itInstruction == instructions.end()) {
			disassembler->followCodePath(buf, pair.first, this);
			itInstruction = instructions.find(pair.first);
		}
		
		if(itInstruction->second.getName() != "MOV DPTR, ") {
			logger->LogWarning("Skipped RemapCall on address " + utils::Int_To_String_Hex(pair.first)
					+ " because it does not start with a MOV DPTR instruction.");
			pair.second.targetAddress = -1;
			continue;
		}
		pair.second.targetAddress = itInstruction->second.arg1;
		
		uint64_t nextAddress = itInstruction->first + itInstruction->second.length;
		itInstruction++;
		if(itInstruction->first != nextAddress) {
			logger->LogWarning("Skipped RemapCall on address " + utils::Int_To_String_Hex(pair.first)
					+ " because MOV DPTR instruction is not directly followed by another instruction.");
			pair.second.targetAddress = -1;
			continue;
		}
		
		if(!itInstruction->second.isJump) {
			logger->LogWarning("Skipped RemapCall on address " + utils::Int_To_String_Hex(pair.first)
					+ " because second instruction is not a jump.");
			pair.second.targetAddress = -1;
			continue;
		}
		auto itFunction = functions.find(itInstruction->second.address);
		if(itFunction == functions.end()) {
			logger->LogWarning("Skipped RemapCall on address " + utils::Int_To_String_Hex(pair.first)
					+ " because second instruction does not jump to a named function.");
			pair.second.targetAddress = -1;
			continue;
		}
		
		int iOffsetBegin = itFunction->second.name.length() - 1;
		while((iOffsetBegin >= 0) && utils::is_hexchar(itFunction->second.name[iOffsetBegin]))  {
			iOffsetBegin--;
		}
		utils::string_to_int_converter convertobj;
		pair.second.targetAddress += convertobj.ConvertToLLI_Hex(itFunction->second.name.substr(
				iOffsetBegin + 1, itFunction->second.name.length() - 1 - iOffsetBegin));
		if(convertobj.MistakeHappened()) {
			logger->LogWarning("Skipped RemapCall on address " + utils::Int_To_String_Hex(pair.first)
					+ " because second instruction jump to a function without a valid hex address at the end of its name.");
			pair.second.targetAddress = -1;
			continue;
		}
		std::cout << "jump to " + utils::Int_To_String_Hex(pair.second.targetAddress) << std::endl;
		
		if(instructions.find(pair.second.targetAddress) == instructions.end()) {
			logger->LogInformation("Remap Call on address " + utils::Int_To_String_Hex(pair.first) + 
					" to address " + utils::Int_To_String_Hex(pair.second.targetAddress) + ": Disassembling target code.");
			disassembler->followCodePath(buf, pair.second.targetAddress, this);
		}
	}
}

void Disassembly::resolveComments()
{
    for(auto itComment = pendingComments.begin(); itComment != pendingComments.end(); itComment++) {
        auto itInstruction = instructions.find(itComment->first);
        if(itInstruction == instructions.end()) {
            logger->LogWarning("Address " + utils::Int_To_String_Hex(itComment->first) + " for comment \"" + 
                    itComment->second + "\" could not be resolved to an instruction.");
        }
        else {
            itInstruction->second.comment += itComment->second;
        }
    }
}

struct RegisterArea
{
    int addressFirst;
    int addressLast;
    int size; // is checked
    const char *name;

    bool contains(int address) {
        return (address >= addressFirst) && (address <= addressLast);
    }
};
static RegisterArea registerAreas[] = {
    { 0xfc00, 0xfc0f, 16, "GPIO function sel" },
    { 0xfc10, 0xfc1f, 16, "GPIO output enable" },
    { 0xfc20, 0xfc2f, 16, "GPIO output" },
    { 0xfc30, 0xfc3f, 16, "GPIO input" },
    { 0xfc40, 0xfc4f, 16, "GPIO pull-up enable" },
    { 0xfc50, 0xfc5f, 16, "GPIO open drain output enable" },
    { 0xfc60, 0xfc6f, 16, "GPIO input enable" },
    { 0xfc70, 0xfc7f, 16, "GPIO various" },
    { 0xfc80, 0xfc8f, 16,  "Keyboard Controller" },
    { 0xfc90, 0xfc9f, 16,  "ENE serial bus controller" },
    { 0xfca0, 0xfcaf, 16,  "Internal keyboard matrix" },
    { 0xfcb0, 0xfcbf, 16,  "for ESB?" },
    { 0xfcc0, 0xfccf, 16,  "for ESB?" },
    { 0xfcd0, 0xfcdf, 16,  "PECI controller" },
    { 0xfce0, 0xfcef, 16,  "Reserved" },
    { 0xfcf0, 0xfcff, 16,  "One Wire Master" },
    { 0xfd00, 0xfdff, 256, "for ESB?" },
    { 0xfe00, 0xfe1f, 32,  "Pulse width modulation" },
    { 0xfe20, 0xfe4f, 48,  "Fan controller" },
    { 0xfe50, 0xfe6f, 32,  "General purpose timer" },
    { 0xfe70, 0xfe7f, 16,  "SPI host/device interface"},
    { 0xfe80, 0xfe8f, 16,  "Watchdog timer" },
    { 0xfe90, 0xfe9f, 16,  "Low pin count interface" },
    { 0xfea0, 0xfebf, 32,  "X-bus interface" },
    { 0xfec0, 0xfecf, 16,  "Consumer IR controller" },
    { 0xfed0, 0xfedf, 16,  "General Waveform Generation" },
    { 0xfee0, 0xfeff, 32,  "PS/2 interface" },
    { 0xff00, 0xff2f, 48,  "Embedded Controller" },
    { 0xff30, 0xff7f, 80,  "General purpose wakeup event" },
    { 0xff80, 0xff8f, 16,  "Reserved" },
    { 0xff90, 0xffbf, 48,  "SMBus controller 0" },
    { 0xffc0, 0xffcf, 16,  "Reserved" },
    { 0xffd0, 0xffff, 48,  "SMBus controller 1" }
};

void Disassembly::autoComment()
{
    int registerAreaCount = sizeof(registerAreas) / sizeof(*registerAreas);
    // check sizes
    for(int i1 = 0; i1 < registerAreaCount; i1++) {
        if(registerAreas[i1].addressFirst + registerAreas[i1].size - 1 != registerAreas[i1].addressLast) {
            logger->LogWarning(std::string("Invalid registerArea ") + registerAreas[i1].name);
        }
    }

    for(auto itInstruction = instructions.begin(); itInstruction != instructions.end(); itInstruction++) {
        if(itInstruction->second.getName() == "MOV DPTR, ") {
            for(int i1 = 0; i1 < registerAreaCount; i1++) {
                if(registerAreas[i1].contains(itInstruction->second.arg1)) {
                    itInstruction->second.ac(std::string("Reg ") + registerAreas[i1].name);
                    //std::cout << utils::Int_To_String_Hex(itInstruction->first) << " " << itInstruction->second.autocomment << std::endl;
                    break;
                }
            }
        }

        if(itInstruction->second.isJump || itInstruction->second.isCondJump) {
            auto itFunction = findFunction(itInstruction->second.address);
            if(itFunction != functions.end()) {
				if(!itFunction->second.contains(itInstruction->first)) { // do not mark itself
				//if((itInstruction->first < itFunction->first) || (itInstruction->first >= itFunction->second.endAddress)) {
					if(itFunction->first == itInstruction->second.address) {
						itInstruction->second.ac("Fnc " + itFunction->second.name);
					}
					else {
						// mark it if it does not jump to the beginning of the function
						itInstruction->second.ac("FncPart " + itFunction->second.name);
					}
				}
            }
            
            auto itRemapCall = remapCalls.find(itInstruction->second.address);
            if(itRemapCall != remapCalls.end()) {
				if(itRemapCall->second.targetAddress >= 0) {
					auto itFunction = findFunction(itRemapCall->second.targetAddress);
					if(itFunction != functions.end()) {
						if(itFunction->first == itRemapCall->second.targetAddress) {
							itInstruction->second.ac("Rcl to Fnc " + itFunction->second.name);
						}
						else {
							itInstruction->second.ac("Rcl to FncPart " + itFunction->second.name);
						}
					}
					else {
						itInstruction->second.ac("Rcl to " + utils::Int_To_String_Hex(itRemapCall->second.targetAddress));
					}
				}
			}
        }
    }
    
    for(const auto &pair : functions) {
		for(const Function::Block &block : pair.second.blocks) {
			auto itInstruction = instructions.find(block.begin);
			if(itInstruction->second.autocomment.empty()) {
				itInstruction->second.ac("Bgn " + pair.second.name);
			}
			
			itInstruction = instructions.find(block.end);
			if((itInstruction != instructions.end()) && (itInstruction != instructions.begin())) {
				itInstruction--;
				if(itInstruction->first + itInstruction->second.length == block.end) {
					if(itInstruction->second.autocomment.empty()) {
						itInstruction->second.ac("End " + pair.second.name);
					}
				}
			}
		}
	}
	
	for(const SwitchStatement &switchStatement : switchStatements) {
		std::vector<SwitchStatement::ValuesCase> valuesCases = switchStatement.groupCasesByValues();
		for(SwitchStatement::ValuesCase valuesCase : valuesCases) {
			auto itInstruction = instructions.find(valuesCase.address);
			if(itInstruction == instructions.end()) {
				logger->LogError("Switch on " + utils::Int_To_String_Hex(switchStatement.address) + 
						": Should have an instruction for case block on address " + utils::Int_To_String_Hex(valuesCase.address));
				continue;
			}
			
			itInstruction->second.ac("case " + valuesCase.formatValues() + " for switch on " + 
					formatWithFunction(switchStatement.address));
		}
	}
}

void Disassembly::resolveFunctionEndAddresses()
{    
    for(auto itFunction = functions.begin(); itFunction != functions.end(); itFunction++) {
		itFunction->second.blocks.clear();
	}
    
    for(auto itFunction = functions.begin(); itFunction != functions.end(); itFunction++) {
		auto itNextFunction = itFunction;
		itNextFunction++;
		
		// if another function already has a block on the same address, delete or shrink the block,
		// because the user has expressed to start a new function on this address by adding this function
		auto itRivalFunction = findFunction(itFunction->first);
		if(itRivalFunction != functions.end()) {
			for(auto itBlock = itRivalFunction->second.blocks.begin(); itBlock != itRivalFunction->second.blocks.end(); itBlock++) {
				if(itBlock->contains(itFunction->first)) {
					//std::cout << "extra function " << itFunction->second.name << " " << itRivalFunction->second.name << " " << utils::Int_To_String_Hex(itFunction->first) << " " << utils::Int_To_String_Hex(itBlock->begin) << std::endl;
					if(itBlock->begin == itFunction->first) {
						// the block has no code before the beginning of the function
						itRivalFunction->second.blocks.erase(itBlock);
					}
					else {
						itBlock->end = itFunction->first;
					}
					break;
				}
			}
		}
		
		recurseFunction(itFunction, itNextFunction, itFunction->first);
		itFunction->second.defragmentBlocks();
	}
}
void Disassembly::recurseFunction(std::map<uint64_t, Function>::iterator itFunction, std::map<uint64_t, Function>::iterator itNextFunction, uint64_t address)
{
	auto itInstruction = instructions.find(address);
	if(itInstruction == instructions.end()) {
		logger->LogError("Disassembly: Function " + itFunction->second.name + 
			" should have been parsed before on address " + utils::Int_To_String_Hex(address));
		return;
	}
	
	auto itNextFunctionInstruction = instructions.end();
	if(itNextFunction != functions.end()) {
		itNextFunctionInstruction = instructions.find(itNextFunction->first);
		if(itNextFunctionInstruction == instructions.end()) {
			logger->LogError("Disassembly: Next Function " + itNextFunction->second.name + 
				" should have been parsed before on address " + utils::Int_To_String_Hex(itNextFunction->first));
			return;
		}
	}
	
	Function::Block newBlock;
	newBlock.begin = address;
	newBlock.end = itInstruction->first + itInstruction->second.length;
	while(itInstruction != itNextFunctionInstruction) {		
		auto itOtherFunction = findFunction(itInstruction->first);
		if(itOtherFunction == itFunction) {
			// this block was already added to the function
			// but we came maybe from a few bytes before, so find the block and add this to the block
			for(Function::Block &block : itOtherFunction->second.blocks) {
				if(block.contains(itInstruction->first)) {
					block.begin = std::min(block.begin, newBlock.begin);
					return;
				}
			}
			logger->LogError("Disassembly: Should have found block");
		}
		else if(itOtherFunction != functions.end()) {
			logger->LogWarning("Disassembly: Function " + itFunction->second.name + " has some parts on address " + 
					utils::Int_To_String_Hex(itInstruction->first) + ", but this block is also used by function " + 
					itOtherFunction->second.name);
			newBlock.end = itInstruction->first;
			if(newBlock.end > newBlock.begin) { // do not store "empty" blocks
				itFunction->second.blocks.push_back(newBlock);
			}
			return;
		}
		
		if((itInstruction->second.getName() == "RET") || (itInstruction->second.getName() == "RETI") || (itInstruction->second.getName() == "JMP @A+DPTR")) {
			itFunction->second.blocks.push_back(newBlock);
			return;
		}
		if(itInstruction->second.isJump) {
			if(itInstruction->second.address != newBlock.end) { // sometimes, there are strange jumps to the next address
				itFunction->second.blocks.push_back(newBlock);
				recurseFunction(itFunction, functions.end(), itInstruction->second.address);
				return;
			}
		}
		if(itInstruction->second.isCondJump && (!itInstruction->second.isCall())) {
			itFunction->second.blocks.push_back(newBlock); // push the block temporarily, so that it is check by findFunction() to avoid endless recursion
			recurseFunction(itFunction, functions.end(), itInstruction->second.address);
			// now find the block we stored before and remove it from the blocks
			// we cannot rely on an index, because other recurseFunction calls might also use an index
			// we also cannot use a pointer, because std::vector sometimes changes the space allocated to a function
			// (but it would be ok if we would allocate the functions with new)
			for(auto itBlock = itFunction->second.blocks.begin(); itBlock != itFunction->second.blocks.end(); itBlock++) {
				if(itBlock->begin == newBlock.begin) { 
					// we found our block
					itFunction->second.blocks.erase(itBlock);
					break;
				}
			}
		}

		itInstruction++;
		if(newBlock.end < itInstruction->first) {
			// some unparsed bytes in between
			break;
		}
		newBlock.end = itInstruction->first + itInstruction->second.length;
	}
	// we got to the next function or the end
	itFunction->second.blocks.push_back(newBlock);
}

void Disassembly::addFunction(int address, Function function)
{
    functions[address] = function;
    if(instructions.find(address) == instructions.end()) {
        Disassembler disassembler(logger);
        disassembler.followCodePath(buf, address, this);

        // now we changed so much that it is easier to simply recalc the text box
        int oldCaretPos = textCtrl->GetCurrentPos();
        textCtrl->ClearAll();
        printToWx(textCtrl);
        textCtrl->GotoPos(oldCaretPos);
    }
    else {
		// TODO this only changes comments for direct calls to the function, but not those who call 
		// a part of this function.
        auto itTarget = jumpTargets.find(address);
        if(itTarget != jumpTargets.end()) {
            for(uint64_t sourceAddress : itTarget->second) {
                instructions[sourceAddress].autocomment += "Fnc " + function.name;
                updateInstructionComment(sourceAddress, instructions[sourceAddress].comment);
            }
        }
        updateFunctionName(address, function.name);
    }
    resolveFunctionEndAddresses();
}
void Disassembly::updateFunctionName(int address, std::string newName)
{
    Function &function = functions[address];
    // TODO this only changes comments for direct calls to the function, but not those who call 
    // a part of this function.
    auto itTarget = jumpTargets.find(address);
    if(itTarget != jumpTargets.end()) {
        for(uint64_t sourceAddress : itTarget->second) {
            utils::str_replace_one(instructions[sourceAddress].autocomment, "Fnc " + function.name, "Fnc " + newName);
            updateInstructionComment(sourceAddress, instructions[sourceAddress].comment);
        }
    }
    function.name = newName;
}
void Disassembly::updateInstructionComment(int address, std::string newComment)
{
    instructions[address].comment = newComment;
    updateLineAtAddress(address);
}

void Disassembly::updateLineAtAddress(int address)
{
    int line = findLineByAddress(address);
    int startPos = textCtrl->PositionFromLine(line);
    int lineLength = textCtrl->LineLength(line) - 1; // -1 to not delete the \n
    textCtrl->DeleteRange(startPos, lineLength);
    TEXTCTRL_START_STYLING(textCtrl, startPos);
    InsertBufAddress(textCtrl, startPos, address);
 
    instructions[address].printToWx(textCtrl, startPos + 6);
}

void Disassembly::deleteFunction(int address)
{
    Function functionCopy = functions[address];
    functions.erase(address);
    resolveFunctionEndAddresses();
    
    // TODO this only changes comments for direct calls to the function, but not those who call 
    // a part of this function.
    auto itTarget = jumpTargets.find(address);
    if(itTarget != jumpTargets.end()) {
        for(uint64_t sourceAddress : itTarget->second) {
            utils::str_replace_one(instructions[sourceAddress].autocomment, "Fnc " + functionCopy.name, "");
            updateInstructionComment(sourceAddress, instructions[sourceAddress].comment);
        }
    }
}

std::map<uint64_t, Function>::iterator Disassembly::findFunction(int addressIn)
{
	for(auto itFunction = functions.begin(); itFunction != functions.end(); itFunction++) {
		if(itFunction->second.contains(addressIn)) {
			return itFunction;
		}
	}
	return functions.end();
	
    /*auto itFunction = functions.begin();
    while(addressIn >= itFunction->second.endAddress) {
        itFunction++;
        if(itFunction == functions.end()) {
            return functions.end();
        }
    }
    if(addressIn >= itFunction->first) {
        return itFunction;
    }
    else {
        return functions.end();
    }*/
}

std::vector<uint64_t> Disassembly::GetFunctionTargetAddresses(uint64_t functionAddress)
{
    std::vector<uint64_t> ret;
    
    const Function &function = functions[functionAddress];
    for(const Function::Block &block : function.blocks) {
		auto itInstruction = instructions.find(block.begin);
		if(itInstruction == instructions.end()) {
			logger->LogError("Invalid block with address " + utils::Int_To_String_Hex(block.begin) + 
					" of function " + function.name);
			continue;
		}
		while(itInstruction->first < block.end) {
			if(itInstruction->second.isJump || itInstruction->second.isCondJump) {
				ret.push_back(itInstruction->second.address);
			}
			itInstruction++;
		}
	}
    return ret;
}

bool Disassembly::seemsToBeInvalidCode(uint64_t address)
{
	if(address + 2 < buf.size()) {
		return (buf[address] == buf[address + 1]) && (buf[address] == buf[address + 2]);
	}
	return false;
					
}

std::string Disassembly::formatWithFunction(uint64_t address)
{
    std::string str = utils::Int_To_String_Hex(address);
    auto itFunction = findFunction(address);
    if(itFunction != functions.end()) {
        str += " (" + itFunction->second.name + ")";
    }
    return str;
}

int Disassembly::findSwitchStatementOnExact(uint64_t address)
{
	for(std::size_t iSwitchStatement = 0; iSwitchStatement < switchStatements.size(); iSwitchStatement++) {
		if(switchStatements[iSwitchStatement].address == address) {
			return iSwitchStatement;
		}
	}
	return -1;
}

} // namespace disas8051
