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
