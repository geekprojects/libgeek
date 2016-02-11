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


#include <math.h>

#include <gtkmm.h>
#include <geek/gtk-chart.h>

class ChartWindow : public Gtk::Window
{
 private:
    int m_i;

    Geek::Gtk::ChartWidget m_chart;
    Geek::Gtk::DataProvider m_dataProvider;

    sigc::connection m_timer;

 public:
    ChartWindow();
    ~ChartWindow();

    bool on_timeout();
};

ChartWindow::ChartWindow()
{
    m_i = 0;
    m_chart.setDataProvider(&m_dataProvider);

    //sigc::slot<bool> my_slot = sigc::bind(
    m_timer = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &ChartWindow::on_timeout),
        100);

    add(m_chart);
    show_all();
}

ChartWindow::~ChartWindow()
{
}

bool ChartWindow::on_timeout()
{
    m_dataProvider.addValue(
        m_i * 2,
        cos(((float)m_i * 1.0f) * (M_PI / 180.0f)) + 0.0f);
    m_chart.dataUpdated();

    m_i += 2;

    return true;
}

int main(int argc, char** argv)
{
    Glib::RefPtr<Gtk::Application> app;
    app = Gtk::Application::create(argc, argv, "com.geekprojects.libgeek");

    ChartWindow chartWindow;

    app->run(chartWindow);

    return 0;
}


