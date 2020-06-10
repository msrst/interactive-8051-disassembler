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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "wx/stc/stc.h"

#include "instruction.hpp"
#include "disassembler.hpp"

namespace disas8051 {

std::string printArgument(int argType, int32_t value)
{
    char *outBuffer = new char[10];
    outBuffer[0] = '\0';

    switch(argType)
    {
    case NONE:        
        break;
    case ADDR11:       
        sprintf(outBuffer, "0x%04X", value);
        break;
    case ADDR16:
        sprintf(outBuffer, "0x%04X", value);
        break;
	case DIRECT:
        if (value < 0x80) sprintf(outBuffer, "X%02Xh", value);
        else strcpy(outBuffer, SFR[value - 0x80]);
        break;
    case IMMED:
        sprintf(outBuffer, "#0x%02X", value);
        break;
    case IMMED16:
        sprintf(outBuffer, "#0x%04X", value);
        break;
    case BIT:
        //the last 128 bits are in 80, 88, 90, 98 that is 0x80 and upward 8 at a time
        if (value < 0x80) //bits from bytes 0x20 to 0x2F
        {
            sprintf(outBuffer, "X%X.%d", 0x20 + (value / 8), value % 8);
        }
        else
        {
            sprintf(outBuffer, "%s.%d", SFR[((value & 0xF8)-0x80)], value & 0x07);
        }
        break;
    case OFFSET:
        sprintf(outBuffer, "0x%04X", value);
        break;
    case A:
        sprintf(outBuffer, "A");
        break;
    case C:
        sprintf(outBuffer, "C");
        break;
    case IR0:
        sprintf(outBuffer, "@R0");
        break;
    case IR1:
        sprintf(outBuffer, "@R1");
        break;
    case R0:
        sprintf(outBuffer, "R0");
        break;
    case R1:
        sprintf(outBuffer, "R1");
        break;
    case R2:
        sprintf(outBuffer, "R2");
        break;
    case R3:
        sprintf(outBuffer, "R3");
        break;
    case R4:
        sprintf(outBuffer, "R4");
        break;
    case R5:
        sprintf(outBuffer, "R5");
        break;
    case R6:
        sprintf(outBuffer, "R6");
        break;
    case R7:
        sprintf(outBuffer, "R7");
        break;
    }

    std::string str(outBuffer);
    delete[] outBuffer;
    return str;
}

std::string Instruction::getName()
{
    return OpCodes[opNum].name;
}
bool Instruction::isCall()
{
	return (getName() == "LCALL ")
			|| (getName() == "ACALL ");
}
			

std::string Instruction::formatSimple()
{
    std::string arg1Str;
    std::string arg2Str;

    if (opNum != 0x85) {
        arg1Str = printArgument(arg1Type, arg1);
        arg2Str = printArgument(arg2Type, arg2);
    }
    else  //some joker decided that this instruction and this instruction only should have reversed operands. Why?! I guess it was really funny.
    {        
        arg2Str = printArgument(arg1Type, arg1);
        arg1Str = printArgument(arg2Type, arg2);
    }
    
    if (arg2Str.empty()) {
        if(arg1Str.empty()) {
            return getName();
        }
        else {
            return getName() + ' ' + arg1Str;
        }
    }
    else {
        return getName() + ' ' + arg1Str + ", " + arg2Str;
    }
}

void Instruction::ac(std::string c)
{
	if(autocomment.empty()) {
		autocomment = c;
	}
	else {
		autocomment += ", ";
		autocomment += c;
	}
}
std::string Instruction::getFullComment()
{
    if(autocomment.empty()) {
        return comment;
    }
    else if(comment.empty()) {
        return autocomment;
    }
    else {
        return autocomment + ", " + comment;
    }
}

void Instruction::printToWx(wxStyledTextCtrl *textCtrl)
{
    std::string fs = formatSimple();
    textCtrl->AppendText(wxString::FromUTF8(fs.c_str()));
    if((fs == "RET") || (fs == "RETI")) {
        textCtrl->SetStyling(fs.length(), (int)Styles::RET_INSTRUCTION);
    }
    else {
        textCtrl->SetStyling(fs.length(), (int)Styles::DEFAULT);
    }

    std::string fullComment = getFullComment();
    if(!fullComment.empty()) {
        fullComment = std::string(24 - fs.size(), ' ') + "; " + fullComment;
        textCtrl->AppendText(wxString::FromUTF8(fullComment.c_str()));
        textCtrl->SetStyling(fullComment.length(), (int)Styles::COMMENT);
    }
}

void Instruction::printToWx(wxStyledTextCtrl *textCtrl, int pos)
{
    std::string fs = formatSimple();
    textCtrl->InsertText(pos, wxString::FromUTF8(fs.c_str()));
    if((fs == "RET") || (fs == "RETI")) {
        textCtrl->SetStyling(fs.length(), (int)Styles::RET_INSTRUCTION);
    }
    else {
        textCtrl->SetStyling(fs.length(), (int)Styles::DEFAULT);
    }

    std::string fullComment = getFullComment();
    if(!fullComment.empty()) {
        fullComment = std::string(24 - fs.size(), ' ') + "; " + fullComment;
        textCtrl->InsertText(pos + fs.length(), wxString::FromUTF8(fullComment.c_str()));
        textCtrl->SetStyling(fullComment.length(), (int)Styles::COMMENT);
    }
}

} // namespace disas8051
