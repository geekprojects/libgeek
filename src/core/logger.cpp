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
    m_name = name;
}

Logger::~Logger()
{
}

void Logger::log(LoggerLevel_t level, const char* msg, ...)
{
    va_list va;
    va_start(va, msg);

    char buf[256];
    vsnprintf(buf, 256, msg, va);

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

    char timeStr[256];
    time_t t;
    struct tm *tm;
    t = time(NULL);
    tm = localtime(&t);
    strftime(timeStr, 256, "%Y/%m/%d %H:%M:%S", tm);

    printf("%s: %s: %s: %s\n", timeStr, levelStr.c_str(), m_name.c_str(), buf);

}
