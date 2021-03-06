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

#ifndef ANNOTATION_CANVAS_HPP_INCLUDED
#define ANNOTATION_CANVAS_HPP_INCLUDED

#include "wx/scrolwin.h"

class Dis8051Frame;

class AnnotationCanvas : public wxWindow
{
private:
    Dis8051Frame *mainFrame;
public:
    AnnotationCanvas(Dis8051Frame *mainFrame, wxWindow *parent, wxWindowID id);
    ~AnnotationCanvas() {}

    int lineHeight;
    bool annotateCalls = false;

private:
    void OnPaint(wxPaintEvent &event);

    DECLARE_EVENT_TABLE();
};

#endif // ANNOTATION_CANVAS_HPP_INCLUDED
