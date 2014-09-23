
#include <geek/gtk-chart.h>

#include <math.h>

using namespace std;
using namespace Geek::Gtk;

ChartWidget::ChartWidget()
{
    m_end = -1;
    m_showZero = true;

    m_dataProvider = NULL;

    set_size_request(250, 200);
}

ChartWidget::~ChartWidget()
{
}

void ChartWidget::setDataProvider(DataProvider* dp)
{
    m_dataProvider = dp;
}

void ChartWidget::dataUpdated()
{
    queue_draw();
}

bool ChartWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_dataProvider == NULL || m_dataProvider->isEmpty())
    {
        return true;
    }


    ::Gtk::Allocation allocation = get_allocation();
    const int width = allocation.get_width();
    const int height = allocation.get_height();

    int leftMargin = 30;
    int topMargin = 10;
    int bottomMargin = 10;

    float min = FLT_MAX;
    float max = FLT_MIN;

    int chartWidth = width;

    chartWidth -= leftMargin;

    /*
     *      +-----+
     *  +---+-----+---+
     *  |   |     |   |
     *  +---+-----+---+
     *      +-----+
     * Min  S     E  Max
     *
     *      +-----+
     *  +---+--+  |
     *  |   |  |  |
     *  +---+--+  |
     *      +-----+
     * Min  S Max E  Max
     */

    int drawWidth = chartWidth;
    int endX = m_dataProvider->getMaxX();
    int startX = endX - drawWidth;

    if (startX < m_dataProvider->getMinX())
    {
        startX = m_dataProvider->getMinX();
        drawWidth = (endX - startX) + 1;
    }

    int i;
    for (i = 0; i < drawWidth; i++)
    {
        unsigned int n = startX + i;
        float point = m_dataProvider->getValue(n);
        if (isnan(point))
        {
            continue;
        }

        if (point < min)
        {
            min = point;
        }
        if (point > max)
        {
            max = point;
        }
    }

    if (m_showZero)
    {
        if (min > 0 && max > 0)
        {
            min = 0;
        }
        else if (min < 0 && max < 0)
        {
            max = 0;
        }
    }

    float range = max - min;

    if (range == 0)
    {
        // Handle sets where every point is the same
        min--;
        max++;
        range += 2;
    }

    cr->set_line_width(1.5f);

    cr->set_source_rgb(0.0, 0.0, 0.0);

    // Draw X axis
    if (min <= 0 && max >= 0)
    {
        float y = (-min / range);
        y *= (height - (topMargin + bottomMargin));
        y += bottomMargin;
        cr->move_to(leftMargin, height - y);
        cr->line_to(chartWidth + leftMargin, height - y);
    }

    // Draw Y Axis
    cr->move_to(leftMargin, 0);
    cr->line_to(leftMargin, height);

    // Draw min marker
    cr->move_to(leftMargin - 5, topMargin);
    cr->line_to(leftMargin, topMargin);

    // Draw max marker
    cr->move_to(leftMargin - 5, height - bottomMargin);
    cr->line_to(leftMargin, height - bottomMargin);

    Pango::FontDescription font;
    font.set_family("Monospace");
    font.set_weight(Pango::WEIGHT_SEMIBOLD);

    char buffer[16];
    int text_width;
    int text_height;

    sprintf(buffer, "%0.1f", max);

    Glib::RefPtr<Pango::Layout> layout = create_pango_layout(buffer);
    layout->set_font_description(font);
    layout->get_pixel_size(text_width, text_height);

    cr->move_to(0, 0);
    layout->show_in_cairo_context(cr);

    sprintf(buffer, "%0.1f", min);

    layout = create_pango_layout(buffer);
    layout->set_font_description(font);
    layout->get_pixel_size(text_width, text_height);

    cr->move_to(0, height - text_height);
    layout->show_in_cairo_context(cr);

    cr->stroke();
    cr->set_line_width(2.0f);
    cr->set_source_rgb(0.0, 0.5, 1.0);

    for (i = 0; i < drawWidth; i++)
    {
        unsigned int n = startX + i;
        float point = m_dataProvider->getValue(n);
        if (isnan(point))
        {
            continue;
        }
        float y = point;

        y -= min;
        y /= range;

        // Add 10 pixel margin to top and bottom
        y *= (height - (topMargin + bottomMargin));
        y += bottomMargin;

#if 0
        cr->rectangle(leftMargin + i, height - y, 1, 1);
#else
        if (i == 0)
        {
            cr->move_to(leftMargin + i, height - y);
        }
        else
        {
            cr->line_to(leftMargin + i, height - y);
        }
#endif
    }
    cr->stroke();

    return true;
}

void ChartWidget::setShowZero(bool showZero)
{
    m_showZero = showZero;
}

DataProvider::DataProvider() :
    m_data(NAN)
{
}

DataProvider::~DataProvider()
{
}

void DataProvider::addValue(int32_t x, float value)
{
    m_data.insert(x, value);
}

float DataProvider::getValue(int32_t x)
{
    return m_data[x];
}


