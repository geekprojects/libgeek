#ifndef __BEYOND_MATHS_H_
#define __BEYOND_MATHS_H_

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

struct Vector
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
        };
        float f[3];
    };

    Vector()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vector(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    void set(float _x, float _y, float _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }


    inline Vector operator + (Vector& v)
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    inline Vector operator - (Vector& v)
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    inline Vector& operator += (Vector& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline Vector& operator -= (Vector& v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    inline Vector& operator *= (float s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    inline Vector& operator /= (float s)
    {
        s = 1.0f / s;
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    inline float dot(const Vector& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline float length2() const
    {
        return dot(*this);
    }

    inline float length() const
    {
        return sqrt(length2());
    }

    inline Vector& normalise()
    {
        float len = length();
        if (len == 0)
        {
            set(0.0f, 0.0f, 0.0f);
            return *this;
        }
        return *this /= len;
    }
};

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
    uint32_t w;
    uint32_t h;

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


class Quaternion
{
 private:
    float m_x;
    float m_y;
    float m_z;
    float m_w;

 public:
    Quaternion()
    {
        set(0.0f, 0.0f, 0.0f);
    }

    Quaternion(const Quaternion& q)
    {
        set(q.m_x, q.m_y, q.m_z, q.m_w);
    }

    Quaternion(float x, float y, float z, float w)
    {
        set(x, y, z, w);
    }
 
    ~Quaternion()
    {
    }

    void set(float x, float y, float z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = 0.0f;
    }

    void set(float x, float y, float z, float w)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    inline void setRotation(const Vector& axis, float angle)
    {
        float len = axis.length();
        float s = sin(angle * 0.5f) / len;
        set(
            axis.x * s,
            axis.y * s,
            axis.z * s,
            cos(angle * 0.5));
    }

    inline void setEuler(float yaw, float pitch, float roll)
    {
        float halfYaw = yaw * 0.5f;
        float halfPitch = pitch * 0.5f;
        float halfRoll = roll * 0.5f;
        float cosYaw = cos(halfYaw);
        float sinYaw = sin(halfYaw);
        float cosPitch = cos(halfPitch);
        float sinPitch = sin(halfPitch);
        float cosRoll = cos(halfRoll);
        float sinRoll = sin(halfRoll);
        set(cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw,
            cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw,
            sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw,
            cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw);
    }

    Quaternion& operator += (const Quaternion& q)
    {
        m_x += q.m_x;
        m_y += q.m_y;
        m_z += q.m_z;
        m_w += q.m_w;
        return *this;
    }

    Quaternion& operator -= (const Quaternion& q)
    {
        m_x -= q.m_x;
        m_y -= q.m_y;
        m_z -= q.m_z;
        m_w -= q.m_w;
        return *this;
    }

    Quaternion& operator *= (float s)
    {
        m_x *= s;
        m_y *= s;
        m_z *= s;
        m_w *= s;
        return *this;
    }

    Quaternion& operator*=(const Quaternion& q)
    {
        set(m_w * q.m_x + m_x * q.m_w + m_y * q.m_z - m_z * q.m_y,
            m_w * q.m_y + m_y * q.m_w + m_z * q.m_x - m_x * q.m_z,
            m_w * q.m_z + m_z * q.m_w + m_x * q.m_y - m_y * q.m_x,
            m_w * q.m_w - m_x * q.m_x - m_y * q.m_y - m_z * q.m_z);
        return *this;
    }

    Vector vector()
    {
        return Vector(m_x, m_y, m_z);
    }

    Quaternion inverse()
    {
        return Quaternion(m_x, m_y, m_z, -m_w);
    }

    Quaternion conjugate()
    {
        return Quaternion(-m_x, -m_y, -m_z, m_w);
    }

    void getMatrix(float m[4][4])
    {
        float wx, wy, wz;
        float xx, xy, xz;
        float yy, yz;
        float zz;
        float x2, y2, z2;

        // Coefficients
        x2 = m_x + m_x; y2 = m_y + m_y;
        z2 = m_z + m_z;
        xx = m_x * x2; xy = m_x * y2; xz = m_x * z2;
        yy = m_y * y2; yz = m_y * z2; zz = m_z * z2;
        wx = m_w * x2; wy = m_w * y2; wz = m_w * z2;

        m[0][0] = 1.0 - (yy + zz); m[1][0] = xy - wz;
        m[2][0] = xz + wy; m[3][0] = 0.0;

        m[0][1] = xy + wz; m[1][1] = 1.0 - (xx + zz);
        m[2][1] = yz - wx; m[3][1] = 0.0;

        m[0][2] = xz - wy; m[1][2] = yz + wx;
        m[2][2] = 1.0 - (xx + yy); m[3][2] = 0.0;

        m[0][3] = 0; m[1][3] = 0;
        m[2][3] = 0; m[3][3] = 1;
    }
};

}; // ::Geek

#endif
