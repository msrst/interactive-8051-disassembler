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
 
#ifndef DISAS8051_MAIN_H_INCLUDED
#define DISAS8051_MAIN_H_INCLUDED

#include <wx/wx.h>
#include "wx/spinctrl.h"
#include "wx/imaglist.h"
#include "wx/artprov.h"
#include "wx/notebook.h"
#include "wx/dataview.h"
#include "wx/hashmap.h"
#include "wx/richtext/richtextctrl.h"
#include "wx/stc/stc.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/checkbox.h"

#include "wxApp.h"

#include "utils/logger.h"
#include "gui/gui_palette.h"
#include "8051/disassembler.hpp"
#include "scroll_overview.hpp"
#include "function_graph.hpp"
#include "annotation_canvas.hpp"

class Dis8051Frame: public wxFrame
{
private:
    boost::shared_ptr<logger::logger> c_logger;
    wxTimer timer_gui_update1;
    Dis8051App *m_app;

    gui_palette c_gui_palette;

    ScrollOverview *scroll_overview;
    wxSplitterWindow *wxsw_main;
    AnnotationCanvas *annotation_canvas;
    wxStyledTextCtrl *wxrt_disassembly;
    wxTextCtrl *wxtc_referalls;
    wxCheckBox *wxc_search_hex;
    wxTextCtrl *wxtc_search;
    wxTextCtrl *wxtc_comment;
    wxTextCtrl *wxtc_function;
    wxCheckBox *wxc_function_shown;
    wxTextCtrl *wxtc_function_blocks;
    FunctionGraph *function_graph;

    wxTextCtrl *wxtc_log;

    std::string firmwareFile;
    std::string metaFile;

    std::shared_ptr<disas8051::Disassembly> disassembly;
    
public:
    Dis8051Frame(wxFrame *frame, const wxString& title, std::string firmwareFile, std::string metaFile, Dis8051App *app);
    ~Dis8051Frame();

    disas8051::Disassembly *getDisassembly() {
        return disassembly.get();
    }

    void GetDisassemblyLinesOnScreen(int *lineBegin, int *lineEnd);
    void OnOverviewLineClicked(int line);
    void OnFunctionClicked(uint64_t address, disas8051::Function *function);
private:
    enum
    {
        idMenuQuit = 1000,
        idMenuAbout,
        idMenuFindNext,
        idMenuFindPrevious,
        idMenuGoto,
        idMenuSaveMetaFile,
        idMenuToggleCallAnnotation,
        idMenuReadAddressFromCode,
        idTimerUpdateUI,
        idDisassemblyTextCtrl,
        idTextCtrlSearch,
        idButtonFindBefore,
        idButtonFindNext,
        idTextCtrlComment,
        idTextCtrlFunction,
        idCheckBoxFunctionShown,
        idButtonDeleteFunction
    };
    void OnClose(wxCloseEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnSaveMetaFile(wxCommandEvent &event);
    void OnTimerUpdateUI(wxTimerEvent &event);
    void OnDisassemblyTextUpdated(wxStyledTextEvent &event);
    void OnDissassemblyCaretMove();
    void OnFindBefore(wxCommandEvent &event);
    void OnFindNext(wxCommandEvent &event);
    void OnGoto(wxCommandEvent &event);
    void OnCommentEnterPressed(wxCommandEvent &event);
    void OnFunctionEnterPressed(wxCommandEvent &event);
    void OnToggleFunctionShown(wxCommandEvent &event);
    void OnFunctionDelete(wxCommandEvent &event);
    void OnToggleCallAnnotation(wxCommandEvent &event);
    void OnReadAddressFromCode(wxCommandEvent &event);

    void messageGUI(std::string msg);

	void doSearch(bool previous);
    std::string getAddressDescription(uint64_t address);
    int queryAddressFromUser(wxString title, int defaultAddres = -1); // return -1 if invalid address, address if correct (also checks bounds).

    DECLARE_EVENT_TABLE()
};

std::basic_string<uint8_t> Uint16t_To_UString(uint16_t var);

#endif // DISAS8051_MAIN_H_INCLUDED
