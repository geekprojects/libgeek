
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

