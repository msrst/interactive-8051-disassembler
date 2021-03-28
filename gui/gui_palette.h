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
 
#ifndef GUI_PALETTE_H_INCLUDED
#define GUI_PALETTE_H_INCLUDED

#include "wx/wx.h"

struct gui_palette {
    wxString textextent_teststring;

    wxColour backgroundcolour_frame;
    wxPen backgroundcolour_frame_pen;
    wxBrush backgroundcolour_frame_brush;

    wxTextAttr logger_textctrl_error_textattr;
    wxTextAttr logger_textctrl_warning_textattr;
    wxTextAttr logger_textctrl_message_textattr;
    wxTextAttr default_textctrl_textattr;
};

void gui_palette_init(gui_palette& gui_palette);
void gui_palette_end(gui_palette& gui_palette);

#endif // GUI_PALETTE_H_INCLUDED
