#include "net_parser.hpp"
#include "logger.hpp"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <tuple>

namespace
{
constexpr char comp_type_map[n_component_type] = 
{
    'V',
    'R',
    'C',
    'L',
    'I'
};
}

bool parse_type(char *token, component_type *type)
{

    for (int32_t i = 0; i < n_component_type; ++i)
    {
        if (comp_type_map[i] == toupper(token[0]))
        {
            *type = static_cast<component_type>(i);
            return true;
        }
    }

    *type = component_type::Unknown;
    return false;
}

bool parse_str(char *token, char *str, int32_t str_len)
{
    int32_t token_len = strlen(token);
    bool result = false;

    if (token_len < str_len)
    {
        memcpy(str, token, token_len);
        str[token_len] = '\0';
        result = true;
    }

    return result;
}

bool parse_line(char *line, line_view *view)
{
    constexpr char delimiters[] = " \t\n";
    char *token = strtok(line, &delimiters[0]);
    if (nullptr == token)
    {
        return false;
    }

    bool result;

    result = parse_str(token, &view->name[0], sizeof(view->name));
    result = parse_type(token, &view->comp_type);
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->p0[0], sizeof(view->p0));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->p1[0], sizeof(view->p1));
    token = strtok(nullptr, &delimiters[0]);

    if ((nullptr == token) || !result)
    {
        return false;
    }

    result = parse_str(token, &view->val[0], sizeof(view->val));

    return result;
}

std::vector<line_view> parse_file(FILE *file)
{
    constexpr int32_t max_line_size = 128;
    char line[max_line_size];

    std::vector<line_view> views;
    views.reserve(32);

    while(1)
    {
        memset(line, '\0', max_line_size);
        fgets(line, max_line_size, file);
        if (feof(file))
        {
            LOG_INFO("Reached end of file");
            break;
        }
        else
        {
            LOG_INFO("Calling parse_line");
            line_view view;
            bool result = parse_line(line, &view);

            if (result)
            {
                LOG_INFO("New view: %s, %s, %s, %s, %c", view.name, view.p0, view.p1, view.val, comp_type_map[static_cast<int32_t>(view.comp_type)]);
                views.push_back(view);
            }
        }
    }
    return views;
}

std::vector<node> parse_views(const std::vector<line_view> &views)
{
    std::vector<node> nodes;
    nodes.reserve(views.size());

    for (auto &v : views)
    {
        node_extract(nodes, v);
    }
    return nodes;
}

graph parse_nodes(const std::vector<node> &nodes, const std::vector<line_view> &views)
{
    graph g;
    int32_t n_node = nodes.size();

    if (0 == n_node)
    {
        LOG_ERROR("There are no nodes");
        return g;
    }

    int32_t n_component_node = 0;
    for (const auto &n : nodes)
    {
        if (static_cast<int32_t>(n.comp_type) < n_component_type)
        {
            ++n_component_node;
        }
    }
    LOG_INFO("Found %d component nodes in %d nodes", n_component_node, n_node);

    g.init(n_node, n_component_node);
    if (g.n_node != n_node)
    {
        LOG_ERROR("Graph creation failed");
        return g;
    }

    for (auto &v : views)
    {
        int32_t elem_idx = node_find_idx_by_name(nodes, v.name);
        if (elem_idx < 0)
        {
            LOG_WARN("Node %s from view cannot be found in nodes", v.name);
            continue;
        }

        int32_t p0_idx = node_find_idx_by_name(nodes, v.p0);
        if (p0_idx < 0)
        {
            LOG_WARN("Node %s from view cannot be found in nodes", v.p0);
            continue;
        }

        int32_t p1_idx = node_find_idx_by_name(nodes, v.p1);
        if (p1_idx < 0)
        {
            LOG_WARN("Node %s from view cannot be found in nodes", v.p1);
            continue;
        }

        g.edge_of(elem_idx, p0_idx) = 1;
        g.edge_of(elem_idx, p1_idx) = 1;
        g.edge_of(p0_idx, elem_idx) = 1;
        g.edge_of(p1_idx, elem_idx) = 1;
    }

    return g;
}

