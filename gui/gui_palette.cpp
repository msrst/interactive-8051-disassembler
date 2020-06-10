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
 
#include "wx/wx.h"
#include "wx/image.h"
#include "wx/imagxpm.h"

#include "../consts.hpp"
#include "gui_palette.h"

void gui_palette_init(gui_palette& gui_palette)
{
    wxMemoryDC dummydc;
    wxSize knopfbmp_standardsize;

    gui_palette.textextent_teststring = wxT("AaZz\x00e4")wxT("0?*#',;.()$");

    gui_palette.backgroundcolour_frame = wxColour(230, 230, 230);
    gui_palette.backgroundcolour_frame_pen = wxPen(gui_palette.backgroundcolour_frame, 1, wxPENSTYLE_SOLID);
    gui_palette.backgroundcolour_frame_brush = wxBrush(gui_palette.backgroundcolour_frame);

    gui_palette.logger_textctrl_fehler_textattr = wxTextAttr(*wxRED, *wxWHITE);
    gui_palette.logger_textctrl_warnung_textattr = wxTextAttr(wxColour(255, 160, 0), *wxWHITE);
    gui_palette.logger_textctrl_meldung_textattr = wxTextAttr(wxColour(31, 75, 147), *wxWHITE);
    gui_palette.standard_textctrl_textattr = wxTextAttr(*wxBLACK, *wxWHITE);
}
void gui_palette_end(gui_palette& c_gui_palette)
{
    //
}
