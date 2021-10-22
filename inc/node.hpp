#ifndef NODE_HPP
#define NODE_HPP

#include "line_view.hpp"
#include <stdint.h>
#include <vector>

struct node
{
    node();
    node(const char *in_val, const char *in_name, component_type in_type);
    inline void set_coord(int32_t in_x, int32_t in_y) { x = in_x; y = in_y; }

    char val[8];
    char name[7];
    component_type comp_type;
    int32_t x;
    int32_t y;
    int32_t rotation;
};

void node_extract(std::vector<node> &nodes, const line_view &view);
int32_t node_find_idx_by_name(const std::vector<node> &nodes, const char *name);

#endif /* NODE_HPP */

