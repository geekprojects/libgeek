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

#ifndef __GEEK_CORE_MATHS_H_
#define __GEEK_CORE_MATHS_H_

#include <stdio.h> // for snprintf
#include <stdint.h>
#include <math.h>

#include <string>

#ifndef MIN
#define MIN(_a, _b) ((_a < _b) ? (_a) : (_b))
#endif
#ifndef MAX
#define MAX(_a, _b) ((_a > _b) ? (_a) : (_b))
#endif

namespace Geek
{

struct Rect;

struct Vector2D
{
    union
    {
        struct
        {
            int x;
            int y;
        };
        int i[2];
    };

    Vector2D()
    {
        x = 0;
        y = 0;
    }

    Vector2D(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    void set(int _x, int _y)
    {
        x = _x;
        y = _y;
    }

    inline Vector2D operator -(const Vector2D& rhs) const
    {
        return Vector2D(x - rhs.x, y - rhs.y);
    }

    inline Vector2D& operator -=(const Vector2D& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    inline Vector2D& operator +=(const Vector2D& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    inline void clip(const Rect& r);
};

struct Rect
{
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;

    Rect()
    {
        x = 0;
        y = 0;
        w = 0;
        h = 0;
    }

    Rect(uint32_t _x, uint32_t _y, uint32_t _w, uint32_t _h)
    {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    uint32_t getX2() { return x + w; }
    uint32_t getY2() { return y + h; }

    std::string toString()
    {
        char buf[128];
        snprintf(buf, 128, "[Rect x=%d, y=%d, w=%d, h=%d]", x, y, w, h);
        return std::string(buf);
    }

    bool intersects(Rect r2)
    {
        int32_t x2 = x + w;
        int32_t y2 = y + h;
        int32_t r2x2 = r2.x + r2.w;
        int32_t r2y2 = r2.y + r2.h;
        return (!( r2.x > x2 || r2x2 < x || r2.y > y2 || r2y2 < y));
    }

    bool contains(Vector2D v)
    {
        return (v.x >= x && v.y >= y && v.x < (x + w) && v.y < (y + h));
    }


    Rect clipCopy(Rect r2)
    { 
        Rect res;
        res.x = MAX(x, r2.x);
        res.y = MAX(y, r2.y);
        res.w = MIN(x + w, r2.x + r2.w) - res.x;
        res.h = MIN(y + h, r2.y + r2.h) - res.y;
        return res;
    }

    Rect clip(Rect r2)
    { 
        x = MAX(x, r2.x);
        y = MAX(y, r2.y);
        w = MIN(x + w, r2.x + r2.w) - x;
        h = MIN(y + h, r2.y + r2.h) - y;
        return *this;
    }
};

}; // ::Geek

#endif
