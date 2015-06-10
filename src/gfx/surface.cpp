
#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>

#include <geek/gfx-surface.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

typedef int v4si __attribute__ ((vector_size (32)));

Surface::Surface() :
    Drawable(0, 0, 0),
    Logger("Surface")
{
}

Surface::Surface(uint32_t width, uint32_t height, uint8_t bpp) :
    Drawable(width, height, bpp),
    Logger("Surface")
{
    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];
}

Surface::Surface(uint32_t width, uint32_t height, uint8_t bpp, uint8_t* data) :
    Drawable(width, height, bpp),
     Logger("Surface")
{
    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];

    memcpy(m_drawingBuffer, data, m_drawingBufferLength);
}

Surface::Surface(Surface* src) :
    Drawable(src->m_width, src->m_height, src->m_bytesPerPixel),
    Logger("Surface")
{
    m_drawingBufferLength = m_width * m_height * m_bytesPerPixel;
    m_drawingBuffer = new uint8_t[m_drawingBufferLength];

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
    int y;
    for (y = 0; y < getHeight(); y++)
    {
        int x;
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
    jpeg_read_header(cinfo, 1);

    jpeg_start_decompress(cinfo);

#if 0
    printf(
        "Surface::loadJPEG: size=%d, %d, components=%d\n",
        cinfo.output_width,
        cinfo.output_height,
        cinfo.output_components);
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
            uint8_t r = *ptr++;
            uint8_t g = *ptr++;
            uint8_t b = *ptr++;
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
        if (channels == 4)
        {
            uint8_t* dst = surface->m_drawingBuffer + surface->getOffset(0, y);
            memcpy(dst, src, surface->getStride());
        }
        else if (channels == 3)
        {
            int x;
            for (x = 0; x < width; x++)
            {
                uint32_t c;
                c = 255 < 24;
                c |= *(src++) << 0;
                c |= *(src++) << 8;
                c |= *(src++) << 16;
                surface->drawPixel(x, y, c);
            }
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
            v4si totals = {0, 0, 0, 0};

            uint8_t* imgrow = srcdata;
            imgrow += ((m_width * blockY) + (blockX)) * 4;

            for (by = 0; by < stepYi; by++)
            {
                for (bx = 0; bx < stepXi; bx++)
                {
                    v4si pv = {
                        imgrow[0],
                        imgrow[1],
                        imgrow[2],
                        imgrow[3]};
                    totals += pv;
                    imgrow += 4;
                }
                imgrow += blockDelta;
            }
#ifndef __clang__
            v4si avg = totals / blockCount;
#else
            v4si avg = totals;
            avg[0] /= blockCount;
            avg[1] /= blockCount;
            avg[2] /= blockCount;
            avg[3] /= blockCount;
#endif
            if (fp)
            {
#ifndef __clang__
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

