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
 
#include "annotation_canvas.hpp"
#include "wxMain.h"
#include "utils/utils.hpp"
#include <iostream>

BEGIN_EVENT_TABLE(AnnotationCanvas, wxWindow)
    EVT_PAINT(AnnotationCanvas::OnPaint)
END_EVENT_TABLE()

AnnotationCanvas::AnnotationCanvas(Dis8051Frame* mainFrame, wxWindow *parent, wxWindowID id)
         : wxWindow(parent, id, wxDefaultPosition, wxSize(90, 300))
{
    this->mainFrame = mainFrame;
}

struct Jump
{
    uint64_t from;
    uint64_t to;

    uint64_t getFirst() const {
        return std::min(from, to);
    }
    uint64_t getLast() const {
        return std::max(from, to);
    }
};
void appendJump(uint64_t from, uint64_t to, std::vector<std::vector<Jump>> &jumps)
{
    Jump newJump{from, to};
    bool foundPlane = false;
    for(std::size_t iPlane = 0; iPlane < jumps.size(); iPlane++) {
        bool planeOk = true;
        for(const Jump &jump : jumps[iPlane]) {
            if((jump.getFirst() <= newJump.getLast()) && (jump.getLast() >= newJump.getFirst())) {
                planeOk = false;
                break;
            }
        }
        if(planeOk) {
            jumps[iPlane].push_back(newJump);
            foundPlane = true;
            break;
        }
    }
    if(!foundPlane) {
        jumps.push_back(std::vector<Jump>{newJump});
    }
}
// this is because wxWidgets DC seems to have difficulties with integer values outside the 16-bit signed region
inline int boundY(int val, int max)
{
    if(val < -1000) {
        return -1000;
    }
    if(val > max+1000) {
        return max+1000;
    }
    return val;
}
void AnnotationCanvas::OnPaint(wxPaintEvent &event)
{
    const int PLANE_MARGIN = 12;
    const int RIGHT_PLANE_MARGIN = 20;
    const int RIGHT_MARGIN = 2;
    const int ARROW_SIZE = 6;

    wxSize ownSize = GetSize();

    wxPaintDC dc(this);

    int lineBegin, lineEnd;
    mainFrame->GetDisassemblyLinesOnScreen(&lineBegin, &lineEnd);
    auto itAddressBegin = mainFrame->getDisassembly()->addressByLine.find(lineBegin);
    auto itAddressEnd = mainFrame->getDisassembly()->addressByLine.find(lineEnd);
    if((itAddressBegin == mainFrame->getDisassembly()->addressByLine.end()) || 
            (itAddressEnd == mainFrame->getDisassembly()->addressByLine.end())) {
        return;
    }
    uint64_t addressBegin = itAddressBegin->second;
    uint64_t addressEnd = itAddressEnd->second;

    auto itInstruction = mainFrame->getDisassembly()->instructions.begin();
    while(itInstruction->first < addressBegin) {
        itInstruction++;
        if(itInstruction == mainFrame->getDisassembly()->instructions.end()) {
            return;
        }
    }

    std::vector<std::vector<Jump>> jumps;
    while(itInstruction->first < addressEnd) {
        if(itInstruction->second.isJump || itInstruction->second.isCondJump) {
			if(itInstruction->second.isCall()) {
				if(annotateCalls) {
					appendJump(itInstruction->first, itInstruction->second.address, jumps);
				}
			}
			else {
				appendJump(itInstruction->first, itInstruction->second.address, jumps);
			}
        }
        auto itTarget = mainFrame->getDisassembly()->jumpTargets.find(itInstruction->first);
        if(itTarget != mainFrame->getDisassembly()->jumpTargets.end()) {
            for(uint64_t sourceAddress : itTarget->second) {
                if((sourceAddress < addressBegin) || (sourceAddress >= addressEnd)) {
                    // otherwise we get / got this also by parsing the instruction
                    appendJump(sourceAddress, itInstruction->first, jumps);
                }
            }
        }
        
        itInstruction++;
        if(itInstruction == mainFrame->getDisassembly()->instructions.end()) {
            return;
        }
    }

    wxPen jumpForwardsPen(wxColour(50, 50, 255), 2);
    wxPen jumpBackwardsPen(wxColour(0, 0, 200), 2);
    for(std::size_t iPlane = 0; iPlane < jumps.size(); iPlane++) {
        for(Jump jump : jumps[iPlane]) {
            // TODO maybe too unperformant!
            int lineFrom = mainFrame->getDisassembly()->findLineByAddress(jump.from);
            int lineTo = mainFrame->getDisassembly()->findLineByAddress(jump.to);

            if(jump.from <= jump.to) {
                dc.SetPen(jumpForwardsPen);
            }
            else { // jump backwards
                dc.SetPen(jumpBackwardsPen);
            }

            int startX = ownSize.GetWidth() - RIGHT_MARGIN;
            int betweenX = ownSize.GetWidth() - RIGHT_PLANE_MARGIN - iPlane * PLANE_MARGIN;
            int fromY = boundY((lineFrom - lineBegin) * lineHeight + lineHeight / 2, ownSize.GetHeight());
            int toY = boundY((lineTo - lineBegin) * lineHeight + lineHeight / 2, ownSize.GetHeight());
            dc.DrawLine(startX, fromY, betweenX, fromY);
            dc.DrawLine(betweenX, fromY, betweenX, toY);
            dc.DrawLine(betweenX, toY, startX, toY);
            dc.DrawLine(startX - ARROW_SIZE, toY - ARROW_SIZE, startX, toY);
            dc.DrawLine(startX - ARROW_SIZE, toY + ARROW_SIZE, startX, toY);            
        }
    }
}
