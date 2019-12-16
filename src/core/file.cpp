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


#include <geek/core-file.h>
#include <geek/core-string.h>

#include <errno.h>
#include <sys/stat.h>

using namespace std;
using namespace Geek::Core;

bool File::mkdirs(string path)
{
    string current = "";
    vector<string> parts = splitString(path, '/');
    vector<string>::iterator it;

    for (it = parts.begin(); it != parts.end(); ++it)
    {
        string part = *it;
        current += "/" + part;

        struct stat stat;
        int res;
        res = ::stat(current.c_str(), &stat);
        int err = errno;
        if (res == -1)
        {
            if (err == ENOENT)
            {
                res = mkdir(current.c_str(), 0777);
                if (res != 0)
                {
                    return false;
                }
            }
        }
        else
        {
            if (!(stat.st_mode & S_IFDIR))
            {
                return false;
            }
        }
    }
    return true;
}

