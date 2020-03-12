
#include <geek/gfx-colour.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

int main(int argc, char** argv)
{

Colour c = Colour::fromHexString("#110230");
assert(c.r == 0x11);
assert(c.g == 0x02);
assert(c.b == 0x30);
printf("c: %s\n", c.toHexString().c_str());

c = Colour::fromHexString("#abc");
printf("c: %s\n", c.toHexString().c_str());
assert(c.r == 0xaa);
assert(c.g == 0xbb);
assert(c.b == 0xcc);

    return 0;
}

