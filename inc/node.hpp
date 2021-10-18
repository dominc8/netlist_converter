#ifndef NODE_HPP
#define NODE_HPP

#include "line_view.hpp"
#include <stdint.h>
#include <vector>

struct node
{
    node();
    node(const char *in_val, const char *in_name, component_type in_type);

    char val[8];
    char name[7];
    component_type comp_type;
};

void node_extract(std::vector<node> &nodes, const line_view &view);
int32_t node_find_idx_by_name(const std::vector<node> &nodes, const char *name);

#endif /* NODE_HPP */

