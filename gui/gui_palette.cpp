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

    gui_palette.logger_textctrl_error_textattr = wxTextAttr(*wxRED, *wxWHITE);
    gui_palette.logger_textctrl_warning_textattr = wxTextAttr(wxColour(255, 160, 0), *wxWHITE);
    gui_palette.logger_textctrl_message_textattr = wxTextAttr(wxColour(31, 75, 147), *wxWHITE);
    gui_palette.default_textctrl_textattr = wxTextAttr(*wxBLACK, *wxWHITE);
}
void gui_palette_end(gui_palette& c_gui_palette)
{
    //
}
