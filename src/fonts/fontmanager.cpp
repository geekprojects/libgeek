/*
 * Frontier - A toolkit for creating simple OS-independent user interfaces
 * Copyright (C) 2018, 2019 Ian Parker <ian@geekprojects.com>
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


#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <geek/fonts.h>

using namespace std;
using namespace Geek;
using namespace Geek::Gfx;

static FT_Error fontManagerRequester(
    FTC_FaceID   face_id,
    FT_Library   library,
    FT_Pointer   request_data,
    FT_Face     *aface )
{
    *aface = (FT_Face)face_id;
    return 0;
}

FontManager::FontManager()
    : Logger("FontManager")
{
}

FontManager::~FontManager()
{
    map<string, FontFamily*>::iterator it;
    for (it = m_fontFamilies.begin(); it != m_fontFamilies.end(); it++)
    {
        delete it->second;
    }

    FTC_Manager_Done(m_cacheManager);
    FT_Done_FreeType(m_library);
}

bool FontManager::init()
{
    FT_Error error;
    error = FT_Init_FreeType(&m_library);
    if (error)
    {
        log(ERROR, "init: Failed to initialise FreeType");
        return true;
    }

    error = FTC_Manager_New(
        m_library,
        ~0,
        ~0,
        ~0,
        &fontManagerRequester,
        NULL,
        &m_cacheManager);
    if (error)
    {
        log(ERROR, "Failed to create Cache Manager");
        return false;
    }

    FT_Library_SetLcdFilter(m_library, FT_LCD_FILTER_DEFAULT);

    error = FTC_CMapCache_New(m_cacheManager, &m_cmapCache);
    if (error)
    {
        log(ERROR, "Failed to create CMAP Cache");
        return false;
    }

    error = FTC_SBitCache_New(m_cacheManager, &m_sbitCache);
    if (error)
    {
        log(ERROR, "Failed to create SBit Cache");
        return false;
    }

    error = FTC_ImageCache_New(m_cacheManager, &m_imageCache);
    if (error)
    {
        log(ERROR, "Failed to create Image Cache");
        return false;
    }

    

    return true;
}

bool FontManager::scan(string dir)
{
    DIR* fd;

    log(DEBUG, "scan: Scanning %s", dir.c_str());

    fd = opendir(dir.c_str());
    if (fd == NULL)
    {
        return false;
    }

    dirent* direntEntry;
    while ((direntEntry = readdir(fd)) != NULL)
    {
        if (direntEntry->d_name[0] == '.')
        {
            continue;
        }

        struct stat stat;
        string path = dir + "/" + direntEntry->d_name;
        lstat(path.c_str(), &stat);
        if (S_ISDIR(stat.st_mode))
        {
            scan(path);
        }
        else if (S_ISREG(stat.st_mode))
        {
            addFontFile(path);
        }
    }

    closedir(fd);

    return true;
}

FontHandle* FontManager::openFont(string familyName, string style, int size)
{
#if 0
    log(DEBUG,
        "openFont: Family: %s, style=%s, size=%d",
        familyName.c_str(),
        style.c_str(),
        size);
#endif
    FontFamily* family = getFontFamily(familyName);
    if (family == NULL)
    {
        log(ERROR, "openFont: Failed to find font family: %s", familyName.c_str());
        return NULL;
    }

    FontFace* face = family->getFace(style);
    if (face == NULL)
    {
        log(ERROR, "openFont: Failed to find font style: %s", familyName.c_str());
        return NULL;
    }

    return face->open(size);
}

bool FontManager::addFontFile(string path)
{
    int index;
    for (index = 0; index < 16; index++)
    {
        FT_Error error;
        FT_Face face;

        error = FT_New_Face(m_library, path.c_str(), index, &face);
        if (error != 0)
        {
            return true;
        }

        if (face->family_name == NULL)
        {
            return false;
        }

#if 0
        log(
            DEBUG,
            "addFontFile: %s: Family: %s, style: 0x%x (%s)",
            path.c_str(),
            face->family_name,
            (int)face->style_flags,
            face->style_name);
#endif

        string familyName(face->family_name);
        FontFamily* family = getFontFamily(familyName);

        if (family == NULL)
        {
            family = new FontFamily(familyName);
            m_fontFamilies.insert(make_pair(familyName, family));
        }

        FontFace* fontFace = new FontFace(
            this,
            family,
            path,
            index,
            face->style_name,
            face->height,
            face->units_per_EM);

        family->addFace(fontFace);
        FT_Done_Face(face);
    }

    return true;
}

FontFamily* FontManager::getFontFamily(string familyName)
{
     FontFamily* family = NULL;
     map<string, FontFamily*>::iterator it;
     it = m_fontFamilies.find(familyName);
     if (it != m_fontFamilies.end())
     {
         family = it->second;
     }
     return family;
}

bool FontManager::write(
    FontHandle* font,
    Surface* surface,
    uint32_t x,
    uint32_t y,
    wstring text,
    uint32_t col,
    bool draw,
    int* widthReturn,
    int maxWidth,
    int rotate)
{
    FT_Error error;

    if (widthReturn != NULL)
    {
        *widthReturn = 0;
    }

    if (font == NULL)
    {
        log(ERROR, "write: font is NULL!");
        return false;
    }

    if (rotate != 0 && rotate != 90 && rotate != 270)
    {
        log(ERROR, "write: Unhandled rotate value: %d", rotate);
        return false;
    }

    int dpiX = 72;
    int dpiY = 72;

    bool highDPI = false;
    if (surface != NULL)
    {
        highDPI = surface->isHighDPI();
    }

    int loadFlags = 0;
    if (draw)
    {
        loadFlags |= FT_LOAD_RENDER;
        col &= 0xffffff;
    }

    FTC_ScalerRec scaler;
    scaler.face_id = font->getFTFace();
    scaler.width = 0;
    scaler.height = font->getPointSize() * 64;
    scaler.pixel = 0;
    scaler.x_res = dpiX;
    scaler.y_res = dpiY;

    if (highDPI)
    {
        scaler.height *= 2;
        if (maxWidth > 0)
        {
            maxWidth *= 2;
        }
    }

    FT_Size size;
    error = FTC_Manager_LookupSize(m_cacheManager, &scaler, &size);
    if (error)
    {
        log(ERROR, "write: Unable to look up font/size!");
        return false;
    }
    FT_Face face = size->face;
    if (face == NULL)
    {
        log(DEBUG,
            "write: Face is NULL? Font=%s:%s",
            font->getFontFace()->getPath().c_str(),
            font->getFontFace()->getStyle().c_str());
        return false;
    }

    bool useKerning = FT_HAS_KERNING(face);

    int origX2 = 0;
    int origY2 = 0;
    if (surface != NULL)
    {
        Surface* rootSurface = surface->getRoot();
        Rect surfaceRect = surface->absolute();

        x += surfaceRect.x;
        y += surfaceRect.y;

        if (highDPI)
        {
            x *= 2;
            y *= 2;
            int sw = surface->getWidth();
            int sh = surface->getWidth();
            if (rootSurface != surface)
            {
                sw *= 2;
                sh *= 2;
            }
            origX2 = (surfaceRect.x * 2) + sw;
            origY2 = (surfaceRect.y * 2) + sh;
        }
        else
        {
            origX2 = surfaceRect.x + surface->getWidth();
            origY2 = surfaceRect.y + surface->getHeight();
        }

        surface = rootSurface;
    }

    if (rotate == 90 && surface != NULL)
    {
        int precalcWidth;
        if (highDPI)
        {
            precalcWidth = width(font, text, maxWidth / 2);
            precalcWidth *= 2;
        }
        else
        {
            precalcWidth = width(font, text, maxWidth);
        }
        y += precalcWidth;
    }

    unsigned int pos;
    unsigned int width = 0;
    uint32_t prevGlyph = 0;

    font->getFontFace()->lock();

    for (pos = 0; pos < text.length(); pos++)
    {
        uint32_t glyphIndex;
        wchar_t currentChar;

        currentChar = text[pos];

        glyphIndex = FTC_CMapCache_Lookup(m_cmapCache, face, -1, currentChar);
        if (glyphIndex == 0)
        {
            glyphIndex = FTC_CMapCache_Lookup(m_cmapCache, face, -1, 0xFFFD);
            if (glyphIndex == 0)
            {
                continue;
            }
        }

        if (useKerning && prevGlyph && glyphIndex)
        {
            FT_Vector delta;
            FT_Get_Kerning(
                face,
                prevGlyph,
                glyphIndex,
                FT_KERNING_DEFAULT,
                &delta );
            if (rotate == 0)
            {
                x += delta.x >> 6;
            }
            else if (rotate == 90)
            {
                y -= delta.x >> 6;
            }
            else if (rotate == 270)
            {
                y += delta.x >> 6;
            }
            width += delta.x >> 6;
        }
        prevGlyph = glyphIndex;

        int top = 0;
        int left = 0;
        int xAdvance = 0;
        int yAdvance = 0;
        bool bitmapValid = true;
        FTC_Node glyphNode;

        FT_Bitmap bitmap;
        bitmap.buffer = NULL;
        bitmap.width = 0;
        bitmap.rows = 0;

        int w = ((scaler.width * scaler.x_res + 36) / dpiX) >> 6;
        int h = ((scaler.height * scaler.y_res + 36) / dpiY) >> 6;
        if (w < 48 && h < 48)
        {
            FTC_SBit sbit;
            error = FTC_SBitCache_LookupScaler(
                m_sbitCache,
                &scaler,
                loadFlags,
                glyphIndex,
                &sbit,
                &glyphNode);
            if (error)
            {
                log(
                    WARN,
                    "write: Unable to look up scaler from sbit cache!?");
                continue;
            }

            bitmap.rows = sbit->height;
            bitmap.width = sbit->width;
            bitmap.pitch = sbit->pitch;
            bitmap.buffer = sbit->buffer;
            left = sbit->left;
            top = sbit->top;
            xAdvance = sbit->xadvance;
            yAdvance = sbit->yadvance;
        }
        else
        {
            FT_Glyph glyph;
            error = FTC_ImageCache_LookupScaler(
                m_imageCache,
                &scaler,
                loadFlags,
                glyphIndex,
                &glyph,
                &glyphNode);
            if (error || glyph == NULL)
            {
                log(
                    WARN,
                    "write: Unable to look up scaler from image cache! c=%c",
                    currentChar);
                continue;
            }

            if (draw && glyph->format != FT_GLYPH_FORMAT_BITMAP)
            {
                log(ERROR, "write: Invalid glyph format!");
                bitmapValid = false;
            }
            else
            {
                FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)glyph;
                bitmap = bitmapGlyph->bitmap;
                left = bitmapGlyph->left;
                top = bitmapGlyph->top;
                xAdvance = glyph->advance.x >> 16;
                yAdvance = glyph->advance.y >> 16;
            }
        }

        if (rotate == 90)
        {
            y -= xAdvance;
        }

        if (draw && bitmap.buffer == NULL)
        {
            if (currentChar != ' ')
            {
                log(
                    ERROR,
                    "write: buffer is NULL? c='%c'",
                    currentChar);
            }
            bitmapValid = false;
        }

        if (draw && bitmapValid)
        {
            int ph = font->getPixelHeight(72);
            if (highDPI)
            {
                ph *= 2;
            }

            int yoff = ph - top;
            yoff -= ph / 4;

            /*
             *   0:
             *   o**
             *   *  *
             *   ****
             *   *  *
             *
             *   90:
             *   o**
             *    * *
             *    * *
             *   ***
             *
             *   270:
             *   o***
             *   * * 
             *   * * 
             *    ***
             */

            unsigned int yp;
            for (yp = 0; yp < bitmap.rows; yp++)
            {
                int x1 = 0;
                int y1 = 0;
                if (rotate == 0)
                {
                    y1 = y + yp + yoff;
                }
                else if (rotate == 90)
                {
                    x1 = x + yp + yoff;
                }
                else if (rotate == 270)
                {
                    x1 = (bitmap.rows - yp) + x;// - yoff;
                }
                if (y1 >= (int)origY2 || x1 >= (int)origX2)
                {
                    break;
                }

                unsigned int xp;
                for (xp = 0; xp < bitmap.width; xp++)
                {
                    if (rotate == 0)
                    {
                        x1 = x + xp + left;
                    }
                    else if (rotate == 90)
                    {
                        y1 = (bitmap.width - xp) + y;// + yoff;
                    }
                    else if (rotate == 270)
                    {
                        y1 = y + xp + yoff;
                    }

                    if (x1 < 0 || x1 >= origX2)
                    {
                        continue;
                    }
                    if (y1 < 0 || y1 >= origY2)
                    {
                        continue;
                    }

                    uint8_t c = bitmap.buffer[(yp * bitmap.width) + xp];
                    if (c > 0)
                    {
                        uint32_t p = (col & 0xffffff) | (c << 24);

                        if (!highDPI)
                        {
                            surface->drawPixel(
                                x1,
                                y1,
                                p);
                        }
                        else
                        {
                            ((HighDPISurface*)surface)->drawSubPixel(
                                x1,
                                y1,
                                p);
                        }
                    }
                }
            }
        }

        if (rotate == 0)
        {
            x += xAdvance;
            y += yAdvance;
            width += xAdvance;
        }
        else if (rotate == 90)
        {
            x += yAdvance;
            width += xAdvance;
        }
        else if (rotate == 270)
        {
            x += yAdvance;
            y += xAdvance;
            width += xAdvance;
        }
        else if (rotate == 180)
        {
        }

        FTC_Node_Unref(glyphNode, m_cacheManager);

        if (draw)
        {
            if ((int)x >= origX2 || (int)y >= origY2)
            {
                break;
            }
        }

        if (maxWidth > 0 && width >= (unsigned int)maxWidth)
        {
            break;
        }
    }

    font->getFontFace()->unlock();

    if (widthReturn != NULL)
    {
        *widthReturn = width;
    }

    return true;
}

int FontManager::width(
    FontHandle* font,
    wstring text,
    int maxWidth)
{
    int result;
    write(font, NULL, 0, 0, text, 0, false, &result, maxWidth);
    return result;
}

