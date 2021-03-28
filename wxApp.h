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
 
#ifndef DISAS8051_APP_H_INCLUDED
#define DISAS8051_APP_H_INCLUDED

#include <wx/app.h>

class Dis8051App : public wxApp
{
private:
    std::string firmwareFile;
    std::string metaFile;

public:
    Dis8051App();

    // for parsing the parameters passed from shell
    virtual void OnInitCmdLine(wxCmdLineParser &parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser &parser);

    virtual bool OnInit();
    virtual int OnExit();
};

#endif // DISAS8051_APP_H_INCLUDED
