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

#include <string.h>

#include <geek/core-string.h>

#include "utf8.h"

using namespace std;
using namespace Geek::Core;

vector<string> Geek::Core::splitString(string line, char splitChar)
{
    vector<string> parts;

    while (line.length() > 0)
    {
        size_t pos = line.find(splitChar);
        if (pos == string::npos)
        {
            pos = line.length();
            if (pos == 0)
            {
                break;
            }
        }
        if (pos >= 1)
        {
            string part = line.substr(0, pos);
            parts.push_back(part);
        }
        if (pos == line.length())
        {
            break;
        }
        line = line.substr(pos + 1);
    }

    return parts;
}

wstring Geek::Core::utf82wstring(const char* start)
{
    if (start == NULL)
    {
        return L"";
    }

    int length = strlen(start);
    if (length <= 0)
    {
        return L"";
    }

    return utf82wstring(start, length);
}

wstring Geek::Core::utf82wstring(const char* start, int length)
{
    if (start == NULL)
    {
        return L"";
    }

    const char* pos = start;
    const char* end = pos + length;
    wstring result = L"";

    try
    {
        while (pos < end)
        {
            wchar_t cur = utf8::next(pos, end);
            if (cur == 0)
            {
                break;
            }
            result += cur;
        }
    }
    catch (Geek::utf8::invalid_utf8& e)
    {
        printf("Geek::Core::utf82wstring: Invalid UTF-8 character");
    }
    return result;
}

string Geek::Core::wstring2utf8(wstring str)
{
    string result = "";

    unsigned int pos;
    for (pos = 0; pos < str.length(); pos++)
    {
        wchar_t c = str.at(pos);
        char buffer[6] = {0, 0, 0, 0, 0, 0};
        char* end = utf8::append(c, buffer);
        char* p;
        for (p = buffer; p < end; p++)
        {
            result += *p;
        }
    }
    return result;
}



