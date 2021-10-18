#include "graph.hpp"
#include "logger.hpp"
#include <stdlib.h>

bool graph::init(int32_t n_node)
{
    matrix = static_cast<uint8_t *>(calloc(n_node * n_node, sizeof(*matrix)));
    this->n_node = (nullptr != matrix) ? n_node : 0;
    return nullptr != matrix;
}

uint8_t& graph::edge_of(int32_t p0, int32_t p1)
{
    return matrix[p0 * n_node + p1];
}

uint8_t graph::edge_of(int32_t p0, int32_t p1) const
{
    return matrix[p0 * n_node + p1];
}
