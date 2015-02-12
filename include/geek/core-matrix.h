#ifndef __GEEK_CORE_MATRIX_H_
#define __GEEK_CORE_MATRIX_H_

#include <string.h>

#define SQR(_v) ((_v) * (_v))
#define ABS(a)		((a) >= 0 ? (a) : -(a))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

namespace Geek
{

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

}; // namespace Geek

#endif
