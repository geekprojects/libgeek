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

#ifndef __LIBGEEK_GTK_OPENGL_WIDGET_H_
#define __LIBGEEK_GTK_OPENGL_WIDGET_H_

#include <gtkmm.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <GL/gl.h>
#include <GL/glx.h>

namespace Geek
{
namespace Gtk
{

class OpenGLWidget : public ::Gtk::DrawingArea
{
 private:
    GLXContext m_context;

 protected:
    Display* getXDisplay();
    Window getXWindow();
    bool makeCurrent();
    void swapBuffers();

 public:
    OpenGLWidget();
    virtual ~OpenGLWidget();

    virtual void on_realize();
    virtual bool on_glDraw() {}
    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);
};

}
}

#endif
