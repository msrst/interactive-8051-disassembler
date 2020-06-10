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
