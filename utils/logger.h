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
 
/*
 * logger.h
 * default implementation of logger_base
 */
 
#ifndef dLOGGER_H_INCLUDED
#define dLOGGER_H_INCLUDED

#include <list>
#include <string>
#include <ctime>
#include <fstream>
#include <boost/thread/mutex.hpp>

#ifdef GUI
#define LOGGER_WXTEXTCTRL
#endif

#include "logger.hpp"
#include "../consts.hpp"
#ifdef LOGGER_WXTEXTCTRL
#include "../gui/gui_palette.h"
#endif

namespace logger {

enum class message_type
{
	information = 0,
	warning,
	error
};
enum class logdisplay_type
{
    textctrl = 0,
    cout,
    file
};

struct message
{
    message_type type;
    std::string messageString;
    long long int time;
};
class logdisplay
{
public:
    logdisplay_type type;
    virtual ~logdisplay() {}
    virtual void newLine(message &newMessage) = 0;
    virtual void beginUpdate() {}
    virtual void endUpdate() {}
};
#ifdef LOGGER_WXTEXTCTRL
class logdisplay_textctrl: public logdisplay
{
public:
    wxTextCtrl* m_textctrl;
    gui_palette* m_gui_palette;
    virtual ~logdisplay_textctrl() {}
    virtual void newLine(message &newMessage);
};
#endif // LOGGER_WXTEXTCTRL
class logdisplay_cout: public logdisplay
{
public:
    virtual ~logdisplay_cout() {}
    virtual void newLine(message &newMessage);
};
class logdisplay_file: public logdisplay
{
public:
    std::string filename;
    std::ofstream file;
    bool isFileOpen;
    logdisplay_file();
    virtual ~logdisplay_file() {}
    virtual void newLine(message &newMessage);
    virtual void beginUpdate();
    virtual void endUpdate();
};

class logger: public logger_base
{
private:
    boost::mutex messagesLock;
    std::list<message> messages;
    boost::mutex newMessagesLock;
    std::list<message> newMessages;
    std::list<logdisplay*> logdisplays;

public:
    logger();
    virtual ~logger();

    virtual void LogError(std::string msg);
    virtual void LogInformation(std::string msg);
    virtual void LogWarning(std::string msg);

   #ifdef LOGGER_WXTEXTCTRL
    void AddLogDisplay(wxTextCtrl* textctrl, gui_palette* c_gui_palette);
   #endif
    void AddLogDisplay_cout();
    void AddLogDisplay(std::string dateiname);

    void updateLogDisplays();

private:
    void registerMessage(message &newMessage);
};

} // namespace logger

#endif // dLOGGER_H_INCLUDED
