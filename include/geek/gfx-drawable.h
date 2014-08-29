#ifndef __LIBGEEK_GFX_DRAWABLE_H_
#define __LIBGEEK_GFX_DRAWABLE_H_

#include <stdint.h>

#include <string>

namespace Geek
{
namespace Gfx
{

class Drawable;
class Surface;

typedef uint32_t(Drawable::*getPixelFunc_t)(int32_t x, int32_t y);
typedef bool(Drawable::*drawPixelFunc_t)(int32_t x, int32_t y, uint32_t c, uint8_t* dest);

class Drawable
{
 private:
    bool saveJPEG(struct jpeg_compress_struct* cinfo);

 protected:
    //DirtyManager m_dirty;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_bytesPerPixel;
    uint32_t m_dpiX;
    uint32_t m_dpiY;

    uint8_t* m_drawingBuffer;
    uint32_t m_drawingBufferLength;

    getPixelFunc_t m_getPixelFunc;
    drawPixelFunc_t m_drawPixelFunc;

    inline uint8_t* getDrawingBuffer() const { return m_drawingBuffer; }

    uint32_t getPixel3(int32_t x, int32_t y);
    uint32_t getPixel4(int32_t x, int32_t y);
    bool drawPixel3(int32_t x, int32_t y, uint32_t c, uint8_t* dest);
    bool drawPixel4(int32_t x, int32_t y, uint32_t c, uint8_t* dest);

 public:
    Drawable(uint32_t width, uint32_t height, uint8_t bpp);
    virtual ~Drawable();

    inline uint32_t getWidth() const { return m_width; }
    inline uint32_t getHeight() const { return m_height; }
    inline uint32_t getBytesPerPixel() const { return m_bytesPerPixel; }
    inline uint32_t getDpiX() const { return m_dpiX; }
    inline uint32_t getDpiY() const { return m_dpiY; }

    //DirtyManager* getDirtyManager() { return &m_dirty; }

    inline uint32_t getOffset(int32_t x, int32_t y) const
    {
        return (((getWidth() * y) + x) * getBytesPerPixel());
    }

    inline uint32_t getStride() const
    {
        return getWidth() * getBytesPerPixel();
    }

    virtual bool drawPixel(int32_t x, int32_t y, uint32_t c);
    virtual bool drawPixel(int32_t x, int32_t y, uint32_t c, uint8_t* dest);
    virtual bool drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c);
    virtual bool drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c);
    virtual bool drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c);
    virtual bool drawGrad(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c1, uint32_t c2);
    virtual bool clear(uint32_t c);
    virtual bool blit(uint8_t* destBuffer, int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha = false);
    virtual bool blit(int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha = false);
    virtual bool blit(int32_t x, int32_t y, Surface* surface, bool forceAlpha = false);

    virtual bool blit(
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
        bool forceAlpha = false);

    virtual uint32_t getPixel(int32_t x, int32_t y);

    virtual bool savePNG(std::string path);
    virtual bool savePNG(int fd);

    virtual bool saveJPEG(uint8_t** buffer, unsigned long* bufferLength);
    virtual bool saveJPEG(std::string path);
    virtual bool saveJPEG(int fd);

};

}; // ::Geek::Gfx
}; // ::Geek

#endif
