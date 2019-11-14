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

FontFamily::FontFamily(string name)
{
    m_name = name;
}

FontFamily::~FontFamily()
{
    vector<FontFace*>::iterator it;
    for (it = m_faces.begin(); it != m_faces.end(); it++)
    {
        FontFace* face = *it;
        delete face;
    }
}

void FontFamily::addFace(FontFace* face)
{
    m_faces.push_back(face);
}

FontFace* FontFamily::getFace(string style)
{
    vector<FontFace*>::iterator it;
    for (it = m_faces.begin(); it != m_faces.end(); it++)
    {
        FontFace* face = *it;
        if (face->getStyle() == style)
        {
            return face;
        }
    }
    return NULL;
}

