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
 
#include <iostream>

#include "logger.h"

#include "../utils/utils.hpp"
#ifdef LOGGER_WXTEXTCTRL
#include "../utils/utils_wx.h"
#endif

#include <boost/thread/lock_guard.hpp>

namespace logger {

#ifdef LOGGER_WXTEXTCTRL
void logdisplay_textctrl::newLine(message& msg)
{
    switch(msg.type)
    {
    case message_type::error:
        m_textctrl->SetDefaultStyle(m_gui_palette->logger_textctrl_error_textattr);
        m_textctrl->AppendText(utils::Format_Defaultdate_wxString(msg.time) + wxT(": ") + wxString::FromUTF8(msg.messageString.c_str()) + wxT("\n"));
        m_textctrl->SetDefaultStyle(m_gui_palette->default_textctrl_textattr);
        break;
    case message_type::warning:
        m_textctrl->SetDefaultStyle(m_gui_palette->logger_textctrl_warning_textattr);
        m_textctrl->AppendText(utils::Format_Defaultdate_wxString(msg.time) + wxT(": ") + wxString::FromUTF8(msg.messageString.c_str()) + wxT("\n"));
        m_textctrl->SetDefaultStyle(m_gui_palette->default_textctrl_textattr);
        break;
    case message_type::information:
        m_textctrl->SetDefaultStyle(m_gui_palette->logger_textctrl_message_textattr);
        m_textctrl->AppendText(utils::Format_Defaultdate_wxString(msg.time) + wxT(": ") + wxString::FromUTF8(msg.messageString.c_str()) + wxT("\n"));
        m_textctrl->SetDefaultStyle(m_gui_palette->default_textctrl_textattr);
        break;
    }
}
#endif // LOGGER_WXTEXTCTRL

void logdisplay_cout::newLine(message& msg)
{
    switch(msg.type)
    {
    case message_type::error:
        std::cout << utils::Format_Defaultdate_stdString(msg.time) << " [error ]: " << msg.messageString << std::endl;
        break;
    case message_type::warning:
        std::cout << utils::Format_Defaultdate_stdString(msg.time) << " [warn  ]: " << msg.messageString << std::endl;
        break;
    case message_type::information:
        std::cout << utils::Format_Defaultdate_stdString(msg.time) << " [inform]: " << msg.messageString << std::endl;
        break;
    }
}

logdisplay_file::logdisplay_file()
{
    isFileOpen = false;
}
void logdisplay_file::beginUpdate()
{
    file.open(filename.c_str(), std::ios_base::out | std::ios_base::app); // If the file already exists, we will append to it. Else, a new file is created.
    if(file)
    {
        isFileOpen = true;
    }
    else
    {
		// only cout to avoid endless recursion
        std::cout << "Logging error: File \"" << filename << "\" could not be opened. " << std::endl;
    }
}
void logdisplay_file::newLine(message& msg)
{
    if(isFileOpen)
    {
        switch(msg.type)
        {
        case message_type::error:
            file << utils::Format_Defaultdate_stdString(msg.time) << " [error ]: " << msg.messageString << std::endl;
            break;
        case message_type::warning:
            file << utils::Format_Defaultdate_stdString(msg.time) << " [warn  ]: " << msg.messageString << std::endl;
            break;
        case message_type::information:
            file << utils::Format_Defaultdate_stdString(msg.time) << " [inform]: " << msg.messageString << std::endl;
            break;
        }
    }
}
void logdisplay_file::endUpdate()
{
    if(isFileOpen)
    {
        file.close();
        isFileOpen = false;
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------

logger::logger()
{
    //
}
logger::~logger()
{
    for(std::list<logdisplay*>::iterator it_logdisplay = logdisplays.begin(); it_logdisplay != logdisplays.end();)
    {
        if(((*it_logdisplay)->type != logdisplay_type::file) && ((*it_logdisplay)->type != logdisplay_type::cout)) {
            delete *it_logdisplay;
            it_logdisplay = logdisplays.erase(it_logdisplay);
        }
        else {
            it_logdisplay++;
        }
    }
    updateLogDisplays(); // little bit unsafe, but to ensure logging..
    for(std::list<logdisplay*>::iterator it_logdisplay = logdisplays.begin(); it_logdisplay != logdisplays.end(); it_logdisplay++)
    {
        delete *it_logdisplay;
    }
}

void logger::LogError(std::string msg)
{
    message newMessage;
    newMessage.time = utils::GetTime_Milliseconds();
    newMessage.type = message_type::error;
    newMessage.messageString = msg;
    registerMessage(newMessage);
}
void logger::LogWarning(std::string msg)
{
    message newMessage;
    newMessage.time = utils::GetTime_Milliseconds();
    newMessage.type = message_type::warning;
    newMessage.messageString = msg;
    registerMessage(newMessage);
}
void logger::LogInformation(std::string msg)
{
    message newMessage;
    newMessage.time = utils::GetTime_Milliseconds();
    newMessage.type = message_type::information;
    newMessage.messageString = msg;
    registerMessage(newMessage);
}
void logger::registerMessage(message &newMessage)
{
    {
        boost::lock_guard<boost::mutex> locker(messagesLock);
        messages.push_back(newMessage);
        if(messages.size() > LOGGER_MAX_LOGS)
        {
            std::list<message>::iterator itMessage = messages.begin();
            itMessage++;
            messages.erase(messages.begin(), itMessage);
        }
    }
    {
        boost::lock_guard<boost::mutex> locker(newMessagesLock);
        newMessages.push_back(newMessage);
        if(newMessages.size() > LOGGER_MAX_LOGS)
        {
            std::list<message>::iterator itMessage = newMessages.begin();
            itMessage++;
            newMessages.erase(newMessages.begin(), itMessage);
        }
    }
    
    // propagate it immediately and do not wait for log display update
    std::cout << utils::Format_Defaultdate_stdString(newMessage.time) << (newMessage.type == message_type::error ? " [error ]: "
			: (newMessage.type == message_type::information ? " [inform]: " : " [warn  ]: ")) << newMessage.messageString << std::endl;
}

#ifdef LOGGER_WXTEXTCTRL
void logger::AddLogDisplay(wxTextCtrl* textctrl, gui_palette* c_gui_palette)
{
    logdisplay_textctrl* newLogdisplay = new logdisplay_textctrl;
    newLogdisplay->type = logdisplay_type::textctrl;
    newLogdisplay->m_textctrl = textctrl;
    newLogdisplay->m_gui_palette = c_gui_palette;
    logdisplays.push_back(newLogdisplay);
}
#endif // LOGGER_WXTEXTCTRL
void logger::AddLogDisplay_cout()
{
    logdisplay_cout* newLogdisplay = new logdisplay_cout;
    newLogdisplay->type = logdisplay_type::cout;
    logdisplays.push_back(newLogdisplay);
}
void logger::AddLogDisplay(std::string dateiname)
{
    logdisplay_file* newLogdisplay = new logdisplay_file;
    newLogdisplay->type = logdisplay_type::file;
    newLogdisplay->filename = dateiname;
    logdisplays.push_back(newLogdisplay);
}

void logger::updateLogDisplays()
{
    std::list<message> newMessagesCopy;
    {
        boost::lock_guard<boost::mutex> locker(newMessagesLock);
        newMessagesCopy = newMessages;
        newMessages.clear();
    }

    for(std::list<logdisplay*>::iterator it_logdisplay = logdisplays.begin(); it_logdisplay != logdisplays.end(); it_logdisplay++)
    {
        (*it_logdisplay)->beginUpdate();
        for(std::list<message>::iterator itMessage = newMessagesCopy.begin(); itMessage != newMessagesCopy.end(); itMessage++)
        {
            (*it_logdisplay)->newLine(*itMessage);
        }
        (*it_logdisplay)->endUpdate();
    }
}

} // namespace logger
