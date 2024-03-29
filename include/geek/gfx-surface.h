/*
 * libgeek - The GeekProjects utility suite
 * Copyright (C) 2014, 2015, 2016 GeekProjects.com
 *
 * This file is part of libgeek.
 *
 * libgeek is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libgeek is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libgeek.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBGEEK_GFX_SURFACE_H_
#define __LIBGEEK_GFX_SURFACE_H_

#include <cstdint>

#include <geek/core-maths.h>
#include <geek/core-logger.h>
#include <geek/gfx-drawable.h>

namespace Geek
{
namespace Gfx
{

class Surface : public Drawable, public Geek::Logger
{
 private:
    static Surface* loadJPEGInternal(struct jpeg_decompress_struct* cinfo);

 protected:
    bool m_highDPI;
    bool m_copy = false;

 public:
    Surface();
    Surface(uint32_t width, uint32_t height, uint8_t bpp);
    Surface(uint32_t width, uint32_t height, uint8_t bpp, uint8_t* data);
    explicit Surface(Surface* src);
    ~Surface() override;

    virtual inline uint8_t* getData() const { return m_drawingBuffer; }
    virtual inline uint32_t getDataLength() const { return m_drawingBufferLength; }

    virtual bool resize(uint32_t width, uint32_t height);

    virtual ::Geek::Rect absolute() const { return {0, 0, (int32_t)m_width, (int32_t)m_height}; }
    virtual Surface* getRoot()
    {
        return this;
    }

    Geek::Gfx::Surface* scaleToFit(int width, int height, bool fp = false);
    Geek::Gfx::Surface* scale(float factor, bool fp = false);
    void rotate(int angle);

    void setAlpha(float alpha);

    bool swapData(Geek::Gfx::Surface* other);

    static Surface* loadImage(std::string path);
    static Surface* loadJPEG(uint8_t* data, uint32_t length);
    static Surface* loadJPEG(std::string path);
    static Surface* loadPNG(std::string path);
    static Surface* loadPNG(uint8_t* data, uint32_t length);
    static Surface* loadTGA(std::string path);

    static Surface* updateSurface(Surface* surface, int width, int height, double scale);

    virtual bool isHighDPI() { return m_highDPI; }

};

class HighDPISurface : public Surface
{
 private:

 public:
    HighDPISurface(uint32_t width, uint32_t height, uint8_t bpp);
    ~HighDPISurface() override;

    virtual bool drawSubPixel(int32_t x, int32_t y, uint32_t c);

    bool drawPixel(int32_t x, int32_t y, uint32_t c) override;
    bool drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c) override;
    bool drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c) override;
    bool drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c) override;
    bool drawGrad(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c1, uint32_t c2) override;
    bool drawCircle(int32_t x, int32_t y, uint32_t r, uint32_t c) override;
    bool drawCircleFilled(int32_t x, int32_t y, uint32_t r, uint32_t c) override;

    bool blit(int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha) override;
    bool blit(int32_t x, int32_t y, Surface* surface, bool forceAlpha) override;
    bool blit(int32_t destX, int32_t destY, Surface* surface, int viewX, int viewY, int viewW, int viewH, bool forceAlpha) override;

    bool blit(
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
        bool forceAlpha) override;
};

class SurfaceViewPort : public Surface
{
 private:
    int32_t m_offsetX;
    int32_t m_offsetY;
    Surface* m_parentSurface;

 public:
    SurfaceViewPort(Surface* parent, int32_t offsetX, int32_t offsetY, uint32_t width, uint32_t height)
    {
        m_parentSurface = parent;
        m_offsetX = offsetX;
        m_offsetY = offsetY;
        m_width = width;
        m_height = height;
        m_bytesPerPixel = getBytesPerPixel();
    }

    SurfaceViewPort(Surface* parent, ::Geek::Rect r)
    {
        m_parentSurface = parent;
        m_offsetX = r.x;
        m_offsetY = r.y;
        m_width = r.w;
        m_height = r.h;
        m_bytesPerPixel = getBytesPerPixel();
    }

    ~SurfaceViewPort() override;

    [[maybe_unused]] virtual inline int32_t getOffsetX() const { return m_offsetX; }
    [[maybe_unused]] virtual inline int32_t getOffsetY() const { return m_offsetY; }

    ::Geek::Rect absolute() const override
    {
        ::Geek::Rect r = m_parentSurface->absolute();
        r.x += m_offsetX;
        r.y += m_offsetY;
        r.w = (int)m_width;
        r.h = (int)m_height;
        return r;
    }

    inline uint8_t* getDrawingBuffer() const override { return m_parentSurface->getDrawingBuffer(); }

    Surface* getRoot() override
    {
        return m_parentSurface->getRoot();
    }

    bool resize(uint32_t width, uint32_t height) override;

    inline bool drawPixel(int32_t x, int32_t y, uint32_t c) override
    {
        return m_parentSurface->drawPixel(m_offsetX + x, m_offsetY + y, c);
    }

    bool drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint32_t c) override
    {
        return m_parentSurface->drawLine(
            m_offsetX + x1, m_offsetY + y1,
            m_offsetX + x2, m_offsetY + y2,
            c);
    }

    bool drawRectFilled(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c) override
    {
        return m_parentSurface->drawRectFilled(
            m_offsetX + x, m_offsetY + y,
            w, h,
            c);
    }

    bool drawRect(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c) override
    {
        return m_parentSurface->drawRect(
            m_offsetX + x, m_offsetY + y,
            w, h,
            c);
    }

    bool drawGrad(int32_t x, int32_t y, uint32_t w, uint32_t h, uint32_t c1, uint32_t c2) override
    {
        return m_parentSurface->drawGrad(
            m_offsetX + x, m_offsetY + y,
            w, h,
            c1, c2);
    }

    bool drawCircle(int32_t x, int32_t y, uint32_t r, uint32_t c) override
    {
        return m_parentSurface->drawCircle(m_offsetX + x, m_offsetY + y, r, c);
    }

    bool drawCircleFilled(int32_t x, int32_t y, uint32_t r, uint32_t c) override
    {
        return m_parentSurface->drawCircleFilled(m_offsetX + x, m_offsetY + y, r, c);
    }

    bool clear(uint32_t c) override
    {
        return drawRectFilled(0, 0, m_width, m_height, c);
    }

    bool blit(uint8_t* destBuffer, int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha) override
    {
        printf("SurfaceViewPort::blit: This really shouldn't be called!\n");
        return false;
    }

    bool blit(int32_t x, int32_t y, uint8_t* data, uint32_t w, uint32_t h, uint32_t bbp, bool alpha) override
    {
        return blitVP(getDrawingBuffer(), x, y, data, w, h, bbp, 0, 0, w, h, alpha);
    }

    bool blit(int32_t x, int32_t y, Surface* surface, bool alpha) override
    {
        return blitVP(getDrawingBuffer(), x, y, surface->getData(), surface->getWidth(), surface->getHeight(), getBytesPerPixel(), 0, 0, surface->getWidth(), surface->getHeight(), alpha);
    }

    bool blit(int32_t destX, int32_t destY, Surface* surface, int viewX, int viewY, int viewW, int viewH, bool forceAlpha) override
    {
        return blitVP(getDrawingBuffer(), destX, destY, surface->getData(), surface->getWidth(), surface->getHeight(), getBytesPerPixel(), viewX, viewY, viewW, viewH, forceAlpha);
    }

    bool blit(
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
        bool forceAlpha) override
    {
        return blitVP(destBuffer, x, y, data, w, h, bytesPerPixel, viewX, viewY, viewWidth, viewHeight, forceAlpha);
    }

    bool blitVP(
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
        Rect destRect(0, 0, getWidth(), getHeight()); // The destination surface
        Rect drawRect(x, y, viewWidth, viewHeight); // The rectangle we're drawing in to
        Rect viewRect(viewX, viewY, viewWidth, viewHeight); // The area of the source we're copying from

#if 0
        printf("blitVP: BEFORE destRect=%s\n", destRect.toString().c_str());
        printf("blitVP: BEFORE drawRect=%s\n", drawRect.toString().c_str());
        printf("blitVP: BEFORE viewRect=%s\n", viewRect.toString().c_str());
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
            if (isHighDPI())
            {
                d *= 2;
            }
            viewRect.x += d;
        }

        if (drawRect.y > y)
        {
            int d = drawRect.y - y;
            if (isHighDPI())
            {
                d *= 2;
            }
            viewRect.y += d;
        }
        viewRect.w = drawRect.w;
        viewRect.h = drawRect.h;

#if 0
        printf("blitVP: AFTER destRect=%s\n", destRect.toString().c_str());
        printf("blitVP: AFTER drawRect=%s\n", drawRect.toString().c_str());
        printf("blitVP: AFTER viewRect=%s\n", viewRect.toString().c_str());
#endif

        return m_parentSurface->blit(
            destBuffer,
            m_offsetX + drawRect.x,
            m_offsetY + drawRect.y,
            data,
            w,
            h,
            bytesPerPixel,
            viewRect.x,
            viewRect.y,
            viewRect.w,
            viewRect.h,
            forceAlpha);
    }

    uint32_t getPixel(int32_t x, int32_t y) override
    {
        return m_parentSurface->getPixel(m_offsetX + x, m_offsetY + y);
    }

    bool isHighDPI() override { return m_parentSurface->isHighDPI(); }
};

} // Geek::Gfx
} // Geek

#endif

