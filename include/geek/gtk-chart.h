#ifndef __LIBGEEK_GDK_CHART_H_
#define __LIBGEEK_GDK_CHART_H_

#include <gtkmm.h>

#include <geek/core-dynamicarray.h>

namespace Geek
{
namespace Gtk
{

class DataProvider
{
 protected:
    Geek::Core::DynamicArray<float> m_data;

 public:
    DataProvider();
    virtual ~DataProvider();

    virtual void addValue(int32_t x, float value);

    virtual float getValue(int32_t x);
    virtual bool isEmpty() { return m_data.isEmpty(); }

    virtual int32_t getMinX() { return m_data.getMinIndex(); }
    virtual int32_t getMaxX() { return m_data.getMaxIndex(); }
};

class ChartWidget : public ::Gtk::DrawingArea
{
 private:
    int m_end;
    bool m_showZero;

    DataProvider* m_dataProvider;

 public:

    ChartWidget();
    virtual ~ChartWidget();

    void setDataProvider(DataProvider* dp);
    void dataUpdated();

    virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

    void setShowZero(bool showZero);
};

};
};

#endif
