#ifndef __LIBGEEK_GDK_CHART_H_
#define __LIBGEEK_GDK_CHART_H_

#include <gtkmm.h>

#include <vector>

namespace Geek
{
namespace Gtk
{

class ChartWidget : public ::Gtk::DrawingArea
{
 private:
    std::vector<float> m_data;
    bool m_showZero;

 public:

    ChartWidget();
    virtual ~ChartWidget();

    void addValue(float value);

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

    void setShowZero(bool showZero);
};

};
};

#endif
