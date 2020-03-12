#ifndef __GEEK_GFX_COLOUR_H_
#define __GEEK_GFX_COLOUR_H_

#include <stdint.h>
#include <string>

namespace Geek
{
namespace Gfx
{

struct Colour
{
    uint8_t alpha;
    uint8_t r;
    uint8_t g;
    uint8_t b;

    Colour()
    {
        alpha = 255;
        r = 0;
        g = 0;
        b = 0;
    }

    Colour(uint8_t _r, uint8_t _g, uint8_t _b)
    {
        alpha = 255;
        r = _r;
        g = _g;
        b = _b;
    }

    Colour(uint8_t _alpha, uint8_t _r, uint8_t _g, uint8_t _b)
    {
        alpha = _alpha;
        r = _r;
        g = _g;
        b = _b;
    }

    void toHSB(double* hsb);
    uint32_t getInt32() { return (alpha << 24) | (r << 16) | (g << 8) | (b); }
    std::string toHexString();

    float redFloat() { return ((float)r / 255.0); }
    float greenFloat() { return ((float)g / 255.0); }
    float blueFloat() { return ((float)b / 255.0); }

    static Colour fromHexString(std::string str);
    static Colour fromHSB(double hue, double saturation, double brightness);
};

};
};

#endif
