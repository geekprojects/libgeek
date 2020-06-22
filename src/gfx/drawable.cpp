/*
 *  libgeek - The GeekProjects utility suite
 *  Copyright (C) 2014, 2015, 2016 GeekProjects.com
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <jpeglib.h>

#include <geek/gfx-drawable.h>
#include <geek/gfx-surface.h>

using namespace Geek;
using namespace Geek::Gfx;

Drawable::Drawable(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
    m_bytesPerPixel = 4;
    m_dpiX = 96;
    m_dpiY = 96;

    m_drawingBuffer = NULL;
    m_drawingBufferLength = 0;
}

Drawable::~Drawable()
{
}

bool Drawable::drawPixel(int32_t x, int32_t y, uint32_t c)
{
    return drawPixel(x, y, c, getDrawingBuffer());
}

static inline void draw32a(uint8_t* src, uint8_t* dest, uint8_t alpha)
{
    
    dest[0] = dest[0] + (((src[0] - dest[0]) * alpha) >> 8);
    dest[1] = dest[1] + (((src[1] - dest[1]) * alpha) >> 8);
    dest[2] = dest[2] + (((src[2] - dest[2]) * alpha) >> 8);
    dest[3] = dest[3] + (((src[3] - dest[3]) * alpha) >> 8);
}

bool Drawable::drawPixel(int32_t x, int32_t y, uint32_t c, uint8_t* dest)
{
    if (dest == NULL)
    {
        return false;
    }

    uint8_t alpha = c >> 24;

    if (alpha == 0)
    {
        return true;
    }

    uintptr_t p = (uintptr_t)dest + getOffset(x, y);

    if (alpha == 255)
    {
        uint32_t* dest = (uint32_t*)p;
        *dest = c;
    }
    else
    {
        draw32a((uint8_t*)&c, (uint8_t*)p, alpha);
    }

    return true;
}


bool Drawable::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c)
{
    uint8_t alpha = c >> 24;

    if (alpha == 0)
    {
        return true;
    }

    uint8_t* drawingBuffer = getDrawingBuffer();

    if (x1 == x2 && y1 == y2)
    {
        if (intersects(x1, y1))
        {
            Drawable::drawPixel(x1, y1, c, drawingBuffer);
        }
        return true;
    }

    if (x1 == x2 || y1 == y2)
    {
        int i;

        if (y1 == y2)
        {
            if (y1 < 0 || y1 >= (int)getHeight())
            {
                return true;
            }

            if (x1 > x2)
            {
                int  t = x1;
                x1 = x2;
                x2 = t;
            }

            if (x1 < 0)
            {
                x1 = 0;
            }
            if (x2 >= (int)getWidth())
            {
                x2 = getWidth() - 1;
            }

            if (x1 > (int)getWidth() || x2 < 0)
            {
                return true;
            }

            uint32_t* p = (uint32_t*)(drawingBuffer + getOffset(x1, y1));
            int dx = x2 - x1;

            if (alpha == 255)
            {
#ifdef HAVE_MEMSET_PATTERN4
                memset_pattern4(p, &c, dx * 4);
#else
                for (i = 0; i < dx; i++, p++)
                {
                    *p = c;
                }
#endif
            }
            else
            {
                for (i = 0; i < dx; i++, p++)
                {
                    draw32a((uint8_t*)&c, (uint8_t*)p, alpha);
                }
            }
        }
        else
        {
            if (x1 < 0 || x1 >= (int)getWidth())
            {
                return true;
            }

            if (y1 > y2)
            {
                int t = y1;
                y1 = y2;
                y2 = t;
            }

            if (y1 < 0)
            {
                y1 = 0;
            }
            if (y2 >= (int)getHeight())
            {
                y2 = getHeight() - 1;
            }

            if (y1 > (int)getWidth() || y2 < 0)
            {
                return true;
            }

            uint32_t* p = (uint32_t*)(drawingBuffer + getOffset(x1, y1));
            int dy = y2 - y1;

            if (alpha == 255)
            {
                for (i = 0; i < dy; i++, p += getWidth())
                {
                    *p = c;
                }
            }
            else
            {
                for (i = 0; i < dy; i++, p++)
                {
                    draw32a((uint8_t*)&c, (uint8_t*)p, alpha);
                }
            }
        }
    }
    else
    {
        int x;
        int y;
        int i;

        int dx = x2 - x1;
        int dy = y2 - y1;
        int dx1 = abs(dx);
        int dy1 = abs(dy);

        int px = 2 * dy1 - dx1;
        int py = 2 * dx1 - dy1;
        if (dy <= dx)
        {
            int xe;

            if (dx >= 0)
            {
                x = x1;
                y = y1;
                xe = x2;
            }
            else
            {
                x = x2;
                y = y2;
                xe = x1;
            }
            if (intersects(x, y))
            {
                Drawable::drawPixel(x, y, c, drawingBuffer);
            }

            for (i = 0; x < xe && x < (int)m_width; i++)
            {
                x++;

                if (px < 0)
                {
                    px = px + 2 * dy1;
                }
                else
                {
                    if((dx<0 && dy<0) || (dx>0 && dy>0))
                    {
                        y++;
                    }
                    else
                    {
                        y--;
                    }
                    px = px + 2 * (dy1 - dx1);
                }

                if (intersects(x, y))
                {
                    Drawable::drawPixel(x, y, c, drawingBuffer);
                }
            }
        }
        else
        {
            int ye;
            if (dy >= 0)
            {
                x=x1;
                y=y1;
                ye=y2;
            }
            else
            {
                x=x2;
                y=y2;
                ye =y1;
            }

            if (intersects(x, y))
            {
                Drawable::drawPixel(x, y, c, drawingBuffer);
            }

            for (i = 0; y < ye && y < (int)m_width; i++)
            {
                y++;
                if (py <= 0)
                {
                    py = py + 2 * dx1;
                }
                else
                {
                    if ((dx<0 && dy<0) || (dx>0 && dy>0))
                    {
                        x++;
                    }
                    else
                    {
                        x--;
                    }
                    py = py + 2 * (dx1 - dy1);
                }

                if (intersects(x, y))
                {
                    Drawable::drawPixel(x, y, c, drawingBuffer);
                }
            }
        }
    }
    return true;
}


bool Drawable::drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
    bool res;

    Rect rect(x, y, w, h);
    res = rect.clip(getRect());
    if (!res)
    {
        return false;
    }

    c |= 0xff000000;

    uint8_t* drawingBuffer = getDrawingBuffer();

    uint8_t* dest = drawingBuffer + getOffset(rect.x, rect.y);
    uint32_t stride = getStride();
#ifndef HAVE_MEMSET_PATTERN4
    stride -= rect.w * 4;
#endif

    int y1;
    for (y1 = 0; y1 < rect.h; y1++)
    {
#if HAVE_MEMSET_PATTERN4
        memset_pattern4(dest, &c, rect.w * 4);
#else
        int x1;
        for (x1 = 0; x1 < rect.w; x1++)
        {
            *((uint32_t*)dest) = c;
            dest += 4;
        }
#endif
        dest += stride;
    }

    return true;
}

bool Drawable::drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
#ifdef DEBUG_DRAWING
    printf("Drawable::drawRect: x=%d, y=%d, w=%d, h=%d, width=%d, height=%d\n", x, y, w, h, getWidth(), getHeight());
#endif
    c |= 0xff000000;

    w--;
    h--;
    Drawable::drawLine(x, y, x + w, y, c);
    Drawable::drawLine(x, y, x, h + y, c);
    Drawable::drawLine(x + w, y, x + w, y + h + 1, c);
    Drawable::drawLine(x, y + h, x + w, y + h, c);
    return true;
}

bool Drawable::drawGrad(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c1, uint32_t c2)
{
    uint8_t* c1bytes = (uint8_t*)&c1;
    uint8_t* c2bytes = (uint8_t*)&c2;

    float d[4];
    d[2] = 0.0;
    d[2] = ((float)c2bytes[2] - (float)c1bytes[2]) / (float)h;
    d[1] = ((float)c2bytes[1] - (float)c1bytes[1]) / (float)h;
    d[0] = ((float)c2bytes[0] - (float)c1bytes[0]) / (float)h;

    float v[4];
    v[3] = 255.0;
    v[2] = (float)(c1bytes[2]);
    v[1] = (float)(c1bytes[1]);
    v[0] = (float)(c1bytes[0]);

    uint32_t j;
    for (j = 0; j < h; j++)
    {
        uint32_t out;
        uint8_t* outbytes = (uint8_t*)&out;
        outbytes[3] = (int)v[3];
        outbytes[2] = (int)v[2];
        outbytes[1] = (int)v[1];
        outbytes[0] = (int)v[0];
        //v[3] += d[3];
        v[2] += d[2];
        v[1] += d[1];
        v[0] += d[0];

        Drawable::drawLine(x, y + j, x + (w - 1), y + j, out);
    }
    return true;
}

bool Drawable::clear(uint32_t c)
{
    return Drawable::drawRectFilled(0, 0, getWidth(), getHeight(), c);
}

void Drawable::darken()
{
    unsigned int i;
    uint8_t* p = getDrawingBuffer();
    for (i = 0; i < m_width * m_height; i++)
    {
        *(p++) >>= 1;
        *(p++) >>= 1;
        *(p++) >>= 1;
        p++;
    }
}

bool Drawable::blit(
    uint8_t* destBuffer,
    int32_t x,
    int32_t y,
    uint8_t* data,
    uint32_t w,
    uint32_t h,
    uint32_t bytesPerPixel,
    bool alpha)
{
    return Drawable::blit(destBuffer, x, y, data, w, h, bytesPerPixel, 0, 0, w, h, alpha);
}

bool Drawable::blit(
    int32_t x,
    int32_t y,
    uint8_t* data,
    uint32_t w,
    uint32_t h,
    uint32_t bytesPerPixel,
    bool alpha)
{
    bool res;
    if (data == NULL)
    {
        return false;
    }
    res = Drawable::blit(getDrawingBuffer(), x, y, data, w, h, bytesPerPixel, alpha);
    if (res)
    {
        //m_dirty.addDirtyRegion(x, y, w, h);
    }
    return res;
}

bool Drawable::blit(
    int32_t x,
    int32_t y,
    Surface* surface,
    bool alpha)
{
    if (surface == NULL)
    {
        return false;
    }

    if (x == 0 && y == 0 && surface->getWidth() == getWidth() && surface->getHeight() == getHeight())
    {
        memcpy(getDrawingBuffer(), surface->getDrawingBuffer(), getWidth() * getHeight() * 4);
        return true;
    }

    return Drawable::blit(x, y, surface->getData(), surface->getWidth(), surface->getHeight(), surface->getBytesPerPixel(), alpha);
}

bool Drawable::blit(int32_t destX, int32_t destY, Surface* surface, int viewX, int viewY, int viewW, int viewH, bool forceAlpha)
{
    return Drawable::blit(
        getDrawingBuffer(),
        destX, destY,
        surface->getDrawingBuffer(),
        surface->getWidth(), surface->getHeight(), surface->getBytesPerPixel(),
        viewX,
        viewY,
        viewW,
        viewH,
        forceAlpha);
}


/*
 * +----------------+
 * |                |
 * |  +.....+       |
 * |  . +-+ .       |
 * |  . | | |       |
 * |  . +-+ |       |
 * |  +.... +       |
 * |                |
 * |                |
 * +----------------+
 *
 */

bool Drawable::blit(
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
    if (srcData == NULL)
    {
        return false;
    }

    Rect destRect(0, 0, getWidth(), getHeight()); // The destination surface
    Rect drawRect(x, y, viewWidth, viewHeight); // The rectangle we're drawing in to
    Rect srcRect(0, 0, srcWidth, srcHeight);
    Rect viewRect(viewX, viewY, viewWidth, viewHeight); // The area of the source we're copying from

#ifdef DEBUG_DRAWBLE_BLIT
    printf("blit: BEFORE destRect=%s\n", destRect.toString().c_str());
    printf("blit: BEFORE drawRect=%s\n", drawRect.toString().c_str());
    printf("blit: BEFORE  srcRect=%s\n",  srcRect.toString().c_str());
    printf("blit: BEFORE viewRect=%s\n", viewRect.toString().c_str());
#endif

    bool res;
    res = drawRect.clip(destRect);
    if (!res)
    {
        return false;
    }

    if (drawRect.x > x)
    {
        int d = drawRect.x - x;
        viewRect.x += d;
    }

    if (drawRect.y > y)
    {
        int d = drawRect.y - y;
        viewRect.y += d;
    }
    viewRect.w = drawRect.w;
    viewRect.h = drawRect.h;

    res = viewRect.clip(srcRect);
    if (!res)
    {
        return false;
    }

    drawRect.w = viewRect.w;
    drawRect.h = viewRect.h;

#ifdef DEBUG_DRAWBLE_BLIT
    printf("blit: AFTER destRect=%s\n", destRect.toString().c_str());
    printf("blit: AFTER drawRect=%s\n", drawRect.toString().c_str());
    printf("blit: AFTER viewRect=%s\n", viewRect.toString().c_str());
#endif

    int32_t y1;
    uint32_t viewStride = viewRect.w * bytesPerPixel;
    uint32_t dataStride = srcWidth * bytesPerPixel;

    uint8_t* dest = destBuffer + getOffset(drawRect.x, drawRect.y);

    srcData += (viewRect.x + (viewRect.y * srcWidth)) * bytesPerPixel;

#ifdef DEBUG_DRAWBLE_BLIT
    printf("Drawable::blit: src: %p: w=%d, h=%d\n", srcData, srcWidth, srcHeight);
    printf("Drawable::blit: dest: %p: x=%d, y=%d. dest w=%d, h=%d\n", dest, x, y, getWidth(), getHeight());
    printf("Drawable::blit: viewX=%d, viewY=%d, viewWidth=%d, viewHeight=%d\n", viewX, viewY, viewWidth, viewHeight);
#endif

    if (!forceAlpha)// && getBytesPerPixel() == bytesPerPixel)
    {
        for (y1 = 0; y1 < (int32_t)viewRect.h; y1++)
        {
            memcpy(dest, srcData, viewStride);
            dest += getStride();
            srcData += dataStride;
        }
    }
    else
    {
        for (y1 = 0; y1 < (int32_t)viewRect.h; y1++)
        {
            uint8_t* src = srcData;
            int32_t x1;
            for (x1 = 0; x1 < viewRect.w; x1++)
            {
                uint32_t c = *((uint32_t*)src);
                drawPixel(drawRect.x + x1, drawRect.y + y1, c, destBuffer);
                src += bytesPerPixel;
            }
            srcData += dataStride;
        }
    }

    return true;
}

static inline uint32_t clipRadius(uint32_t w, uint32_t h, uint32_t r)
{
    if (w < (r * 2))
    {
        r = (w / 2) - 1;
    }
    if (h < (r * 2))
    {
        r = (h / 2) - 1;
    }
    return r;
}

/*
 *    r
 * +----+
 * |   /
 * |  -
 * | /
 * |/
 * +
 *
 */

bool Drawable::drawCorner(int32_t x, int32_t y, Corner corner, uint32_t r, uint32_t c)
{
    int cx = r;
    int cy = 0;
    int err = 0;
    while (cx >= cy)
    {
        switch (corner)
        {
            case TOP_LEFT:
                drawPixel(x + cx, y + cy, c);
                drawPixel(x + cy, y + cx, c);
                break;

            case TOP_RIGHT:
                drawPixel(x - cx, y + cy, c);
                drawPixel(x - cy, y + cx, c);
                break;

            case BOTTOM_RIGHT:
                drawPixel(x - cx, y - cy, c);
                drawPixel(x - cy, y - cx, c);
                break;

            case BOTTOM_LEFT:
                drawPixel(x + cx, y - cy, c);
                drawPixel(x + cy, y - cx, c);
                break;
        }
 
        if (err <= 0)
        {
            cy += 1;
            err += 2*cy + 1;
        }
 
        if (err > 0)
        {
            cx -= 1;
            err -= 2*cx + 1;
        }
    }

    return true;
}

bool Drawable::drawRectRounded(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t c)
{
    r = clipRadius(w, h, r);

    int32_t xr1 = x + r;
    int32_t xr2 = (x + w) - (r + 1);
    int32_t yr1 = y + r;
    int32_t yr2 = (y + h) - (r + 1);

    drawLine(xr1, y, xr2, y, c);
    drawLine(xr1, y + h -1, xr2, y + h - 1, c);
    drawLine(x, yr1, x, yr2, c);
    drawLine(x + w - 1, yr1, x + w - 1, yr2, c);

    int cx = r;
    int cy = 0;
    int err = 0;
    while (cx >= cy)
    {
        drawPixel(xr1 - cx, yr1 - cy, c);
        drawPixel(xr1 - cy, yr1 - cx, c);

        drawPixel(xr1 - cx, yr2 + cy, c);
        drawPixel(xr1 - cy, yr2 + cx, c);

        drawPixel(xr2 + cx, yr1 - cy, c);
        drawPixel(xr2 + cy, yr1 - cx, c);

        drawPixel(xr2 + cx, yr2 + cy, c);
        drawPixel(xr2 + cy, yr2 + cx, c);
 
        if (err <= 0)
        {
            cy += 1;
            err += 2*cy + 1;
        }
 
        if (err > 0)
        {
            cx -= 1;
            err -= 2*cx + 1;
        }
    }

    return true;
}
/*
 *      
 *   +--+-----+--+
 *   | /|     |\ |
 *   |/ |     | \|
 *   +--+     +--+
 *   |  |     |  |
 *   |  |     |  |
 *   +--+     +--+
 *   |\ |     | /|
 *   | \|     |/ |
 *   +--+-----+--+
 *
 */
bool Drawable::drawRectFilledRounded(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t c)
{
    r = clipRadius(w, h, r);

    if (r <= 1)
    {
        return drawRectFilled(x, y, w, h, c);
    }

    drawRectFilled(x + r, y, w - (r * 2), h, c);
    drawRectFilled(x, y + r, r, h - (r * 2), c);
    drawRectFilled(x + (w - r), y + r, r - 1, h - (r * 2), c);

    uint32_t x2 = x + w - 1;
    uint32_t y2 = y + h - 1;

    // Corners...
    uint32_t cy;
    for (cy = 0; cy < r; cy++)
    {
        uint32_t cx;
        for (cx = 0; cx < r; cx++)
        {
            uint32_t c2 = ((r - cx) * (r - cx)) + ((r - cy) * (r - cy));
            if (c2 < (r * r))
            {
                drawPixel(x + cx, y + cy, c);
                drawPixel(x2 - cx, y + cy, c);
                drawPixel(x + cx, y2 - cy, c);
                drawPixel(x2 - cx, y2 - cy, c);
            }
        }
    }

    return true;
}

bool Drawable::drawGradRounded(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t r, uint32_t c1, uint32_t c2)
{
    r = clipRadius(w, h, r);

    if (r <= 1)
    {
        return drawGrad(x, y, w, h, c1, c2);
    }

    uint8_t* c1bytes = (uint8_t*)&c1;
    uint8_t* c2bytes = (uint8_t*)&c2;

    float d[4];
    d[2] = 0.0;
    d[2] = ((float)c2bytes[2] - (float)c1bytes[2]) / (float)h;
    d[1] = ((float)c2bytes[1] - (float)c1bytes[1]) / (float)h;
    d[0] = ((float)c2bytes[0] - (float)c1bytes[0]) / (float)h;

    float v[4];
    v[3] = 255.0;
    v[2] = (float)(c1bytes[2]);
    v[1] = (float)(c1bytes[1]);
    v[0] = (float)(c1bytes[0]);

    uint32_t y1;
    for (y1 = 0; y1 < h; y1++)
    {
        uint32_t out;
        uint8_t* outbytes = (uint8_t*)&out;
        outbytes[3] = (int)v[3];
        outbytes[2] = (int)v[2];
        outbytes[1] = (int)v[1];
        outbytes[0] = (int)v[0];
        //v[3] += d[3];
        v[2] += d[2];
        v[1] += d[1];
        v[0] += d[0];

        int xoff = 0;
        int wl = w;

        if (y1 <= r || y1 > (h - r) - 1)
        {
            int yr = y1;
            if (y1 > (h - r) - 1)
            {
                yr = (h - y1) - 1;
            }

            uint32_t i;
            for (i = 0; i <= r + 1; i++)
            {
                //printf("Drawable::drawGradRounded: i=%d, y1=%d: %d + %d <= %d\n", i, y1, ((r - i) * (r - i)), ((r - y1) * (r - y1)), (r * r));
                if ((((r - i) * (r - i)) + ((r - yr) * (r - yr))) <= (r * r))
                {
                    xoff = i;
                    break;
                }
            }
            wl -= (xoff * 2);
            //printf("Drawable::drawGradRounded: y1=%d, r=%d, xoff=%d, wl=%d\n", y1, r, xoff, wl);
            //out = 0xffff0000;
        }

        drawLine(x + xoff, y + y1, x + xoff + (wl - 1), y + y1, out);
        //Drawable::drawLine(x, y + j, x + (w - 1), y + j, out);
    }
    return true;
}

bool Drawable::drawCircle(int32_t x, int32_t y, uint32_t r, uint32_t c)
{
    int cx = r;
    int cy = 0;
    int err = 0;
    while (cx >= cy)
    {
        Drawable::drawPixel(x - cx, y - cy, c);
        Drawable::drawPixel(x - cx, y + cy, c);
        Drawable::drawPixel(x + cx, y - cy, c);
        Drawable::drawPixel(x + cx, y + cy, c);

        if (err <= 0)
        {
            cy += 1;
            err += 2*cy + 1;
        }

        if (err > 0)
        {
            cx -= 1;
            err -= 2*cx + 1;
        }
    }

    return true;
}

bool Drawable::drawCircleFilled(int32_t x, int32_t y, uint32_t r, uint32_t c)
{
    unsigned int y0;
    for (y0 = 0; y0 < r; y0++)
    {
        unsigned int x0;
        for (x0 = 0; x0 < r; x0++)
        {
            float d = sqrt((double)((x0 * x0) + (y0 * y0)));
            if (d <= (double)r)
            {
                Drawable::drawPixel(x - x0, y - y0, c);
                Drawable::drawPixel(x - x0, y + y0, c);
                Drawable::drawPixel(x + x0, y - y0, c);
                Drawable::drawPixel(x + x0, y + y0, c);
            }
        }
    }

    return true;
}

uint32_t Drawable::getPixel(int32_t x, int32_t y)
{
    uintptr_t p = (uintptr_t)getDrawingBuffer() + getOffset(x, y);
    uint32_t* dest = (uint32_t*)p;
    return *dest;
}

bool Drawable::saveJPEG(std::string path)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT , 00644);
    if (fd == -1)
    {
        return false;
    }

    bool res;
    res = saveJPEG(fd);
    close(fd);
    return res;
}

bool Drawable::saveJPEG(int fd)
{
    FILE* file = fdopen(fd, "w");

    struct jpeg_compress_struct cinfo;
    memset(&cinfo, 0, sizeof(cinfo));

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);

    bool res;
    res = saveJPEG(&cinfo);
    fclose(file);
    return res;
}

bool Drawable::saveJPEG(uint8_t** buffer, unsigned long* bufferLength)
{
    struct jpeg_compress_struct cinfo;
    memset(&cinfo, 0, sizeof(cinfo));

    jpeg_create_compress(&cinfo);

    jpeg_mem_dest(&cinfo, buffer, bufferLength);
    bool res;
    res = saveJPEG(&cinfo);
    return res;
}

bool Drawable::saveJPEG(struct jpeg_compress_struct* cinfo)
{
    struct jpeg_error_mgr jerr;
    memset(&jerr, 0, sizeof(jerr));
    cinfo->err = jpeg_std_error(&jerr);

    cinfo->image_width = getWidth();
    cinfo->image_height = getHeight() - 1;
    cinfo->input_components = 3;
    cinfo->in_color_space = JCS_RGB;

    jpeg_set_defaults(cinfo);

    jpeg_set_quality(cinfo, 95, TRUE); // Hardcode quality to 95 for now

    jpeg_start_compress(cinfo, TRUE);

    int stride = getWidth() * 3;
    JSAMPROW row_pointer[1];
    row_pointer[0] = (JSAMPROW)malloc(stride);
    memset(row_pointer[0], 0, stride);

    uint32_t* data = (uint32_t*)getDrawingBuffer();
    while (cinfo->next_scanline < cinfo->image_height)
    {
        unsigned int x;
        for (x = 0; x < cinfo->image_width; x++)
        {
            uint32_t c;
            if (m_bytesPerPixel == 4)
            {
                c = *(data++);
            }
            else
            {
                c = getPixel(x, cinfo->next_scanline);
            }
            row_pointer[0][(x * 3) + 2] = c >> 0;
            row_pointer[0][(x * 3) + 1] = c >> 8;
            row_pointer[0][(x * 3) + 0] = c >> 16;
        }
        jpeg_write_scanlines(cinfo, row_pointer, 1);
    }

    free(row_pointer[0]);

    jpeg_finish_compress(cinfo);
    return true;
}

bool Drawable::savePNG(std::string path)
{
    return false;
}

bool Drawable::savePNG(int fd)
{
    return false;
}

