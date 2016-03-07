/*
 * libgeek - The GeekProjects utility suite
 * Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 * This file is part of libgeek.
 *
 * libgeek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgeek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBGEEK_CORE_LOGGER_H_
#define __LIBGEEK_CORE_LOGGER_H_

#include <string>

namespace Geek {

enum LoggerLevel_t
{
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger
{
 private:
    std::string m_name;
    int m_depth;

 public:
    Logger(std::string name);
    ~Logger();

    void log(LoggerLevel_t level, const char* __format, ...);

    // Not thread safe!
    void pushDepth() { m_depth++; }
    void popDepth()  { m_depth--; }
};

};

#endif
