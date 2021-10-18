#include "node.hpp"
#include "logger.hpp"
#include <string.h>

node::node() : val{}, name{} {}

node::node(const char *in_val, const char *in_name, component_type in_type)
    : node()
{
    strncpy(&val[0], &in_val[0], sizeof(val) - 1);
    strncpy(&name[0], &in_name[0], sizeof(name) - 1);
    comp_type = in_type;
}

void node_extract(std::vector<node> &nodes, const line_view &view)
{
    bool add_name = true;
    bool add_p0 = true;
    bool add_p1 = true;
    for (node &n : nodes)
    {
        if (0 == strncmp(view.name, n.name, sizeof(n.name)))
        {
            LOG_WARN("Node with the name %s already exists, val=%s, type=%d", n.name,
                    n.val, static_cast<int32_t>(n.comp_type));
            add_name = false;
        }
        if (0 == strncmp(view.p0, n.name, sizeof(n.name)))
        {
            LOG_INFO("Node p0 with the name %s already exists, val=%s, type=%d", n.name,
                    n.val, static_cast<int32_t>(n.comp_type));
            add_p0 = false;
        }
        if (0 == strncmp(view.p1, n.name, sizeof(n.name)))
        {
            LOG_WARN("Node p1 with the name %s already exists, val=%s, type=%d", n.name,
                    n.val, static_cast<int32_t>(n.comp_type));
            add_p1 = false;
        }
    }

    if (add_name)
    {
        nodes.emplace_back(view.val, view.name, view.comp_type);
        if (add_p0)
        {
            nodes.emplace_back(view.val, view.p0, component_type::DotPoint);
        }
        if (add_p1)
        {
            nodes.emplace_back(view.val, view.p1, component_type::DotPoint);
        }
    }
}

int32_t node_find_idx_by_name(const std::vector<node> &nodes, const char *name)
{
    int32_t idx = -1;
    for (int32_t i = 0; i < nodes.size(); ++i)
    {
        if (0 == strncmp(nodes[i].name, name, sizeof(nodes[i].name)))
        {
            idx = i;
            break;
        }
    }

    return idx;
}

