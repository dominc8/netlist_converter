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

bool node_extract(std::vector<node> &nodes, line_view &view)
{
    for (node &n : nodes)
    {
        if (0 == strncmp(view.name, n.name, sizeof(n.name)))
        {
            LOG_WARN("Node with the name %s already exists, val=%s, type=%d", n.name,
                    n.val, static_cast<int32_t>(n.comp_type));
            return false;
        }
    }

    nodes.emplace_back(view.val, view.name, view.comp_type);
    return true;
}

