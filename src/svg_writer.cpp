#include "logger.hpp"
#include "svg_writer.hpp"
#include <stdio.h>
#include <string>

namespace
{
void begin_svg(FILE *f, const std::vector<node> &nodes)
{
    int32_t min_x = nodes[0].x;
    int32_t max_x = nodes[0].x;
    int32_t min_y = nodes[0].y;
    int32_t max_y = nodes[0].y;

    for (const auto &n : nodes)
    {
        min_x = (n.x < min_x) ? n.x : min_x;
        max_x = (n.x > max_x) ? n.x : max_x;
        min_y = (n.y < min_y) ? n.y : min_y;
        max_y = (n.y > max_y) ? n.y : max_y;
    }

    int32_t dx = max_x - min_x;
    int32_t dy = max_y - min_y;
    int32_t x_border = 30;
    int32_t y_border = 30;


    int32_t width = dx + 2 * x_border;
    int32_t height = dy + 2 * y_border;
    int32_t x0 = min_x - x_border;
    int32_t y0 = min_y - y_border;

    fprintf(f, "<?xml version=\"1.0\"?>\n"
            "<svg\n"
            "    xmlns=\"http://www.w3.org/2000/svg\"\n"
            "    xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
            "    xmlns:ev=\"http://www.w3.org/2001/xml-events\"\n"
            "    version=\"1.1\"\n"
            "    baseProfile=\"full\"\n"
            "    viewBox=\"%d %d %d %d\">\n",
            x0, y0, width, height
           );
}

void end_svg(FILE *f)
{
    fprintf(f, "</svg>");
}

void write_svg_wire(FILE *f, const node &n1, const node &n2)
{
    fprintf(f, "        <g>\n"
               "            <path fill=\"none\" d=\"M %d %d L %d %d\" stroke=\"#000000\" />\n"
               "        </g>\n",
               n1.x, n1.y, n2.x, n2.y);
}

void write_svg_wires(FILE *f, const graph &g, const std::vector<node> &nodes)
{
    fprintf(f, "    <g>\n");
    for (int32_t i = 0; i < g.n_node; ++i)
    {
        for (int32_t j = 0; j < i; ++j)
        {
            if (g.edge_of(i, j))
            {
                write_svg_wire(f, nodes[i], nodes[j]);
            }
        }
    }
    fprintf(f, "    </g>\n");
}
}

void write_layout_svg(const graph &g, const std::vector<node> &nodes, const char *filename)
{
    std::string s(filename);
    s.append(".new.svg");
    FILE *f = fopen(s.c_str(), "w");
    if (f == nullptr)
    {
        LOG_ERROR("Cannot open file %s", filename);
        return;
    }
    begin_svg(f, nodes);

    write_svg_wires(f, g, nodes);

    end_svg(f);
    fclose(f);
}

