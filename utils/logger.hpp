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
 
#ifndef LOGGER_HPP_INCLUDED
#define LOGGER_HPP_INCLUDED

#include <string>
#include <boost/shared_ptr.hpp>

namespace logger {

class logger_base
{
public:
    virtual ~logger_base() {}

    virtual void LogError(std::string fehler) = 0;
    virtual void LogWarning(std::string warnung) = 0;
    virtual void LogInformation(std::string meldung) = 0;
};

typedef boost::shared_ptr<logger_base> logger_base_ptr;

} // namespace logger

#endif // LOGGER_HPP_INCLUDED
