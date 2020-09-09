/*
 *  libgeek - The GeekProjects utility suite
 *  Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 *  This file is part of libgeek.
 *
 *  libgeek is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libgeek is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdarg.h>
#include <geek/core-logger.h>

using namespace std;
using namespace Geek;

Logger::Logger(std::string name)
{
    m_depth = 0;

    setLoggerName(name);
}

Logger::Logger(std::wstring name)
{
    m_depth = 0;

    setLoggerName(name);
}

Logger::~Logger()
{
}

void Logger::setLoggerName(std::string name)
{
    m_name = name;
}

void Logger::setLoggerName(std::wstring name)
{
    m_name = "";

    unsigned int i;
    for (i = 0; i < name.length(); i++)
    {
        m_name += (char)name.at(i);
    }
}

void Logger::log(LoggerLevel_t level, const char* msg, ...)
{
    va_list va;
    va_start(va, msg);

    logv(level, msg, va);

    va_end(va);
}

void Logger::debug(const char* msg, ...)
{
    va_list va;
    va_start(va, msg);

    logv(DEBUG, msg, va);

    va_end(va);
}

void Logger::error(const char* msg, ...)
{
    va_list va;
    va_start(va, msg);

    logv(ERROR, msg, va);

    va_end(va);
}

void Logger::logv(LoggerLevel_t level, const char* msg, va_list va)
{
    char buf[4096];
    vsnprintf(buf, 4096, msg, va);

    std::string levelStr = "";
    switch (level)
    {
        case DEBUG:
            levelStr = "DEBUG";
            break;
        case INFO:
            levelStr = "INFO";
            break;
        case WARN:
            levelStr = "WARN";
            break;
        case ERROR:
            levelStr = "ERROR";
            break;
        default:
            levelStr = "UNKNOWN";
            break;
    }

    string spaces = "";
    int i;
    for (i = 0; i < m_depth; i++)
    {
        spaces += "  ";
    }

    char timeStr[256];
    time_t t;
    t = time(NULL);

    struct tm tm;
    localtime_r(&t, &tm);

    strftime(timeStr, 256, "%Y/%m/%d %H:%M:%S", &tm);

    printf("%s: %s: %s: %s%s\n", timeStr, levelStr.c_str(), m_name.c_str(), spaces.c_str(), buf);
}

