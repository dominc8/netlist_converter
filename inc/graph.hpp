#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdint.h>

struct graph
{
    bool init(int32_t n_node);
    uint8_t& edge_of(int32_t p0, int32_t p1);

    uint8_t *matrix;
    int32_t n_node;
};

#endif /* GRAPH_HPP */

