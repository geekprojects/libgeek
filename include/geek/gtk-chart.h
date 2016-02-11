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
