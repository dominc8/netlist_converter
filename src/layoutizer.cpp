#include "component_type.hpp"
#include "layoutizer.hpp"
#include "logger.hpp"
#include "ogdf_utils.hpp"
#include <stdio.h>
#include <math.h>
namespace
{
void get_neighbours(const graph &g, int32_t node_idx, int32_t *neighbours)
{
    int32_t tmp_neighbours[3];
    int32_t neighbour_cnt = 0;
    auto *edges = &g.matrix[node_idx * g.n_all_node];

    for (int32_t i = 0; i < g.n_node; ++i)
    {
        tmp_neighbours[neighbour_cnt] = i;
        if (edges[i])
        {
            ++neighbour_cnt;
        }
    }

    neighbours[0] = tmp_neighbours[0];
    neighbours[1] = tmp_neighbours[1];
}

bool node_is_in_corner(const node &middle_node, const node &neighbour1, const node &neighbour2)
{
    bool is_in_corner = false;

    if (((middle_node.x == neighbour1.x) && (middle_node.y == neighbour2.y)) ||
        ((middle_node.x == neighbour2.x) && (middle_node.y == neighbour1.y)))
    {
        is_in_corner = true;
    }

    return is_in_corner;
}

int32_t node_uncornerize(node &comp_node, const node &neighbour1, const node &neighbour2)
{
    int32_t offset_x, offset_y, rotation;
    int32_t n2_offset_x, n2_offset_y;
    int32_t lost_neighbour_idx {0};

    offset_x = (neighbour1.x - comp_node.x) / 2;
    offset_y = (neighbour1.y - comp_node.y) / 2;

    n2_offset_x = (neighbour2.x - comp_node.x) / 2;
    n2_offset_y = (neighbour2.y - comp_node.y) / 2;

    LOG_INFO("Offsets: n1=(%d, %d), n2=(%d, %d)", offset_x, offset_y, n2_offset_x, n2_offset_y);

    if (std::abs(n2_offset_x) > std::abs(offset_x))
    {
        LOG_INFO("offset_x = n2");
        offset_x = n2_offset_x;
    }

    if (std::abs(n2_offset_y) > std::abs(offset_y))
    {
        LOG_INFO("offset_y = n2");
        offset_y = n2_offset_y;
    }

    if (std::abs(offset_x) > std::abs(offset_y))
    {
        lost_neighbour_idx = offset_x == n2_offset_x ? 0 : 1;
        offset_y = 0;
        rotation = 90;
        LOG_INFO("offset_x %d chosen, lost_neighbour_idx %d", offset_x, lost_neighbour_idx);
    }
    else
    {
        lost_neighbour_idx = offset_y == n2_offset_y ? 0 : 1;
        offset_x = 0;
        rotation = 0;
        LOG_INFO("offset_y %d chosen, lost_neighbour_idx %d", offset_y, lost_neighbour_idx);
    }

    LOG_INFO("Moving node %s from (%d, %d) by offset (%d, %d)", comp_node.name, comp_node.x, comp_node.y, offset_x, offset_y);

    comp_node.x += offset_x;
    comp_node.y += offset_y;
    comp_node.rotation = rotation;

    return lost_neighbour_idx;
}
}

void layout(graph &g, std::vector<node> &nodes, const std::vector<line_view> &views, const char *filename)
{
    ogdf_layout(g, nodes);
    LOG_INFO("Graph before:");
    for (int32_t i = 0; i < g.n_node; ++i)
    {
        for (int32_t j = 0; j < g.n_node; ++j)
        {
            if (g.edge_of(i, j))
            {
                LOG_INFO("%s -> %s", nodes[i].name, nodes[j].name);
            }
        }
    }

    int32_t n_node = nodes.size();
    for (int32_t i = 0; i < n_node; ++i)
    {
        if (static_cast<int32_t>(nodes[i].comp_type) < n_component_type)
        {
            int32_t neighbours[2];
            get_neighbours(g, i, &neighbours[0]);

            auto &comp_node = nodes[i];
            auto &neighbour1 = nodes[neighbours[0]];
            auto &neighbour2 = nodes[neighbours[1]];
            if (node_is_in_corner(comp_node, neighbour1, neighbour2))
            {
                char tmp_name[7];
                sprintf(tmp_name, "n%d", g.n_node);
                node new_tmp_node("", tmp_name, component_type::DotPoint);
                new_tmp_node.set_coord(comp_node.x, comp_node.y);
                int32_t lost_neighbour_idx = node_uncornerize(comp_node, neighbour1, neighbour2);
                LOG_INFO("%s lost neighbour %s", comp_node.name, nodes[neighbours[lost_neighbour_idx]].name);


                nodes.push_back(new_tmp_node);
                g.edge_of(i, neighbours[lost_neighbour_idx]) = 0;
                g.edge_of(neighbours[lost_neighbour_idx], i) = 0;
                g.edge_of(i, g.n_node) = 1;
                g.edge_of(g.n_node, i) = 1;
                g.edge_of(g.n_node, neighbours[lost_neighbour_idx]) = 1;
                g.edge_of(neighbours[lost_neighbour_idx], g.n_node) = 1;
                LOG_INFO("Added helper node %s at %d, %d", nodes[g.n_node].name, nodes[g.n_node].x, nodes[g.n_node].y);
                g.n_node++;
            }
            else
            {
                int32_t rotation = 0;
                if ((comp_node.y == neighbour1.y) && (comp_node.y == neighbour1.y))
                {
                    rotation = 90;
                }
                comp_node.rotation = rotation;
            }
        }
    }

    LOG_INFO("Graph after:");
    for (int32_t i = 0; i < g.n_node; ++i)
    {
        for (int32_t j = 0; j < g.n_node; ++j)
        {
            if (g.edge_of(i, j))
            {
                LOG_INFO("%s -> %s", nodes[i].name, nodes[j].name);
            }
        }
    }

    // voltage rotation
    for (int32_t i = 0; i < views.size(); ++i)
    {
        if (component_type::V == views[i].comp_type)
        {
            int32_t v_idx = node_find_idx_by_name(nodes, &views[i].name[0]);
            int32_t plus_idx = node_find_idx_by_name(nodes, &views[i].p0[0]);
            node &v_node = nodes[v_idx];
            const node &plus_node = nodes[plus_idx];

            if (((v_node.rotation == 0) && (plus_node.y > v_node.y)) ||
                ((v_node.rotation == 90) && (plus_node.x < v_node.x)))
            {
                v_node.rotation += 180;
            }
        }
    }

    save_layout_svg(g, nodes, filename);
}

