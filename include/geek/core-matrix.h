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

#ifndef __GEEK_CORE_MATRIX_H_
#define __GEEK_CORE_MATRIX_H_

#include <string.h>
#include <geek/core-maths.h>

#define SQR(_v) ((_v) * (_v))
#define ABS(a)		((a) >= 0 ? (a) : -(a))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

namespace Geek
{

const float DEG2RAD = 3.141593f / 180.0f;
const float RAD2DEG = 180.0f / 3.141593f;
const float EPSILON = 0.00001f;

struct Matrix;

struct CentredMatrix
{
    int m_radius;
    int m_stride;
    double* m_data;
    double* m_center;

    CentredMatrix(int radius)
    {
        m_radius = radius;
        m_stride = 2 * m_radius + 1;
        m_data = new double[m_stride * m_stride];
        m_center = m_data + (m_stride * m_radius + m_radius);

        memset(m_data, 0, sizeof(double) * m_stride * m_stride);
    }

    ~CentredMatrix()
    {
        delete[] m_data;
    }

    inline void set(int x, int y, double v)
    {
        m_center[m_stride * y + x] = v;
    }

    inline double get(int x, int y) const
    {
        return m_center[m_stride * y + x];
    }

    void fill(
        double f (const int, const int, const double),
        const double fun_arg);
    void fill(
        double f (const int, const int, const double, const double),
        const double fun_arg1,
        const double fun_arg2);

    void convolve_mat(const CentredMatrix* mata, const CentredMatrix* matb);
    void convolve_star_mat(const CentredMatrix* mata, const CentredMatrix* matb);

    Matrix* makeSCentredMatrix(int m, double noiseFactor);
    Matrix* makeSMatrix(int m, double noiseFactor);
    Matrix* copyCVec(int m);
    Matrix* copyVec(int m);

    void dump();
};

struct Matrix
{
    int m_cols;
    int m_rows;
    double* m_data;

    Matrix(int r, int c)
    {
        m_rows = r;
        m_cols = c;
        m_data = new double[m_cols * m_rows];

        memset(m_data, 0, sizeof(double) * m_cols * m_rows);
    }

    ~Matrix()
    {
        delete[] m_data;
    }

    inline void set(int r, int c, double v)
    {
        m_data[m_rows * c + r] = v;
    }

    inline double get(int r, int c) const
    {
        return m_data[m_rows * c + r];
    }

    CentredMatrix* copyCVec2Mat(int m);
    CentredMatrix* copyVec2Mat(int m);
};

class Matrix4
{
 private:
    float m[16];
    float tm[16];

 public:
    // constructors
    Matrix4()
    {
        identity();
    }

    Matrix4(const float src[16])
    {
        set(src);
    }

    Matrix4(
        float m00, float m01, float m02, float m03, // 1st column
        float m04, float m05, float m06, float m07, // 2nd column
        float m08, float m09, float m10, float m11, // 3rd column
        float m12, float m13, float m14, float m15) // 4th column
    {
        set(m00, m01, m02, m03,  m04, m05, m06, m07,  m08, m09, m10, m11,  m12, m13, m14, m15);
    }

    void set(const float src[16])
    {
        set(
            src[0],
            src[1],
            src[2],
            src[3],
            src[4],
            src[5],
            src[6],
            src[7],
            src[8],
            src[9],
            src[10],
            src[11],
            src[12],
            src[13],
            src[14],
            src[15]);
    }

    void set(
        float m00, float m01, float m02, float m03, // 1st column
        float m04, float m05, float m06, float m07, // 2nd column
        float m08, float m09, float m10, float m11, // 3rd column
        float m12, float m13, float m14, float m15) // 4th column
    {
        m[0] = m00;  m[1] = m01;  m[2] = m02;  m[3] = m03;
        m[4] = m04;  m[5] = m05;  m[6] = m06;  m[7] = m07;
        m[8] = m08;  m[9] = m09;  m[10]= m10;  m[11]= m11;
        m[12]= m12;  m[13]= m13;  m[14]= m14;  m[15]= m15;
    }

    void setRow(int index, const float row[4])
    {
        m[index] = row[0];  m[index + 4] = row[1];  m[index + 8] = row[2];  m[index + 12] = row[3];
    }

    void setRow(int index, const Vector &v)
    {
        m[index] = v.x;  m[index + 4] = v.y;  m[index + 8] = v.z;
    }

    void setColumn(int index, const float col[4])
    {
        m[index*4] = col[0];  m[index*4 + 1] = col[1];  m[index*4 + 2] = col[2];  m[index*4 + 3] = col[3];
    }

    void setColumn(int index, const Vector &v)
    {
        m[index*4] = v.x;  m[index*4 + 1] = v.y;  m[index*4 + 2] = v.z;
    }

    const float* get() const
    {
        return m;
    }

    const float* getTranspose() // return transposed matrix
    {
        tm[0] = m[0];   tm[1] = m[4];   tm[2] = m[8];   tm[3] = m[12];
        tm[4] = m[1];   tm[5] = m[5];   tm[6] = m[9];   tm[7] = m[13];
        tm[8] = m[2];   tm[9] = m[6];   tm[10]= m[10];  tm[11]= m[14];
        tm[12]= m[3];   tm[13]= m[7];   tm[14]= m[11];  tm[15]= m[15];
        return tm;
    }

    float getDeterminant() const
    {
        return m[0] * getCofactor(m[5],m[6],m[7], m[9],m[10],m[11], m[13],m[14],m[15]) -
               m[1] * getCofactor(m[4],m[6],m[7], m[8],m[10],m[11], m[12],m[14],m[15]) +
               m[2] * getCofactor(m[4],m[5],m[7], m[8],m[9], m[11], m[12],m[13],m[15]) -
               m[3] * getCofactor(m[4],m[5],m[6], m[8],m[9], m[10], m[12],m[13],m[14]);
    }

    //Matrix3 getRotationMatrix() const;              // return 3x3 rotation part
    Vector getAngle() const                        // return (pitch, yaw, roll)
    {
        float pitch, yaw, roll;         // 3 angles

        // find yaw (around y-axis) first
        // NOTE: asin() returns -90~+90, so correct the angle range -180~+180
        // using z value of forward vector
        yaw = RAD2DEG * asinf(m[8]);
        if(m[10] < 0)
        {
            if(yaw >= 0) yaw = 180.0f - yaw;
            else         yaw =-180.0f - yaw;
        }

        // find roll (around z-axis) and pitch (around x-axis)
        // if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
        if(m[0] > -EPSILON && m[0] < EPSILON)
        {
            roll  = 0;  //@@ assume roll=0
            pitch = RAD2DEG * atan2f(m[1], m[5]);
        }
        else
        {
            roll = RAD2DEG * atan2f(-m[4], m[0]);
            pitch = RAD2DEG * atan2f(-m[9], m[10]);
        }

        return Vector(pitch, yaw, roll);
    }

    Matrix4 &identity()
    {
        m[0] = m[5] = m[10] = m[15] = 1.0f;
        m[1] = m[2] = m[3] = m[4] = m[6] = m[7] = m[8] = m[9] = m[11] = m[12] = m[13] = m[14] = 0.0f;
        return *this;
    }

    Matrix4 &transpose();                            // transpose itself and return reference
    Matrix4 &invert();                               // check best inverse method before inverse
    Matrix4 &invertEuclidean();                      // inverse of Euclidean transform matrix
    Matrix4 &invertAffine();                         // inverse of affine transform matrix
    Matrix4 &invertProjective();                     // inverse of projective matrix using partitioning
    Matrix4 &invertGeneral();                        // inverse of generic matrix

    // transform matrix
    Matrix4& translate(float x, float y, float z)    // translation by (x,y,z)
    {
        m[0] += m[3] * x;   m[4] += m[7] * x;   m[8] += m[11]* x;   m[12]+= m[15]* x;
        m[1] += m[3] * y;   m[5] += m[7] * y;   m[9] += m[11]* y;   m[13]+= m[15]* y;
        m[2] += m[3] * z;   m[6] += m[7] * z;   m[10]+= m[11]* z;   m[14]+= m[15]* z;

        return *this;
    }

    Matrix4& translate(const Vector &v)             //
    {
        return translate(v.x, v.y, v.z);
    }

#if 0
    Matrix4 &rotate(float angle, const Vector &axis); // rotate angle(degree) along the given axix
    Matrix4 &rotate(float angle, float x, float y, float z);
#endif
    Matrix4 &rotateX(float angle)                    // rotate on X-axis with degree
    {
        float c = cosf(angle * DEG2RAD);
        float s = sinf(angle * DEG2RAD);
        float m1 = m[1],  m2 = m[2],
            m5 = m[5],  m6 = m[6],
            m9 = m[9],  m10= m[10],
            m13= m[13], m14= m[14];

        m[1] = m1 * c + m2 *-s;
        m[2] = m1 * s + m2 * c;
        m[5] = m5 * c + m6 *-s;
        m[6] = m5 * s + m6 * c;
        m[9] = m9 * c + m10*-s;
        m[10]= m9 * s + m10* c;
        m[13]= m13* c + m14*-s;
        m[14]= m13* s + m14* c;

        return *this;
    }

    Matrix4 &rotateY(float angle)                    // rotate on Y-axis with degree
    {
        float c = cosf(angle * DEG2RAD);
        float s = sinf(angle * DEG2RAD);
        float m0 = m[0],  m2 = m[2],
            m4 = m[4],  m6 = m[6],
            m8 = m[8],  m10= m[10],
            m12= m[12], m14= m[14];

        m[0] = m0 * c + m2 * s;
        m[2] = m0 *-s + m2 * c;
        m[4] = m4 * c + m6 * s;
        m[6] = m4 *-s + m6 * c;
        m[8] = m8 * c + m10* s;
        m[10]= m8 *-s + m10* c;
        m[12]= m12* c + m14* s;
        m[14]= m12*-s + m14* c;

        return *this;
    }
    Matrix4 &rotateZ(float angle)                    // rotate on Z-axis with degree
    {
        float c = cosf(angle * DEG2RAD);
        float s = sinf(angle * DEG2RAD);
        float m0 = m[0],  m1 = m[1],
            m4 = m[4],  m5 = m[5],
            m8 = m[8],  m9 = m[9],
            m12= m[12], m13= m[13];

        m[0] = m0 * c + m1 *-s;
        m[1] = m0 * s + m1 * c;
        m[4] = m4 * c + m5 *-s;
        m[5] = m4 * s + m5 * c;
        m[8] = m8 * c + m9 *-s;
        m[9] = m8 * s + m9 * c;
        m[12]= m12* c + m13*-s;
        m[13]= m12* s + m13* c;

        return *this;
    }

#if 0
Matrix4 &scale(float scale);                     // uniform scale
    Matrix4 &scale(float sx, float sy, float sz);    // scale by (sx, sy, sz) on each axis
    Matrix4 &lookAt(float tx, float ty, float tz);   // face object to the target direction
        Matrix4 &lookAt(float tx, float ty, float tz, float ux, float uy, float uz);

    Matrix4 &lookAt(const Vector &target);

    Matrix4 &lookAt(const Vector &target, const Vector &up);
    //@@Matrix4&    skew(float angle, const Vector3& axis); //

    // operators
    Matrix4 operator+(const Matrix4 &rhs) const;    // add rhs
    Matrix4 operator-(const Matrix4 &rhs) const;    // subtract rhs
    Matrix4 &operator+=(const Matrix4 &rhs);         // add rhs and update this object
#endif

    Vector     operator*(const Vector& rhs) const     // multiplication: v' = M * v
    {
        return Vector(m[0]*rhs.x + m[4]*rhs.y + m[8]*rhs.z + m[12],
                      m[1]*rhs.x + m[5]*rhs.y + m[9]*rhs.z + m[13],
                      m[2]*rhs.x + m[6]*rhs.y + m[10]*rhs.z+ m[14]);
    }
    Matrix4     operator*(const Matrix4& n) const     // multiplication: M3 = M1 * M2
    {
        return Matrix4(m[0]*n[0]  + m[4]*n[1]  + m[8]*n[2]  + m[12]*n[3],   m[1]*n[0]  + m[5]*n[1]  + m[9]*n[2]  + m[13]*n[3],   m[2]*n[0]  + m[6]*n[1]  + m[10]*n[2]  + m[14]*n[3],   m[3]*n[0]  + m[7]*n[1]  + m[11]*n[2]  + m[15]*n[3],
                       m[0]*n[4]  + m[4]*n[5]  + m[8]*n[6]  + m[12]*n[7],   m[1]*n[4]  + m[5]*n[5]  + m[9]*n[6]  + m[13]*n[7],   m[2]*n[4]  + m[6]*n[5]  + m[10]*n[6]  + m[14]*n[7],   m[3]*n[4]  + m[7]*n[5]  + m[11]*n[6]  + m[15]*n[7],
                       m[0]*n[8]  + m[4]*n[9]  + m[8]*n[10] + m[12]*n[11],  m[1]*n[8]  + m[5]*n[9]  + m[9]*n[10] + m[13]*n[11],  m[2]*n[8]  + m[6]*n[9]  + m[10]*n[10] + m[14]*n[11],  m[3]*n[8]  + m[7]*n[9]  + m[11]*n[10] + m[15]*n[11],
                       m[0]*n[12] + m[4]*n[13] + m[8]*n[14] + m[12]*n[15],  m[1]*n[12] + m[5]*n[13] + m[9]*n[14] + m[13]*n[15],  m[2]*n[12] + m[6]*n[13] + m[10]*n[14] + m[14]*n[15],  m[3]*n[12] + m[7]*n[13] + m[11]*n[14] + m[15]*n[15]);
    }
    Matrix4&    operator*=(const Matrix4& rhs)          // multiplication: M1' = M1 * M2
    {
        *this = *this * rhs;
        return *this;
    }

    float       operator[](int index) const             // subscript operator v[0], v[1]
    {
        return m[index];
    }
    float&      operator[](int index)                   // subscript operator v[0], v[1]
    {
        return m[index];
    }

 private:
    float       getCofactor(float m0, float m1, float m2,
                            float m3, float m4, float m5,
                            float m6, float m7, float m8) const
    {
        return m0 * (m4 * m8 - m5 * m7) -
               m1 * (m3 * m8 - m5 * m6) +
               m2 * (m3 * m7 - m4 * m6);
    }

};

}; // namespace Geek

#endif
