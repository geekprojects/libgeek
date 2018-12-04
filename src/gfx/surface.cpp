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
#include <string.h>
#include <jpeglib.h>
#include <png.h>

#include <geek/gfx-surface.h>
#include <geek/core-compiler.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

Surface::Surface() :
    Drawable(0, 0),
    Logger("Surface")
{
    m_highDPI = false;
}

Surface::Surface(uint32_t width, uint32_t height, uint8_t bpp) :
    Drawable(width, height),
    Logger("Surface")
{
    if (bpp != 4)
    {
        fprintf(stderr, "Surface::Surface: Surface only supports 4 bytes per pixel");
        exit(1);
    }

    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];
    m_highDPI = false;
}

Surface::Surface(uint32_t width, uint32_t height, uint8_t bpp, uint8_t* data) :
    Drawable(width, height),
     Logger("Surface")
{
    if (bpp != 4)
    {
        fprintf(stderr, "Surface::Surface: Surface only supports 4 bytes per pixel");
        exit(1);
    }

    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];
    m_highDPI = false;

    memcpy(m_drawingBuffer, data, m_drawingBufferLength);
}

Surface::Surface(Surface* src) :
    Drawable(src->m_width, src->m_height),
    Logger("Surface")
{
    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];
    m_highDPI = false;

    memcpy(m_drawingBuffer, src->m_drawingBuffer, m_drawingBufferLength);
}

Surface::~Surface()
{
    if (m_drawingBuffer != NULL)
    {
        delete[] m_drawingBuffer;
    }
}

bool Surface::resize(uint32_t width, uint32_t height)
{
    uint32_t origWidth = m_width;
    uint32_t origHeight = m_height;
    uint8_t* resizedData;

    if (m_drawingBuffer == NULL)
    {
        return false;
    }

    m_width = width;
    m_height = height;

    int len = m_width * m_height * m_bytesPerPixel;
    resizedData = new uint8_t[m_width * m_height * m_bytesPerPixel];
    memset(resizedData, 0, len);

    uint32_t minWidth = MIN(origWidth, width);
    uint32_t minHeight = MIN(origHeight, height);

    blit(
        resizedData, 0, 0,
        m_drawingBuffer, origWidth, origHeight, m_bytesPerPixel,
        0, 0, minWidth, minHeight);

    delete m_drawingBuffer;
    m_drawingBuffer = resizedData;

    return true;
}

void Surface::rotate(int angle)
{
    if (angle == 0)
    {
        return;
    }
    else if (angle != 90 && angle != 270)
    {
        log(ERROR, "rotate: Angle %d not supported", angle);
        return;
    }

    Surface* rotated = new Surface(getHeight(), getWidth(), getBytesPerPixel());

    int w = m_width;
    int h = m_height;

    // Hardly the fastest rotation ever, but It Works(tm)
    uint32_t* data = (uint32_t*)getData();
    unsigned int y;
    for (y = 0; y < getHeight(); y++)
    {
        unsigned int x;
        for (x = 0; x < getWidth(); x++)
        {
            if (angle == 90)
            {
                rotated->drawPixel(y, x, *data);
            }
            else
            {
                rotated->drawPixel(y, (w - x) - 1, *data);
            }
            data++;
        }
    }
    swapData(rotated);
    m_width = h;
    m_height = w;
    delete rotated;
}

void Surface::setAlpha(float alpha)
{
    uint32_t aint = (int)(255.0 * alpha) << 24;
    uint32_t* data = (uint32_t*)getData();
    unsigned int y;
    for (y = 0; y < getHeight(); y++)
    {
        unsigned int x;
        for (x = 0; x < getWidth(); x++)
        {
            uint32_t p = (*data) & 0x00ffffff;
            *data = p | aint;
            data++;
        }
    }
}

SurfaceViewPort::~SurfaceViewPort()
{
}

bool SurfaceViewPort::resize(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;

    return true;
}

bool Surface::swapData(Surface* other)
{
    if (other->m_drawingBufferLength != m_drawingBufferLength)
    {
        return false;
    }

    uint8_t* tmp = other->m_drawingBuffer;
    other->m_drawingBuffer = m_drawingBuffer;
    m_drawingBuffer = tmp;
    return true;
}

Surface* Surface::loadJPEG(string path)
{
    Surface* surface = NULL;
    FILE* fd;

    fd = fopen(path.c_str(), "r");
    if (fd == NULL)
    {
        printf("Surface::loadJPEG: failed to open file: %s\n", path.c_str());
        return NULL;
    }

    struct jpeg_decompress_struct cinfo;
    memset(&cinfo, 0, sizeof(cinfo));
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fd);
    surface = Surface::loadJPEGInternal(&cinfo);
    fclose(fd);
    return surface;
}

Surface* Surface::loadJPEG(uint8_t* data, uint32_t length)
{
    struct jpeg_decompress_struct cinfo;
    memset(&cinfo, 0, sizeof(cinfo));
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, length);

    Surface* surface = NULL;
    surface = Surface::loadJPEGInternal(&cinfo);

    return surface;
}

Surface* Surface::loadJPEGInternal(struct jpeg_decompress_struct* cinfo)
{
    Surface* surface = NULL;
    struct jpeg_error_mgr jerr;
    cinfo->err = jpeg_std_error(&jerr);
    jpeg_read_header(cinfo, (boolean)1);

    jpeg_start_decompress(cinfo);

#if 1
    printf(
        "Surface::loadJPEG: size=%d, %d, components=%d\n",
        cinfo->output_width,
        cinfo->output_height,
        cinfo->output_components);
#endif

    int row_stride = cinfo->output_width * cinfo->output_components;

    surface = new Surface(
        cinfo->output_width,
        cinfo->output_height,
        4);//cinfo.output_components);
    surface->clear(0);

    JSAMPARRAY buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);

    uint32_t* data = (uint32_t*)surface->getData();
    while (cinfo->output_scanline < cinfo->output_height)
    {
        jpeg_read_scanlines(cinfo, buffer, 1);
        uint8_t* ptr = buffer[0];

        uint32_t x;
        for (x = 0; x < cinfo->output_width; x++)
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
if (cinfo->output_components == 1)
{
r = *ptr++;
g = r;
b = r;
}
else
{
            r = *ptr++;
            g = *ptr++;
            b = *ptr++;
}
            *(data++) = 0xff000000 | r << 0 | g << 8 | b << 16;
        }
    }

    jpeg_finish_decompress(cinfo);
    jpeg_destroy_decompress(cinfo);

    return surface;
}

Surface* Surface::loadPNG(string path)
{
    Surface* surface = NULL;

    FILE* fd;
    fd = fopen(path.c_str(), "r");
    if (fd == NULL)
    {
        return NULL;
    }

    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
    {
        fclose(fd);
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fd);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fd);
        /* If we get here, we had a problem reading the file */
        return NULL;
    }

    png_init_io(png_ptr, fd);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);
    int channels = png_get_channels(png_ptr, info_ptr);
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

    surface = new Surface(width, height, 4);

    int y;
    for (y = 0; y < height; y++)
    {
        uint8_t* src = (uint8_t*)row_pointers[y];
        uint8_t* dst = surface->m_drawingBuffer + surface->getOffset(0, y);

        int x;
        for (x = 0; x < width; x++)
        {
#if 0
            uint32_t c = 0;
            c |= *(src++) << 0;
            c |= *(src++) << 8;
            c |= *(src++) << 16;
            if (channels == 4)
            {
                //uint8_t* dst = surface->m_drawingBuffer + surface->getOffset(0, y);
                //memcpy(dst, src, surface->getStride());
                c |= (0 - *(src++)) << 24;
            }
            else if (channels == 3)
            {
                c |= 255 << 24;
            }
            surface->drawPixel(x, y, c);
#else
            uint8_t b = *(src++);
            uint8_t g = *(src++);
            uint8_t r = *(src++);
            *(dst++) = r;
            *(dst++) = g;
            *(dst++) = b;
            if (channels == 4)
            {
                *(dst++) = *(src++);
            }
else
{
dst++;
}
#endif
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fd);
    return surface;
}

Surface* Surface::scaleToFit(int width, int height, bool fp)
{
    float zx = (float)width / (float)m_width;
    float zy = (float)height / (float)m_height;
    return scale(MIN(zx, zy), fp);
}

Surface* Surface::scale(float factor, bool fp)
{
    int width = m_width * factor;
    int height = m_height * factor;

    Surface* scaled = new Surface(width, height, 4);

    float stepX = (float)m_width / (float)width;
    float stepY = (float)m_height / (float)height;
    int stepXi = (int)round(stepX);
    int stepYi = (int)round(stepY);
    int blockCount = (stepXi * stepYi);
    int blockDelta = (m_width - stepXi) * 4;

    uint32_t* data = (uint32_t*)scaled->getData();
    uint8_t* srcdata = (uint8_t*)getData();

    int y;
    for (y = 0; y < height; y++)
    {
        int blockY = floor((float)y * stepY);
        int x;
        for (x = 0; x < width; x++)
        {

            int blockX = floor((float)x * stepX);
            int bx;
            int by;
            Geek::v4si totals = {0, 0, 0, 0};

            uint8_t* imgrow = srcdata;
            imgrow += ((m_width * blockY) + (blockX)) * 4;

            for (by = 0; by < stepYi; by++)
            {
                for (bx = 0; bx < stepXi; bx++)
                {
                    Geek::v4si pv = {
                        imgrow[0],
                        imgrow[1],
                        imgrow[2],
                        imgrow[3]};
#ifdef GEEK_SUPPORTS_VECTOR_TYPE
                    totals += pv;
#else
                    totals[0] += pv[0];
                    totals[1] += pv[1];
                    totals[2] += pv[2];
                    totals[3] += pv[3];
#endif
                    imgrow += 4;
                }
                imgrow += blockDelta;
            }
#ifdef GEEK_SUPPORTS_VECTOR_TYPE
            Geek::v4si avg = totals;
#else
            Geek::v4si avg;
            avg[0] = totals[0];
            avg[1] = totals[1];
            avg[2] = totals[2];
            avg[3] = totals[3];
#endif

#ifdef GEEK_SUPPORTS_VECTOR_MATHS
            avg /= blockCount;
#else
            avg[0] /= blockCount;
            avg[1] /= blockCount;
            avg[2] /= blockCount;
            avg[3] /= blockCount;
#endif
            if (fp)
            {
#ifdef GEEK_SUPPORTS_VECTOR_MATHS
                avg &= 0xc0;
#else
                avg[0] &= 0xc0;
                avg[1] &= 0xc0;
                avg[2] &= 0xc0;
                avg[3] &= 0xc0;
#endif
            }

            *(data++) = 0xff000000 | (avg[0] << 16) | (avg[1] << 8) | (avg[2] << 0);
        }
    }

    return scaled;
}

Surface* Surface::loadTGA(string path)
{
    FILE* fd = fopen(path.c_str(), "r");
    if (fd == NULL)
    {
        printf("Surface::loadTGA: ERROR: Unable to open file: %s\n", path.c_str());
        return NULL;
    }

    uint8_t idLen = fgetc(fd);
    uint8_t colourMapType = fgetc(fd);
    uint8_t imageType = fgetc(fd);

#if 0
    printf("Surface::loadTGA: ldLen=%d, colourMapType=%d, imageType=%d\n", idLen, colourMapType, imageType);
#endif

    if (colourMapType != 0)
    {
        printf("Surface::loadTGA: ERROR: Unsupported colour map type: %d\n", colourMapType);
        fclose(fd);
        return NULL;
    }

    if (imageType != 2 && imageType != 10)
    {
        printf("Surface::loadTGA: ERROR: Unsupported image type: %d\n", imageType);
        fclose(fd);
        return NULL;
    }

    fseek(fd, 5 + 4, SEEK_CUR);

    // TODO: Little Endian Only!
    uint16_t width = 0;
    uint16_t height = 0;
    size_t res;

    res = fread(&width, 2, 1, fd);
    if (res <= 0)
    {
        fclose(fd);
        return NULL;
    }

    res = fread(&height, 2, 1, fd);
    if (res <= 0)
    {
        fclose(fd);
        return NULL;
    }

    uint8_t bpp = fgetc(fd);
    /*uint8_t imageDesc =*/ fgetc(fd);

#if 0
    printf("Surface::loadTGA: width=%d, height=%d, bpp=%d, imageDesc=0x%x\n", width, height, bpp, imageDesc);
#endif

    if (bpp != 24)
    {
        printf("Surface::loadTGA: ERROR: Unsupported BPP: %d\n", bpp);
        fclose(fd);
        return NULL;
    }

    fseek(fd, idLen, SEEK_CUR);

    Surface* surface = new Surface(width, height, 4);

    unsigned int x = 0;
    unsigned int y = 0;
    if (imageType == 2)
    {
        // Uncompressed. Easy!
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                uint8_t b = fgetc(fd);
                uint8_t g = fgetc(fd);
                uint8_t r = fgetc(fd);
                surface->drawPixel(x, height - (y + 1), 0xff000000 | (b << 16) | (g << 8) | r);
            }
        }
    }
    else
    {
        while (y < height && !feof(fd))
        {
            uint8_t c = fgetc(fd);
            int length = (c & 0x7f) + 1;

            uint32_t p = 0;
            if (c & 0x80)
            {
                // Run Length
                uint8_t b = fgetc(fd);
                uint8_t g = fgetc(fd);
                uint8_t r = fgetc(fd);
                p = 0xff000000 | (b << 16) | (g << 8) | r;
            }

            int i;
            for (i = 0; i < length && (y < height) ; i++)
            {
                if (!(c & 0x80))
                {
                    // Raw pixel
                    uint8_t b = fgetc(fd);
                    uint8_t g = fgetc(fd);
                    uint8_t r = fgetc(fd);
                    p = 0xff000000 | (b << 16) | (g << 8) | r;
                }
                surface->drawPixel(x, height - (y + 1), p);

                x++;
                if (x >= width)
                {
                    x = 0;
                    y++;
                }
            }
        }
    }

    fclose(fd);
    return surface;
}

Surface* Surface::loadImage(std::string path)
{
    unsigned int pos = path.rfind('.');
    string ext = path.substr(pos);
    if (!strncasecmp(ext.c_str(), ".jpg", 4) || !strncasecmp(ext.c_str(), ".jpeg", 5))
    {
        return loadJPEG(path);
    }
    else if (!strncasecmp(ext.c_str(), ".png", 4))
    {
        return loadPNG(path);
    }
    else if (!strncasecmp(ext.c_str(), ".tga", 4))
    {
        return loadTGA(path);
    }
    printf("Surface::loadImage: Unknown image type: %s\n", ext.c_str());
    return NULL;
}

