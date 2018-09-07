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


#include <geek/core-matrix.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

using namespace Geek;

#ifndef MAX
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#endif
#ifndef MIN
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#endif

static inline int as_idx (const int k, const int l, const int m)
{
    return ((k + m) * (2 * m + 1) + (l + m));
}

static inline int as_cidx (const int k, const int l)
{
    const int a = MAX (ABS (k), ABS (l));
    const int b = MIN (ABS (k), ABS (l));
    return ((a * (a + 1)) / 2 + b);
}

void CentredMatrix::fill(
    double f (const int, const int, const double),
    const double fun_arg)
{
    int x, y;

    for (y = -m_radius; y <= m_radius; y++)
    {
        for (x = -m_radius; x <= m_radius; x++)
        {
            set(x, y, f (x, y, fun_arg));
        }
    }
}

void CentredMatrix::fill(
    double f (const int, const int, const double, const double),
    const double fun_arg1,
    const double fun_arg2)
{
    int x, y;

    for (y = -m_radius; y <= m_radius; y++)
    {
        for (x = -m_radius; x <= m_radius; x++)
        {
            set(x, y, f (x, y, fun_arg1, fun_arg2));
        }
    }
}

void CentredMatrix::convolve_mat(const CentredMatrix* mata, const CentredMatrix* matb)
{
    int xr;
    int yr;
    int xa;
    int ya;

    for (yr = -m_radius; yr <= m_radius; yr++)
    {
        for (xr = -m_radius; xr <= m_radius; xr++)
        {
            const int ya_low = MAX (-mata->m_radius, -matb->m_radius - yr);
            const int ya_high = MIN (mata->m_radius, matb->m_radius - yr);
            const int xa_low = MAX (-mata->m_radius, -matb->m_radius - xr);
            const int xa_high = MIN (mata->m_radius, matb->m_radius - xr);
            double val = 0.0;

            for (ya = ya_low; ya <= ya_high; ya++)
            {
                for (xa = xa_low; xa <= xa_high; xa++)
                {
                    val += mata->get(xa, ya) * matb->get(xr - xa, yr - ya);
                }
            }
            set(xr, yr, val);
        }
    }
}

void CentredMatrix::convolve_star_mat(const CentredMatrix* mata, const CentredMatrix* matb)
{
    int xr;
    int yr;
    int xa;
    int ya;

    for (yr = -m_radius; yr <= m_radius; yr++)
    {
        for (xr = -m_radius; xr <= m_radius; xr++)
        {
            const int ya_low = MAX (-mata->m_radius, -matb->m_radius - yr);
            const int ya_high = MIN (mata->m_radius, matb->m_radius - yr);
            const int xa_low = MAX (-mata->m_radius, -matb->m_radius - xr);
            const int xa_high = MIN (mata->m_radius, matb->m_radius - xr);
            double val = 0.0;

            for (ya = ya_low; ya <= ya_high; ya++)
            {
                for (xa = xa_low; xa <= xa_high; xa++)
                {
                    val += mata->get(xa, ya) * matb->get(xr + xa, yr + ya);
                }
            }
            set(xr, yr, val);
        }
    }
}

Matrix* CentredMatrix::makeSCentredMatrix(int m, double noiseFactor)
{
    const int mat_size = as_cidx (m + 1, 0);
    Matrix *result = new Matrix(mat_size, mat_size);

    int yr;
    for (yr = 0; yr <= m; yr++)
    {
        int xr;
        for (xr = 0; xr <= yr; xr++)
        {
            int yc;
            for (yc = -m; yc <= m; yc++)
            {
  int xc;
                for (xc = -m; xc <= m; xc++)
                {
                    double v = result->get(as_cidx (xr, yr), as_cidx (xc, yc));
                    v += get(xr - xc, yr - yc);
                    if ((xr == xc) && (yr == yc))
                    {
                        v += noiseFactor;
                    }
                    result->set(as_cidx (xr, yr), as_cidx (xc, yc), v);
                }
            }
        }
    }
    return result;
}

Matrix* CentredMatrix::makeSMatrix(int m, double noiseFactor)
{
    int matrixSize = SQR(2 * m * 1);
    Matrix* result = new Matrix(matrixSize, matrixSize);

    int yr;
    for (yr = -m; yr <= m; yr++)
    {
        int xr;
        for (xr = -m; xr <= m; xr++)
        {
            int yc;
            for (yc = -m; yc <= m; yc++)
            {
                int xc;
                for (xc = -m; xc <= m; xc++)
                {
                    result->set(
                        as_idx (xr, yr, m),
                        as_idx (xc, yc, m),
                        get( xr - xc, yr - yc));
                    if ((xr == xc) && (yr == yc))
                    {
                        double v;
                        v = result->get( as_idx (xr, yr, m),
                                     as_idx (xc, yc, m));
                        result->set(
                            as_idx (xr, yr, m),
                            as_idx (xc, yc, m),
                            v + noiseFactor);
                    }
                }
            }
        }
    }
    return result;
}

Matrix* CentredMatrix::copyCVec(int m)
{
    Matrix* result = new Matrix(as_cidx(m + 1, 0), 1);
    int y;
    int index = 0;

    for (y = 0; y <= m; y++)
    {
        int x;
        for (x = 0; x <= y; x++)
        {
            result->set(index, 0, get(x, y));
            index++;
        }
    }
    return result;
}

Matrix* CentredMatrix::copyVec(int m)
{
    Matrix* result = new Matrix(SQR (2 * m + 1), 1);
    int y;
    int index = 0;

    for (y = -m; y <= m; y++)
    {
        int x;
        for (x = -m; x <= m; x++)
        {
            result->set(index, 0, get(x, y));
            index++;
        }
    }
    return (result);
}

void CentredMatrix::dump()
{
    double* d = m_data;
    int y;
    for (y = 0; y < m_stride; y++)
    {
        int x;
        for (x = 0; x < m_stride; x++)
        {
            printf("%0.2f ", *(d++));
        }
        printf("\n");
    }
}

CentredMatrix * Matrix::copyCVec2Mat (const int m)
{
    CentredMatrix *result = new CentredMatrix(m);
    int y;

    for (y = -m; y <= m; y++)
    {
        int x;
        for (x = -m; x <= m; x++)
        {
            result->set( x, y, get( as_cidx (x, y), 0));
        }
    }
    return result;
}

CentredMatrix* Matrix::copyVec2Mat(const int m)
{
    CentredMatrix* result = new CentredMatrix(m);
    int y;

    for (y = -m; y <= m; y++)
    {
        int x;
        for (x = -m; x <= m; x++)
        {
            result->set(x, y, get( as_idx (x, y, m), 0));
        }
    }
    return (result);
}



