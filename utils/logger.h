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
