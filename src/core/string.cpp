
#include <geek/core-string.h>

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

