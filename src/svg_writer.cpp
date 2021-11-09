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

void write_svg_component_node(FILE *f, const node &n)
{
    switch(n.comp_type)
    {
        case component_type::R:
        {
            constexpr int32_t W = 20;
            constexpr int32_t L = 40;
            int32_t rect_x, rect_y, rect_w, rect_h;
            if (n.rotation == 0)
            {
                rect_x = n.x - W/2;
                rect_y = n.y - L/2;
                rect_w = W;
                rect_h = L;
            }
            else
            {
                rect_x = n.x - L/2;
                rect_y = n.y - W/2;
                rect_w = L;
                rect_h = W;
            }
            fprintf(f, "    <g>\n"
                       "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
                       "    </g>\n",
                       rect_x, rect_y, rect_w, rect_h);
            break;
        }
        //case component_type::V:
        //case component_type::L:
        //case component_type::C:
        //case component_type::I:
        default:
            LOG_WARN("Type of node %s not yet implemented", n.name);
    }
}

void write_svg_ground_node(FILE *f, const node &n)
{
}

void write_svg_nodes(FILE *f, const std::vector<node> &nodes)
{
    for (const auto &n : nodes)
    {
        if (static_cast<int32_t>(n.comp_type) < n_component_type)
        {
            write_svg_component_node(f, n);
        }
        else if (n.comp_type == component_type::Ground)
        {
            write_svg_ground_node(f, n);
        }
    }
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
    write_svg_nodes(f, nodes);

    end_svg(f);
    fclose(f);
}

