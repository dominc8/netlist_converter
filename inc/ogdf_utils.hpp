#ifndef OGDF_UTILS_HPP
#define OGDF_UTILS_HPP

#include "graph.hpp"
#include "node.hpp"

namespace ogdf
{
    class Graph;
    using node = class NodeElement*;
    template <class E, class INDEX> class Array;
}

void create_edges(ogdf::Graph &G, ogdf::Array<ogdf::node, int> &v, const graph &g);
void save_layout_svg(const graph &g, const std::vector<node> &nodes, const char *filename);
void ogdf_layout(graph &g, std::vector<node> &nodes);

#endif /* OGDF_UTILS_HPP */

