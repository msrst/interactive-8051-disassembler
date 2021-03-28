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
 
#include "scroll_overview.hpp"
#include "wxMain.h"
#include <iostream>

BEGIN_EVENT_TABLE(ScrollOverview, wxScrolledWindow)
    EVT_LEFT_DOWN(ScrollOverview::OnLeftDown)
END_EVENT_TABLE()

const int YSCROLL_STEP_SIZE = 10;

ScrollOverview::ScrollOverview(Dis8051Frame* mainFrame, wxWindow *parent, wxWindowID id) : wxScrolledWindow(parent, id, wxDefaultPosition, wxSize(150, 300))
{
    this->mainFrame = mainFrame;
    w = 400;
    h = mainFrame->getDisassembly()->buf.length();

    /* init scrolled area size, scrolling speed, etc. */
    SetScrollbars(1, YSCROLL_STEP_SIZE, w, h/YSCROLL_STEP_SIZE + 1, 0, 0);
}

void ScrollOverview::OnDraw(wxDC& dc)
{
    const int LINE_WIDTH = 80;
    const int CODEVIEW_RECT_WIDTH = 30;
	
	wxPen invalidCodePen = wxPen(wxColor(180, 180, 0));
    wxPen unparsedCodePen = wxPen(wxColor(200, 200, 200));

    int viewStartX, viewStartY, viewSizeX, viewSizeY;
    GetViewStart(&viewStartX, &viewStartY);
    GetClientSize(&viewSizeX, &viewSizeY);
    viewStartY *= YSCROLL_STEP_SIZE;
    int lineBegin, lineEnd;
    mainFrame->GetDisassemblyLinesOnScreen(&lineBegin, &lineEnd);

    auto it = mainFrame->getDisassembly()->instructions.begin();
    int address = viewStartY;
    while(it != mainFrame->getDisassembly()->instructions.end()) {
        if(int(it->first + it->second.length) >= address) {
            break;
        }
        it++;
    }
    int yEnd = std::min(viewStartY + viewSizeY, (int)mainFrame->getDisassembly()->buf.length());
    //std::cout << viewStartX << " " << viewStartY << " " << viewSizeX << " " << viewSizeY << " " << yEnd << " on " << lineBegin << " - " << lineEnd << std::endl;
    if(it != mainFrame->getDisassembly()->instructions.end()) {
        for(; address < yEnd; address++) {
            if(int(it->first + it->second.length) < address) {
                it++;
                if(it == mainFrame->getDisassembly()->instructions.end()) {
                    break;
                }
            }
            if(int(it->first) > address) {
				dc.SetPen(unparsedCodePen);
				if(mainFrame->getDisassembly()->seemsToBeInvalidCode(address)) {
					dc.SetPen(invalidCodePen);
				}
            }
            else {
                if(it->second.getName() == "RET") {
                    dc.SetPen(*wxRED_PEN);
                }
                else if(it->second.getName() == "RETI") {
                    dc.SetPen(*wxCYAN_PEN);
                }
                else {
                    dc.SetPen(*wxWHITE_PEN);
                }
            }
            dc.DrawLine(0, address, LINE_WIDTH, address);
        }
    }
    for(; address < yEnd; address++) {
		if(mainFrame->getDisassembly()->seemsToBeInvalidCode(address)) {
			dc.SetPen(invalidCodePen);
		}
		else {
			dc.SetPen(unparsedCodePen);
		}
        dc.DrawLine(0, address, LINE_WIDTH, address);
    }

    dc.SetBrush(*wxBLACK_BRUSH);
    auto itLineBegin = mainFrame->getDisassembly()->addressByLine.find(lineBegin);
    if(itLineBegin != mainFrame->getDisassembly()->addressByLine.end()) {
        if(std::abs((int)itLineBegin->second - viewStartY) < 5000) { // this is because wxDC seems to clip at
            // 16-bit pixel integers, which results in the current position displayed at 0 but also at 65535
            auto itLineEnd = mainFrame->getDisassembly()->addressByLine.find(lineEnd);
            if(itLineEnd == mainFrame->getDisassembly()->addressByLine.end()) {
                dc.DrawRectangle(LINE_WIDTH + 10, itLineBegin->second, 
                    CODEVIEW_RECT_WIDTH, mainFrame->getDisassembly()->buf.length() - itLineBegin->second);
            }
            else {
                dc.DrawRectangle(LINE_WIDTH + 10, itLineBegin->second, 
                    CODEVIEW_RECT_WIDTH, itLineEnd->second - itLineBegin->second);
            }
        }
    }
}

void ScrollOverview::OnLeftDown(wxMouseEvent &event)
{
    wxPoint mousePos = CalcUnscrolledPosition(ScreenToClient(wxGetMousePosition()));
    int lineBegin = mainFrame->getDisassembly()->findLineByAddress(mousePos.y);
    if(lineBegin > 0) {
        //std::cout << "left down at " << mousePos.x << " " << mousePos.y << " -> " << lineBegin << std::endl;
        mainFrame->OnOverviewLineClicked(lineBegin);
        // do not get the focus, because the focus is given to the disassembly window to mark the current line
    }
    else {
        event.Skip(); // let the scroll overview window get the focus
    }
}   
