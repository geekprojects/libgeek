
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

Drawable::Drawable(uint32_t width, uint32_t height, uint8_t bpp)
{
    m_width = width;
    m_height = height;
    m_bytesPerPixel = bpp;
    m_dpiX = 96;
    m_dpiY = 96;

    m_drawingBuffer = NULL;
    m_drawingBufferLength = 0;

    switch (m_bytesPerPixel)
    {
        case 3:
            m_getPixelFunc = (getPixelFunc_t)&Drawable::getPixel3;
            m_drawPixelFunc = (drawPixelFunc_t)&Drawable::drawPixel3;
            break;

        case 4:
            m_getPixelFunc = (getPixelFunc_t)&Drawable::getPixel4;
            m_drawPixelFunc = (drawPixelFunc_t)&Drawable::drawPixel4;
            break;

        default:
            m_getPixelFunc = NULL;
            m_drawPixelFunc = NULL;
            break;
    }
}

Drawable::~Drawable()
{
}

bool Drawable::drawPixel(int32_t x, int32_t y, uint32_t c)
{
    return drawPixel(x, y, c, getDrawingBuffer());
}

bool Drawable::drawPixel(int32_t x, int32_t y, uint32_t c, uint8_t* dest)
{
#ifdef DRAW_PIXEL_CHECKS
    if (x < 0 || y < 0 || x > (int32_t)getWidth() || y > (int32_t)getHeight())
    {
        return false;
    }

    if (dest == NULL)
    {
        printf("Drawable::drawPixel: dest is NULL!?\n");
        return false;
    }
#endif
    return (this->*Drawable::m_drawPixelFunc)(x, y, c, dest);
}

bool Drawable::drawPixel3(int32_t x, int32_t y, uint32_t c, uint8_t* dest)
{
    uint8_t alpha = c >> 24;
    if (alpha == 0)
    {
        return true;
    }

    uintptr_t p = (uintptr_t)dest + getOffset(x, y);

    uint32_t* p32 = (uint32_t*)p;
    *p32 &= 0xff000000;
    *p32 |= c & 0x00ffffff;

    return true;
}

static inline void draw32a(uint8_t* src, uint8_t* dest, float alpha)
{
    dest[0] += (uint8_t)(((src[0] - dest[0]) * alpha));
    dest[1] += (uint8_t)(((src[1] - dest[1]) * alpha));
    dest[2] += (uint8_t)(((src[2] - dest[2]) * alpha));
    dest[3] = 255;
}

bool Drawable::drawPixel4(int32_t x, int32_t y, uint32_t c, uint8_t* dest)
{

    uint8_t alpha = c >> 24;

#if 1
    if (alpha == 0)
    {
        return true;
    }
#endif

    uintptr_t p = (uintptr_t)dest + getOffset(x, y);

#if 1
    if (alpha == 255)
    {
        uint32_t* dest = (uint32_t*)p;
        *dest = c;
    }
    else
#endif
    {
        draw32a((uint8_t*)&c, (uint8_t*)dest, (float)alpha / 255.0f);
    }

    return true;
}


bool Drawable::drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c)
{
    uint32_t dx = abs(x1 - x2);
    uint32_t dy = abs(y1 - y2);

    c |= 0xff000000;

    if (dx !=0 && dy != 0)
    {
        printf(
            "Drawable::drawLine: We only draw straight lines for now!\n");
    }

    uint8_t* drawingBuffer = getDrawingBuffer();

    uint32_t i;
    if (dx > dy)
    {
        uint32_t x = MIN(x1, x2);
        for (i = 0; i < dx; i++)
        {
            Drawable::drawPixel(x + i, y1, c, drawingBuffer);
        }
    }
    else
    {
        uint32_t y = MIN(y1, y2);
        for (i = 0; i < dy; i++)
        {
            Drawable::drawPixel(x1, y + i, c, drawingBuffer);
        }
    }
    return true;
}


bool Drawable::drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
    uint32_t x1;
    uint32_t y1;

    if (x < 0)
    {
        x = 0;
    }
    else if (x > (int32_t)getWidth())
    {
#ifdef DEBUG_DRAWING
        printf(
            "Drawable::drawRectFilled: %d,%d - %d,%d overlaps %d,%d - %d,%d\n",
            x, y, x + w, y + h,
            0, 0, getWidth() - 1, getHeight() - 1);
#endif
        return false;
    }
    else if ((x + w) > (int32_t)getWidth())
    {
#ifdef DEBUG_DRAWING
        printf(
            "Drawable::drawRectFilled: %d,%d - %d,%d overlaps %d,%d - %d,%d\n",
            x, y, x + w, y + h,
            0, 0, getWidth() - 1, getHeight() - 1);
#endif
        return false;
    }

    if (y < 0)
    {
        y = 0;
    }
    else if (y > (int32_t)getHeight())
    {
#ifdef DEBUG_DRAWING
        printf(
            "Drawable::drawRectFilled: %d,%d - %d,%d overlaps %d,%d - %d,%d\n",
            x, y, x + w, y + h,
            0, 0, getWidth() -1, getHeight() - 1);
#endif
        return false;
    }
    else if ((y + h) > (int32_t)getHeight())
    {
#ifdef DEBUG_DRAWING
        printf(
            "Drawable::drawRectFilled: %d,%d - %d,%d overlaps %d,%d - %d,%d\n",
            x, y, x + w, y + h,
            0, 0, getWidth() - 1, getHeight() - 1);
#endif
        return false;
    }

    c |= 0xff000000;

    uint8_t* drawingBuffer = getDrawingBuffer();

    int bpp = getBytesPerPixel();
    if (bpp == 4 || bpp == 32)
    {
        uint8_t* dest = drawingBuffer + getOffset(x, y);
        uint32_t stride = getStride() - (w * bpp);
        for (y1 = 0; y1 < h; y1++)
        {
            for (x1 = 0; x1 < w; x1++)
            {
                if (bpp == 4)
                {
                    *((uint32_t*)dest) = c;
                }
                else
                {
                    *((uint32_t*)dest) &= 0xff000000;
                    *((uint32_t*)dest) |= c & 0x00ffffff;
                }
                dest += bpp;
            }
            dest += stride;
        }
    }
    else
    {
        for (y1 = 0; y1 < h; y1++)
        {
            for (x1 = 0; x1 < w; x1++)
            {
                Drawable::drawPixel(x1 + x, y1 + y, c, drawingBuffer);
            }
        }
    }

    //m_dirty.addDirtyRegion(x, y, w, h);

    return true;
}

bool Drawable::drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c)
{
#ifdef DEBUG_DRAWING
    printf("Drawable::drawRect: x=%d, y=%d, w=%d, h=%d, width=%d, height=%d\n", x, y, w, h, getWidth(), getHeight());
#endif
    c |= 0xff000000;
    //m_dirty.addDirtyRegion(x, y, w, h);
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
    d[2] = 0;
    d[2] = ((float)c2bytes[2] - (float)c1bytes[2]) / h;
    d[1] = ((float)c2bytes[1] - (float)c1bytes[1]) / h;
    d[0] = ((float)c2bytes[0] - (float)c1bytes[0]) / h;

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
        v[3] += d[3];
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

bool Drawable::blit(
    uint8_t* destBuffer,
    int32_t x,
    int32_t y,
    uint8_t* data,
    uint32_t w,
    uint32_t h,
    uint32_t bytesPerPixel,
    int32_t viewX,
    int32_t viewY,
    uint32_t viewWidth,
    uint32_t viewHeight,
    bool forceAlpha)
{
    if (data == NULL)
    {
        return false;
    }

    if (x < 0)
    {
        viewX -= x;
        viewWidth += x; // This will subtract the overhang from the view width!
        x = 0;
    }

    if (x + viewWidth >= getWidth())
    {
        viewWidth = getWidth() - x;
    }

    // Nothing to draw, or overflow
    if (viewWidth == 0 || viewWidth > getWidth())
    {
        return true;
    }

    if (y < 0)
    {
        viewY -= y;
        viewHeight += y; // This will subtract the overhang from the view height!
        y = 0;
    }

    if (y + viewHeight >= getHeight())
    {
        viewHeight = getHeight() - y;
    }

    // Nothinb to draw or overflow
    if (viewHeight == 0 || viewHeight > getHeight())
    {
        return true;
    }

    if (destBuffer == NULL)
    {
        destBuffer = getDrawingBuffer();
    }

    int32_t y1;
    uint32_t viewStride = viewWidth * bytesPerPixel;
    uint32_t dataStride = w * bytesPerPixel;

    uint8_t* dest = destBuffer + getOffset(x, y);

    data += (viewX + (viewY * w)) * bytesPerPixel;

#if 0
    printf("Drawable::blit: src: %p: w=%d, h=%d\n", data, w, h);
    printf("Drawable::blit: dest: %p: x=%d, y=%d. dest w=%d, h=%d\n", dest, x, y, getWidth(), getHeight());
    printf("Drawable::blit: viewX=%d, viewY=%d, viewWidth=%d, viewHeight=%d\n", viewX, viewY, viewWidth, viewHeight);
#endif

    if (!forceAlpha && getBytesPerPixel() == bytesPerPixel)
    {
        for (y1 = 0; y1 < (int32_t)viewHeight; y1++)
        {
            memcpy(dest, data, viewStride);
            dest += getStride();
            data += dataStride;
        }
    }
    else
    {
        for (y1 = 0; y1 < (int32_t)viewHeight; y1++)
        {
            uint8_t* src = data;
            uint32_t x1;
            for (x1 = 0; x1 < viewWidth; x1++)
            {
                switch (bytesPerPixel)
                {
                    case 3:
                    {
                        uint32_t c = *((uint32_t*)src) & 0x00ffffff;
                        drawPixel(x + x1, y + y1, c, destBuffer);
                    } break;

                    case 4:
                    {
                        uint32_t c = *((uint32_t*)src);
                        drawPixel(x + x1, y + y1, c, destBuffer);
                    } break;
                }
                src += bytesPerPixel;
            }
            data += dataStride;
        }
    }

    if (destBuffer == getDrawingBuffer())
    {
        //m_dirty.addDirtyRegion(x, y, viewWidth, viewHeight);
    }

    return true;
}

uint32_t Drawable::getPixel(int32_t x, int32_t y)
{
    return (this->*Drawable::m_getPixelFunc)(x, y);
}


uint32_t Drawable::getPixel3(int32_t x, int32_t y)
{
    uintptr_t p = (uintptr_t)getDrawingBuffer() + getOffset(x, y);
    uint32_t* dest = (uint32_t*)p;
    return (*dest) & 0x00ffffff;
}

uint32_t Drawable::getPixel4(int32_t x, int32_t y)
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
        int x;
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
            row_pointer[0][(x * 3) + 2] = c >> 16;
            row_pointer[0][(x * 3) + 1] = c >> 8;
            row_pointer[0][(x * 3) + 0] = c >> 0;
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

