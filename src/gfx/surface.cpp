/*
 *  libgeek - The GeekProjects utility suite
 *  Copyright (C) 2014, 2015, 2016, 2019 GeekProjects.com
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


#include <cstdio>
#include <cstring>
#include <jpeglib.h>
#include <png.h>

#include <geek/gfx-surface.h>
#include <geek/core-compiler.h>
#include <geek/core-data.h>

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
    m_drawingBuffer = data;
    m_highDPI = false;
    m_copy = true;
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
    if (!m_copy && m_drawingBuffer != nullptr)
    {
        delete[] m_drawingBuffer;
    }
}

bool Surface::resize(uint32_t width, uint32_t height)
{
    uint32_t origWidth = m_width;
    uint32_t origHeight = m_height;
    uint8_t* resizedData;

    if (m_drawingBuffer == nullptr)
    {
        return false;
    }

    m_width = width;
    m_height = height;

    unsigned int len = m_width * m_height * m_bytesPerPixel;
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

    auto rotated = new Surface(getHeight(), getWidth(), getBytesPerPixel());

    int w = (int)getWidth();
    int h = (int)getHeight();

    // Hardly the fastest rotation ever, but It Works(tm)
    auto data = (uint32_t*)getData();
    int y;
    for (y = 0; y < h; y++)
    {
        int x;
        for (x = 0; x < w; x++)
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
    uint32_t aint = (uint32_t)(255.0 * alpha) << 24u;
    auto* data = (uint32_t*)getData();
    unsigned int y;
    for (y = 0; y < getHeight(); y++)
    {
        unsigned int x;
        for (x = 0; x < getWidth(); x++)
        {
            uint32_t p = (*data) & 0x00ffffffu;
            *data = p | aint;
            data++;
        }
    }
}

SurfaceViewPort::~SurfaceViewPort() = default;

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
    Surface* surface;
    FILE* fd;

    fd = fopen(path.c_str(), "r");
    if (fd == nullptr)
    {
        printf("Surface::loadJPEG: failed to open file: %s\n", path.c_str());
        return nullptr;
    }

    jpeg_decompress_struct cinfo = {};
    memset(&cinfo, 0, sizeof(cinfo));
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, fd);
    surface = Surface::loadJPEGInternal(&cinfo);
    fclose(fd);
    return surface;
}

Surface* Surface::loadJPEG(uint8_t* data, uint32_t length)
{
    jpeg_decompress_struct cinfo = {};
    memset(&cinfo, 0, sizeof(cinfo));
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, data, length);

    return Surface::loadJPEGInternal(&cinfo);
}

Surface* Surface::loadJPEGInternal(struct jpeg_decompress_struct* cinfo)
{
    Surface* surface;
    jpeg_error_mgr jerr = {};
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

    unsigned int row_stride = cinfo->output_width * cinfo->output_components;

    surface = new Surface(
        cinfo->output_width,
        cinfo->output_height,
        4);//cinfo.output_components);
    surface->clear(0);

    JSAMPARRAY buffer = (*cinfo->mem->alloc_sarray)
        ((j_common_ptr)cinfo, JPOOL_IMAGE, row_stride, 1);

    auto data = (uint32_t*)surface->getData();
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

static Surface* loadPNGInternal(png_structp png_ptr, png_infop info_ptr)
{
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png_ptr);
        printf("loadPNGInternal: Calling png_set_tRNS_to_alpha\n");
    }

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, nullptr);

    unsigned int width = png_get_image_width(png_ptr, info_ptr);
    unsigned int height = png_get_image_height(png_ptr, info_ptr);
    int channels = png_get_channels(png_ptr, info_ptr);
    png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

    printf("loadPNGInternal: channels=%d", channels);

    auto surface = new Surface(width, height, 4);

    int y;
    for (y = 0; y < height; y++)
    {
        auto src = (uint8_t*)row_pointers[y];
        uint8_t* dst = surface->getDrawingBuffer() + surface->getOffset(0, y);

        int x;
        for (x = 0; x < width; x++)
        {
            uint8_t r = *(src++);
            uint8_t g = *(src++);
            uint8_t b = *(src++);
            *(dst++) = r;
            *(dst++) = g;
            *(dst++) = b;
            if (channels == 4)
            {
                *(dst++) = *(src++);
            }
            else
            {
                *(dst++) = 0xff;
            }
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return surface;
}

Surface* Surface::loadPNG(string path)
{
    FILE* fd;
    fd = fopen(path.c_str(), "r");
    if (fd == nullptr)
    {
        return nullptr;
    }

    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr)
    {
        fclose(fd);
        return nullptr;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        fclose(fd);
        return nullptr;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fd);
        /* If we get here, we had a problem reading the file */
        return nullptr;
    }

    png_init_io(png_ptr, fd);

    Surface* surface = loadPNGInternal(png_ptr, info_ptr);

    fclose(fd);
    return surface;
}

struct pngbuffer
{
    char* ptr;
    uint32_t remaining;
};

void readPNGFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
    png_voidp io_ptr = png_get_io_ptr(png_ptr);
    pngbuffer* buffer = reinterpret_cast<pngbuffer*>(io_ptr);

    unsigned int copyLen = byteCountToRead;
    if (buffer->remaining < copyLen)
    {
        copyLen = buffer->remaining;
    }

    memcpy(outBytes, buffer->ptr, copyLen);
    buffer->ptr += copyLen;
    buffer->remaining -= copyLen;
}

Surface* Surface::loadPNG(uint8_t* data, uint32_t length)
{
    png_structp png_ptr;
    png_infop info_ptr;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr)
    {
        return nullptr;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr)
    {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return nullptr;
    }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        /* Free all of the memory associated with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        /* If we get here, we had a problem reading the file */
        return nullptr;
    }

    pngbuffer* buffer = new pngbuffer();
    buffer->ptr = (char*)data;
    buffer->remaining = length;

    png_set_read_fn(png_ptr, buffer, readPNGFromMemory);

    Surface* surface = loadPNGInternal(png_ptr, info_ptr);

    delete buffer;

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
    unsigned int width = (int)((float)m_width * factor);
    unsigned int height = (int)((float)m_height * factor);

    Surface* scaled = new Surface(width, height, 4);

    float stepX = (float)m_width / (float)width;
    float stepY = (float)m_height / (float)height;
    unsigned int stepXi = (int)round(stepX);
    unsigned int stepYi = (int)round(stepY);
    int blockCount = (int)(stepXi * stepYi);
    unsigned int blockDelta = (m_width - stepXi) * 4;

    auto data = (uint32_t*)scaled->getData();
    auto srcdata = (uint8_t*)getData();

    unsigned int y;
    for (y = 0; y < height; y++)
    {
        int blockY = floor((float)y * stepY);
        unsigned int x;
        for (x = 0; x < width; x++)
        {
            int blockX = floor((float)x * stepX);
            unsigned int bx;
            unsigned int by;
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

            *(data++) = (avg[3] << 24 | avg[2] << 16) | (avg[1] << 8) | (avg[0] << 0);
        }
    }

    return scaled;
}

Surface* Surface::loadTGA(string path)
{
    FILE* fd = fopen(path.c_str(), "r");
    if (fd == nullptr)
    {
        printf("Surface::loadTGA: ERROR: Unable to open file: %s\n", path.c_str());
        return nullptr;
    }

    char buf[3];
    size_t res;
    res = fread(buf, 3, 1, fd);
    if (res < 1)
    {
        printf("Surface::loadTGA: ERROR: Unable to open file header: %s\n", path.c_str());
        fclose(fd);
        return nullptr;
    }

    uint8_t idLen = buf[0];
    uint8_t colourMapType = buf[1];
    uint8_t imageType = buf[2];

#if 0
    printf("Surface::loadTGA: ldLen=%d, colourMapType=%d, imageType=%d\n", idLen, colourMapType, imageType);
#endif

    if (colourMapType != 0)
    {
        printf("Surface::loadTGA: ERROR: Unsupported colour map type: %d\n", colourMapType);
        fclose(fd);
        return nullptr;
    }

    if (imageType != 2 && imageType != 10)
    {
        printf("Surface::loadTGA: ERROR: Unsupported image type: %d\n", imageType);
        fclose(fd);
        return nullptr;
    }

    fseek(fd, 5 + 4, SEEK_CUR);

    // TODO: Little Endian Only!
    uint16_t width = 0;
    uint16_t height = 0;

    res = fread(&width, 2, 1, fd);
    if (res < 1)
    {
        fclose(fd);
        return nullptr;
    }

    res = fread(&height, 2, 1, fd);
    if (res < 1)
    {
        fclose(fd);
        return nullptr;
    }

    res = fread(buf, 2, 1, fd);
    if (res < 1)
    {
        printf("Surface::loadTGA: ERROR: Error reading from file\n");
        fclose(fd);
        return nullptr;
    }
    uint8_t bpp = buf[0];

#if 0
    printf("Surface::loadTGA: width=%d, height=%d, bpp=%d, imageDesc=0x%x\n", width, height, bpp, imageDesc);
#endif

    if (bpp != 24)
    {
        printf("Surface::loadTGA: ERROR: Unsupported BPP: %d\n", bpp);
        fclose(fd);
        return nullptr;
    }

    res = fseek(fd, idLen, SEEK_CUR);
    if (res != 0)
    {
        printf("Surface::loadTGA: ERROR: Failed to seek to data\n");
        fclose(fd);
        return nullptr;
    }

    Surface* surface = new Surface(width, height, 4);

    int x = 0;
    int y = 0;
    if (imageType == 2)
    {
        // Uncompressed. Easy!
        for (y = 0; y < height; y++)
        {
            for (x = 0; x < width; x++)
            {
                res = fread(buf, 3, 1, fd);
                if (res < 1)
                {
                    printf("Surface::loadTGA: ERROR: Failed to read data\n");
                    delete surface;
                    fclose(fd);
                    return nullptr;
                }

                uint8_t b = buf[0];
                uint8_t g = buf[1];
                uint8_t r = buf[2];
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
            if (feof(fd))
            {
                printf("Surface::loadTGA: ERROR: File truncated?\n");
                delete surface;
                fclose(fd);
                return nullptr;
            }

            uint32_t p = 0;
            if (c & 0x80)
            {
                res = fread(buf, 3, 1, fd);
                if (res < 1)
                {
                    printf("Surface::loadTGA: ERROR: Failed to read data\n");
                    delete surface;
                    fclose(fd);
                    return nullptr;
                }

                uint8_t b = buf[0];
                uint8_t g = buf[1];
                uint8_t r = buf[2];
 
                p = 0xff000000 | (b << 16) | (g << 8) | r;
            }

            int i;
            for (i = 0; i < length && (y < height) ; i++)
            {
                if (!(c & 0x80))
                {
                    res = fread(buf, 3, 1, fd);
                    if (res < 1)
                    {
                        printf("Surface::loadTGA: ERROR: Failed to read data\n");
                        delete surface;
                        fclose(fd);
                        return nullptr;
                    }

                    uint8_t b = buf[0];
                    uint8_t g = buf[1];
                    uint8_t r = buf[2];
 
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
    return nullptr;
}

Surface* Surface::updateSurface(Surface* surface, int width, int height, double scale)
{
    if (surface == nullptr || surface->getWidth() != width * scale || surface->getHeight() != height * scale)
    {
        delete surface;

        if (scale > 1)
        {
            surface = new HighDPISurface(width, height, 4);
        }
        else
        {
            surface = new Surface(width, height, 4);
        }
        return surface;
    }
    return surface;
}

