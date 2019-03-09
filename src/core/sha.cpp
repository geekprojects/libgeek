
#include <geek/core-sha.h>

#include "sha/sha.h"

using namespace std;
using namespace Geek::Core;

static string hex(uint8_t* bytes, int length)
{
    string str = "";
    int i;
    for (i = 0; i < length; i++)
    {
        char bytestr[5];
        sprintf(bytestr, "%02x", bytes[i]);
        str += bytestr;
    }
    return str;
}

string SHA::sha256(string text)
{
    USHAContext context;
    int res;

    res = USHAReset(&context, SHA256);
    if (res != shaSuccess)
    {
        printf("SHA::sha256: Failed to init context\n");
        return "";
    }

    res = USHAInput(&context, (uint8_t*)text.c_str(), text.length());
    if (res != shaSuccess)
    {
        printf("SHA::sha256: Failed to hash text\n");
        return "";
    }

    uint8_t digest[SHA256HashSize];
    res = USHAResult(&context, digest);
    if (res != shaSuccess)
    {
        printf("SHA::sha256: Failed to hash text\n");
        return "";
    }

    return hex(digest, SHA256HashSize);
}

string SHA::sha256File(string path)
{
    USHAContext context;
    int res;

    res = USHAReset(&context, SHA256);
    if (res != shaSuccess)
    {
        printf("SHA::sha256: Failed to init context\n");
        return "";
    }

    FILE* fd = fopen(path.c_str(), "r");
    if (fd == NULL)
    {
        printf("SHA::sha256: Failed to open path: %s\n", path.c_str());
        return "";
    }

    char buffer[4096];
    while (!feof(fd))
    {
        int r;
        r = fread(buffer, 1, 4096, fd);
        if (r <= 0)
        {
            break;
        }
        res = USHAInput(&context, (uint8_t*)buffer, r);
        if (res != shaSuccess)
        {
            printf("SHA::sha256: Failed to hash file\n");
            fclose(fd);
            return "";
        }
    }
    fclose(fd);

    uint8_t digest[SHA256HashSize];
    res = USHAResult(&context, digest);
    if (res != shaSuccess)
    {
        printf("SHA::sha256: Failed to hash text\n");
        return "";
    }

    return hex(digest, SHA256HashSize);
}

