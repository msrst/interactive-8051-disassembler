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
