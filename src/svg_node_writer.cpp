#include "logger.hpp"
#include "svg_node_writer.hpp"

namespace
{
constexpr int32_t label_dist = 5;
constexpr int32_t font_size = 16; // move to common or sth

enum class orientation
{
    vertical = 0,
    horizontal
};

struct node_text_view
{
    node_text_view(const node &n):
        name{&n.name[0]}, val{&n.val[0]}
    {
        if ((n.rotation == 0) || (n.rotation == 180))
        {
            orient = orientation::vertical;
        }
        else
        {
            orient = orientation::horizontal;
        }
    }

    int32_t x_name;
    int32_t y_name;
    int32_t x_val;
    int32_t y_val;
    const char *name;
    const char *val;
    orientation orient;
};

void write_svg_labels(FILE *f, const node_text_view &view)
{
    const char *style = (view.orient == orientation::vertical) ? "vert" : "hori";
    fprintf(f, "        <text x=\"%d\" y=\"%d\" class=\"%s\">%s</text>\n"
               "        <text x=\"%d\" y=\"%d\" class=\"%s\">%s</text>\n",
               view.x_name, view.y_name, style, view.name,
               view.x_val, view.y_val, style, view.val);
}

}

void write_svg_capacitor(FILE *f, const node &n)
{
    constexpr int32_t W = 20;
    constexpr int32_t L = 10;
    node_text_view view(n);
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
        view.x_name = n.x + W/2 + label_dist;
        view.y_name = n.y;
        view.x_val = view.x_name;
        view.y_val = view.y_name + font_size;
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
        view.x_name = n.x;
        view.y_name = n.y - W/2 - label_dist;
        view.x_val = n.x;
        view.y_val = n.y + W/2 + label_dist + font_size;
    }
    fprintf(f, "    <g>\n"
               "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff\" />\n"
               "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" />\n"
               "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" />\n",
               x1[0], y1[0], width, height,
               x1[0], y1[0], x2[0], y2[0], x1[1], y1[1], x2[1], y2[1]);
    write_svg_labels(f, view);
    fprintf(f, "    </g>\n");
}

void write_svg_resistor(FILE *f, const node &n)
{
    constexpr int32_t W = 20;
    constexpr int32_t L = 40;
    node_text_view view(n);
    int32_t rect_x, rect_y, rect_w, rect_h;
    if (n.rotation == 0)
    {
        rect_x = n.x - W/2;
        rect_y = n.y - L/2;
        rect_w = W;
        rect_h = L;
        view.x_name = n.x + W/2 + label_dist;
        view.y_name = n.y;
        view.x_val = view.x_name;
        view.y_val = view.y_name + font_size;
    }
    else
    {
        rect_x = n.x - L/2;
        rect_y = n.y - W/2;
        rect_w = L;
        rect_h = W;
        view.x_name = n.x;
        view.y_name = n.y - W/2 - label_dist;
        view.x_val = view.x_name;
        view.y_val = n.y + W/2 + label_dist + font_size;
    }

    fprintf(f, "    <g>\n"
               "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n",
               rect_x, rect_y, rect_w, rect_h);
    write_svg_labels(f, view);
    fprintf(f, "    </g>\n");
}

void write_svg_inductor(FILE *f, const node &n)
{
    constexpr int32_t n_arc = 3;
    constexpr int32_t W = 12; // single arc width
    constexpr int32_t L = 8; // arc amplitude
    node_text_view view(n);
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
        view.x_name = n.x + L + label_dist;
        view.y_name = n.y;
        view.x_val = view.x_name;
        view.y_val = view.y_name + font_size;
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
        view.x_name = n.x;
        view.y_name = n.y - label_dist;
        view.x_val = view.x_name;
        view.y_val = n.y + label_dist + font_size;
    }

    fprintf(f, "    <g>\n"
               "        <rect x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" style=\"fill:#ffffff\" />\n"
               "        <path d=\"M %d,%d C %d,%d %d,%d %d,%d C %d,%d %d,%d %d,%d C %d,%d %d,%d %d,%d \" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n",
               x0, y0, width, height,
               x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3],
               x[1] + x_offset, y[1] + y_offset, x[2] + x_offset, y[2] + y_offset, x[3] + x_offset, y[3] + y_offset,
               x[1] + 2*x_offset, y[1] + 2*y_offset, x[2] + 2*x_offset, y[2] + 2*y_offset, x[3] + 2*x_offset, y[3] + 2*y_offset);
    write_svg_labels(f, view);
    fprintf(f, "    </g>\n");
}

namespace
{
struct svg_source_view
{
    int32_t radius[2];
    int32_t c_x;
    int32_t c_y;
    int32_t line_x[2];
    int32_t line_y[2];
};

void prepare_source_views(const node &n, node_text_view &text_view, svg_source_view &source_view)
{
    constexpr int32_t R = 18;
    constexpr int32_t R_inner = 15; // For current source
    constexpr int32_t A = 8; // arrow length, indirectly depends on arrow params defined in svg header

    source_view.radius[0] = R;
    source_view.radius[1] = R_inner;

    int32_t cx = n.x, cy = n.y;

    source_view.c_x = cx;
    source_view.c_y = cy;
    int32_t lx1, lx2, ly1, ly2;

    int32_t arrow = A;
    if ((n.rotation == 90) || (n.rotation == 180))
    {
        arrow = -arrow;
    }
    if (n.comp_type == component_type::V)
    {
        arrow = -arrow;
        source_view.radius[1] = 0;
    }

    if ((n.rotation == 0) || (n.rotation == 180))
    {
        source_view.line_x[0] = cx;
        source_view.line_x[1] = cx;
        source_view.line_y[0] = cy - arrow;
        source_view.line_y[1] = cy + arrow;
        text_view.x_name = cx + R + label_dist;
        text_view.y_name = cy;
        text_view.x_val = text_view.x_name;
        text_view.y_val = cy + font_size;
    }
    else
    {
        source_view.line_x[0] = cx - arrow;
        source_view.line_x[1] = cx + arrow;
        source_view.line_y[0] = cy;
        source_view.line_y[1] = cy;
        text_view.x_name = cx;
        text_view.y_name = cy - R - label_dist;
        text_view.x_val = cx;
        text_view.y_val = cy + R + font_size;
    }
}

void write_svg_source(FILE *f, const node &n)
{
    svg_source_view source_view;
    node_text_view text_view(n);

    prepare_source_views(n, text_view, source_view);
    fprintf(f, "    <g>\n"
               "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
               "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
               "        <line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke=\"#000000\" stroke-width=\"1\" marker-end=\"url(#arrowhead)\" />\n",
               source_view.c_x, source_view.c_y, source_view.radius[0],
               source_view.c_x, source_view.c_y, source_view.radius[1],
               source_view.line_x[0], source_view.line_y[0], source_view.line_x[1], source_view.line_y[1]);
    write_svg_labels(f, text_view);
    fprintf(f, "    </g>\n");
}
}

void write_svg_voltage(FILE *f, const node &n)
{
    write_svg_source(f, n);
}

void write_svg_current(FILE *f, const node &n)
{
    write_svg_source(f, n);
}

void write_svg_dot(FILE *f, const node &n)
{
    constexpr int32_t R = 2;
    fprintf(f, "    <g>\n"
               "        <circle cx=\"%d\" cy=\"%d\" r=\"%d\" style=\"fill:#000000\" />\n"
               "    </g>\n",
               n.x, n.y, R);
}

void write_svg_ground(FILE *f, const node &n)
{
    constexpr int32_t offset = 20;
    constexpr int32_t W = 20;
    constexpr int32_t L = 10;
    fprintf(f, "    <g>\n"
               "        <path d=\"M %d,%d h %d v %d\" style=\"fill:#ffffff;stroke:#000000;stroke-width:1\" />\n"
               "        <path d=\"M %d,%d h %d l %d,%d l %d,%d h %d\" style=\"fill:#000000;stroke:#000000;stroke-width:1\" />\n"
               "    </g>\n",
               n.x, n.y, offset, offset,
               n.x + offset, n.y + offset,
               W/2,
               -W/2, L,
               -W/2, -L,
               W/2);
    write_svg_dot(f, n);
}

