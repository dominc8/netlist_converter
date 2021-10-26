#include "asc_writer.hpp"
#include "logger.hpp"

namespace
{
const char* const symbol_map[n_component_type] = 
{
    "voltage",
    "res",
    "cap",
    "ind",
};
}

void write_header(FILE *f)
{
    fprintf(f, "Version 4\n"
               "SHEET 1 680 480\n");
}

void write_node_symbol(FILE *f, const node &n)
{
    int32_t x = n.x;
    int32_t y = n.y;

    if (n.comp_type != component_type::V)
    {
        x = n.rotation == 0 ? x - 16 : x;
    }
    y = n.rotation == 90 ? y - 16 : y;
    fprintf(f, "SYMBOL %s %d %d R%d\n"
               "SYMATTR InstName %s\n"
               "SYMATTR Value %s\n",
               symbol_map[static_cast<int32_t>(n.comp_type)], x, y, n.rotation,
               n.name, n.val);
}

void write_gnd_node(FILE *f, const node &n)
{
    fprintf(f, "FLAG %d %d 0\n", n.x, n.y);
}

void write_wire(FILE *f, const node &n1, const node &n2)
{
    int32_t offset_x[2] {}, offset_y[2] {};
    int32_t mul[2] {};

    if (static_cast<int32_t>(n1.comp_type) < n_component_type)
    {
        mul[0] = 1;
    }
    if (static_cast<int32_t>(n2.comp_type) < n_component_type)
    {
        mul[1] = 1;
    }
    if (n1.x == n2.x)
    {
        if (n1.y > n2.y)
        {
            offset_y[0] = 16;
        }
        else
        {
            offset_y[1] = 16;
        }
    }
    else
    {
        if (n1.x < n2.x)
        {
            offset_x[0] = -16;
        }
        else
        {
            offset_x[1] = -16;
        }
    }
    offset_x[0] *= mul[0];
    offset_x[1] *= mul[1];
    offset_y[0] *= mul[0];
    offset_y[1] *= mul[1];
    LOG_INFO("Offsetting wire from %s by (%d, %d) to %s by (%d, %d)", n1.name, offset_x[0], offset_y[0],
                                                                      n2.name, offset_x[1], offset_y[1]);
    fprintf(f, "WIRE %d %d %d %d\n", n1.x + offset_x[0], n1.y + offset_y[0],
                                     n2.x + offset_x[1], n2.y + offset_y[1]);
}

void write_layout(FILE *f, const graph &g, const std::vector<node> &nodes)
{
    write_header(f);
    for (const auto &n : nodes)
    {
        if (static_cast<int32_t>(n.comp_type) < n_component_type)
        {
            write_node_symbol(f, n);
        }
        else if (n.comp_type == component_type::Ground)
        {
            write_gnd_node(f, n);
        }
        else
        {
            fprintf(f, "FLAG %d %d %s\n", n.x, n.y, n.name);
        }
    }

    for (int32_t i = 0; i < g.n_node; ++i)
    {
        for (int32_t j = 0; j < i; ++j)
        {
            if (g.edge_of(i, j))
            {
                write_wire(f, nodes[i], nodes[j]);
            }
        }
    }
}

