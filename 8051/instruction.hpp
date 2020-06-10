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
 
#ifndef INSTRUCTION_8051_HPP_INCLUDED
#define INSTRUCTION_8051_HPP_INCLUDED

#include <string>
#include "wx/version.h"

class wxStyledTextCtrl;

#if wxCHECK_VERSION(3, 1, 0)
// this removes a depracted warning
#define TEXTCTRL_START_STYLING(textCtrl, pos) (textCtrl)->StartStyling(pos)
#else 
// StartStyling(pos) is not available in older wx versions
#define TEXTCTRL_START_STYLING(textCtrl, pos) (textCtrl)->StartStyling(pos, 0xff)
#endif

namespace disas8051 {

enum class Styles : int {
    DEFAULT = 0,
    BUF_ADDRESS,
    BYTES,
    RET_INSTRUCTION,
    COMMENT
};

typedef std::basic_string<uint8_t> ustring;

class Instruction {
public:
    uint8_t opNum;
	int length;
	int32_t arg1;
    int arg1Type;
    int32_t arg2;
    int arg2Type;
    bool isJump;
    bool isCondJump;
    uint64_t address;

    std::string autocomment;
    std::string comment;

	void ac(std::string c); // adds an autocomment
    std::string getFullComment();

    std::string getName();
    bool isCall();

    std::string formatSimple();
    void printToWx(wxStyledTextCtrl *textCtrl);
    void printToWx(wxStyledTextCtrl *textCtrl, int pos);
};

} // namespace disas8051

#endif // INSTRUCTION_8051_HPP_INCLUDED
