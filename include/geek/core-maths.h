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

#include <cstdio>
#include <cstdint>
#include <cmath>

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

    inline bool operator ==(const Vector2D& rhs) const
    {
        return (x == rhs.x && y == rhs.y);
    }

    inline bool operator !=(const Vector2D& rhs) const
    {
        return (x != rhs.x || y != rhs.y);
    }

    inline Vector2D operator -(const Vector2D& rhs) const
    {
        return {x - rhs.x, y - rhs.y};
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

    inline Vector2D& operator +=(const Vector2D rhs)
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

    Rect(int32_t _x, int32_t _y, int32_t _w, int32_t _h)
    {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    int32_t getX2() const { return (x + w) - 1; }
    int32_t getY2() const { return (y + h) - 1; }

    std::string toString() const
    {
        char buf[128];
        snprintf(buf, 128, "[Rect x=%d, y=%d, w=%d, h=%d]", x, y, w, h);
        return std::string(buf);
    }

    bool intersects(Rect r2) const
    {
        int32_t x2 = x + w;
        int32_t y2 = y + h;
        int32_t r2x2 = r2.x + r2.w;
        int32_t r2y2 = r2.y + r2.h;
        return (!( r2.x > x2 || r2x2 < x || r2.y > y2 || r2y2 < y));
    }

    bool contains(Vector2D v) const
    {
        return (v.x >= x && v.y >= y && v.x < (x + w) && v.y < (y + h));
    }

    Rect clipCopy(Rect r2) const
    { 
        Rect res;
        res.x = MAX(x, r2.x);
        res.y = MAX(y, r2.y);
        res.w = MIN(x + w, r2.x + r2.w) - res.x;
        res.h = MIN(y + h, r2.y + r2.h) - res.y;
        return res;
    }

    bool clip(Rect r2)
    { 
        if (x < r2.x)
        {
            int d = r2.x - x;
            x = r2.x;
            w -= d;
        }
        else if (x > r2.getX2())
        {
            return false;
        }

        if (y < r2.y)
        {
            int d = r2.y - y;
            y = r2.y;
            h -= d;
        }
        else if (y > r2.getY2())
        {
            return false;
        }

        w = MIN(x + w, r2.x + r2.w) - x;
        h = MIN(y + h, r2.y + r2.h) - y;

        return true;
    }
};

struct Vector
{
    union
    {
        struct
        {
            double x;
            double y;
            double z;
            double _align;
        };
        double f[4];
    };

    Vector()
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;
    }

    Vector(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector(const Vector& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    void set(double _x, double _y, double _z)
    {
        x = _x;
        y = _y;
        z = _z;
    }

    inline Vector operator + (const Vector v) const
    {
        return Vector(x + v.x, y + v.y, z + v.z);
    }

    inline Vector operator - (const Vector& v) const
    {
        return Vector(x - v.x, y - v.y, z - v.z);
    }

    inline Vector operator - () const
    {
        return Vector(-x, -y, -z);
    }

    inline Vector operator * (const Vector& v) const
    {
        return Vector(x * v.x, y * v.y, z * v.z);
    }

    inline Vector operator * (double m) const
    {
        return Vector(x * m, y * m, z * m);
    }

    inline Vector operator / (double m) const
    {
        return Vector(x / m, y / m, z / m);
    }

    inline Vector& operator += (Vector v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    inline Vector& operator -= (Vector v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    inline Vector& operator *= (double s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    inline Vector& operator /= (double s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    inline double dot(const Vector& v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    inline Vector cross(const Vector& v) const
    {
        return Vector(
            (y * v.z) - (z * v.y),
            (z * v.x) - (x * v.z),
            (x * v.y) - (y * v.x));
    }

    inline double length2() const
    {
        return dot(*this);
    }

    inline double length() const
    {
        return sqrt(length2());
    }

    inline double distance(Vector& other) const
    {
        Vector diff = *this;
        diff -= other;
        return diff.length();
    }

    inline Vector& normalise()
    {
        double len = length();
        if (len == 0)
        {
            set(0.0f, 0.0f, 0.0f);
            return *this;
        }
        return *this /= len;
    }

    inline bool operator != (const Vector& rhs) const
    {
        return
            this->x != rhs.x ||
            this->y != rhs.y ||
            this->z != rhs.z;
    }

    inline Vector rotate(double theta) const
    {
        double cs = cos(theta);
        double sn = sin(theta);

        Vector result;
        result.x = x * cs - y * sn;
        result.y = x * sn + y * cs;
        return result;
    }

    static Vector null()
    {
        return Vector(NAN, NAN, NAN);
    }

    std::string toString() const
    {
        std::string str = "";
        char buf[16];
        snprintf(buf, 16, "%0.2f", x);
        str += buf;
        str += ",";
        snprintf(buf, 16, "%0.2f", y);
        str += buf;
        str += ",";
        snprintf(buf, 16, "%0.2f", z);
        str += buf;
        return str;
    }
};

struct BoundingBox
{
    Vector min;
    Vector max;

    double size() const
    {
        double smin = min.length();
        double smax = max.length();
        if (smin > smax)
        {
            return smin;
        }
        else
        {
            return smax;
        }
    }
};

class Quaternion
{
 public:
    double m_x;
    double m_y;
    double m_z;
    double m_w;

 public:
    Quaternion()
    {
        set(0.0f, 0.0f, 0.0f);
    }

    Quaternion(const Quaternion& q)
    {
        set(q.m_x, q.m_y, q.m_z, q.m_w);
    }

    Quaternion(double x, double y, double z, double w)
    {
        set(x, y, z, w);
    }

    ~Quaternion()
    {
    }

    void set(double x, double y, double z)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = 0.0f;
    }

    void set(double x, double y, double z, double w)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }


    /*
     * http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/index.htm
     */
    void setFromMatrix(double matrix[3][3])
    {
        double tr = matrix[0][0] + matrix[1][1] + matrix[2][2];

        if (tr > 0)
        {
            double S = sqrt(tr+1.0) * 2; // S=4*m_w
            m_w = 0.25 * S;
            m_x = (matrix[2][1] - matrix[1][2]) / S;
            m_y = (matrix[0][2] - matrix[2][0]) / S;
            m_z = (matrix[1][0] - matrix[0][1]) / S;
        }
        else if ((matrix[0][0] > matrix[1][1])&(matrix[0][0] > matrix[2][2]))
        {
            double S = sqrt(1.0 + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2; // S=4*m_x
            m_w = (matrix[2][1] - matrix[1][2]) / S;
            m_x = 0.25 * S;
            m_y = (matrix[0][1] + matrix[1][0]) / S;
            m_z = (matrix[0][2] + matrix[2][0]) / S;
        }
        else if (matrix[1][1] > matrix[2][2])
        {
            double S = sqrt(1.0 + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2; // S=4*m_y
            m_w = (matrix[0][2] - matrix[2][0]) / S;
            m_x = (matrix[0][1] + matrix[1][0]) / S;
            m_y = 0.25 * S;
            m_z = (matrix[1][2] + matrix[2][1]) / S; 
        }
        else
        {
            double S = sqrt(1.0 + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2; // S=4*m_z
            m_w = (matrix[1][0] - matrix[0][1]) / S;
            m_x = (matrix[0][2] + matrix[2][0]) / S;
            m_y = (matrix[1][2] + matrix[2][1]) / S;
            m_z = 0.25 * S;
        }
    }

    double getX() { return m_x; }
    double getY() { return m_y; }
    double getZ() { return m_z; }
    double getW() { return m_w; }

    void calculateW()
    {
        double t;
        t = 1.0f - (m_x * m_x) - (m_y * m_y) - (m_z * m_z);
        if (t < 0.0f)
        {
            m_w = 0.0f;
        }
        else
        {
            m_w = -sqrt(t);
        }
    }

    inline void setRotation(const Vector& axis, double angle)
    {
        double len = axis.length();
        double s = sin(angle * 0.5f) / len;
        set(
            axis.x * s,
            axis.y * s,
            axis.z * s,
            cos(angle * 0.5));
    }

    inline void setAngleAxis(double angle, Vector& v)
    {
        double s = sin(angle * 0.5f);
        set(
            v.x * s,
            v.y * s,
            v.z * s,
            cos(angle * 0.5));
    }

    inline void setEuler(double yaw, double pitch, double roll)
    {
        double halfYaw = yaw * 0.5f;
        double halfPitch = pitch * 0.5f;
        double halfRoll = roll * 0.5f;
        double cosYaw = cos(halfYaw);
        double sinYaw = sin(halfYaw);
        double cosPitch = cos(halfPitch);
        double sinPitch = sin(halfPitch);
        double cosRoll = cos(halfRoll);
        double sinRoll = sin(halfRoll);
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

    Quaternion& operator *= (double s)
    {
        m_x *= s;
        m_y *= s;
        m_z *= s;
        m_w *= s;
        return *this;
    }

    Quaternion& operator*=(const Quaternion& q)
    {
        double x = (m_x * q.m_w) + (m_w * q.m_x) + (m_y * q.m_z) - (m_z * q.m_y);
        double y = (m_y * q.m_w) + (m_w * q.m_y) + (m_z * q.m_x) - (m_x * q.m_z);
        double z = (m_z * q.m_w) + (m_w * q.m_z) + (m_x * q.m_y) - (m_y * q.m_x);
        double w = (m_w * q.m_w) - (m_x * q.m_x) - (m_y * q.m_y) - (m_z * q.m_z);
        set(x, y, z, w);

        return *this;
    }

    Quaternion& operator *= (Vector& v)
    {
        double w = - (m_x * v.x) - (m_y * v.y) - (m_z * v.z);
        double x =   (m_w * v.x) + (m_y * v.z) - (m_z * v.y);
        double y =   (m_w * v.y) + (m_z * v.x) - (m_x * v.z);
        double z =   (m_w * v.z) + (m_x * v.y) - (m_y * v.x);
        set(x, y, z, w);

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

    double magnitude()
    {
        return sqrt(
            (m_x * m_x) +
            (m_y * m_y) +
            (m_z * m_z) +
            (m_w * m_w));
    }

    Quaternion& normalise()
    {
        double mag = magnitude();

          /* check for bogus length, to protect against divide by zero */
          if (mag > 0.0f)
          {
              /* normalize it */
              double oneOverMag = 1.0f / mag;

              m_x *= oneOverMag;
              m_y *= oneOverMag;
              m_z *= oneOverMag;
              m_w *= oneOverMag;
          }
          return *this;
    }

    Vector rotateVector(Vector& in)
    {
        Quaternion conj = conjugate();
        conj.normalise();

        Quaternion tmp(*this);
        tmp *= in;
        tmp *= conj;

        return tmp.vector();
    }

    void getMatrix(double m[4][4])
    {
        double wx, wy, wz;
        double xx, xy, xz;
        double yy, yz;
        double zz;
        double x2, y2, z2;

        // Coefficients
        x2 = m_x + m_x; y2 = m_y + m_y;
        z2 = m_z + m_z;
        xx = m_x * x2; xy = m_x * y2; xz = m_x * z2;
        yy = m_y * y2; yz = m_y * z2; zz = m_z * z2;
        wx = m_w * x2; wy = m_w * y2; wz = m_w * z2;

        m[0][0] = 1.0 - (yy + zz);
        m[1][0] = xy - wz;
        m[2][0] = xz + wy;
        m[3][0] = 0.0;

        m[0][1] = xy + wz;
        m[1][1] = 1.0 - (xx + zz);
        m[2][1] = yz - wx;
        m[3][1] = 0.0;

        m[0][2] = xz - wy;
        m[1][2] = yz + wx;
        m[2][2] = 1.0 - (xx + yy);
        m[3][2] = 0.0;

        m[0][3] = 0;
        m[1][3] = 0;
        m[2][3] = 0;
        m[3][3] = 1;
    }

    static Quaternion rotationBetweenVectors(Vector start, Vector dest)
    {
        start = start.normalise();
        dest = dest.normalise();

        double cosTheta = dest.dot(start);

        Vector rotationAxis;
        if (cosTheta < -1 + 0.0001f)
        {
            rotationAxis = Vector(0.0, 0.0, 1.0).cross(start);
            if (rotationAxis.length2() < 0.1)
            {
                rotationAxis = Vector(1.0, 0.0, 0.0).cross(start);
            }
            rotationAxis = rotationAxis.normalise();

            Quaternion q;
            q.setAngleAxis(M_PI, rotationAxis);
            return q;
        }

        rotationAxis = start.cross(dest);

        double s = sqrt((1 + cosTheta) * 2.0);
        double invs = 1.0 / s;

        return Quaternion(
            rotationAxis.x * invs,
            rotationAxis.y * invs,
            rotationAxis.z * invs,
            s * 0.5);
    }

    static Quaternion lookAt(Vector dir, Vector up)
    {
        if (dir.length() < 0.0001)
        {
            // Too small!
            return Quaternion();
        }

        // Recompute up so that it's perpendicular to the direction
        // You can skip that part if you really want to force desiredUp
        Vector right = dir.cross(up);
        up = right.cross(dir);

        Quaternion rot1 = rotationBetweenVectors(Vector(0.0, 0.0, 1.0), dir);

        Vector newUp = Vector(0.0, 1.0, 0.0);
        newUp = rot1.rotateVector(newUp);
        Quaternion rot2 = rotationBetweenVectors(newUp, up);
        rot2 *= rot1;
        return rot2;
   }
};

}; // ::Geek

#endif
