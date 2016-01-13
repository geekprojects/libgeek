
#include <geek/gfx-surface.h>

using namespace Geek;
using namespace Geek::Gfx;

int main(int argc, char** argv)
{
    Surface* test1 = Surface::loadImage("test.tga");
if (test1 != NULL)
{
test1->saveJPEG("out1.jpg");
delete test1;
}

    Surface* test2 = Surface::loadImage("test2.tga");
if (test2 != NULL)
{
test2->saveJPEG("out2.jpg");
delete test2;
}
    return 0;
}

