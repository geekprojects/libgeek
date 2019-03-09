
#include <stdio.h>
#include <geek/core-sha.h>

using namespace std;
using namespace Geek::Core;

int main(int argc, char** argv)
{
    string hash;

    hash = SHA::sha256("Hello world");
    printf("%s\n", hash.c_str());

    hash = SHA::sha256File("/etc/hosts");
    printf("%s\n", hash.c_str());

    return 0;
}

