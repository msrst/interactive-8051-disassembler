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
 
#ifndef GUI_PALETTE_H_INCLUDED
#define GUI_PALETTE_H_INCLUDED

#include "wx/wx.h"

struct gui_palette {
    wxString textextent_teststring;

    wxColour backgroundcolour_frame;
    wxPen backgroundcolour_frame_pen;
    wxBrush backgroundcolour_frame_brush;

    wxTextAttr logger_textctrl_fehler_textattr;
    wxTextAttr logger_textctrl_warnung_textattr;
    wxTextAttr logger_textctrl_meldung_textattr;
    wxTextAttr standard_textctrl_textattr;
};

void gui_palette_init(gui_palette& gui_palette);
void gui_palette_end(gui_palette& gui_palette);

#endif // GUI_PALETTE_H_INCLUDED
