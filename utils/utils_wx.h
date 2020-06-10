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
 
#ifndef UTILS_WX_INCLUDED
#define UTILS_WX_INCLUDED

#include "wx/dc.h"
#include "wx/dcmemory.h"
#include "wx/string.h"

#include "utils.hpp"

namespace utils {

wxString Format_Defaultdate_wxString(long long int datum);

} // namespace utils


wxString fn_Char_To_wxUTF8String(uint32_t zeichen);
std::string fn_wxUTF8String_ToUTF8String(wxString string); // Unter Linux(Ubuntu), vielleicht auch unter Windows, ergibt
                                    // z. B. wxString(wxT("abc\x00e4sb")).c_str().AsChar() den String "" - diese Funktion konvertiert im Gegensatz dazu korrekt zu UTF8
                                    // (ähnlich wie fn_winString_ToUTF8String)

enum wxbuildinfoformat {
    wxbuildinfof_short_f, wxbuildinfof_long_f };
wxString fn_wxbuildinfo(wxbuildinfoformat format);

#endif // UTILS_WX_INCLUDED
