#ifndef LAYOUTIZER_HPP
#define LAYOUTIZER_HPP

#include "graph.hpp"
#include "node.hpp"

void layout(graph &g, std::vector<node> &nodes, const std::vector<line_view> &views, const char *filename);

#endif /* LAYOUTIZER_HPP */

