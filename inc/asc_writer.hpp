#ifndef ASC_WRITER_HPP
#define ASC_WRITER_HPP

#include <stdio.h>
#include "graph.hpp"
#include "node.hpp"

void write_header(FILE *f);
void write_node_symbol(FILE *f, const node &n);
void write_layout(FILE *f, const graph &g, const std::vector<node> &nodes);

#endif /* ASC_WRITER_HPP */

