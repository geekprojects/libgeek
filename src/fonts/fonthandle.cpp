/*
 * Frontier - A toolkit for creating simple OS-independent user interfaces
 * Copyright (C) 2018 Ian Parker <ian@geekprojects.com>
 *
 * This file is part of Frontier.
 *
 * Frontier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Frontier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Frontier.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <geek/fonts.h>

using namespace std;
using namespace Geek;

FontHandle::FontHandle(FontManager* fontManager, FontFace* face, int pointSize)
{
    m_fontManager = fontManager;
    m_face = face;
    m_pointSize = pointSize;
}

FontHandle::~FontHandle()
{
}

FT_Face FontHandle::getFTFace()
{
    return m_face->getFTFace();
}

int FontHandle::getPixelHeight(int dpi)
{
    map<int, int>::iterator it;
    it = m_pixelSizes.find(dpi);
    if (it != m_pixelSizes.end())
    {
        return it->second;
    }

    FT_Face ftFace = m_face->getFTFace();
    float heightf = (ftFace->height * ((float)m_pointSize * (float)dpi / 72.0f)) / ftFace->units_per_EM;
    int height = (int)heightf;

    m_pixelSizes.insert(make_pair(dpi, height));
    return height;
}

