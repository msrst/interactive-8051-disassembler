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
 
#ifndef SCROLL_OVERVIEW_HPP_INCLUDED
#define SCROLL_OVERVIEW_HPP_INCLUDED

#include "wx/scrolwin.h"

class Dis8051Frame;

class ScrollOverview : public wxScrolledWindow
{
private:
    Dis8051Frame *mainFrame;
    int w,h;
public:
    ScrollOverview(Dis8051Frame *mainFrame, wxWindow *parent, wxWindowID id);
    ~ScrollOverview() {}

private:
    void OnDraw(wxDC& dc);
    void OnLeftDown(wxMouseEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif // SCROLL_OVERVIEW_HPP_INCLUDED
