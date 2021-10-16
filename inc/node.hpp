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

bool node_extract(std::vector<node> &nodes, line_view &view);

#endif /* NODE_HPP */

