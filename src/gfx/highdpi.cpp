/*
 *  libgeek - The GeekProjects utility suite
 *  Copyright (C) 2018 GeekProjects.com
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


#include <stdio.h>
#include <string.h>

#include <geek/gfx-surface.h>
#include <geek/core-compiler.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

#define HIGHDPI_RATIO 2

HighDPISurface::HighDPISurface(uint32_t width, uint32_t height, uint8_t bpp) :
    Surface(width * HIGHDPI_RATIO, height * HIGHDPI_RATIO, bpp)
{
    m_highDPI = true;
}

HighDPISurface::~HighDPISurface()
{
}

bool HighDPISurface::drawSubPixel(int32_t x, int32_t y, uint32_t c)
{
    return Geek::Gfx::Surface::drawPixel(x, y, c, getDrawingBuffer());
}

bool HighDPISurface::drawPixel(int32_t x, int32_t y, uint32_t c)
{
    return HighDPISurface::drawPixel(x, y, c, getDrawingBuffer());
}

bool HighDPISurface::drawPixel(int32_t x, int32_t y, uint32_t c, uint8_t* dest)
{
    x *= 2;
    y *= 2;
    Surface::drawPixel(x, y, c, getDrawingBuffer());
    Surface::drawPixel(x + 1, y, c, getDrawingBuffer());
    Surface::drawPixel(x, y + 1, c, getDrawingBuffer());
    Surface::drawPixel(x + 1, y + 1, c, getDrawingBuffer());
    return true;
}

bool HighDPISurface::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c)
{
    x1 *= 2;
    y1 *= 2;
    x2 *= 2;
    y2 *= 2;

    if (x1 == x2)
    {
        Surface::drawLine(x1, y1, x2, y2 + 1, c);
        Surface::drawLine(x1 + 1, y1, x2 + 1, y2 + 1, c);
    }
    else if (y1 == y2)
    {
        Surface::drawLine(x1, y1, x2 + 1, y2, c);
        Surface::drawLine(x1, y1 + 1, x2 + 1, y2 + 1, c);
    }
    else
    {
        Surface::drawLine(x1, y1, x2, y2, c);
        Surface::drawLine(x1 + 1, y1, x2 + 1, y2, c);
        Surface::drawLine(x1, y1 + 1, x2, y2 + 1, c);
        Surface::drawLine(x1 + 1, y1 + 1, x2 + 1, y2 + 1, c);
    }
    return true;
}

bool HighDPISurface::drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
    x *= 2;
    y *= 2;
    w *= 2;
    h *= 2;
    return Surface::drawRectFilled(x, y, w, h, c);
}

bool HighDPISurface::drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
    x *= 2;
    y *= 2;
    w *= 2;
    h *= 2;
    return Surface::drawRect(x, y, w, h, c);
}

bool HighDPISurface::drawGrad(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c1, uint32_t c2)
{
    x *= 2;
    y *= 2;
    w *= 2;
    h *= 2;
    return Surface::drawGrad(x, y, w, h, c1, c2);
}

bool HighDPISurface::drawCircle(int32_t x, int32_t y, uint32_t r, uint32_t c)
{
    return Surface::drawCircle(x * 2, y * 2, r * 2, c);
}

bool HighDPISurface::drawCircleFilled(int32_t x, int32_t y, uint32_t r, uint32_t c)
{
    return Surface::drawCircleFilled(x * 2, y * 2, r * 2, c);
}

bool HighDPISurface::blit(int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha)
{
    return Surface::blit(x * 2, y * 2, data, w, h, bbp, alpha);
}

bool HighDPISurface::blit(int32_t x, int32_t y, Surface* surface, bool forceAlpha)
{
    return Surface::blit(x * 2, y * 2, surface, forceAlpha);
}

bool HighDPISurface::blit(int32_t destX, int32_t destY, Surface* surface, int viewX, int viewY, int viewW, int viewH, bool forceAlpha)
{
    return Surface::blit(destX * 2, destY * 2, surface, viewX, viewY, viewW, viewH, forceAlpha);
}

bool HighDPISurface::blit(
        uint8_t* destBuffer,
        int32_t x,
        int32_t y,
        uint8_t* srcData,
        uint32_t srcWidth,
        uint32_t srcHeight,
        uint32_t bytesPerPixel,
        int32_t viewX,
        int32_t viewY,
        uint32_t viewWidth,
        uint32_t viewHeight,
        bool forceAlpha)
{
    return Surface::blit(
        destBuffer,
        x * 2,
        y * 2,
        srcData,
        srcWidth,
        srcHeight,
        bytesPerPixel,
        viewX,
        viewY,
        viewWidth * 2,
        viewHeight * 2,
        forceAlpha);
}

