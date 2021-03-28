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
 
#include <unistd.h>

#include "wx/cmdline.h"
#include "wx/filedlg.h"
#include "wx/filename.h"

#include "wxApp.h"
#include "wxMain.h"

#include "utils/utils_wx.h"

#ifndef _WIN32
#include "res/kassette.xpm"
#endif

IMPLEMENT_APP(Dis8051App);

const std::string FILE_UNDEFINED_STR = "undefined";

Dis8051App::Dis8051App()
{
    firmwareFile = FILE_UNDEFINED_STR;
    metaFile = FILE_UNDEFINED_STR;
}

void Dis8051App::OnInitCmdLine(wxCmdLineParser &parser)
{
    // Wichtig: Unicode-Zeichen sorgen daf�r, dass ./programm --help nichts ausgibt!
    wxApp::OnInitCmdLine(parser);
    parser.AddSwitch(wxT("v"), wxT("version"), wxT("Gibt die Version aus"));
    parser.AddLongOption(wxT("mfile"), wxT("Metadata filename, e. g. meta.txt, default: <firmware_file>.txt"), 
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    parser.AddParam(wxT("Firmware filename, e. g. in.bin"), 
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
}
bool Dis8051App::OnCmdLineParsed(wxCmdLineParser &parser)
{
    if(!(wxApp::OnCmdLineParsed(parser)))
    {
        std::cout << "Error parsing the command line." << std::endl;
        return false;
    }

    //Check if the user asked for the version
    if(parser.Found(wxT("v")))
    {
#ifndef __WXMSW__
        wxLog::SetActiveTarget(new wxLogStderr);
#endif // __WXMSW__
        wxLogMessage(wxT("8051 disassembler") wxT("\nBuild-Datum: ") + wxString::FromAscii(__DATE__) + wxT("\nCopyright Matthias Rosenthal"));
        return false;
    }

    wxString wxs_zws1;
    if(parser.Found(wxT("mfile"), &wxs_zws1)) {
        metaFile = fn_wxUTF8String_ToUTF8String(wxs_zws1);
    }

    //Check for a filename
    if(parser.GetParamCount() > 0)
    {
        wxString wxFirmwareFile = parser.GetParam(0);
        //Under Windows when invoking via a document in Explorer, we are passed the short form.
        //So normalize and make the long form.
        wxFileName fName(wxFirmwareFile);
        fName.Normalize(wxPATH_NORM_LONG|wxPATH_NORM_DOTS|wxPATH_NORM_TILDE|wxPATH_NORM_ABSOLUTE);
        firmwareFile = fn_wxUTF8String_ToUTF8String(fName.GetFullPath());
    }

    if(firmwareFile == FILE_UNDEFINED_STR) {
        wxFileDialog openFileDialog(nullptr, _("Open firmware file"), "", "",
                        "Binary files (*.*)|*.*", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
        if (openFileDialog.ShowModal() != wxID_OK) {
            return false;
        }
        
        firmwareFile = fn_wxUTF8String_ToUTF8String(openFileDialog.GetPath());
    }

    if(metaFile == FILE_UNDEFINED_STR) {
        metaFile = firmwareFile + ".txt";
    }

    return true;
}

bool Dis8051App::OnInit()
{
	if(!wxApp::OnInit()) {
        return false;
    }
	
    Dis8051Frame* frame = new Dis8051Frame(0L, wxT("interactive 8051 disassembler"), firmwareFile, metaFile, this);
#ifdef _WIN32
    frame->SetIcon(wxICON(aaaa)); // described in resource file
#else
    frame->SetIcon(wxIcon(kassette_xpm));
#endif
    frame->Show();

    return true;
}
int Dis8051App::OnExit()
{
    std::cout << "bye." << std::endl;
    return 0;
}
