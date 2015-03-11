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
