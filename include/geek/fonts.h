/*
 * libgeek - The GeekProjects utility suite
 * Copyright (C) 2018,2019 Ian Parker <ian@geekprojects.com>
 *
 * This file is part of libgeek.
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

#ifndef __LIBGEEK_FONTS_H_
#define __LIBGEEK_FONTS_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_LCD_FILTER_H

#include <string>
#include <vector>
#include <map>

#include <geek/core-logger.h>
#include <geek/gfx-surface.h>

namespace Geek {

class FontFamily;
class FontFace;
class FontHandle;

class FontManager : public Geek::Logger
{
 private:
    std::map<std::string, FontFamily*> m_fontFamilies;

    FT_Library m_library;
    FTC_Manager m_cacheManager;
    FTC_CMapCache m_cmapCache;
    FTC_SBitCache m_sbitCache;
    FTC_ImageCache m_imageCache;

    bool addFontFile(std::string path);

 public:

    FontManager();
    ~FontManager();

    bool init();
    bool scan(std::string dir);

    FontHandle* openFont(std::string name, std::string style, int size);

    FontFamily* getFontFamily(std::string familyName);

    FT_Library getFTLibrary() { return m_library; }

    bool write(
        FontHandle* font,
        Geek::Gfx::Surface* s,
        uint32_t x, uint32_t y,
        std::wstring text,
        uint32_t c,
        bool draw,
        int* width,
        int maxWidth = -1,
        int rotate = 0);

    int width(
        FontHandle* font,
        std::wstring chr,
        int maxWidth = -1);
};

class FontFamily
{
 private:
    std::vector<FontFace*> m_faces;

 public:
    FontFamily();
    ~FontFamily();

    void addFace(FontFace* face);

    FontFace* getFace(std::string style);
};

class FontFace
{
 private:
    FontManager* m_manager;

    std::string m_path;
    std::string m_style;

    // Size
    int m_height;
    int m_unitsPerEM;

    int m_references;
    FT_Face m_face;

 public:

    FontFace(FontManager* manager, std::string path, std::string style, int height, int unitsPerEM);
    ~FontFace();

    std::string getPath() { return m_path; }
    std::string getStyle() { return m_style; }
    int getPixelHeight(int dpi, int points);

    FontHandle* open(int pointSize);
    void close(FontHandle* handle);
    FT_Face getFTFace();
};

class FontHandle
{
 private:
    FontManager* m_fontManager;
    FontFace* m_face;
    int m_pointSize;

    std::map<int, int> m_pixelSizes;

 public:
    FontHandle(FontManager* fontManager, FontFace* face, int pointSize);
    ~FontHandle();

    FT_Face getFTFace();
    FontFace* getFontFace() { return m_face; }
    int getPointSize() { return m_pointSize; }

    int getPixelHeight(int dpi);

    FontManager* getFontManager() { return m_fontManager; }
};

};

#endif
