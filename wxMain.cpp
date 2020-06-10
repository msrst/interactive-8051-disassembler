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
 
#include <cmath>
#include "wx/config.h"
#include "wx/textdlg.h"
#include "wx/clipbrd.h"

#include "wxMain.h"
#include "utils/utils_wx.h"

#include <boost/filesystem.hpp>

BEGIN_EVENT_TABLE(Dis8051Frame, wxFrame)
    EVT_CLOSE(Dis8051Frame::OnClose)
    EVT_MENU(idMenuQuit, Dis8051Frame::OnQuit)
    EVT_MENU(idMenuAbout, Dis8051Frame::OnAbout)
    EVT_MENU(idMenuSaveMetaFile, Dis8051Frame::OnSaveMetaFile)
    EVT_MENU(idMenuGoto, Dis8051Frame::OnGoto)
    EVT_MENU(idMenuFindPrevious, Dis8051Frame::OnFindBefore)
    EVT_MENU(idMenuFindNext, Dis8051Frame::OnFindNext)
    EVT_MENU(idMenuToggleCallAnnotation, Dis8051Frame::OnToggleCallAnnotation)
    EVT_MENU(idMenuReadAddressFromCode, Dis8051Frame::OnReadAddressFromCode)
    EVT_TIMER(idTimerOberflaecheAktualisieren1, Dis8051Frame::OnTimerOberflaecheAktualisieren1)
    EVT_STC_UPDATEUI(idDisassemblyTextCtrl, Dis8051Frame::OnDisassemblyTextUpdated)
    EVT_BUTTON(idButtonFindBefore, Dis8051Frame::OnFindBefore)
    EVT_BUTTON(idButtonFindNext, Dis8051Frame::OnFindNext)
    EVT_TEXT_ENTER(idTextCtrlComment, Dis8051Frame::OnCommentEnterPressed)
    EVT_TEXT_ENTER(idTextCtrlFunction, Dis8051Frame::OnFunctionEnterPressed)
    EVT_CHECKBOX(idCheckBoxFunctionShown, Dis8051Frame::OnToggleFunctionShown)
    EVT_BUTTON(idButtonDeleteFunction, Dis8051Frame::OnFunctionDelete)
END_EVENT_TABLE()

Dis8051Frame::Dis8051Frame(wxFrame *frame, const wxString& title, std::string firmwareFile, std::string metaFile, Dis8051App *app)
    : wxFrame(frame, -1, title), 
		timer_gui_update1(this, idTimerOberflaecheAktualisieren1)
{
    m_app = app;
    gui_palette_init(c_gui_palette);
    
    this->firmwareFile = firmwareFile;
    this->metaFile = metaFile;

    c_logger.reset(new logger::logger);
    c_logger->LogInformation("started interactive 8051 disassembler, logging to log.txt");
    c_logger->AddLogDisplay("log.txt");

	disassembly = std::make_shared<disas8051::Disassembly>(c_logger.get());
	std::string bufString;
	if(utils::GetFileString(firmwareFile, &bufString, true)) {
		c_logger->LogInformation("Loaded " + std::to_string(bufString.length()) + " bytes from " + firmwareFile);
		disassembly->buf = utils::StdStringToUstring(bufString);
		disas8051::Disassembler disassembler(c_logger.get());
        if(disassembly->openMetaFile(metaFile) == 0) {
            c_logger->LogInformation("Loaded metadata file " + metaFile);
        }
        else {
            // create a new meta file
            disassembly->functions[0] = disas8051::Function{"start"};
            c_logger->LogInformation("Created new meta, starting at address 0");
        }

        for(auto itFunction = disassembly->functions.begin(); itFunction != disassembly->functions.end(); itFunction++) {
		    disassembler.followCodePath(disassembly->buf, itFunction->first, disassembly.get());
        }
        disassembly->resolveRemapCalls(&disassembler);
        disassembly->resolveComments();
        disassembly->resolveFunctionEndAddresses();
        disassembly->autoComment();
	}
	else {
		c_logger->LogError("Could not open file " + firmwareFile);
        wxMessageBox(wxString::Format(wxT("Error: Could not open firmware file %s"), wxString::FromUTF8(firmwareFile.c_str())));
	}

    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuSaveMetaFile, _("&Save\tCtrl-S"));
    fileMenu->AppendSeparator();
    fileMenu->Append(idMenuGoto, _("&Goto line\tCtrl-G"));
    fileMenu->Append(idMenuFindNext, _("Find &next\tF3"));
    fileMenu->Append(idMenuFindPrevious, _("Find &previous\tShift-F3"));
    fileMenu->Append(idMenuToggleCallAnnotation, _("Toggle call annotation\tCtrl-K"), _("Toggle whether calling arrow from the current code to some function are displayed"));
    fileMenu->Append(idMenuReadAddressFromCode, _("Clipboard address from code\tCtrl-R"), _("Queries an address and copies the 2 bytes from this address to clipboard"));
    fileMenu->AppendSeparator();
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);

    wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
    
    //wxPanel *panel_top = new wxPanel(this);
    wxNotebook *notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT);
    
    wxPanel *panel_disassembly = new wxPanel(notebook);    
    notebook->AddPage(panel_disassembly, wxT("Disassembly"), false);
	wxBoxSizer *wxsz_panel_disassembly = new wxBoxSizer(wxHORIZONTAL);

    scroll_overview = new ScrollOverview(this, panel_disassembly, wxID_ANY);
	wxsz_panel_disassembly->Add(scroll_overview, 0, wxALL | wxEXPAND, 2);

	wxsw_main = new wxSplitterWindow(panel_disassembly, wxID_ANY);

	wxPanel *panel_left = new wxPanel(wxsw_main);
    wxBoxSizer *wxsz_left = new wxBoxSizer(wxVERTICAL);
    
    wxSplitterWindow *wxsw_disassembly = new wxSplitterWindow(panel_left, wxID_ANY);
    //wxBoxSizer *wxsz_disassembly = new wxBoxSizer(wxHORIZONTAL);
    annotation_canvas = new AnnotationCanvas(this, wxsw_disassembly, wxID_ANY);
    //wxsz_disassembly->Add(annotation_canvas, 0, wxALL | wxEXPAND, 2);
	wxrt_disassembly = new wxStyledTextCtrl(wxsw_disassembly, idDisassemblyTextCtrl);//, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_READONLY | wxTE_MULTILINE | wxTE_RICH);
    wxrt_disassembly->SetCaretLineVisible(true);
	wxFont font(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
	wxrt_disassembly->StyleSetFont((int)disas8051::Styles::DEFAULT, font);
	wxrt_disassembly->StyleSetFont((int)disas8051::Styles::BUF_ADDRESS, font);
	wxrt_disassembly->StyleSetForeground((int)disas8051::Styles::BUF_ADDRESS, wxColour(0, 255, 0));
	wxrt_disassembly->StyleSetFont((int)disas8051::Styles::BYTES, font);
	wxrt_disassembly->StyleSetForeground((int)disas8051::Styles::BYTES, wxColour(0, 100, 0));
	wxrt_disassembly->StyleSetFont((int)disas8051::Styles::RET_INSTRUCTION, font);
	wxrt_disassembly->StyleSetForeground((int)disas8051::Styles::RET_INSTRUCTION, wxColour(200, 0, 0));
	wxrt_disassembly->StyleSetFont((int)disas8051::Styles::COMMENT, font);
	wxrt_disassembly->StyleSetForeground((int)disas8051::Styles::COMMENT, wxColour(200, 0, 0));
    annotation_canvas->lineHeight = font.GetPixelSize().GetHeight();
	std::cout << "LineHeight: " << annotation_canvas->lineHeight << std::endl;
    #ifdef _WIN32 // also 64-bit
		annotation_canvas->lineHeight += 3; // no time to check why this is needed
	#endif
	disassembly->printToWx(wxrt_disassembly);
	wxsw_disassembly->SplitVertically(annotation_canvas, wxrt_disassembly, 100);
	wxsw_disassembly->SetMinimumPaneSize(10); // prevent unsplitting
    //wxsz_disassembly->Add(wxrt_disassembly, 1, wxALL | wxEXPAND, 2);
    //wxsz_left->Add(wxsz_disassembly, 1, wxALL | wxEXPAND, 2);
    wxsz_left->Add(wxsw_disassembly, 1, wxALL | wxEXPAND, 2);
    
	wxStaticBoxSizer *wxsbz_current_line = new wxStaticBoxSizer(wxVERTICAL, panel_left, wxT("Current Line"));
    wxBoxSizer *wxsz_comment = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lbl_comment = new wxStaticText(panel_left, wxID_ANY, wxT("Comment: "));
    wxsz_comment->Add(lbl_comment, 0, wxALL, 2);
    wxtc_comment = new wxTextCtrl(panel_left, idTextCtrlComment, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    wxsz_comment->Add(wxtc_comment, 1, wxALL, 2);
    wxsbz_current_line->Add(wxsz_comment, 0, wxALL | wxEXPAND, 2);
    wxBoxSizer *wxsz_function = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lbl_function = new wxStaticText(panel_left, wxID_ANY, wxT("Function: "));
    wxsz_function->Add(lbl_function, 0, wxALL, 2);
    wxtc_function = new wxTextCtrl(panel_left, idTextCtrlFunction, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    wxsz_function->Add(wxtc_function, 1, wxALL, 2);
    wxc_function_shown = new wxCheckBox(panel_left, idCheckBoxFunctionShown, wxT("shown"));
    wxsz_function->Add(wxc_function_shown, 0, wxALL, 2);
    wxButton *wxb_delete_function = new wxButton(panel_left, idButtonDeleteFunction, wxT("delete"));
    wxsz_function->Add(wxb_delete_function, 0, wxALL, 2);
    wxsbz_current_line->Add(wxsz_function, 0, wxALL | wxEXPAND, 2);
    wxtc_function_blocks = new wxTextCtrl(panel_left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    wxsbz_current_line->Add(wxtc_function_blocks, 0, wxALL | wxEXPAND, 2);    
    wxBoxSizer *wxsz_search = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *lbl_search = new wxStaticText(panel_left, wxID_ANY, wxT("Search: "));
    wxsz_search->Add(lbl_search, 0, wxALL, 2);
    wxtc_search = new wxTextCtrl(panel_left, idTextCtrlSearch);
    wxsz_search->Add(wxtc_search, 1, wxALL, 2);
    wxc_search_hex = new wxCheckBox(panel_left, wxID_ANY, wxT("hex"));
    wxsz_search->Add(wxc_search_hex, 0, wxALL, 2);
    wxButton *wxb_find_before = new wxButton(panel_left, idButtonFindBefore, wxT("Previous"));
    wxsz_search->Add(wxb_find_before, 0, wxALL, 2);
    wxButton *wxb_find_next = new wxButton(panel_left, idButtonFindNext, wxT("Next"));
    wxsz_search->Add(wxb_find_next, 0, wxALL, 2);
    wxsbz_current_line->Add(wxsz_search, 0, wxALL | wxEXPAND, 2);
    wxtc_referalls = new wxTextCtrl(panel_left, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    wxsbz_current_line->Add(wxtc_referalls, 0, wxALL | wxEXPAND, 2);    
    wxsz_left->Add(wxsbz_current_line, 0, wxALL | wxEXPAND, 2);
    panel_left->SetSizerAndFit(wxsz_left);

	//wxPanel *panel_right = new wxPanel(wxsw_main);
    function_graph = new FunctionGraph(this, wxsw_main, wxID_ANY);
    function_graph->RefreshCache();
	wxsw_main->SplitVertically(panel_left, function_graph, 500);
	wxsw_main->SetMinimumPaneSize(10); // prevent unsplitting
	wxsz_panel_disassembly->Add(wxsw_main, 1, wxALL | wxEXPAND, 2);

	panel_disassembly->SetSizerAndFit(wxsz_panel_disassembly);

    wxPanel *panel_log = new wxPanel(notebook);    
    notebook->AddPage( panel_log, wxT("Log"), false);
	wxBoxSizer *wxsz_panel_log = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* wxtc_log = new wxTextCtrl(panel_log, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(500, 200), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH);
    c_logger->AddLogDisplay(wxtc_log, &c_gui_palette);
    wxsz_panel_log->Add(wxtc_log, 1, wxEXPAND | wxALL, 2);

	panel_log->SetSizerAndFit(wxsz_panel_log);
	
	notebook->SetSelection(0); // Activate the "Capture" tab

    topSizer->Insert(0, notebook, wxSizerFlags(5).Expand().Border());
    topSizer->Layout();
    SetSizer(topSizer);
    topSizer->Fit(this);
    topSizer->SetSizeHints(this);
    
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
	time_t now = time(NULL);
	tm *info = localtime(&now);
    if(info->tm_hour < 11)
		SetStatusText(_("Good Morning"),0);
	else if(info->tm_hour < 17)
		SetStatusText(_("Good Afternoon"),0);
	else
		SetStatusText(_("Good Evening"),0);
    SetStatusText(wxT("Copyright Matthias Rosenthal"), 1);

    timer_gui_update1.Start(TIMER_GUI_UPDATE_INTERVAL_MS);

	SetSize(wxSize(1200, 900));
}

Dis8051Frame::~Dis8051Frame()
{
    c_logger.reset();

    gui_palette_end(c_gui_palette);
}

void Dis8051Frame::OnClose(wxCloseEvent &event)
{
	std::cout << "stopping" << std::endl;
    timer_gui_update1.Stop();
    std::cout << "stopped." << std::endl;

    Destroy();
}

void Dis8051Frame::OnQuit(wxCommandEvent &event)
{
	std::cout << "stopping" << std::endl;
    timer_gui_update1.Stop();
    std::cout << "stopped." << std::endl;
    
    Destroy();
}


void Dis8051Frame::OnAbout(wxCommandEvent &event)
{
    wxString msg = wxT("Interactive 8051 disassembler\n"
			"Currently targeted for ENE KB9012 (Keyboard Controller)\nCopyright Matthias Rosenthal\n") + 
			fn_wxbuildinfo(wxbuildinfof_long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void Dis8051Frame::OnTimerOberflaecheAktualisieren1(wxTimerEvent& event)
{
    c_logger->updateLogDisplays();
}

void Dis8051Frame::OnSaveMetaFile(wxCommandEvent &event)
{
    disassembly->saveMetaFile(metaFile);
    messageGUI("Saved to " + metaFile);
}

void Dis8051Frame::OnDisassemblyTextUpdated(wxStyledTextEvent &event) 
{
    if(event.GetUpdated() & wxSTC_UPDATE_V_SCROLL) {
        scroll_overview->Refresh();
        annotation_canvas->Refresh();
    }
    if(event.GetUpdated() & wxSTC_UPDATE_SELECTION) {
        OnDissassemblyCaretMove();
    }
}
void Dis8051Frame::OnDissassemblyCaretMove()
{
    int currentLine = wxrt_disassembly->GetCurrentLine();
    auto itAddress = disassembly->addressByLine.find(currentLine);
    if(itAddress != disassembly->addressByLine.end()) {
		
        auto itInstruction = disassembly->instructions.find(itAddress->second);
        if(itInstruction != disassembly->instructions.end()) {
            wxtc_comment->SetValue(wxString::FromUTF8(itInstruction->second.comment.c_str()));
			SetStatusText(wxString::FromUTF8(("Address: " + utils::Int_To_String_Hex(itAddress->second) + 
					", Bytes: " + fn_FormatToHex(disassembly->buf.substr(itAddress->second, 
					itInstruction->second.length), true)).c_str()), 0);
        }
        else {
            wxtc_comment->SetValue(wxEmptyString);
			SetStatusText(wxString::Format(wxT("Address: %5x"), (int)itAddress->second), 0);
        }

        auto itTarget = disassembly->jumpTargets.find(itAddress->second);
        if(itTarget != disassembly->jumpTargets.end()) {
            if(!itTarget->second.empty()) {
                std::string references = "Referenced by " + getAddressDescription(itTarget->second[0]);
                for(std::size_t i1 = 1; i1 < itTarget->second.size(); i1++) {
                    references += ", " + getAddressDescription(itTarget->second[i1]);
                }
                wxtc_referalls->SetValue(wxString::FromUTF8(references.c_str()));
            }
            else {
                wxtc_referalls->SetValue(wxEmptyString);
            }
        }
        else {
            wxtc_referalls->SetValue(wxEmptyString);
        }

        auto itFunction = disassembly->findFunction(itAddress->second);
        if(itFunction != disassembly->functions.end()) {
            wxtc_function->SetValue(wxString::FromUTF8(itFunction->second.name.c_str()));
            wxtc_function_blocks->SetValue(wxString::FromUTF8(itFunction->second.getBlockSummary().c_str()));
            wxc_function_shown->SetValue(itFunction->second.isShownInGraph);
        }
        else {
            wxtc_function->SetValue(wxEmptyString);
            wxtc_function_blocks->SetValue(wxEmptyString);
            wxc_function_shown->SetValue(false);
        }
        return;
    }
    wxtc_comment->SetValue(wxEmptyString);
    wxtc_referalls->SetValue(wxEmptyString);
    wxtc_function->SetValue(wxEmptyString);
}

void Dis8051Frame::OnOverviewLineClicked(int line)
{
    wxrt_disassembly->SetFocus();
    wxrt_disassembly->GotoLine(line);
}

void Dis8051Frame::OnFunctionClicked(uint64_t address, disas8051::Function *function)
{
    int line = disassembly->findLineByAddress(address);
    if(line < 0) {
        c_logger->LogError("Line should have been existed");
        return;
    }
    wxrt_disassembly->SetFocus();
    wxrt_disassembly->GotoLine(line);
}

void Dis8051Frame::GetDisassemblyLinesOnScreen(int *lineBegin, int *lineEnd)
{
    *lineBegin = wxrt_disassembly->GetFirstVisibleLine();
    *lineEnd = *lineBegin + wxrt_disassembly->LinesOnScreen();
}

void Dis8051Frame::doSearch(bool previous)
{
	if(wxc_search_hex->GetValue()) {
		std::string hexstring = fn_wxUTF8String_ToUTF8String(wxtc_search->GetValue());
		std::size_t ihexstring = 0;
		disas8051::ustring searchbytes;
		while(true) {
			while((ihexstring < hexstring.size() && (hexstring[ihexstring] == ' '))) {
				ihexstring++;
			}
			if(ihexstring == hexstring.size()) {
				break;
			}
			if(ihexstring + 2 > hexstring.size()) {
				messageGUI("invalid hexstring: it does not end with a block of two hex digits");
				return;
			}
			
			if((!utils::is_hexchar(hexstring[ihexstring])) || (!utils::is_hexchar(hexstring[ihexstring+1]))) {
				messageGUI("invalid hex digit");
				return;
			}
			searchbytes.append(1, utils::Hex_To_uint8(hexstring[ihexstring], hexstring[ihexstring+1]));
			
			ihexstring += 2;
		}
		
		if(searchbytes.empty()) {
			messageGUI("empty search string");
			return;
		}
		
		int currentLine = wxrt_disassembly->GetCurrentLine();
		auto itAddress = disassembly->addressByLine.find(currentLine);
		if(itAddress == disassembly->addressByLine.end()) {
			messageGUI("no address corresponding to that line");
			return;
		}
			
		std::size_t pos;
		if(previous) {
			if(itAddress->second <= 0) {
				messageGUI("Not found");
				return;
			}
			pos = disassembly->buf.rfind(searchbytes, itAddress->second - 1);
			std::cout << "searching backwards from " << utils::Int_To_String_Hex(itAddress->second) << ": " << fn_FormatToHex(searchbytes, true) << std::endl;
		}
		else {
			pos = disassembly->buf.find(searchbytes, itAddress->second + 1);
			std::cout << "searching forwards from " << utils::Int_To_String_Hex(itAddress->second) << ": " << fn_FormatToHex(searchbytes, true) << std::endl;
		}
		
		if(pos == std::string::npos) {
			messageGUI("Not found");
			return;
		}
		
		int line = disassembly->findLineByAddress(pos);
		if(line >= 0) {
			wxrt_disassembly->GotoLine(line);
			wxrt_disassembly->SetFocus();
		}
		else {
			messageGUI("Could not find line corresponding to position found");
		}
	}
	else {
		wxrt_disassembly->SearchAnchor();
		
		std::cout << "searching for \"" + fn_wxUTF8String_ToUTF8String(wxtc_search->GetValue()) << '"' << std::endl;
		
		int pos;
		if(previous) {
			pos = wxrt_disassembly->SearchPrev(wxSTC_FIND_MATCHCASE, wxtc_search->GetValue());
		}
		else {
			pos = wxrt_disassembly->SearchNext(wxSTC_FIND_MATCHCASE, wxtc_search->GetValue());
		}
		
		if(pos >= 0) {
			std::cout << (previous ? 1 : 0) << " goto " << (pos + (previous ? 0 : 1)) << std::endl;
			wxrt_disassembly->GotoPos(pos + (previous ? 0 : 1)); // without '+1' on next search, the next match would be the same
			wxrt_disassembly->SetFocus();
		}
		else {
			messageGUI("Not found");
		}
	}
}
void Dis8051Frame::OnFindBefore(wxCommandEvent &event)
{
	doSearch(true);
}
void Dis8051Frame::OnFindNext(wxCommandEvent &event)
{
    doSearch(false);
}
void Dis8051Frame::OnGoto(wxCommandEvent &event)
{
    int address = queryAddressFromUser(wxT("Goto"));
    if(address >= 0) {
		int line = disassembly->findLineByAddress(address);
		if(line >= 0) {
			wxrt_disassembly->GotoLine(line);
			wxrt_disassembly->SetFocus();
		}
		else {
			messageGUI("Could not find corresponding line");
		}
    }
}
void Dis8051Frame::OnCommentEnterPressed(wxCommandEvent &event)
{
    int currentLine = wxrt_disassembly->GetCurrentLine();
    auto itAddress = disassembly->addressByLine.find(currentLine);
    if(itAddress != disassembly->addressByLine.end()) {
        auto itInstruction = disassembly->instructions.find(itAddress->second);
        if(itInstruction != disassembly->instructions.end()) {
            disassembly->updateInstructionComment(itAddress->second, 
                    fn_wxUTF8String_ToUTF8String(wxtc_comment->GetValue()));
            messageGUI("Updated comment");
        }
        else {
            messageGUI("No instruction corresponding to that address");
        }
    }
    else {
        messageGUI("No address corresponding to that line");
    }
}
void Dis8051Frame::OnFunctionEnterPressed(wxCommandEvent &event)
{
    int currentLine = wxrt_disassembly->GetCurrentLine();
    auto itAddress = disassembly->addressByLine.find(currentLine);
    std::string name = fn_wxUTF8String_ToUTF8String(wxtc_function->GetValue());
    if(itAddress != disassembly->addressByLine.end()) {
        auto itFunction = disassembly->functions.find(itAddress->second);
        if(itFunction != disassembly->functions.end()) {
            if(name.empty()) {
                messageGUI("Please press the delete button to confirm that you really want to delete that function");
            }
            else {
                disassembly->updateFunctionName(itAddress->second, name);
                function_graph->RefreshCache();
                function_graph->Refresh();
                messageGUI("Renamed function to " + name);
            }
        }
        else {
            if(name.empty()) {
                messageGUI("No empty name allowed");
            }
            else {
                disas8051::Function function;
                function.posX = 10;
                function.posY = 10;
                function.isInterrupt = false;
                function.name = name;
                if(disassembly->instructions.find(itAddress->second) == disassembly->instructions.end()) {
					int address = queryAddressFromUser(wxT("Enter address of the new function"), (int)itAddress->second);
					if(address >= 0) {
						disassembly->addFunction(address, function);
						scroll_overview->Refresh(); // we disassembled some code
						function_graph->RefreshCache();
						function_graph->Refresh();
						wxc_function_shown->SetValue(true);
						messageGUI("Added function " + name);
					}
                }
                else {
                    disassembly->addFunction(itAddress->second, function);
                    function_graph->RefreshCache();
                    function_graph->Refresh();
                    messageGUI("Added function " + name);
                }
            }
        }
    }
    else {
        messageGUI("No address");
    }
}
void Dis8051Frame::OnToggleFunctionShown(wxCommandEvent &event)
{
	int currentLine = wxrt_disassembly->GetCurrentLine();
    auto itAddress = disassembly->addressByLine.find(currentLine);
    if(itAddress != disassembly->addressByLine.end()) {
        auto itFunction = disassembly->findFunction(itAddress->second);
        if(itFunction != disassembly->functions.end()) {
			itFunction->second.isShownInGraph = wxc_function_shown->GetValue();
            function_graph->Refresh();
        }
        else {
            messageGUI("No function on this address");
        }
    }
    else {
        messageGUI("No function on this line");
    }
}
void Dis8051Frame::OnFunctionDelete(wxCommandEvent &event)
{
    int currentLine = wxrt_disassembly->GetCurrentLine();
    auto itAddress = disassembly->addressByLine.find(currentLine);
    if(itAddress != disassembly->addressByLine.end()) {
        auto itFunction = disassembly->findFunction(itAddress->second);
        if(itFunction != disassembly->functions.end()) {
            messageGUI("Deleted function " + itFunction->second.name);
            disassembly->deleteFunction(itFunction->first);
            wxtc_function->SetValue(wxEmptyString);
            wxtc_function_blocks->SetValue(wxEmptyString);
            function_graph->RefreshCache();
            function_graph->Refresh();
        }
        else {
            messageGUI("No function on this address");
        }
    }
    else {
        messageGUI("No function on this line");
    }
}

void Dis8051Frame::OnToggleCallAnnotation(wxCommandEvent &event)
{
	annotation_canvas->annotateCalls = !annotation_canvas->annotateCalls;
	annotation_canvas->Refresh();
}

void Dis8051Frame::OnReadAddressFromCode(wxCommandEvent &event)
{
	int address = queryAddressFromUser("Copy 16-Bit address from code to clipboard");
	if(address >= 0) {
		// Write some text to the clipboard
		if (wxTheClipboard->Open())
		{
			std::string addressStr = fn_FormatToHex(disassembly->buf.substr(address, 1), true) +
					fn_FormatToHex(disassembly->buf.substr(address+1, 1), true);
			// This data objects are held by the clipboard,
			// so do not delete them in the app.
			wxTheClipboard->SetData(new wxTextDataObject(wxString::FromUTF8(addressStr.c_str())));
			wxTheClipboard->Close();
			
			messageGUI("Copied " + addressStr + " to clipboard.");
		}
		else {
			messageGUI("Could not open clipboard.");
		}
	}
}

void Dis8051Frame::messageGUI(std::string msg)
{
    SetStatusText(wxString::FromUTF8(msg.c_str()), 0);
    c_logger->LogInformation(msg);
}

std::string Dis8051Frame::getAddressDescription(uint64_t address)
{
    std::string str = disassembly->formatWithFunction(address);
	if(disassembly->instructions[address].isCall()) {
		str += " (call)";
	}
    return str;
}

std::basic_string<uint8_t> Uint16t_To_UString(uint16_t var)
{
	return std::basic_string<uint8_t>(1, *((uint8_t*)&var))
			+ std::basic_string<uint8_t>(1, *((uint8_t*)&var + 1));
}

int Dis8051Frame::queryAddressFromUser(wxString title, int defaultAddress)
{
	wxTextEntryDialog dlg(this, wxT("Hex address"), title);
	if(defaultAddress >= 0) {
		dlg.SetValue(wxString::Format(wxT("%x"), defaultAddress));
	}
    if(dlg.ShowModal() == wxID_OK) {
        utils::string_to_int_limited_converter convertobj;
        uint64_t address = convertobj.ConvertToLLI_Hex_Limited(
                fn_wxUTF8String_ToUTF8String(dlg.GetValue()), 0, disassembly->buf.length());
        if(convertobj.MistakeHappened() || convertobj.LimitMistakeHappened()) {
            messageGUI("Invalid address");
            return -1;
        }
        else {
			return address;
		}
	}
	else {
		return -1;
	}
}
