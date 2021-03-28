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
 
#ifndef FUNCTION_GRAPH_HPP_INCLUDED
#define FUNCTION_GRAPH_HPP_INCLUDED

#include <vector>
#include <unordered_map>
#include <map>
#include "wx/scrolwin.h"

#include "8051/function.hpp"
#include "8051/remap_call.hpp"

class Dis8051Frame;

class FunctionGraph : public wxScrolledWindow
{
private: 
    const int MARGIN = 2;
    Dis8051Frame *mainFrame;
    int w,h;

    struct CachedFunction
    {
        uint64_t address;
        disas8051::Function *function;
        std::map<std::size_t, bool> targetIndexes; // true if direct call, false if only a part called
        std::vector<std::size_t> remapCallIndexes;
        int rectWidth;
        int rectHeight;
    };
    std::vector<CachedFunction> cachedFunctions;
    
    struct CachedRemapCall
    {
		uint64_t address;
		disas8051::RemapCall *remapCall;
		int targetIndex; // -1 if the target function is not named by user
		bool isDirectCall; // false if only a part of the targetIndex-function called
		// currently unused! (it is not drawn)
        int rectWidth;
        int rectHeight;
	};
    std::vector<CachedRemapCall> cachedRemapCalls;

    wxFont font;

    bool isDragging = false;
    int dragStartX;
    int dragStartY;
    int dragCurrentX;
    int dragCurrentY;
    bool isDraggingFunction = true; // currently only functions because remap calls are currently not drawn!
    std::size_t draggedFunctionIndex; // function index if isDraggingFunction == true, else remap call index

public:
    FunctionGraph(Dis8051Frame *mainFrame, wxWindow *parent, wxWindowID id);
    ~FunctionGraph() {}

    void RefreshCache();

private:
    void OnDraw(wxDC& dc);
    void OnLeftDown(wxMouseEvent &event);
    void OnMouseMove(wxMouseEvent &event);
    void OnLeftUp(wxMouseEvent &event);
    void OnLeftDClick(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);

    int getFunctionIndex(wxPoint mousePos);
    void DrawConnectorLine(wxDC *dc, int x1, int y1, int x2, int y2);

    DECLARE_EVENT_TABLE()
};

#endif // FUNCTION_GRAPH_HPP_INCLUDED
