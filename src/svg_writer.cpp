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

    // for drawing arrows in voltage sources
    fprintf(f, "    <defs>\n"
               "        <marker id=\"arrowhead\" markerWidth=\"8\" markerHeight=\"8\" refX=\"0\" refY=\"4\" orient=\"auto\">\n"
               "        <polygon points=\"0 0, 4 4, 0 8\" />\n"
               "        </marker>\n"
               "    </defs>\n");
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
        case component_type::V:
        {
            constexpr int32_t R = 15;
            constexpr int32_t A = 8; // depends on arrow params defined in header
            int32_t cx = n.x, cy = n.y;
            int32_t lx1, lx2, ly1, ly2;
            if (n.rotation == 0)
            {
                lx1 = n.x;
                lx2 = n.x;
                ly1 = n.y + A;
                ly2 = n.y - A;
            }
            else if (n.rotation == 90)
            {
                lx1 = n.x - A;
                lx2 = n.x + A;
                ly1 = n.y;
                ly2 = n.y;
            }
            else if (n.rotation == 180)
            {
                lx1 = n.x;
                lx2 = n.x;
                ly1 = n.y - A;
                ly2 = n.y + A;
            }
            else // 270
            {
                lx1 = n.x + A;
                lx2 = n.x - A;
                ly1 = n.y;
                ly2 = n.y;
            }

            fprintf(f, "    <g>\n"
                       "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
                       "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" marker-end=\"url(#arrowhead)\" />\n"
                       "    </g>\n",
                       cx, cy, R, lx1, ly1, lx2, ly2);
            break;
        }
        case component_type::L:
        {
            constexpr int32_t n_arc = 3;
            constexpr int32_t W = 12; // single arc width
            constexpr int32_t L = 8; // arc amplitude
            int32_t x[4], y[4];
            int32_t x_offset, y_offset;
            int32_t x0, y0, width, height;

            if (n.rotation == 0)
            {
                x_offset = 0;
                y_offset = W;

                x[0] = n.x;
                y[0] = n.y - n_arc * W / 2;
                x[1] = n.x + L;
                y[1] = y[0] + W/3;
                x[2] = n.x + L;
                y[2] = y[0] + (W - W/3);
                x[3] = n.x;
                y[3] = y[0] + W;

                x0 = n.x - L/2;
                y0 = y[0];
                width = 3 * L / 2;
                height = n_arc * W;
            }
            else
            {
                x_offset = W;
                y_offset = 0;

                x[0] = n.x - n_arc * W / 2;
                y[0] = n.y;
                x[1] = x[0] + W/3;
                y[1] = n.y + L;
                x[2] = x[0] + (W - W/3);
                y[2] = n.y + L;
                x[3] = x[0] + W;
                y[3] = n.y;

                x0 = x[0];
                y0 = n.y - L/2;
                width = n_arc * W;
                height = 3 * L / 2;
            }
            fprintf(f, "    <g>\n"
                       "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff\" />\n"
                       "        <path d=\"M %d,%d C %d,%d %d,%d %d,%d C %d,%d %d,%d %d,%d C %d,%d %d,%d %d,%d \" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
                       "    </g>\n",
                       x0, y0, width, height,
                       x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3],
                       x[1] + x_offset, y[1] + y_offset, x[2] + x_offset, y[2] + y_offset, x[3] + x_offset, y[3] + y_offset,
                       x[1] + 2*x_offset, y[1] + 2*y_offset, x[2] + 2*x_offset, y[2] + 2*y_offset, x[3] + 2*x_offset, y[3] + 2*y_offset);
            break;
        }
        case component_type::C:
        {
            constexpr int32_t W = 20;
            constexpr int32_t L = 10;
            int32_t x1[2], x2[2], y1[2], y2[2];
            int32_t width, height;
            if (n.rotation == 0)
            {
                width = W;
                height = L;

                x1[0] = n.x - W/2;
                x1[1] = n.x - W/2;
                x2[0] = n.x + W/2;
                x2[1] = n.x + W/2;

                y1[0] = n.y - L/2;
                y1[1] = n.y + L/2;
                y2[0] = n.y - L/2;
                y2[1] = n.y + L/2;
            }
            else
            {
                width = L;
                height = W;

                x1[0] = n.x - L/2;
                x1[1] = n.x + L/2;
                x2[0] = n.x - L/2;
                x2[1] = n.x + L/2;

                y1[0] = n.y - W/2;
                y1[1] = n.y - W/2;
                y2[0] = n.y + W/2;
                y2[1] = n.y + W/2;
            }
            fprintf(f, "    <g>\n"
                       "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff\" />\n"
                       "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" />\n"
                       "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" />\n"
                       "    </g>\n",
                       x1[0], y1[0], width, height, 
                       x1[0], y1[0], x2[0], y2[0], x1[1], y1[1], x2[1], y2[1]);
            break;
        }
        case component_type::I:
        {
            constexpr int32_t R_outer = 18;
            constexpr int32_t R = 15;
            constexpr int32_t A = 8; // depends on arrow params defined in header
            int32_t cx = n.x, cy = n.y;
            int32_t lx1, lx2, ly1, ly2;
            if (n.rotation == 0)
            {
                lx1 = n.x;
                lx2 = n.x;
                ly1 = n.y - A;
                ly2 = n.y + A;
            }
            else if (n.rotation == 90)
            {
                lx1 = n.x + A;
                lx2 = n.x - A;
                ly1 = n.y;
                ly2 = n.y;
            }
            else if (n.rotation == 180)
            {
                lx1 = n.x;
                lx2 = n.x;
                ly1 = n.y + A;
                ly2 = n.y - A;
            }
            else // 270
            {
                lx1 = n.x - A;
                lx2 = n.x + A;
                ly1 = n.y;
                ly2 = n.y;
            }

            fprintf(f, "    <g>\n"
                       "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
                       "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
                       "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" marker-end=\"url(#arrowhead)\" />\n"
                       "    </g>\n",
                       cx, cy, R_outer, cx, cy, R, lx1, ly1, lx2, ly2);
            break;
        }
        default:
            LOG_WARN("Type of node %s not yet implemented", n.name);
    }
}

void write_dotpoint(FILE *f, int32_t x, int32_t y)
{
    constexpr int32_t R = 2;
    fprintf(f, "    <g>\n"
               "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#000000\" />\n"
               "    </g>\n",
               x, y, R);
}

void write_svg_ground_node(FILE *f, const node &n)
{
    constexpr int32_t offset = 20;
    constexpr int32_t W = 20;
    constexpr int32_t L = 10;
    fprintf(f, "    <g>\n"
               "        <path d=\"M %d,%d h %d v %d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
               "        <path d=\"M %d,%d h %d l %d,%d l %d,%d h %d\" style=\"fill:#000000;stroke:#000000;stroke-width:1\" />\n"
               "    </g>\n",
               n.x, n.y,
               offset, offset,
               n.x + offset, n.y + offset,
               W/2,
               -W/2, L,
               -W/2, -L,
               W/2);
    write_dotpoint(f, n.x, n.y);
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

void write_svg_true_dotpoints(FILE *f, const graph &g, const std::vector<node> &nodes)
{
    for (int32_t i = 0; i < g.n_node; ++i)
    {
        const auto &n = nodes[i];
        if (n.comp_type == component_type::DotPoint)
        {
            if (g.count_neighbours(i) > 2)
            {
                write_dotpoint(f, n.x, n.y);
            }
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
    write_svg_true_dotpoints(f, g, nodes);

    end_svg(f);
    fclose(f);
}

