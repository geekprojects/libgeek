
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

    for (it = parts.begin(); it != parts.end(); it++)
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

