#ifndef OGDF_UTILS_HPP
#define OGDF_UTILS_HPP

#include "graph.hpp"
#include "node.hpp"
//namespace ogdf
//{
//    class Graph;
//    class node;
//    //template <class arg> class Array;
//    template <class E, class INDEX = int> class Array;
//}
#include <ogdf/basic/Array.h>
#include <ogdf/basic/Graph.h>

void create_edges(ogdf::Graph &G, ogdf::Array<ogdf::node> &v, const graph &g);
void save_layout_svg(const graph &g, const std::vector<node> &nodes, const char *filename);
void ogdf_layout(graph &g, std::vector<node> &nodes);

#endif /* OGDF_UTILS_HPP */

