#include "graph.hpp"
#include "logger.hpp"
#include <stdlib.h>

bool graph::init(int32_t n_node, int32_t n_component_node)
{
    /* allocate additional n_component_node nodes*/
    int32_t all_nodes = n_node + n_component_node;
    matrix = static_cast<uint8_t *>(calloc(all_nodes * all_nodes, sizeof(*matrix)));
    this->n_node = (nullptr != matrix) ? n_node : 0;
    this->n_all_node = (nullptr != matrix) ? n_all_node : 0;
    return nullptr != matrix;
}

void graph::deinit(void)
{
    free(matrix);
}

uint8_t& graph::edge_of(int32_t p0, int32_t p1)
{
    return matrix[p0 * n_node + p1];
}

uint8_t graph::edge_of(int32_t p0, int32_t p1) const
{
    return matrix[p0 * n_node + p1];
}

