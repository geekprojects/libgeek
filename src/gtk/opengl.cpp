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


#include <geek/gtk-opengl.h>

using namespace Geek::Gtk;

OpenGLWidget::OpenGLWidget()
{
    set_double_buffered(false);

    set_events(
        Gdk::EXPOSURE_MASK |
        Gdk::BUTTON_PRESS_MASK |
        Gdk::BUTTON_RELEASE_MASK |
        Gdk::POINTER_MOTION_MASK |
        Gdk::POINTER_MOTION_HINT_MASK |
        Gdk::KEY_PRESS_MASK |
        Gdk::KEY_RELEASE_MASK);
}

OpenGLWidget::~OpenGLWidget()
{
}

Display* OpenGLWidget::getXDisplay()
{
    Glib::RefPtr< Gdk::Window> window = get_window ();
    Glib::RefPtr< Gdk::Display > display = window->get_display();
    return gdk_x11_display_get_xdisplay(display->gobj());
}

void OpenGLWidget::on_realize()
{
    ::Gtk::DrawingArea::on_realize();

    Display* xdisplay = gdk_x11_get_default_xdisplay();
    printf("OpenGLWidget::on_realize: xdisplay=%p\n", xdisplay);
    int xscreen = DefaultScreen(xdisplay);

    Glib::RefPtr< Gdk::Screen> screen = get_screen();

    int attributes[] = {
        GLX_RGBA,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        GLX_DOUBLEBUFFER, True,
        GLX_DEPTH_SIZE, 12,
        None };

    XVisualInfo *xvisual;
    xvisual = glXChooseVisual (xdisplay, xscreen, attributes);
    GdkVisual* visual = gdk_x11_screen_lookup_visual (screen->gobj(), xvisual->visualid);
    gtk_widget_set_visual ((GtkWidget*)gobj(), visual);

    m_context = glXCreateContext (xdisplay, xvisual, NULL, true);
    printf("OpenGLWidget::on_realize: context=%p\n", m_context);
}


Window OpenGLWidget::getXWindow()
{
    Glib::RefPtr< Gdk::Window> window = get_window ();
    return gdk_x11_window_get_xid(window->gobj());
}

bool OpenGLWidget::makeCurrent()
{
    return glXMakeCurrent(getXDisplay(), getXWindow(), m_context);
}

void OpenGLWidget::swapBuffers()
{
    glXSwapBuffers(getXDisplay(), getXWindow());
}

bool OpenGLWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    bool res;
    res = makeCurrent();
    if (!res)
    {
        return true;
    }

    res = on_glDraw();

    swapBuffers();

    return res;
}

