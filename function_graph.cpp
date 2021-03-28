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
 
#include "function_graph.hpp"
#include "wxMain.h"
#include <iostream>

BEGIN_EVENT_TABLE(FunctionGraph, wxScrolledWindow)
    EVT_LEFT_DOWN(FunctionGraph::OnLeftDown)
    EVT_MOTION(FunctionGraph::OnMouseMove)
    EVT_LEFT_UP(FunctionGraph::OnLeftUp)
    EVT_KEY_DOWN(FunctionGraph::OnKeyDown)
    EVT_LEFT_DCLICK(FunctionGraph::OnLeftDClick)
END_EVENT_TABLE()

FunctionGraph::FunctionGraph(Dis8051Frame* mainFrame, wxWindow *parent, wxWindowID id) : 
        wxScrolledWindow(parent, id, wxDefaultPosition, wxSize(150, 300)),
        font(wxFontInfo(9))
{
    this->mainFrame = mainFrame;
    w = 1600;
    h = 5000;

    /* init scrolled area size, scrolling speed, etc. */
    SetScrollbars(10,10, w/10, h/10, 0, 0);
}

void FunctionGraph::OnDraw(wxDC& dc)
{
	wxPen indirectRemapCallPen(wxColor(120, 120, 255));
	
    int viewStartX, viewStartY, viewSizeX, viewSizeY;
    GetViewStart(&viewStartX, &viewStartY);
    GetClientSize(&viewSizeX, &viewSizeY);
    //std::cout << viewStartX << " " << viewStartY << " " << viewSizeX << " " << viewSizeY << std::endl;

    dc.SetFont(font);
    dc.SetPen(*wxBLACK_PEN);
    for(std::size_t iFunction = 0; iFunction < cachedFunctions.size(); iFunction++) {
        const CachedFunction &cFunction = cachedFunctions[iFunction];
        if(!cFunction.function->isShownInGraph) {
			continue;
		}
		
        //std::cout << iFunction << " " << cFunction.address << " " << cFunction.function->posX << " " << cFunction.function->posY << std::endl;
        int offsetX = 0;
        int offsetY = 0;
        if(isDragging && isDraggingFunction && (iFunction == draggedFunctionIndex)) {
            offsetX = dragCurrentX - dragStartX;
            offsetY = dragCurrentY - dragStartY;
            //std::cout << "offset " << offsetX << " " << offsetY << std::endl;
        }

        if(cFunction.function->isInterrupt) {
            dc.SetBrush(*wxGREY_BRUSH);
        }
        else {
            dc.SetBrush(*wxWHITE_BRUSH);
        }
        dc.DrawRectangle(cFunction.function->posX + offsetX, cFunction.function->posY + offsetY,
                cFunction.rectWidth, cFunction.rectHeight);
        dc.DrawText(wxString::FromUTF8(cFunction.function->name.c_str()), 
                cFunction.function->posX + offsetX + MARGIN, cFunction.function->posY + offsetY + MARGIN);
    }
    
    for(std::size_t iFunction = 0; iFunction < cachedFunctions.size(); iFunction++) {
        const CachedFunction &cFunction = cachedFunctions[iFunction];
        if(!cFunction.function->isShownInGraph) {
			continue;
		}
		
        for(const auto &pair : cFunction.targetIndexes) {
			if(pair.first == iFunction) {
				continue;
			}
			if(!cachedFunctions[pair.first].function->isShownInGraph) {
				continue;
			}
			
			dc.SetPen(pair.second ? (*wxBLACK_PEN) : (*wxGREY_PEN));
			if(cachedFunctions[pair.first].function->name.find("PUTCHAR") != std::string::npos) {
				dc.SetPen(*wxRED_PEN);
			}
            DrawConnectorLine(&dc, cFunction.function->posX + cFunction.rectWidth, 
					cFunction.function->posY + cFunction.rectHeight / 2,
					cachedFunctions[pair.first].function->posX,
					cachedFunctions[pair.first].function->posY + cachedFunctions[pair.first].rectHeight / 2);
        }
        
        for(int iRemapCall : cFunction.remapCallIndexes) {
			const CachedRemapCall &cRemapCall = cachedRemapCalls[iRemapCall];
			if(cRemapCall.targetIndex >= 0) {
				if(!cachedFunctions[cRemapCall.targetIndex].function->isShownInGraph) {
					continue;
				}
				
				dc.SetPen(cRemapCall.isDirectCall ? (*wxBLUE_PEN) : indirectRemapCallPen);
				DrawConnectorLine(&dc, cFunction.function->posX + cFunction.rectWidth, 
						cFunction.function->posY + cFunction.rectHeight / 2,
						cachedFunctions[cRemapCall.targetIndex].function->posX,
						cachedFunctions[cRemapCall.targetIndex].function->posY + cachedFunctions[cRemapCall.targetIndex].rectHeight / 2);
			}
		}			
    }
    
    /*dc.SetBrush(wxBrush(wxColor(100, 100, 255)));
    for(int iRemapCall = 0; iRemapCall < cachedRemapCalls.size(); iRemapCall++) {
		const CachedRemapCall &cRemapCall = cachedRemapCalls[iRemapCall];
        int offsetX = 0;
        int offsetY = 0;
        if(isDragging && (!isDraggingFunction) && (iRemapCall == draggedFunctionIndex)) {
            offsetX = dragCurrentX - dragStartX;
            offsetY = dragCurrentY - dragStartY;
            //std::cout << "offset " << offsetX << " " << offsetY << std::endl;
        }

        dc.DrawRectangle(cRemapCall.remapCall->posX + offsetX, cRemapCall.remapCall->posY + offsetY,
                cRemapCall.rectWidth, cRemapCall.rectHeight);
        dc.DrawText(wxString::Format(wxT("%x"), (int)cRemapCall.remapCall->targetAddress), 
                cRemapCall.remapCall->posX + offsetX + MARGIN, cRemapCall.remapCall->posY + offsetY + MARGIN);
	}*/
}
void FunctionGraph::DrawConnectorLine(wxDC *dc, int x1, int y1, int x2, int y2)
{
	const int BACKWARDS_MARGIN_X = 2;
	const int BACKWARDS_MARGIN_Y = 12;
	
	int xDiff = x2 - x1;
	if(xDiff >= 0) {
		wxPoint points[4];
		points[0] = wxPoint(x1, y1);
		points[1] = wxPoint(x1 + xDiff / 2, y1);
		points[2] = wxPoint(x1 + xDiff / 2, y2);
		points[3] = wxPoint(x2, y2);
		dc->DrawLines(4, points);
	}
	else {
		wxPoint points[8];
		points[0] = wxPoint(x1, y1);
		points[1] = wxPoint(x1 + BACKWARDS_MARGIN_X, y1);
		points[2] = wxPoint(x1 + BACKWARDS_MARGIN_X, y1 + BACKWARDS_MARGIN_Y);
		points[3] = wxPoint(x1 + xDiff / 2, y1 + BACKWARDS_MARGIN_Y);
		points[4] = wxPoint(x1 + xDiff / 2, y2 + BACKWARDS_MARGIN_Y);
		points[5] = wxPoint(x2 - BACKWARDS_MARGIN_X, y2 + BACKWARDS_MARGIN_Y);
		points[6] = wxPoint(x2 - BACKWARDS_MARGIN_X, y2);
		points[7] = wxPoint(x2, y2);
		dc->DrawLines(8, points);
	}
}

void FunctionGraph::OnLeftDown(wxMouseEvent &event)
{
    wxPoint mousePos = CalcUnscrolledPosition(ScreenToClient(wxGetMousePosition()));
    event.Skip(); // let the function graph window get the focus

    int iFunction = getFunctionIndex(mousePos);
    if(iFunction >= 0) {
        draggedFunctionIndex = iFunction;
        dragStartX = mousePos.x;
        dragStartY = mousePos.y;
        isDragging = true;
    }
}
int FunctionGraph::getFunctionIndex(wxPoint mousePos)
{
    for(std::size_t iFunction = 0; iFunction < cachedFunctions.size(); iFunction++) {
        const CachedFunction &cFunction = cachedFunctions[iFunction];
        if(!cFunction.function->isShownInGraph) {
			continue;
		}
		
        if((mousePos.x >= cFunction.function->posX - MARGIN)
                 && (mousePos.x < cFunction.function->posX + cFunction.rectWidth + MARGIN)
                 && (mousePos.y >= cFunction.function->posY - MARGIN) 
                 && (mousePos.y < cFunction.function->posY + cFunction.rectHeight + MARGIN)) {
            //std::cout << "hit function " << iFunction << cFunction.function->name << std::endl;
            return iFunction;
        }
    }
    return -1;
}
void FunctionGraph::OnMouseMove(wxMouseEvent &event)
{
    wxPoint mousePos = CalcUnscrolledPosition(ScreenToClient(wxGetMousePosition()));

    if(isDragging) {
        //int oldDragX = dragCurrentX;
        //int oldDragY = dragCurrentY;
        dragCurrentX = mousePos.x;
        dragCurrentY = mousePos.y;
        Refresh(); // TODO use RefreshRect here from oldDragXY to dragCurrentXY, it's flickering really too much under windows
    }
}
void FunctionGraph::OnLeftUp(wxMouseEvent &event)
{
    wxPoint mousePos = CalcUnscrolledPosition(ScreenToClient(wxGetMousePosition()));

    if(isDragging) {
        CachedFunction &cFunction = cachedFunctions[draggedFunctionIndex];
        cFunction.function->posX += mousePos.x - dragStartX;
        cFunction.function->posY += mousePos.y - dragStartY;
        isDragging = false;
        Refresh();
    }
}

void FunctionGraph::OnKeyDown(wxKeyEvent &event)
{
    wxChar key = event.GetKeyCode();
    if(key == WXK_ESCAPE) {
        if(isDragging) {
            isDragging = false;
            Refresh();
        }
    }
}

void FunctionGraph::OnLeftDClick(wxMouseEvent &event)
{
    wxPoint mousePos = CalcUnscrolledPosition(ScreenToClient(wxGetMousePosition()));
    event.Skip(); // let the function graph window get the focus

    int iFunction = getFunctionIndex(mousePos);
    if(iFunction >= 0) {
        mainFrame->OnFunctionClicked(cachedFunctions[iFunction].address, cachedFunctions[iFunction].function);
    }
}

// -1 if not found
int searchFunction(disas8051::Disassembly *disassembly, const std::unordered_map<uint64_t, std::size_t> &addressToFunction, int address, bool &directCall)
{
	auto itFunction = addressToFunction.find(address);
	if(itFunction == addressToFunction.end()) {
		auto itDisFunction = disassembly->findFunction(address);
		if(itDisFunction != disassembly->functions.end()) {
			// call / jump to a part of the function
			itFunction = addressToFunction.find(itDisFunction->first);
			directCall = false;
			return itFunction->second;
		}
	}
	else {
		directCall = true;
		return itFunction->second;
	}
	return -1;
}

void FunctionGraph::RefreshCache()
{
    wxMemoryDC dc;
    dc.SetFont(font);

    std::unordered_map<uint64_t, std::size_t> addressToFunction;

    cachedFunctions.resize(mainFrame->getDisassembly()->functions.size());
    std::size_t iFunction = 0;
    for(auto &pair : mainFrame->getDisassembly()->functions) {
        CachedFunction cachedFunction;
        cachedFunction.address = pair.first;
        cachedFunction.function = &(pair.second);
        dc.GetTextExtent(wxString::FromUTF8(pair.second.name.c_str()), 
                &(cachedFunction.rectWidth), &(cachedFunction.rectHeight));
        cachedFunction.rectWidth += MARGIN * 2;
    // #ifdef __unix__
    //     // I really don't know why this is necessary, but it seemed to be only
    //     // to be the double height since I changed wxWidgets from the ubuntu built-in
    //     // to wx3.1.3
    //     cachedFunction.rectHeight = cachedFunction.rectHeight / 2 + MARGIN * 2;
    // #else
        cachedFunction.rectHeight += MARGIN * 2;
    // #endif
        cachedFunctions[iFunction] = cachedFunction;
        addressToFunction[pair.first] = iFunction;
        iFunction++;
    }
    
    std::unordered_map<uint64_t, int> addressToRemapCall;
    
    cachedRemapCalls.resize(mainFrame->getDisassembly()->remapCalls.size());
    int iRemapCall = 0;
    for(auto &pair : mainFrame->getDisassembly()->remapCalls) {
        CachedRemapCall cachedRemapCall;
        cachedRemapCall.address = pair.first;
        cachedRemapCall.remapCall = &(pair.second);
        dc.GetTextExtent(wxString::Format(wxT("%x"), (int)pair.second.targetAddress), 
                &(cachedRemapCall.rectWidth), &(cachedRemapCall.rectHeight));
        cachedRemapCall.rectWidth += MARGIN * 2;
        cachedRemapCall.rectHeight += MARGIN * 2;
        
		cachedRemapCall.targetIndex = searchFunction(mainFrame->getDisassembly(), addressToFunction, 
				pair.second.targetAddress, cachedRemapCall.isDirectCall);
        
        cachedRemapCalls[iRemapCall] = cachedRemapCall;        
        addressToRemapCall[pair.first] = iRemapCall;
        iRemapCall++;
    }

    for(iFunction = 0; iFunction < cachedFunctions.size(); iFunction++) {
        CachedFunction &cFunction = cachedFunctions[iFunction];
        std::vector<uint64_t> sourceAddresses = mainFrame->getDisassembly()->GetFunctionTargetAddresses(cFunction.address);
        for(uint64_t sourceAddress : sourceAddresses) {
            bool directCall;
            int index = searchFunction(mainFrame->getDisassembly(), addressToFunction, sourceAddress, directCall);
            if(index >= 0) {
				if(directCall) {
					cFunction.targetIndexes[index] = true;
				}
				else {
					auto itTargetIndex = cFunction.targetIndexes.find(cachedFunctions[index].address);
					if(itTargetIndex == cFunction.targetIndexes.end()) { // do not override if there is also a jump / call to the first address
						cFunction.targetIndexes[index] = false;
					}
				}
			}
			
			auto itRemapCall = addressToRemapCall.find(sourceAddress);
			if(itRemapCall != addressToRemapCall.end()) {
				cFunction.remapCallIndexes.push_back(itRemapCall->second);
			}
        }
    }
}
