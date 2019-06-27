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

FontFace::FontFace(
    FontManager* manager,
    string path,
    string style,
    int height,
    int unitsPerEM)
{
    m_manager = manager;
    m_path = path;
    m_style = style;
    m_height = height;
    m_unitsPerEM = unitsPerEM;

    m_references = 0;
    m_face = NULL;
}

FontFace::~FontFace()
{
}

FontHandle* FontFace::open(int pointSize)
{
    FT_Error error;

    if (m_face == NULL)
    {
        error = FT_New_Face(
            m_manager->getFTLibrary(),
            m_path.c_str(),
            0,
            &m_face);
        if (error != 0)
        {
            return NULL;
        }
    }

    m_references++;

    return new FontHandle(m_manager, this, pointSize);
}

void FontFace::close(FontHandle* handle)
{
    m_references--;
    if (m_references <= 0)
    {
        // Free font
    }
    delete handle;
}

FT_Face FontFace::getFTFace()
{
    return m_face;
}

