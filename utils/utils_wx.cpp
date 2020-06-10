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
 
#include <iostream>

#include "utils_wx.h"

namespace utils {

wxString Format_Defaultdate_wxString(long long int datum)
{
    time_t sec = datum / 1000;
    long int ms = datum - ((long long int)sec) * 1000;
    char* c_datum = new char[100];
    tm* tmZeit = localtime(&sec);
    std::strftime(c_datum, 100, "%d.%m.%Y %X", tmZeit);
    wxString ret = wxString(c_datum);
    if(ms >= 100)
    {
    #ifdef _WIN32
        ret.append(wxString::Format(wxT(":%li"), ms)); // %i
    #else
        ret.append(wxString::Format(wxT(":%d"), (int)ms));
    #endif
    }
    else if(ms >= 10)
    {
    #ifdef _WIN32
        ret.append(wxString::Format(wxT(":0%li"), ms)); // %i
    #else
        ret.append(wxString::Format(wxT(":0%d"), (int)ms));
    #endif
    }
    else
    {
    #ifdef _WIN32
        ret.append(wxString::Format(wxT(":00%li"), ms)); // %i
    #else
        ret.append(wxString::Format(wxT(":00%d"), (int)ms));
    #endif
    }
    delete c_datum;
    return ret;
}

} // namespace utils

wxString fn_Char_To_wxUTF8String(uint32_t zeichen)
{
    std::string stdstr = utils::Char_To_UTF8String(zeichen);
    return wxString::FromUTF8(stdstr.c_str(), stdstr.length());
}
std::string fn_wxUTF8String_ToUTF8String(wxString string)
{
    std::string stdstr;
    for(wxString::iterator it_char = string.begin(); it_char != string.end(); it_char++) {
        stdstr.append(utils::Char_To_UTF8String((*it_char).GetValue())); // (*it_char) ist ein Objekt vom Typ wxChar, wxChar ist wxUniChar, wxUniChar::GetValue() gibt wxUint32 zurück
    }
    return stdstr;
}

wxString fn_wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == wxbuildinfof_long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__WXMAC__)
        wxbuild << _T("-Mac");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}
