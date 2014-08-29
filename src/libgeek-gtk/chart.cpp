
#include <geek/gtk-chart.h>

#include <math.h>

using namespace std;
using namespace Geek::Gtk;

ChartWidget::ChartWidget()
{
#if 0
    int i;
    for (i = 0; i < 360 * 10; i++)
    {
        m_data.push_back(cos(((float)i * 1.0f) * (M_PI / 180.0f)) + 2.0f);
    }
#endif
}

ChartWidget::~ChartWidget()
{
}

void ChartWidget::addValue(float value)
{
    m_data.push_back(value);

    queue_draw();
}

bool ChartWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
    if (m_data.empty())
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

    int drawWidth = width;

    drawWidth -= leftMargin;

    if (drawWidth > (int)m_data.size())
    {
        drawWidth = (int)m_data.size();
    }

    int i;
    for (i = 0; i < drawWidth; i++)
    {
        unsigned int n = (m_data.size() - drawWidth) + i;
        if (n >= m_data.size())
        {
            drawWidth = i - 1;
            break;
        }
        float point = m_data[n];
        if (point < min)
        {
            min = point;
        }
        if (point > max)
        {
            max = point;
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

    cr->set_line_width(1.0);

    cr->set_source_rgb(0.0, 0.0, 0.0);

    // Draw X axis
    if (min <= 0 && max >= 0)
    {
        float y = (-min / range);
        y *= (height - (topMargin + bottomMargin));
        y += bottomMargin;
        cr->move_to(leftMargin, height - y);
        cr->line_to(drawWidth, height - y);
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
    font.set_weight(Pango::WEIGHT_BOLD);

    Glib::RefPtr<Pango::Layout> layout;
    char buffer[16];
    int text_width;
    int text_height;

    sprintf(buffer, "%0.1f", max);

    layout = create_pango_layout(buffer);
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

    for (i = 0; i < drawWidth; i++)
    {
        unsigned int n = (m_data.size() - drawWidth) + i;
        float point = m_data[n];
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

