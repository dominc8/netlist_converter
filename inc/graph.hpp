#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <stdint.h>

struct graph
{
    bool init(int32_t n_node, int32_t n_component_node);
    void deinit(void);
    uint8_t& edge_of(int32_t p0, int32_t p1);
    uint8_t edge_of(int32_t p0, int32_t p1) const;

    uint8_t *matrix {nullptr};
    int32_t n_node {0};
    int32_t n_all_node {0};
};

#endif /* GRAPH_HPP */

