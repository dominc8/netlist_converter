#include "asc_writer.hpp"
#include "logger.hpp"
#include <cmath>

namespace
{
const char* const symbol_map[n_component_type] = 
{
    "voltage",
    "res",
    "cap",
    "ind",
};
int32_t calc_rotation(const graph &g, const std::vector<node> &nodes, int32_t node_idx, int32_t &offset_x, int32_t &offset_y)
{
    constexpr int32_t max_neighbour_cnt = 2;
    int32_t neighbours[max_neighbour_cnt * 10];
    uint8_t *edge = &g.matrix[node_idx * g.n_node];
    int32_t neighbour_cnt = 0;
    int32_t rotation;

    for (int32_t i = 0; i < g.n_node; ++i)
    {
        neighbours[neighbour_cnt] = i;
        if (edge[i])
        {
            ++neighbour_cnt;
        }
    }

    LOG_INFO("node %s has %d neighbours", nodes[node_idx].name, neighbour_cnt);

    const auto &neighbour1 = nodes[neighbours[0]];
    const auto &neighbour2 = nodes[neighbours[1]];
    const auto &n = nodes[node_idx];

    offset_x = 0;
    offset_y = 0;

    if ((n.x == neighbour1.x) && (n.x == neighbour2.x))
    {
        rotation = 0;
    }
    else if ((n.y == neighbour1.y) && (n.y == neighbour2.y))
    {
        rotation = 90;
    }
    else
    {
        // node in corner

        if (std::abs(n.x - neighbour1.x) > std::abs(n.y - neighbour1.y))
        {
            offset_x = (neighbour1.x - n.x) / 2;
        }
        else
        {
            offset_y = (neighbour1.y - n.y) / 2;
        }

        if (std::abs(n.x - neighbour2.x) > std::abs(n.y - neighbour2.y))
        {
            int32_t n2_offset = (neighbour2.x - n.x) / 2;
            offset_x = offset_x > n2_offset ? offset_x : n2_offset;
        }
        else
        {
            int32_t n2_offset = (neighbour2.y - n.y) / 2;
            offset_y = offset_y > n2_offset ? offset_y : n2_offset;
        }

        if (offset_x > offset_y)
        {
            rotation = 0;
            offset_y = 0;
        }
        else
        {
            rotation = 90;
            offset_x = 0;
        }

    }

    return rotation;
}
}

void write_header(FILE *f)
{
    fprintf(f, "Version 4\n"
               "SHEET 1 680 480\n");
}

void write_node_symbol(FILE *f, const node &n, int32_t rotation, int32_t offset_x, int32_t offset_y)
{
    fprintf(f, "SYMBOL %s %d %d R%d\n"
               "SYMATTR InstName %s\n"
               "SYMATTR Value %s\n",
               symbol_map[static_cast<int32_t>(n.comp_type)], n.x + offset_x, n.y + offset_y, rotation,
               n.name, n.val);
}

void write_layout(FILE *f, const graph &g, const std::vector<node> &nodes)
{
    write_header(f);
    for (int32_t i = 0; i < nodes.size(); ++i)
    {
        const auto &n = nodes[i];
        if (static_cast<int32_t>(n.comp_type) >= n_component_type)
        {
            continue;
        }
        int32_t offset_x, offset_y;
        int32_t rotation = calc_rotation(g, nodes, i, offset_x, offset_y);
        write_node_symbol(f, n, rotation, offset_x, offset_y);
    }
}

