#include <geek/gfx-surface.h>

using namespace Geek;
using namespace Geek::Gfx;

int main(int argc, char** argv)
{
    Surface* surface1 = new HighDPISurface(500, 500, 4);
    //Surface* surface1 = new Surface(500, 500, 4);
    surface1->clear(0x00ff0000);

int i;
for (i = 0; i < 500; i += 10)
{
surface1->drawLine(0, i, 499, i, 0xff000000);
surface1->drawLine(i, 0, i, 499, 0xff000000);
}


    Surface* surfaceVP = new SurfaceViewPort(surface1, 100, 100, 200, 200);
    surfaceVP->clear(0x000000ff);

    Surface* surface2 = new Surface(100, 100, 4);
    surface2->clear(0x0000ff00);
/*
printf("test: Blit to VP 1\n");
    surfaceVP->blit(-50, -50, surface2, 0, 0, 100, 100, false);
printf("test: Blit to VP 2\n");
    surfaceVP->blit(50, 50, surface2, 0, 0, 100, 100, false);

printf("test: Blit to VP 3\n");
    surfaceVP->blit(175, 175, surface2, 0, 0, 100, 100, false);
*/
printf("test: Blit to VP 4\n");
    surfaceVP->blit(50, 0, surface2, 0, 0, 200, 200, false);


#if 0
    Surface* surface3 = new Surface(100, 100, 4);
    surface2->clear(0x00888888);

surface3->drawLine(0, 0, 100, 100, 0xffff0000);
surface3->drawLine(100, 0, 0, 100, 0xff00ff00);

printf("test: Blit to surface 0\n");
    surface1->blit(25, 25, surface3, 0, 0, 100, 100, false);

printf("test: Blit to surface 1\n");
    surface1->blit(-25, -25, surface3, 0, 0, 100, 100, false);

printf("test: Blit to surface 2\n");
    surface1->blit(475, 475, surface3, 0, 0, 100, 100, false);

printf("test: Blit to surface 3\n");
    surface1->blit(-25, 475, surface3, 0, 0, 100, 100, false);
#endif

printf("test: Saving...\n");
    surface1->saveJPEG("blit.jpg");

    return 0;
}

