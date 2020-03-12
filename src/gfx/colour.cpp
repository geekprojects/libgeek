
#include <geek/gfx-colour.h>

#include <math.h>

using namespace std;
using namespace Geek::Gfx;

void Colour::toHSB(double* hsb)
{
    double hue;
    double saturation;
    double brightness;

    int cmax = (r > g) ? r : g;
    if (b > cmax)
    {
        cmax = b;
    }

    int cmin = (r < g) ? r : g;
    if (b < cmin)
    {
        cmin = b;
    }
  
    brightness = ((float) cmax) / 255.0f;
    if (cmax != 0)
    {
        saturation = ((float) (cmax - cmin)) / ((float) cmax);
    }
    else
    {
        saturation = 0;
    }

    if (saturation == 0)
    {
        hue = 0;
    }
    else
    {
        double redc = ((double) (cmax - r)) / ((double) (cmax - cmin));
        double greenc = ((double) (cmax - g)) / ((double) (cmax - cmin));
        double bluec = ((double) (cmax - b)) / ((double) (cmax - cmin));
        if (r == cmax)
        {
            hue = bluec - greenc;
        }
        else if (g == cmax)
        {
            hue = 2.0f + redc - bluec;
        }
        else
        {
            hue = 4.0f + greenc - redc;
        }

        hue = hue / 6.0f;
        if (hue < 0)
        {
            hue = hue + 1.0f;
        }
    }

    hsb[0] = hue;
    hsb[1] = saturation;
    hsb[2] = brightness;
}

Colour Colour::fromHSB(double hue, double saturation, double brightness)
{
    if (saturation <= 0.0)
    {
        brightness = brightness * 255.0f + 0.5f;
        int br = (int)brightness;
        return Colour(br, br, br);
    }

    double h = (hue - (double)floor(hue)) * 6.0f;
    double f = h - (double)floor(h);
    double p = brightness * (1.0f - saturation);
    double q = brightness * (1.0f - saturation * f);
    double t = brightness * (1.0f - (saturation * (1.0f - f)));

    brightness = brightness * 255.0f + 0.5f;
    t = t * 255.0f + 0.5f;
    p = p * 255.0f + 0.5f;
    q = q * 255.0f + 0.5f;

    Colour c;
    switch ((int)h)
    {
        case 0:
            c.r = (int)(brightness);
            c.g = (int)(t);
            c.b = (int)(p);
            break;
        case 1:
            c.r = (int)(q);
            c.g = (int)(brightness);
            c.b = (int)(p);
            break;
        case 2:
            c.r = (int)(p);
            c.g = (int)(brightness);
            c.b = (int)(t);
            break;
        case 3:
            c.r = (int)(p);
            c.g = (int)(q);
            c.b = (int)(brightness);
            break;
        case 4:
            c.r = (int)(t);
            c.g = (int)(p);
            c.b = (int)(brightness);
            break;
        case 5:
            c.r = (int)(brightness);
            c.g = (int)(p);
            c.b = (int)(q);
            break;
    }

    return c;
}

string Colour::toHexString()
{
    string hexstr = "#";
    char buf[10];
    snprintf(buf, 10, "%02x", r);
    hexstr += string(buf);
    snprintf(buf, 10, "%02x", g);
    hexstr += string(buf);
    snprintf(buf, 10, "%02x", b);
    hexstr += string(buf);

    return hexstr;
}

Colour Colour::fromHexString(std::string str)
{
    int pos = 0;
    int len = str.length();
    if (len > 0 && str.at(pos) == '#')
    {
        pos++;
        len--;
    }

    Colour c(0, 0, 0);
    if (len == 6)
    {
        char buf[3];
        buf[2] = 0;

        buf[0] = str.at(pos++);
        buf[1] = str.at(pos++);
        c.r = strtol(buf, NULL, 16);

        buf[0] = str.at(pos++);
        buf[1] = str.at(pos++);
        c.g = strtol(buf, NULL, 16);

        buf[0] = str.at(pos++);
        buf[1] = str.at(pos++);
        c.b = strtol(buf, NULL, 16);
    }
    else if (len == 3)
    {
        char buf[2];
        buf[1] = 0;

        buf[0] = str.at(pos++);
        c.r = strtol(buf, NULL, 16);
        c.r = (c.r << 4) | c.r;

        buf[0] = str.at(pos++);
        c.g = strtol(buf, NULL, 16);
        c.g = (c.g << 4) | c.g;

        buf[0] = str.at(pos++);
        c.b = strtol(buf, NULL, 16);
        c.b = (c.b << 4) | c.b;
    }

    return c;
}

